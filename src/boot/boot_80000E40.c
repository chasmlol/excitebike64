#include "boot.h"

void boot_draw_char(s32 c, s32 x, s32 y, u16 *fb, s32 color)
{
    s32 i, j;
    u8 *src;
    u32 v = (u8)(c - 0x20);
    u32 col;
    i = (v >> 4) * 8;
    col = (v & 0xF) * 8;
    src = sBootFontBitmap + ((0x18 - i) * 128 + col);

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (src[(7 - i) * 128 + j]) {
                fb[(y + i) * 320 + (x + j)] = color;
            }
        }
    }
}

void boot_draw_string(u8 *s, s32 x, s32 y, u16 *fb, s32 color)
{
    s32 i, j;
    u8 *src;
    u8 ch;
    u32 v;

    ch = *s;
    while (ch != 0) {
        v = (u8)(*s - 0x20);
        i = (v >> 4) * 8;
        j = (v & 0xF) * 8;
        src = sBootFontBitmap + ((0x18 - i) * 128 + j);

        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                if (src[(7 - i) * 128 + j]) {
                    fb[(y + i) * 320 + (x + j)] = color;
                }
            }
        }
        s++;
        x += 8;
        ch = *s;
    }
    osWritebackDCacheAll();
}

void boot_func_80000FC8(s32 x0, s32 x1, s32 y0)
{
    s32 x, k, t, q;
    u8 *b;

    b = (u8 *)0x80200000;
    for (x = x0; x < x1; x++) {
        for (k = 0; k < 8; k++) {
            q = (k * 31) / 7;
            *(u16 *)(b + ((((y0 + k) * 320) + x) * 2)) = (u16)((q << 6) | 0xF800 | (q << 1));
        }
        for (k = 0; k < 3; k++) {
            q = ((2 - k) * 31) / 2;
            *(u16 *)(b + 0x1400 + ((((y0 + k) * 320) + x) * 2)) = (u16)((q << 6) | 0xF800 | (q << 1));
        }
    }
    boot_func_80006D60();
}

typedef struct { u8 b[0x30]; } Blk48;
typedef struct { s32 a; s32 b; } Pair;
typedef struct { s32 w0; s32 w1; Pair seg[4]; u32 rom; u32 sum; } Hdr_2;

void boot_thread_main(void)
{
    Hdr_2 hdr;
    s32 loc[2];
    s32 n;
    s32 off;
    s32 *q;
    s32 *base;
    u8 *buf;
    s32 y;
    s32 j;
    s32 k, d, m;
    s32 *pa;
    s32 *pb;
    s32 *ps;
    s32 *pd;
    u8 *src;
    u16 *pal;
    u16 *dst;

    u32 s1;
    u32 ret;
    u32 r;
    u32 t4;
    u32 fp;

    sBootFramebuffer = 0x80200000;
    sBootUnpackBuf = 0x80300000;
    boot_mem_fill((void *)0x80200000, 0, 0x28000);
    boot_init_prenmi_queue();
    boot_check_reset();
    osCreateViManager(10);
    if (osTvType == 1) {
        osViSetMode(&sBootViModeNtsc);
    } else if (osTvType == 2) {
        osViSetMode(&sBootViModeMpal);
    }
    osViBlack(1);
    if (osTvType == 0) {
        for (;;) ;
    }
    osViSetSpecialFeatures(0xAA);
    y = 0;
    boot_check_reset();
    gSwdLoadCur = (u32)sBootSplashSwd;
    gSwdLoadEnd = (u32)sBootSplashSwd + 0x1688;
    gSwdSaveCur = sBootUnpackBuf;
    gSwdSaveEnd = sBootUnpackBuf + 0x25800;
    boot_swd_expand();
    boot_check_reset();
    src = (u8 *)(sBootUnpackBuf + 0x28);
    pal = (u16 *)((u32)src + 0x12C00);
    dst = (u16 *)sBootFramebuffer;
    for (; y < 0xF0; y++) {
        j = 0;
        d = (0xEF - y) * 0x140;
        for (; j < 0x140; j++) {
            dst[d + j] = pal[src[y * 0x140 + j]];
        }
    }
    osWritebackDCache(0, 0x800000);
    boot_check_reset();
    osViBlack(0);
    osViSwapBuffer_2(sBootFramebuffer);
    y = 0;
    osViBlack(y);
    osViSwapBuffer_2(sBootFramebuffer);
    osCreateMesgQueue(&sBootViQueue, sBootViMsgs, 8);
    osViSetEvent(&sBootViQueue, y, 1);
    boot_check_reset();
    do {
        osRecvMesg(&sBootViQueue, loc, 1);
        boot_check_reset();
        y++;
    } while (y < 0x78);
    t4 = (u32)gBootGameContainerRom;
    boot_dma_init();
    buf = (u8 *)0x80100000;
    y = boot_dma_start(sBootCartHandle, 0, t4, (u32)buf, 0xAF000);
    do {
    } while (boot_dma_get_status(y) != 3);
    osInvalDCache(0, 0x400000);
    boot_check_reset();
    *(Blk48 *)&hdr = *(Blk48 *)buf;
    sBootGameDst = 0x80300000;
    sBootGameSize = 0;
    sBootGameSrc = (u32)(buf + 0x28);
    base = (s32 *)&hdr;
    for (y = 0; y < 4; y++) {
        q = (s32 *)((u8 *)base + (y * 8 + 4));
        pa = base + y * 2;
        gSwdLoadCur = sBootGameSrc;
        gSwdLoadEnd = sBootGameSrc + q[2];
        gSwdSaveCur = sBootGameDst;
        gSwdSaveEnd = sBootGameDst + pa[2];
        boot_swd_expand();
        sBootGameSrc += q[2];
        sBootGameDst += pa[2];
        sBootGameSize += pa[2];
    }
    boot_check_reset();
    boot_relocate_stub((u32)gBootStubStart, (u32)gBootStubEnd - (u32)gBootStubStart, 0x802F0000);
    n = hdr.seg[0].b + 0x28;
    hdr.rom = t4;
    hdr.sum = t4 + n + hdr.seg[1].b + hdr.seg[2].b + hdr.seg[3].b;
    *(Blk48 *)0x803FFFC0 = *(Blk48 *)&hdr;
    *(u32 *)((u32)gBootStubSrc + sBootStubDelta) = 0x80300008;
    *(u32 *)((u32)gBootStubDst + sBootStubDelta) = 0x80000408;
    *(u32 *)((u32)gBootStubSize + sBootStubDelta) = sBootGameSize;
    *(u32 *)((u32)gBootStubEntry + sBootStubDelta) = 0x80000400;
    boot_check_reset();
    sBootStubFunc = (void (*)(void))0x802F0000;
    __osDisableInt();
    sBootStubFunc();
}

void boot_relocate_stub(u32 *from, u32 size, u32 *dst)
{
    u32 *src = from;
    u32 i;
    u32 w, op, t, nx;

    sBootStubDelta = (u32)dst - (u32)from;
    for (i = 0; i < size; src++, i += 4, dst++) {
        w = *src;
        op = w & 0xFC000000;
        if (op == 0x08000000 || op == 0x0C000000) {
            t = w & 0x3FFFFFF;
            t <<= 2;
            if (t != 0x400) {
                t += sBootStubDelta;
            }
            t >>= 2;
            t &= 0x3FFFFFF;
            w = op | t;
            *dst = w;
        } else if ((w & 0xFFE00000) == 0x3C000000) {
            nx = src[1];
            op = nx & 0xFC000000;
            if (op == 0x8C000000 || op == 0xAC000000) {
                t = w << 16;
                t += (s16)src[1];
                t += sBootStubDelta;
                w = (w & 0xFFFF0000) | (u16)(t >> 16);
                if (t & 0x8000) {
                    w += 0x10000;
                }
                *dst++ = w;
                *dst = (src[1] & 0xFFFF0000) | (t & 0xFFFF);
                src++;
                i++;
            } else if (op == 0x34000000) {
                t = w << 16;
                t |= nx & 0xFFFF;
                t += sBootStubDelta;
                w = (w & 0xFFFF0000) | (u16)(t >> 16);
                *dst++ = w;
                *dst = (src[1] & 0xFFFF0000) | (t & 0xFFFF);
                src++;
                i++;
            } else {
                *dst = w;
            }
        } else {
            *dst = w;
        }
    }
    osWritebackDCache(0, 0x800000);
    osInvalDCache(0, 0x800000);
    osInvalICache(0, 0x800000);
}

s32 boot_init_controllers(void)
{
    s32 n;
    s32 bVar1;
    s32 bVar2;
    s32 dd;
    s32 ret;
    u32 buf[2];
    u32 *bp;

    osCreateMesgQueue(&sBootSiQueue, sBootSiMsgs, 8);
    osSetEventMesg(5, &sBootSiQueue, 0);
    if (osContInit(&sBootSiQueue, sBootContPattern, sBootContStatus) != 0) {
        while (1) {}
    }
    osContStartQuery(&sBootSiQueue);
    bp = buf;
    osRecvMesg(&sBootSiQueue, bp, 1);
    osContGetQuery(sBootContStatus);
    osContStartReadData(&sBootSiQueue);
    osRecvMesg(&sBootSiQueue, bp, 1);
    osContGetReadData(sBootContPads);
    bVar1 = 0;
    bVar2 = 0;
    for (n = 0; n < 4; n++) {
        if (sBootContStatus[n].errno == 0) {
            if ((sBootContPads[n].button & 0x1000) && n == 0) {
                bVar1 = 1;
            }
            switch (osPfsInitPak(&sBootSiQueue, &sBootPfs[n], n)) {
            case 0: dd = 0; break;
            case 1: dd = 1; break;
            case 2: dd = 2; break;
            case 3: dd = 3; break;
            case 4: dd = 4; break;
            case 5: dd = 5; break;
            case 6: dd = 6; break;
            case 7: dd = 7; break;
            case 8: dd = 8; break;
            case 9: dd = 9; break;
            case 10:
            case 11:
                if (osMotorInit(&sBootSiQueue, &sBootMotorPfs[n], n) == 0) {
                    __osMotorAccess(&sBootMotorPfs[n], 0);
                } else if (osGbpakInit(&sBootSiQueue, &sBootGbPfs, n) != 0) {
                    bVar2 = 1;
                }
                break;
            }
        }
    }
    if (bVar1 != 0 || bVar2 != 0) {
        return 1;
    }
    return 0;
}

void boot_init_prenmi_queue(void) {
    osCreateMesgQueue(&sBootPreNmiQueue, sBootPreNmiMsgs, 8);
    osSetEventMesg(0xE, &sBootPreNmiQueue, 0);
}

void boot_check_reset(void)
{
    u32 buf[2];
    if (osRecvMesg(&sBootPreNmiQueue, buf, 0) == 0) {
        boot_mem_fill(sBootFramebuffer, 0, 0x25800);
        osWritebackDCache(0, 0x400000);
        while (1) {}
    }
}
