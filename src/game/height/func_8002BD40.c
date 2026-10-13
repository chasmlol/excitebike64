#include "ctx.h"

typedef struct {
    u8 pad0[0x2E];
    u16 h2e;
    u8 pad1[0x3C];
    f32 f6c;
    f32 f70;
    f32 f74;
} Obj;

typedef struct {
    s32 w0;
    u16 h4;
    u16 h6;
    s32 w8;
    s32 w12;
    s32 w16;
    s16 s20;
    s16 s22;
} Track;

typedef struct {
    s16 h[2];
    s32 w;
} Tmp30;

typedef struct {
    u16 flags;
    u16 pad;
    Track *node;
} Slot;

extern Slot D_800E7FC8[];
extern s32 D_800C9684;

f32 func_8002BBD4(Obj *p, s32 a, s32 b);
void func_80011A58(s32 *p, s32 a, s32 b, s32 c);
void func_80012DC8(s32 *a, s32 *b, s32 *c);
void func_8001254C(s32 a, s32 b, s16 *c, s32 *d);
void func_8002B5F8(s32 *p);

void func_8002BD40(Obj *param_1, s32 *param_2)
{
    u16 uVar1;
    Track *piVar2;
    f32 fT;
    f32 fA;
    f32 fB;
    u32 uA;
    u32 uB;
    u32 sum4;
    s32 vS;
    s32 iVar5;
    s32 local_78[4];
    s32 local_68[3];
    s32 aiStack_58[10];
    Tmp30 t30;
    s16 local_28[4];

    uVar1 = D_800E7FC8[D_800C9684].flags;
    if (uVar1 & 4) {
        D_800E7FC8[D_800C9684].flags = uVar1 & 0xFFFB;
    }
    piVar2 = D_800E7FC8[D_800C9684].node;
    fT = func_8002BBD4(param_1, piVar2->s20, piVar2->s22);
    vS = piVar2->w0 + (s32)((f32)(piVar2->w8 - piVar2->w0) * fT);
    fA = (f32)piVar2->w12 * fT;
    uA = (u32)fA;
    fB = (f32)piVar2->w16 * fT;
    sum4 = piVar2->h4 + uA;
    uB = (u32)fB;
    func_80011A58(aiStack_58, (-((piVar2->h6 + uB) & 0xFFFF)) + 1000, 0,
                  (param_1->h2e + sum4) & 0xFFFF);
    local_78[0] = 0;
    local_78[1] = 0;
    local_78[2] = vS;
    func_80012DC8(local_78, aiStack_58, local_68);
    param_2[0] = (s32)(param_1->f6c * 1000.0f + (f32)local_68[0]);
    param_2[1] = (s32)(param_1->f70 * 1000.0f + (f32)local_68[1] + 1000.0f);
    iVar5 = (s32)(param_1->f74 * 1000.0f + (f32)local_68[2]);
    param_2[2] = iVar5;
    func_8001254C((s32)(param_1->f6c * 1000.0f - (f32)param_2[0]),
                  (s32)(param_1->f74 * 1000.0f - (f32)iVar5), t30.h, &t30.w);
    func_8001254C(t30.w, (s32)((param_1->f70 + 0.7f) * 1000.0f - (f32)param_2[1]), local_28, &t30.w);
    param_2[4] = 0;
    {
        s32 ta = local_28[0] + -0x4000;
        s32 tb = -t30.h[0];
        param_2[3] = ta;
        param_2[5] = tb;
    }
    func_8002B5F8(param_2);
}
