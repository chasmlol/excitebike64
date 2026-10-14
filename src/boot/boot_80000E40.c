#include "boot.h"

void boot_func_80000E40(s32 c, s32 x, s32 y, u16 *fb, s32 color)
{
    s32 i, j;
    u8 *src;
    u32 v = (u8)(c - 0x20);
    u32 col;
    i = (v >> 4) * 8;
    col = (v & 0xF) * 8;
    src = D_8000CE10 + ((0x18 - i) * 128 + col);

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (src[(7 - i) * 128 + j]) {
                fb[(y + i) * 320 + (x + j)] = color;
            }
        }
    }
}

void boot_func_80000EEC(u8 *s, s32 x, s32 y, u16 *fb, s32 color)
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
        src = D_8000CE10 + ((0x18 - i) * 128 + j);

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

void boot_func_800010D0(void)
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

    D_80017CA8 = 0x80200000;
    D_800180FC = 0x80300000;
    boot_func_80000704((void *)0x80200000, 0, 0x28000);
    boot_init_prenmi_queue();
    boot_func_80001A6C();
    osCreateViManager(10);
    if (osTvType == 1) {
        osViSetMode(&D_8000FF30);
    } else if (osTvType == 2) {
        osViSetMode(&D_800107F0);
    }
    osViBlack(1);
    if (osTvType == 0) {
        for (;;) ;
    }
    osViSetSpecialFeatures(0xAA);
    y = 0;
    boot_func_80001A6C();
    D_800113B4 = (u32)D_8000DE10;
    D_800113B8 = (u32)D_8000DE10 + 0x1688;
    D_800113BC = D_800180FC;
    D_800113C0 = D_800180FC + 0x25800;
    func_800113E4();
    boot_func_80001A6C();
    src = (u8 *)(D_800180FC + 0x28);
    pal = (u16 *)((u32)src + 0x12C00);
    dst = (u16 *)D_80017CA8;
    for (; y < 0xF0; y++) {
        j = 0;
        d = (0xEF - y) * 0x140;
        for (; j < 0x140; j++) {
            dst[d + j] = pal[src[y * 0x140 + j]];
        }
    }
    osWritebackDCache(0, 0x800000);
    boot_func_80001A6C();
    osViBlack(0);
    osViSwapBuffer_2(D_80017CA8);
    y = 0;
    osViBlack(y);
    osViSwapBuffer_2(D_80017CA8);
    osCreateMesgQueue(&D_80017C28, D_80017E98, 8);
    osViSetEvent(&D_80017C28, y, 1);
    boot_func_80001A6C();
    do {
        osRecvMesg(&D_80017C28, loc, 1);
        boot_func_80001A6C();
        y++;
    } while (y < 0x78);
    t4 = (u32)D_B0012200;
    boot_func_80000C80();
    buf = (u8 *)0x80100000;
    y = boot_func_80000A30(D_80017C04, 0, t4, (u32)buf, 0xAF000);
    do {
    } while (boot_func_80000D28(y) != 3);
    osInvalDCache(0, 0x400000);
    boot_func_80001A6C();
    *(Blk48 *)&hdr = *(Blk48 *)buf;
    D_80017E90 = 0x80300000;
    D_80017CAC = 0;
    D_800180F8 = (u32)(buf + 0x28);
    base = (s32 *)&hdr;
    for (y = 0; y < 4; y++) {
        q = (s32 *)((u8 *)base + (y * 8 + 4));
        pa = base + y * 2;
        D_800113B4 = D_800180F8;
        D_800113B8 = D_800180F8 + q[2];
        D_800113BC = D_80017E90;
        D_800113C0 = D_80017E90 + pa[2];
        func_800113E4();
        D_800180F8 += q[2];
        D_80017E90 += pa[2];
        D_80017CAC += pa[2];
    }
    boot_func_80001A6C();
    boot_func_80001660((u32)D_80011584, (u32)D_800115F8 - (u32)D_80011584, 0x802F0000);
    n = hdr.seg[0].b + 0x28;
    hdr.rom = t4;
    hdr.sum = t4 + n + hdr.seg[1].b + hdr.seg[2].b + hdr.seg[3].b;
    *(Blk48 *)0x803FFFC0 = *(Blk48 *)&hdr;
    *(u32 *)((u32)D_800115E4 + D_800180F0) = 0x80300008;
    *(u32 *)((u32)D_800115EC + D_800180F0) = 0x80000408;
    *(u32 *)((u32)D_800115E8 + D_800180F0) = D_80017CAC;
    *(u32 *)((u32)D_800115F0 + D_800180F0) = 0x80000400;
    boot_func_80001A6C();
    D_80017EB8 = (void (*)(void))0x802F0000;
    __osDisableInt();
    D_80017EB8();
}

void boot_func_80001660(u32 *from, u32 size, u32 *dst)
{
    u32 *src = from;
    u32 i;
    u32 w, op, t, nx;

    D_800180F0 = (u32)dst - (u32)from;
    for (i = 0; i < size; src++, i += 4, dst++) {
        w = *src;
        op = w & 0xFC000000;
        if (op == 0x08000000 || op == 0x0C000000) {
            t = w & 0x3FFFFFF;
            t <<= 2;
            if (t != 0x400) {
                t += D_800180F0;
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
                t += D_800180F0;
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
                t += D_800180F0;
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

s32 boot_func_8000182C(void)
{
    s32 n;
    s32 bVar1;
    s32 bVar2;
    s32 dd;
    s32 ret;
    u32 buf[2];
    u32 *bp;

    osCreateMesgQueue(&D_80017E78, D_800180D0, 8);
    osSetEventMesg(5, &D_80017E78, 0);
    if (osContInit(&D_80017E78, D_80017E94, D_80017CB0) != 0) {
        while (1) {}
    }
    osContStartQuery(&D_80017E78);
    bp = buf;
    osRecvMesg(&D_80017E78, bp, 1);
    osContGetQuery(D_80017CB0);
    osContStartReadData(&D_80017E78);
    osRecvMesg(&D_80017E78, bp, 1);
    osContGetReadData(D_80017C10);
    bVar1 = 0;
    bVar2 = 0;
    for (n = 0; n < 4; n++) {
        if (D_80017CB0[n].errno == 0) {
            if ((D_80017C10[n].button & 0x1000) && n == 0) {
                bVar1 = 1;
            }
            switch (osPfsInitPak(&D_80017E78, &D_80017CC0[n], n)) {
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
                if (osMotorInit(&D_80017E78, &D_80017F10[n], n) == 0) {
                    __osMotorAccess(&D_80017F10[n], 0);
                } else if (osGbpakInit(&D_80017E78, &D_80017C40, n) != 0) {
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
    osCreateMesgQueue(&D_80017E60, D_800180B0, 8);
    osSetEventMesg(0xE, &D_80017E60, 0);
}

void boot_func_80001A6C(void)
{
    u32 buf[2];
    if (osRecvMesg(&D_80017E60, buf, 0) == 0) {
        boot_func_80000704(D_80017CA8, 0, 0x25800);
        osWritebackDCache(0, 0x400000);
        while (1) {}
    }
}
