#include "ctx.h"

extern u32 D_800EFD70;
extern u32 D_800EFDBC;
void func_8006FE84(u8 *p, s32 a);

void func_80022A68(u8 *p)
{
    s32 st = *(u8 *)(p + 0x3FF);

    if (st == 1) goto arm1;
    if (st < 2) return;
    if (st == 2) goto arm2;
    return;

arm1:
    if (*(s8 *)(p + 0x3D0) == -1 && *(f32 *)(p + 0x420) < 8.0f && *(s32 *)(p + 0x744) < -30) {
        *(u8 *)(p + 0x3FF) = 2;
        func_8006FE84(p, *(s16 *)(p + 0x668) + 0x13);
        if ((D_800EFD70 & 0x8000) == 0) {
            *(u16 *)(p + 0x5AA) = 10;
        }
    }
    return;

arm2:
    if (*(s8 *)(p + 0x3D0) < 0) {
        *(u8 *)(p + 0x3FF) = 3;
        if (D_800EFDBC == 5) {
            *(u8 *)(p + 0x3FF) = 0;
            *(u8 *)(p + 0x6E8) = 1;
            *(u16 *)(p + 0x5AA) = *(u16 *)(p + 0x5A8);
        }
    }
}
