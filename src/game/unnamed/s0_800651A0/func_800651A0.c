#include "ctx.h"

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80065904(s32 p);
void func_800B05E0();
void func_800B2550();
void func_800656B4();
void func_800B3020();
void func_800B2B00();
void func_800B2CC0();
void func_800B3080();
void func_800B1B40();
void func_800B1DE0();
extern void func_800652DC();

extern u8 D_800F13A8[];
extern u8 D_800F0DC8[];
extern u8 D_800F13B8[];
extern u8 D_800F13F8[];
extern u8 D_800F13D0[];
extern u8 D_800F0DD0[];
extern u8 D_800F11F8[];
extern u8 D_800F11F0[];
extern s32 D_800F13E8;
extern s16 D_800F0DA8;
extern s32 D_800CCCC8;
extern s32 D_800CCCCC;
extern s32 D_800F0DA0;

void func_800651A0(u8 *param_1)
{
    func_8000ED10(D_800F13A8, 0, 0x10);
    func_8000ED10(D_800F0DC8, 0, 8);
    D_800F13E8 = -1;
    D_800F0DA8 = 1;
    D_800CCCC8 = 0;
    D_800F0DA0 = 1;
    D_800CCCCC = 0;
    func_800B05E0(D_800F13B8, D_800F13F8, 8);
    func_800B05E0(D_800F13D0, D_800F0DD0, 8);
    func_800B2550(param_1 + 10);
    func_800656B4(0);
    func_800656B4(1);
    func_800656B4(2);
    func_80065904(1);
    func_800B3020(1);
    func_800B2B00(0xAA);
    func_800B2CC0(0x80200000);
    func_800B2CC0(0x80200000);
    func_800B3080(0);
    func_800B1B40(D_800F11F8, 1, func_800652DC, 0, D_800F11F0, param_1);
    func_800B1DE0(D_800F11F8);
}
