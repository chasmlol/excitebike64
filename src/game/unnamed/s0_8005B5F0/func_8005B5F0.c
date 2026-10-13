#include "ctx.h"

extern s32 D_800EE490;
extern u8 *D_800EE498;
extern s32 D_800EFDBC;
extern s32 D_800E76D0;

void func_8005B5F0(u8 *p)
{
    s32 iVar1;
    f32 fVar2;
    f32 fVar3;
    u32 ev;
    u8 b;

    iVar1 = *(s16 *)(p + 0x628) + 1;
    fVar2 = *(f32 *)(D_800EE498 + *(s16 *)(p + 0x628) * 0x6C + 0x10) * (1.0f - *(f32 *)(p + 0x63C))
          + *(f32 *)(D_800EE498 + (iVar1 % D_800EE490) * 0x6C + 0x10) * *(f32 *)(p + 0x63C);
    ev = D_800EFDBC;
    if (ev == 10) goto L_e4;
    if (ev < 11) {
        if (ev == 7) goto L_e4;
    } else if (ev == 0xF) {
        fVar2 = 50.0f;
    }
    goto L_gen;

L_e4:
    if (!(0.5f < *(f32 *)(p + 0x424))) goto ZERO;
STORE20:
    *(s32 *)(p + 0x5B0) = 0x20;
    return;
ZERO:
    *(s32 *)(p + 0x5B0) = 0;
    return;

L_gen:
    b = *(u8 *)(p + 0x3FF);
    if (b == 1) goto C1;
    if (b == 2) goto C2;
    goto CS;
C1:
    fVar2 = 0.0f;
    goto CS;
C2:
    fVar2 = 5.0f;
CS:
    fVar3 = *(f32 *)(p + 0x420) - fVar2;
    if (0.5f < fVar3) {
        *(s16 *)(p + 0x55C) = 6;
        *(s16 *)(p + 0x4B8) = 6;
        goto STORE20;
    }
    if (fVar3 < -0.5f && *(s32 *)(p + 0x744) < 3 && *(u8 *)(p + 0x7A6) == 0) {
        if (fVar3 < -5.0f && *(f32 *)(p + 0x648) < 9500.0f) {
            if ((*(u32 *)(p + 0x5B4) & 0x20000) == 0) {
                *(s32 *)(p + 0x5B8) = D_800E76D0;
            }
            *(s32 *)(p + 0x5B0) = 0x20000;
            return;
        }
        *(s32 *)(p + 0x5B0) = 0x10;
    }
    return;

}
