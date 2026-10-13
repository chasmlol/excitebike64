#include "ctx.h"

extern u8 D_800D6D00[];
extern u8 D_800E9228[];
extern u8 D_800E92A8[];
extern void *D_800E9214;
extern s32 D_800E9210;
extern s32 D_800E9298;

void *func_800BC908(s32 size, s32 fill, void *src, s32 n);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80036230(void);

void func_8003A694(s32 p)
{
    D_800E9214 = func_800BC908(0xD80, 0, D_800D6D00, 0x60);
    func_8000ED10((u8 *)D_800E9214, 0, 0xD80);
    D_800E9210 = 0;
    func_8000ED10(D_800E9228, 0, 0x40);
    func_8000ED10(D_800E92A8, 0, 0x20);
    D_800E9298 = p;
    func_80036230();
}
