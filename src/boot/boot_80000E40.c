#include "boot.h"

/* The debug font, sBootFontBitmap: 8x8 characters from ' ', 16 to a row, in a bitmap of 128 one-byte pixels a row
   stored bottom up. None of the drawing functions here is called. */

/* Draws character `c` at (x, y) of the 320-pixel-wide framebuffer `fb` in `color` (its set pixels only). */
void boot_draw_char(s32 c, s32 x, s32 y, u16 *fb, s32 color)
{
    s32 i, j;
    u8 *src;
    u32 glyph = (u8)(c - 0x20);
    u32 col;
    i = (glyph >> 4) * 8;
    col = (glyph & 0xF) * 8;
    src = sBootFontBitmap + ((0x18 - i) * 128 + col);

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (src[(7 - i) * 128 + j]) {
                fb[(y + i) * BOOT_SCREEN_WIDTH + (x + j)] = color;
            }
        }
    }
}

/* Draws the string `s` from (x, y) of `fb` as boot_draw_char does, then writes back the data cache. */
void boot_draw_string(u8 *s, s32 x, s32 y, u16 *fb, s32 color)
{
    s32 i, j;
    u8 *src;
    u8 ch;
    u32 glyph;

    ch = *s;
    while (ch != 0) {
        glyph = (u8)(*s - 0x20);
        i = (glyph >> 4) * 8;
        j = (glyph & 0xF) * 8;
        src = sBootFontBitmap + ((0x18 - i) * 128 + j);

        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                if (src[(7 - i) * 128 + j]) {
                    fb[(y + i) * BOOT_SCREEN_WIDTH + (x + j)] = color;
                }
            }
        }
        s++;
        x += 8;
        ch = *s;
    }
    osWritebackDCacheAll();
}

/* Draws a progress bar from column `left` to `right` on the boot framebuffer: 8 rows from `top` shading from red to
   white, and 8 rows lower a 3-row edge from white back to red. Then writes back the data cache. */
void boot_draw_progress_bar(s32 left, s32 right, s32 top)
{
    s32 x, row, t, shade;
    u8 *fb;

    fb = (u8 *)BOOT_FRAMEBUFFER;
    for (x = left; x < right; x++) {
        for (row = 0; row < 8; row++) {
            shade = (row * 31) / 7;
            *(u16 *)(fb + ((((top + row) * BOOT_SCREEN_WIDTH) + x) * 2)) = (u16)((shade << 6) | 0xF800 | (shade << 1));
        }
        for (row = 0; row < 3; row++) {
            shade = ((2 - row) * 31) / 2;
            *(u16 *)(fb + 0x1400 + ((((top + row) * BOOT_SCREEN_WIDTH) + x) * 2))
                = (u16)((shade << 6) | 0xF800 | (shade << 1));
        }
    }
    osWritebackDCacheAll();
}

/* The container header, copied as a 48-byte block. */
typedef struct { u8 b[0x30]; } BootHeaderBytes;
typedef struct { s32 unpackedSize; s32 packedSize; } BootSegment;
/* The container header: its tag, the segments, then two words the boot loader fills in for the game. */
typedef struct {
    s32 tag0;               /* "BRSO" */
    s32 tag1;               /* "HYES" */
    BootSegment seg[4];
    u32 containerRom;       /* the container's ROM address */
    u32 resourceRom;        /* the ROM address after the segments: the game's resources (gResourceRomAddr) */
} BootHeader;

/* The boot loader's thread: sets up the VI (a PAL console hangs), shows the splash screen for BOOT_SPLASH_FRAMES
   frames, DMAs the BRSOHYES container and unpacks the game image's segments to BOOT_UNPACK_BUF, leaves the
   container header at BOOT_HEADER_ADDR, then runs the copy stub at BOOT_STUB_ADDR, which moves the game over the
   boot loader and jumps to its entry. boot_check_reset between the steps hangs once the reset button is pressed. */
void boot_thread_main(void)
{
    BootHeader hdr;
    s32 msg[2];
    s32 romOffset;
    s32 off;
    s32 *segPacked;
    s32 *hdrWords;
    u8 *container;
    s32 n;
    s32 x;
    s32 k, row, m;
    s32 *segUnpacked;
    s32 *pb;
    s32 *ps;
    s32 *pd;
    u8 *pixels;
    u16 *palette;
    u16 *fb;

    u32 s1;
    u32 ret;
    u32 r;
    u32 containerRom;
    u32 fp;
#if VERSION != VERSION_LODGENET
    s32 splash;
#endif

    sBootFramebuffer = BOOT_FRAMEBUFFER;
    sBootUnpackBuf = BOOT_UNPACK_BUF;
    boot_mem_fill((void *)BOOT_FRAMEBUFFER, 0, 0x28000);
    boot_init_prenmi_queue();
    boot_check_reset();
    osCreateViManager(10);
#if VERSION == VERSION_EU
    osViSetMode(&sBootViModePal);
    osViBlack(1);
    if (osTvType != OS_TV_PAL) {
        for (;;) ;
    }
#else
    if (osTvType == OS_TV_NTSC) {
        osViSetMode(&sBootViModeNtsc);
    } else if (osTvType == OS_TV_MPAL) {
        osViSetMode(&sBootViModeMpal);
    }
    osViBlack(1);
    if (osTvType == OS_TV_PAL) {
        for (;;) ;
    }
#endif
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF | OS_VI_DIVOT_OFF | OS_VI_DITHER_FILTER_OFF);
#if VERSION == VERSION_LODGENET
    n = 0;
#else
    /* Whether to draw the splash: not with START held on controller 1 (boot_init_controllers); 0xDEAF at
       0x80000406 tells the game to open the Controller Pak screen. */
    if (boot_init_controllers()) {
        splash = 0;
        *(u32 *)0x80000404 = (*(u32 *)0x80000404 & 0xFFFF0000) | 0xDEAF;
    } else {
        splash = 1;
    }
#endif
    boot_check_reset();
    /* The splash screen, drawn bottom up (0xEF is the last row). */
    gSwdLoadCur = (u32)sBootSplashSwd;
    gSwdLoadEnd = (u32)sBootSplashSwd + BOOT_SPLASH_SWD_SIZE;
    gSwdSaveCur = sBootUnpackBuf;
    gSwdSaveEnd = sBootUnpackBuf + 0x25800;
    boot_swd_expand();
    boot_check_reset();
    pixels = (u8 *)(sBootUnpackBuf + BOOT_SPLASH_HEADER_SIZE);
    palette = (u16 *)((u32)pixels + BOOT_SCREEN_PIXELS);
#if VERSION == VERSION_LODGENET
    fb = (u16 *)sBootFramebuffer;
    for (; n < BOOT_SCREEN_HEIGHT; n++) {
        x = 0;
        row = (0xEF - n) * BOOT_SCREEN_WIDTH;
        for (; x < BOOT_SCREEN_WIDTH; x++) {
            fb[row + x] = palette[pixels[n * BOOT_SCREEN_WIDTH + x]];
        }
    }
#else
    if (splash) {
        for (n = 0; n < BOOT_SCREEN_HEIGHT; n++) {
            x = 0;
            row = (0xEF - n) * BOOT_SCREEN_WIDTH;
            for (; x < BOOT_SCREEN_WIDTH; x++) {
                ((u16 *)sBootFramebuffer)[row + x] = palette[pixels[n * BOOT_SCREEN_WIDTH + x]];
            }
        }
    }
#endif
    osWritebackDCache(0, 0x800000);
    boot_check_reset();
    osViBlack(0);
    osViSwapBuffer_2(sBootFramebuffer);
    n = 0;
    osViBlack(n);
    osViSwapBuffer_2(sBootFramebuffer);
    osCreateMesgQueue(&sBootViQueue, sBootViMsgs, 8);
    osViSetEvent(&sBootViQueue, n, 1);
    boot_check_reset();
    do {
        osRecvMesg(&sBootViQueue, msg, OS_MESG_BLOCK);
        boot_check_reset();
        n++;
    } while (n < BOOT_SPLASH_FRAMES);
    /* The container; `n` is now the DMA request. */
    containerRom = (u32)gBootGameContainerRom;
    boot_dma_init();
    container = (u8 *)BOOT_CONTAINER_BUF;
    n = boot_dma_start(sBootCartHandle, OS_READ, containerRom, (u32)container, BOOT_CONTAINER_SIZE);
    do {
    } while (boot_dma_get_status(n) != BOOT_DMA_DONE);
    osInvalDCache(0, 0x400000);
    boot_check_reset();
    *(BootHeaderBytes *)&hdr = *(BootHeaderBytes *)container;
    sBootGameDst = BOOT_UNPACK_BUF;
    sBootGameSize = 0;
    sBootGameSrc = (u32)(container + BOOT_CONTAINER_HEADER_SIZE);
    hdrWords = (s32 *)&hdr;
    /* segPacked[2] is hdr.seg[n].packedSize, segUnpacked[2] hdr.seg[n].unpackedSize. */
    for (n = 0; n < BOOT_SEGMENT_COUNT; n++) {
        segPacked = (s32 *)((u8 *)hdrWords + (n * 8 + 4));
        segUnpacked = hdrWords + n * 2;
        gSwdLoadCur = sBootGameSrc;
        gSwdLoadEnd = sBootGameSrc + segPacked[2];
        gSwdSaveCur = sBootGameDst;
        gSwdSaveEnd = sBootGameDst + segUnpacked[2];
        boot_swd_expand();
        sBootGameSrc += segPacked[2];
        sBootGameDst += segUnpacked[2];
        sBootGameSize += segUnpacked[2];
    }
    boot_check_reset();
    boot_relocate_stub((u32)gBootStubStart, (u32)gBootStubEnd - (u32)gBootStubStart, BOOT_STUB_ADDR);
    romOffset = hdr.seg[0].packedSize + BOOT_CONTAINER_HEADER_SIZE;
    hdr.containerRom = containerRom;
    hdr.resourceRom = containerRom + romOffset + hdr.seg[1].packedSize + hdr.seg[2].packedSize + hdr.seg[3].packedSize;
    *(BootHeaderBytes *)BOOT_HEADER_ADDR = *(BootHeaderBytes *)&hdr;
    /* The parameters of the stub's relocated copy. */
    *(u32 *)((u32)gBootStubSrc + sBootStubDelta) = BOOT_GAME_COPY_SRC;
    *(u32 *)((u32)gBootStubDst + sBootStubDelta) = BOOT_GAME_COPY_DST;
    *(u32 *)((u32)gBootStubSize + sBootStubDelta) = sBootGameSize;
    *(u32 *)((u32)gBootStubEntry + sBootStubDelta) = BOOT_GAME_ENTRY;
    boot_check_reset();
    sBootStubFunc = (void (*)(void))BOOT_STUB_ADDR;
    __osDisableInt();
    sBootStubFunc();
}

/* Copies the copy stub (`size` bytes at `from`) to `dst`, relocating it by the distance it moves (sBootStubDelta):
   the targets of j and jal, except a jump to the entry (0x80000400), and the addresses of lui + lw/sw and lui + ori
   pairs. Then writes back and invalidates the caches. `i` counts bytes, but a pair adds only 1 for its second
   word. */
void boot_relocate_stub(u32 *from, u32 size, u32 *dst)
{
    u32 *src = from;
    u32 i;
    u32 insn, op, addr, next;

    sBootStubDelta = (u32)dst - (u32)from;
    for (i = 0; i < size; src++, i += 4, dst++) {
        insn = *src;
        op = insn & MIPS_OP_MASK;
        if (op == MIPS_OP_J || op == MIPS_OP_JAL) {
            addr = insn & MIPS_TARGET_MASK;
            addr <<= 2;
            if (addr != 0x400) {
                addr += sBootStubDelta;
            }
            addr >>= 2;
            addr &= MIPS_TARGET_MASK;
            insn = op | addr;
            *dst = insn;
        } else if ((insn & MIPS_LUI_MASK) == MIPS_OP_LUI) {
            next = src[1];
            op = next & MIPS_OP_MASK;
            if (op == MIPS_OP_LW || op == MIPS_OP_SW) {
                /* The load's offset is signed: the upper half goes up by one when the lower half is negative. */
                addr = insn << 16;
                addr += (s16)src[1];
                addr += sBootStubDelta;
                insn = (insn & 0xFFFF0000) | (u16)(addr >> 16);
                if (addr & 0x8000) {
                    insn += 0x10000;
                }
                *dst++ = insn;
                *dst = (src[1] & 0xFFFF0000) | (addr & 0xFFFF);
                src++;
                i++;
            } else if (op == MIPS_OP_ORI) {
                addr = insn << 16;
                addr |= next & 0xFFFF;
                addr += sBootStubDelta;
                insn = (insn & 0xFFFF0000) | (u16)(addr >> 16);
                *dst++ = insn;
                *dst = (src[1] & 0xFFFF0000) | (addr & 0xFFFF);
                src++;
                i++;
            } else {
                *dst = insn;
            }
        } else {
            *dst = insn;
        }
    }
    osWritebackDCache(0, 0x800000);
    osInvalDCache(0, 0x800000);
    osInvalICache(0, 0x800000);
}

/* Probes the controllers and their accessories: a Controller Pak, else a Rumble Pak (stopped), else a Game Boy Pak.
   Returns 1 when controller 1 holds START or an accessory is none of these, else 0. Hangs if the controllers cannot
   be set up. LodgeNet never calls it; retail's boot_thread_main does. */
s32 boot_init_controllers(void)
{
    s32 chan;
    s32 startHeld;
    s32 unknownPak;
    s32 pakResult;
    s32 ret;
    u32 msgBuf[2];
    u32 *msg;

    osCreateMesgQueue(&sBootSiQueue, sBootSiMsgs, 8);
    osSetEventMesg(OS_EVENT_SI, &sBootSiQueue, 0);
    if (osContInit(&sBootSiQueue, sBootContPattern, sBootContStatus) != 0) {
        while (1) {}
    }
    osContStartQuery(&sBootSiQueue);
    msg = msgBuf;
    osRecvMesg(&sBootSiQueue, msg, OS_MESG_BLOCK);
    osContGetQuery(sBootContStatus);
    osContStartReadData(&sBootSiQueue);
    osRecvMesg(&sBootSiQueue, msg, OS_MESG_BLOCK);
    osContGetReadData(sBootContPads);
    startHeld = 0;
    unknownPak = 0;
    for (chan = 0; chan < MAXCONTROLLERS; chan++) {
        if (sBootContStatus[chan].errno == 0) {
            if ((sBootContPads[chan].button & START_BUTTON) && chan == 0) {
                startHeld = 1;
            }
            /* pakResult keeps the result, unused. */
            switch (osPfsInitPak(&sBootSiQueue, &sBootPfs[chan], chan)) {
            case 0: pakResult = 0; break;
            case 1: pakResult = 1; break;
            case 2: pakResult = 2; break;
            case 3: pakResult = 3; break;
            case 4: pakResult = 4; break;
            case 5: pakResult = 5; break;
            case 6: pakResult = 6; break;
            case 7: pakResult = 7; break;
            case 8: pakResult = 8; break;
            case 9: pakResult = 9; break;
            case PFS_ERR_ID_FATAL:
            case PFS_ERR_DEVICE:
                if (osMotorInit(&sBootSiQueue, &sBootMotorPfs[chan], chan) == 0) {
                    __osMotorAccess(&sBootMotorPfs[chan], 0);
                } else if (osGbpakInit(&sBootSiQueue, &sBootGbPfs, chan) != 0) {
                    unknownPak = 1;
                }
                break;
            }
        }
    }
    if (startHeld != 0 || unknownPak != 0) {
        return 1;
    }
    return 0;
}

/* Has PRE-NMI (the reset button) sent to sBootPreNmiQueue. */
void boot_init_prenmi_queue(void)
{
    osCreateMesgQueue(&sBootPreNmiQueue, sBootPreNmiMsgs, 8);
    osSetEventMesg(OS_EVENT_PRENMI, &sBootPreNmiQueue, 0);
}

/* Once the reset button has been pressed, clears the screen and hangs. */
void boot_check_reset(void)
{
    u32 msg[2];
    if (osRecvMesg(&sBootPreNmiQueue, msg, OS_MESG_NOBLOCK) == 0) {
        boot_mem_fill(sBootFramebuffer, 0, BOOT_FB_SIZE);
        osWritebackDCache(0, 0x400000);
        while (1) {}
    }
}
