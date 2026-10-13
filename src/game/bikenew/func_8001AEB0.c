#include "ctx.h"

#define FLD(p, off) (*(f32 *)((u8 *)(p) + (off)))

typedef struct {
    f32 f[10];
} Rec28;

extern Rec28 D_800C8E98[];
extern f32 D_800C8DF8[];
extern s32 D_800EFDBC;

void func_8001AEB0(u8 *a0, s32 a1)
{
    if (a0[0x6E8] == 0) {
        FLD(a0, 0x1BC) = 250.0f;
        FLD(a0, 0x7B8) = D_800C8E98[a1].f[0];
        FLD(a0, 0x7BC) = D_800C8E98[a1].f[1] * (D_800C8DF8[14] * D_800C8DF8[5]) * 32.0f * FLD(a0, 0x7D0);
        FLD(a0, 0x7C4) = D_800C8E98[a1].f[2] * D_800C8DF8[12] * 3.1415927f * 0.00390625f;
        FLD(a0, 0x7C8) = D_800C8E98[a1].f[3] / 48.0f;
        FLD(a0, 0x7D0) = D_800C8E98[a1].f[4] * 0.57f;
        FLD(a0, 0x7C0) = D_800C8E98[a1].f[5];
        FLD(a0, 0x7B4) = D_800C8E98[a1].f[6];
        FLD(a0, 0x7D4) = D_800C8E98[a1].f[7];
        FLD(a0, 0x7D8) = D_800C8E98[a1].f[8];
        return;
    }
    FLD(a0, 0x1BC) = 250.0f;
    FLD(a0, 0x7D4) = 0.3f;
    FLD(a0, 0x7B8) = 1.0f;
    FLD(a0, 0x7C0) = 1.0f;
    {
        f32 k57 = 0.57f;
        f32 t = D_800C8DF8[12] * 3.1415927f * 0.00390625f;
        FLD(a0, 0x7D0) = k57;
        FLD(a0, 0x7C8) = 0.020833334f;
        FLD(a0, 0x7C4) = t;
        FLD(a0, 0x7BC) = D_800C8DF8[14] * D_800C8DF8[5] * 32.0f * k57;
    }
    if (D_800EFDBC == 0xF) {
        FLD(a0, 0x7D8) = 0.7f;
    } else {
        FLD(a0, 0x7D8) = 1.0f;
    }
    FLD(a0, 0x7B4) = D_800C8E98[a1].f[6];
}
