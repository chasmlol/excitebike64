#include "ctx.h"

typedef struct {
    u8 pad0[0x6C];
    f32 fA;
    u8 pad1[4];
    f32 fB;
    u8 pad2[0x7E0 - 0x78];
} Rec37;

extern s32 D_800E8090[];
extern s32 D_800E8508;
extern Rec37 D_800E37C0[];
extern s32 D_800E7978[];

s32 func_8002E920(s32 a0, s32 a1, s32 a2);
f32 func_800AEA30(f32 x);
f32 func_800ADFC0(f32 x);
void func_8001254C(s32 a, s32 b, s16 *c, s32 *d);
f32 sqrtf(f32 x);

void func_8002D1F0(s32 param_1, s32 param_2)
{
    u32 bp = (u32)D_800E8090;
    s32 off = param_2 * 12;
    s32 *t = (s32 *)(off + (s32)bp);
    s32 iVar1;
    f32 r;
    f32 fVar2, fVar4;
    f32 fVar3;
    f32 dz;
    s32 bw;
    s32 iStack2c;

    iVar1 = func_8002E920(t[0], t[1] + 0x1F4, t[2]);
    iVar1 = iVar1 + 200;
    if (D_800E8508 != 0 && t[1] < iVar1) {
        fVar3 = D_800E37C0[param_1].fA * 1000.0f - (f32)t[0];
        dz = D_800E37C0[param_1].fB * 1000.0f - (f32)t[2];
        r = sqrtf(fVar3 * fVar3 + dz * dz);
        fVar4 = (f32)D_800E7978[param_2] * 3.1415925f * 3.0517578e-05f;
        fVar3 = r;
        fVar2 = func_800AEA30(fVar4);
        fVar4 = func_800ADFC0(fVar4);
        fVar4 = (fVar2 / fVar4) * fVar3 - (f32)(t[1] - iVar1);
        func_8001254C((s32)fVar4, (s32)fVar3, (s16 *)&bw, &iStack2c);
        D_800E7978[param_2] = (s32)((s16 *)&bw)[0];
        t[1] = iVar1;
    }
}
