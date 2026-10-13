#include "ctx.h"

extern u16 D_800C94C0;
extern f32 D_800C94B8;
extern f32 D_800C94BC;
extern s32 D_800EFDBC;
extern void func_8001254C(s32 a, s32 b, s16 *c, s32 *d);

f32 func_8001E89C(u8 *p)
{
    struct { s16 h[2]; s32 w; } lo;
    f32 fVar2, fVar3, fVar4, fX, fR;

    if (*(s16 *)(p + 0x416) == 0) {
        if (*(f32 *)(p + 0x424) < 1.0f && *(u8 *)(p + 0x41B) == 0) {
            if ((*(u32 *)(p + 0x5B0) & 0x20) != 0) {
                *(s16 *)(p + 0x412) = (s16)(s32)((f32)(s32)*(s16 *)(p + 0x412) + *(f32 *)(p + 0x424) * 1066.6666f);
            } else {
                *(u16 *)(p + 0x412) = D_800C94C0;
            }
            if (*(u8 *)(p + 0x41A) < 0x10) *(u8 *)(p + 0x41A) += 2;
            if (*(u8 *)(p + 0x419) < 0x10) *(u8 *)(p + 0x419) += 2;
        } else {
            if (*(s32 *)(p + 0x1C) >= 0x1001 &&
                (*(u8 *)(p + 0x6E8) != 0 || (*(u32 *)(p + 0x5B0) & 0x10000) != 0 ||
                 *(u8 *)(p + 0x41B) != 0 || *(u8 *)(p + 0x41C) != 0 || *(f32 *)(p + 0x424) < 10.0f)) {
                if (*(u8 *)(p + 0x41A) < 0x10) *(u8 *)(p + 0x41A) += 2;
            } else {
                if (*(u8 *)(p + 0x41A) != 0) *(u8 *)(p + 0x41A) -= 1;
            }
            if (*(s32 *)(p + 0x1C) < -0x1000 &&
                (*(u8 *)(p + 0x6E8) != 0 || (*(u32 *)(p + 0x5B0) & 0x10000) != 0 ||
                 *(u8 *)(p + 0x41B) != 0 || *(u8 *)(p + 0x41C) == 1 || *(f32 *)(p + 0x424) < 10.0f)) {
                if (*(u8 *)(p + 0x419) < 0x10) *(u8 *)(p + 0x419) += 2;
            } else {
                if (*(u8 *)(p + 0x419) != 0) *(u8 *)(p + 0x419) -= 1;
            }
        }
    }
    func_8001254C((s32)(*(f32 *)(p + 0xA4) * 1000.0f), (s32)(*(f32 *)(p + 0xB0) * 1000.0f), lo.h, &lo.w);
    fVar4 = (f32)(s32)lo.h[0] * 0.001953125f;

    if (*(s32 *)(p + 0x744) < 0) {
        fVar3 = (f32)(s32)*(s8 *)(p + 0x5D0);
        fVar2 = *(f32 *)(p + 0x420);
        if (fVar2 < 0.0f) {
            fVar3 = 0.0f;
        } else if (fVar2 < 4.0f) {
            fVar3 = fVar3 * fVar2 * 0.25f;
        }
        if (*(s32 *)(p + 0x5C0) < 0) fVar3 = fVar3 * 0.6667f;
        if (*(s32 *)(p + 0x5C0) >= -9 && (fVar3 - fVar4) * fVar4 < 0.0f) {
            fVar3 = fVar4 + (fVar3 - fVar4) * 0.25f;
        }
    } else if (*(s8 *)(p + 0x3D0) == -1) {
        if (*(u8 *)(p + 0x6E8) != 0) {
            fVar3 = (f32)(s32)*(s8 *)(p + 0x5D0) * 0.25f;
        } else {
            fVar3 = (f32)(s32)*(s8 *)(p + 0x5D0) * 0.5f;
        }
    } else {
        fVar3 = 0.0f;
        if ((u8)(*(u8 *)(p + 0x3D0) - 0x10) < 2) {
            fVar3 = (f32)((((s32)((u32)*(u16 *)(p + 0x622) << 16)) >> 27) * 3);
        }
    }
    if (*(u8 *)(p + 0x6E9) != 0) fVar3 = 0.0f;
    fX = fVar3 - fVar4;
    if (12.0f < fX) fX = 12.0f;
    else if (fX < -12.0f) fX = -12.0f;

    if (*(u8 *)(p + 0x6E8) == 0) {
        fX = D_800C94B8 * fX - D_800C94BC * *(f32 *)(p + 0x438);
    } else {
        fX = fX * 0.015f - *(f32 *)(p + 0x438) * 0.1f;
    }
    fR = 0.0f;
    if (D_800EFDBC != 0xF) fR = fX;
    return fR;
}
