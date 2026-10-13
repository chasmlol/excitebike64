#include "ctx.h"

extern u8 D_8000CE10[];
void boot_func_80006D60(void);

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
    boot_func_80006D60();
}
