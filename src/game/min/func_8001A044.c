#include "ctx.h"

extern s32 *D_800C2354;

s32 *func_800BC908(u32 a0, u32 a1, char *a2, s32 a3);
s32 *func_800C1E60(s32 *a0, u32 a1, s32 a2, s32 a3, s32 a4);

void func_8001A044(void)
{
    s32 i;
    s32 *p;
    s32 off;

    D_800C2354 = func_800BC908(0x148, 0, "min.c", 0x4E3);
    i = 0;
    do {
        p = func_800C1E60(0, 0x40, 1, 2, 0x20);
        off = i * 4;
        *(s32 *)((u8 *)D_800C2354 + off) = (s32)p;
        i++;
    } while (i < 0x4A);
}
