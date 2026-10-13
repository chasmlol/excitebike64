#include "ctx.h"

extern u8 D_800EFD70[];
extern void func_8005FE10(s32);

void func_80019EC0(s32 param_1)
{
    u8 *b;
    u8 *gb;
    s32 s2;
    s32 s3;
    u8 s4;
    u8 s5;
    s32 idx;

    if (param_1 != 0) {
        gb = (u8 *)0x800E0000;
        __asm__ __volatile__("" : "+r"(gb));
        b = D_800EFD70;
        s2 = *(s32 *)(b + 0x1c);
        __asm__ __volatile__("" : "+r"(s2) : : "memory");
        *(s32 *)(gb + 0x3700) = -1;
        s3 = *(s8 *)(b + 0x2c8);
        s4 = *(u8 *)(b + 0x43c);
        s5 = *(u8 *)(b + 0x7aa);
        func_8005FE10(param_1);
        idx = *(s32 *)(gb + 0x3700) ^ 1;
        *(s32 *)(b + 0x1c + idx * 4) = s2;
        *(s8 *)(b + 0x2c8 + idx) = s3;
        *(u8 *)(b + 0x43c + (*(s32 *)(gb + 0x3700) ^ 1)) = s4;
        *(u8 *)(b + 0x7aa + (*(s32 *)(gb + 0x3700) ^ 1)) = s5;
        *(u8 *)(b + 0x2c8 + *(s32 *)(gb + 0x3700)) = 0;
    }
}
