#include "ctx.h"

extern s32 D_800C9684;
extern f32 D_800E8020[];

f32 func_80013408(f32 *a, f32 *b);
s32 func_80013010(s32 x);
f32 func_80013098(f32 p1, f32 p2);

#define FLD(off) (*(f32 *)(p + (off)))

f32 func_800295E0(u8 *p)
{
    f32 fVar2, fVar3;
    f32 fVar4, r;
    f32 fVar5;
    f32 A[3];
    f32 B[3];
    s32 iVar1;
    f32 tt;



    if (*(s32 *)(p + 0x744) > 0 && 0.25f < FLD(0x6f4)) {
        fVar4 = 0.5f;
        A[0] = (FLD(0x6fc) + FLD(0x71c)) * fVar4 - FLD(0x6c);
        A[1] = (FLD(0x700) + FLD(0x720)) * fVar4 - FLD(0x70);
        A[2] = (FLD(0x704) + FLD(0x724)) * fVar4 - FLD(0x74);
        fVar2 = func_80013098(__builtin_sqrtf(A[0] * A[0] + A[2] * A[2]) + 1.0f, -A[1] - 1.0f);
        fVar2 = (fVar2 * 32768.0f) / 3.1415927f;
        if (FLD(0x58) < 0.0f) {
            B[0] = FLD(0x708) + FLD(0x728);
            B[1] = FLD(0x70c) + FLD(0x72c);
            B[2] = FLD(0x710) + FLD(0x730);
            fVar3 = func_80013408(B, A);
            r = fVar3 * fVar4;
            r = r / __builtin_sqrtf(A[0] * A[0] + A[1] * A[1] + A[2] * A[2]);
            if (-0.25f < r) {
                tt = r + 0.25f;
                iVar1 = func_80013010((s32)(tt * 16384.0f));
                fVar2 = fVar2 + (f32)(s32)(s16)iVar1;
            }
        }
        D_800E8020[D_800C9684] = fVar2;
    } else if (*(s32 *)(p + 0x744) >= -31) {
        fVar2 = D_800E8020[D_800C9684];
    } else {
        fVar2 = 2000.0f;
    }
    return fVar2;
}
