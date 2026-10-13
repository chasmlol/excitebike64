#include "ctx.h"

extern u32 D_800D5604;
extern u32 D_800E32A0;
extern u8 D_800C04EC[];
extern u8 D_800D563C[];
extern u32 D_800FE860;
extern u8 D_800E32A4[];
extern u8 D_800E35C8[];
extern u32 D_800D5710;
extern u32 D_800D5730;
extern u32 D_800C2B5C;
extern u32 D_800C2B60;
extern u32 D_800C29E0;

void func_800BBAB0(s32 a0);
void func_800BBABC(s32 a, s32 b);
void func_800B8B0C();
void func_800B8B34(s32 a, s32 b, s32 c, s32 d, void *e, void *f);
void func_800B99F0(s32 a);
void func_800B9A18(s32 a);
void func_800110DC(void);
void func_80010FC8(void);
void func_8004B710(s32 a);

void func_8000F060(void)
{
    s32 l[4];

    D_800D5604 = 0;
    D_800E32A0 = 0x100;
    func_800BBAB0((s32)D_800C04EC);
    D_800FE860 = (u32)D_800D563C;
    func_800B8B0C(&l[0], &l[1], &l[2], &l[3]);
    {
        s32 p0, p1, p2, p3;
        p0 = l[0];
        p1 = l[1];
        p2 = l[2];
        p3 = l[3];
        func_800B8B34(p0, p1, p2, p3, (void *)D_800E32A4, (void *)D_800E35C8);
    }
    if (D_800D5604 != 0) {
        func_800B99F0(0x400);
        func_800B9A18(0x258);
        D_800D5710 = (u32)func_800110DC;
        D_800D5730 = (u32)func_800110DC;
        D_800C2B5C = 1;
        D_800C2B60 = 0;
        func_8004B710(4);
        func_800BBABC(D_800C29E0, 0x25800);
        func_80010FC8();
        func_800BBABC(0, 0);
    }
}
