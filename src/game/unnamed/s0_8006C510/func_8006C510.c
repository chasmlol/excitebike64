#include "ctx.h"

extern u8 D_800F1540[];
extern f32 D_800F1510[];
extern u8 D_800F16E8[];
extern s32 D_800C2594;
extern s32 D_800F18F0;

void func_80013650(s32 *ab, f32 *src);
void func_800139A8(s32 *src, u16 *aux, s16 *dst);
void func_80011490(s16 *dst, s32 a1, s32 a2, s32 a3, s32 x, s32 y, s32 z, s32 a7);
void func_800120A4(s16 *src, s16 *dst, u32 tbl);
Gfx *func_80002418(Gfx *g, s32 base, s32 i);

Gfx *func_8006C510(Gfx *param_1, s32 param_2, s32 param_3)
{
    s32 aiStack_e0[10];
    s32 aux[3];
    s16 asStack_a8[32];
    s16 auStack_68[32];
    f32 *q;
    f32 fx;
    s32 i;
    s32 iVar5;
    s32 tbl;
    s32 iVar1;
    Gfx *gA, *gB, *gC, *gD, *gE, *gF;
    Gfx *gP, *gQ, *gT, *gU, *gV;

    func_80013650(aiStack_e0, D_800F1510 + 12);
    aux[0] = 0;
    aux[1] = 0;
    aux[2] = 0;
    func_800139A8(aiStack_e0, (u16 *)aux, auStack_68);
    func_80011490(asStack_a8, 0, 0, 0, (s32)(D_800F1510[0] * 1000.0f), (s32)(D_800F1510[1] * 1000.0f),
                  (s32)(D_800F1510[2] * 1000.0f), 8);
    tbl = (u32)D_800F16E8 + (param_3 << 8);
    func_800120A4(auStack_68, asStack_a8, tbl);

    gA = param_1++;
    ((u32 *)gA)[0] = 0xDA380003;
    ((u32 *)gA)[1] = tbl;
    gB = param_1++;
    ((u32 *)gB)[0] = 0xE7000000;
    ((u32 *)gB)[1] = 0;
    gC = param_1++;
    ((u32 *)gC)[0] = 0xD9FEFFFF;
    ((u32 *)gC)[1] = 0;
    gD = param_1++;
    ((u32 *)gD)[0] = 0xE200001C;
    ((u32 *)gD)[1] = 0x442230;
    gE = param_1++;
    ((u32 *)gE)[0] = 0xFCFFFFFF;
    ((u32 *)gE)[1] = 0xFFFCF279;
    gF = param_1++;
    ((u32 *)gF)[0] = 0xE7000000;
    ((u32 *)gF)[1] = 0;

    for (i = 0; i < D_800C2594; i++) {
        param_1 = func_80002418(param_1, param_2, i);
        gP = param_1++;
        gQ = param_1++;
        ((u32 *)gP)[0] = 0xDA380007;
        ((u32 *)gP)[1] = param_2 + i * 0x40 + 0x80000000;
        ((u32 *)gQ)[0] = 0xDE000000;
        ((u32 *)gQ)[1] = D_800F18F0;
    }

    gT = param_1++;
    ((u32 *)gT)[0] = 0xE3001001;
    ((u32 *)gT)[1] = 0x8000;
    gU = param_1++;
    ((u32 *)gU)[0] = 0xE200001C;
    ((u32 *)gU)[1] = 0x442230;
    gV = param_1++;
    ((u32 *)gV)[0] = 0xFCFFFFFF;
    ((u32 *)gV)[1] = 0xFFFCF279;
    return param_1;
}
