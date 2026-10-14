#include "game.h"

struct Blk_29 {
    s32 next;
    u8 b4, b5, b6, b7, b8, b9;
    u8 pad[2];
    u32 tag;
    u32 pad2;
    u8 buf[0x50];
};

Blk_29 *sram_init(void)
{
    u32 r;
    if (sSramPiHandle.tag != 0xA8000000) {
        sSramPiHandle.b4 = 3;
        sSramPiHandle.b5 = 5;
        sSramPiHandle.b8 = 0xC;
        sSramPiHandle.b6 = 0xD;
        sSramPiHandle.b7 = 2;
        sSramPiHandle.tag = 0xA8000000;
        sSramPiHandle.b9 = 1;
        mem_fill(sSramPiHandle.buf, 0, 0x60);
        r = __osDisableInt();
        sSramPiHandle.next = __osPiTable;
        __osPiTable = (s32)&sSramPiHandle;
        __osRestoreInt(r);
    }
    return &sSramPiHandle;
}

int sram_read(u32 param_1)
{
    s32 x;

    osInvalDCache(param_1, 0x8000);
    dma_start(sSramHandle, 0, 0x8000000, param_1, 0x8000);
    if (x < 0) {
        return 1;
    }
    dma_wait(x);
    return 0;
}

s32 sram_write(s32 p)
{
    s32 r;

    osWritebackDCache(p, 0x8000);
    dma_start(sSramHandle, 1, 0x08000000, p, 0x8000);
    if (r < 0) {
        return 1;
    }
    dma_wait(r);
    return 0;
}
