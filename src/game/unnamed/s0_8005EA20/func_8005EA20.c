#include "ctx.h"

extern u8 D_800F0850[];
extern u8 D_800F093C[];
extern u8 D_800F0944[];
extern u8 D_800F091C[];
extern s32 D_800F0A30;
extern s32 D_800F0A28;
extern s32 D_800F0A34;
extern s32 D_800F094C;
extern s32 D_800F0910;
extern s32 D_800F0950;
extern s32 D_800F0954;
extern s32 D_800F0A3C;
extern s32 D_800F0A1C;
extern s32 D_800F0924;
extern s32 D_800EFDAC;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8005F924(void);
void func_8005EB68(s32 a0, s32 a1);

void func_8005EA20(u8 *param_1, s32 param_2)
{
    func_8000ED10(D_800F0850, 0, 0xc0);
    func_8000ED10(D_800F093C, 0, 6);
    func_8000ED10(D_800F0944, 0, 6);
    func_8000ED10(D_800F091C, 0, 6);
    D_800F0A30 = 0;
    D_800F0A3C = (s32)param_1;
    D_800F0954 = (s32)param_1;
    D_800F0950 = param_2;
    D_800F0910 = param_2;
    D_800F0A28 = 8;
    D_800F0A34 = 0;
    D_800F094C = 0;
    func_8000ED10(param_1, 0, param_2);
    func_8005EB68(0x7b, 8);
    func_8005EB68(0, 8);
    func_8005EB68(0, 8);
    func_8005EB68(0, 8);
    func_8005F924();
    D_800F0A1C = D_800EFDAC;
    D_800F0924 = D_800F0954;
    func_8005EB68(0, 8);
    func_8005EB68(0, 8);
    func_8005EB68(0, 8);
}
