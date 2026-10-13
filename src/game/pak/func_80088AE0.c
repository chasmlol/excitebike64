#include "ctx.h"

extern s32 D_800F4A08;
extern u8 D_800F4A5C;
extern s32 D_800E2004;
extern s32 D_800F4A78;
extern s32 D_800F4A7C;
extern s32 D_800F4A04;
extern s32 D_800F483C;
extern s32 D_800F4AF0;
extern s32 D_800F4A28[];
extern s32 D_800F47C0;
extern s32 D_800F49F8;
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern s16 D_800F4868[];
extern u8 D_800F4A88[];
extern s32 D_800F4A20;
extern s32 D_800F49F0;
extern s32 D_800F49F4;
extern s32 D_800F4A60;
extern s32 D_800F4A58;
extern s32 D_800DA710;
extern s32 D_800D13A0;
extern s32 D_800D1488;
extern s32 D_800D14A8;
extern s32 D_800D1418;
extern s32 D_800D1434;
extern s32 *D_800CE164;

void func_8008C6D0(void);
void func_800AE240(s32 *p, f32 a, f32 b, f32 c, f32 d, f32 e, f32 f, f32 g);
s32 func_800BF79C(s32 a, s32 b);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);

void func_80088AE0(void)
{
    s32 *g;
    s32 wA, hA, wB, hB, wC, wD, hC, hD;
    f32 fa, fb, fc, fd;

    D_800F4A08 = 0;
    D_800F4A5C = 0;
    D_800E2004 = 0;
    D_800F4A78 = -1;
    D_800F4A7C = -1;
    D_800F4A04 = 0;
    D_800F483C = -1;
    D_800F4AF0 = 0;
    D_800F4A28[1] = 0;
    D_800F4A28[2] = 0;
    D_800F4A28[3] = 0;
    D_800F4A28[4] = 0;
    D_800F4A28[5] = 0;
    D_800F4A28[6] = 0;
    g = &D_800F49F8;
    *g = D_800F47C0;
    func_8008C6D0();
    wA = *(volatile s32 *)&D_800F13EC;
    hA = *(volatile s32 *)&D_800F13F0;
    wB = *(volatile s32 *)&D_800F13EC;
    hB = *(volatile s32 *)&D_800F13F0;
    wC = *(volatile s32 *)&D_800F13EC;
    fa = (f32)-(wC / 2);
    wD = *(volatile s32 *)&D_800F13EC;
    fb = (f32)(wD / 2);
    hC = *(volatile s32 *)&D_800F13F0;
    fc = (f32)-(hC / 2);
    hD = *(volatile s32 *)&D_800F13F0;
    fd = (f32)(hD / 2);
    D_800F4868[0] = (s16)(wA << 1);
    D_800F4868[1] = (s16)(hA << 1);
    D_800F4868[4] = (s16)(wB << 1);
    D_800F4868[5] = (s16)(hB << 1);
    func_800AE240((s32 *)D_800F4A88, fa, fb, fc, fd, 50.0f, 16384.0f, 1.0f);
    D_800F4A20 = func_800BF79C(*g, D_800D13A0);
    D_800F49F0 = func_800BF79C(*g, D_800D1488);
    D_800F49F4 = func_800BF79C(*g, D_800D14A8);
    D_800F4A60 = func_800BF79C(*g, D_800D1418);
    D_800F4A58 = func_800BF79C(*g, D_800D1434);
    D_800CE164 = func_800C1E60((s32 *)0, 0x80, 1, 4, 0);
}
