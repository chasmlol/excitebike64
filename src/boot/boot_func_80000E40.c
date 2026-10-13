#include "ctx.h"

extern u8 D_8000CE10[];

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

