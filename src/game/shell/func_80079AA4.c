#include "ctx.h"

typedef struct {
    u8 pad[0x444];
    u16 *buf;
} Ctx;

extern Ctx D_800EFD70;
extern u8 D_800D94B0[];

void func_80079AA4(void)
{
    s32 i;
    s32 v;

    D_800EFD70.buf = (u16 *)func_800BC908(0x800, 0, D_800D94B0, 0x174F);
    D_800EFD70.buf[0] = 0;
    D_800EFD70.buf[0x100] = 0;
    D_800EFD70.buf[0x200] = 0;
    D_800EFD70.buf[0x300] = 0;
    i = 1;
    do {
        D_800EFD70.buf[i] = ((i >> 3) << 1) | 0x18C1;
        v = i >> 3;
        D_800EFD70.buf[i + 0x100] = (v << 11) | 0xC7;
        D_800EFD70.buf[i + 0x200] = (v << 11) | (v << 6) | 7;
        D_800EFD70.buf[i + 0x300] = (v << 6) | 0x1807;
        i++;
    } while (i < 0x100);
}
