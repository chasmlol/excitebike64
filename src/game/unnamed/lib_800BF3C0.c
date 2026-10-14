#include "game.h"

struct Blk_29 {
    s32 next;
    u8 b4, b5, b6, b7, b8, b9;
    u8 pad[2];
    u32 tag;
    u32 pad2;
    u8 buf[0x50];
};

Blk_29 *func_800BF3C0(void)
{
    u32 r;
    if (D_80100A30.tag != 0xA8000000) {
        D_80100A30.b4 = 3;
        D_80100A30.b5 = 5;
        D_80100A30.b8 = 0xC;
        D_80100A30.b6 = 0xD;
        D_80100A30.b7 = 2;
        D_80100A30.tag = 0xA8000000;
        D_80100A30.b9 = 1;
        func_8000ED10(D_80100A30.buf, 0, 0x60);
        r = __osDisableInt();
        D_80100A30.next = __osPiTable;
        __osPiTable = (s32)&D_80100A30;
        __osRestoreInt(r);
    }
    return &D_80100A30;
}

int func_800BF460(u32 param_1)
{
    s32 x;

    osInvalDCache(param_1, 0x8000);
    func_800BEC20(D_80100700, 0, 0x8000000, param_1, 0x8000);
    if (x < 0) {
        return 1;
    }
    func_800BEFB4(x);
    return 0;
}

s32 func_800BF4C4(s32 p)
{
    s32 r;

    osWritebackDCache(p, 0x8000);
    func_800BEC20(D_80100700, 1, 0x08000000, p, 0x8000);
    if (r < 0) {
        return 1;
    }
    func_800BEFB4(r);
    return 0;
}
