#include "ctx.h"

extern s32 D_800F0A2C;
extern s32 D_800F0A38;
extern s32 D_800F0A24;
extern s32 D_800F0A18;
extern s32 D_800EFDAC;
extern s32 D_800F0938;
extern u8 D_800F0928[];
extern u8 D_800F0930[];
extern u8 D_800F0914[];

void func_8005FA18(void);
s32 func_80060204(s32 n);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_8005FE10(s32 param_1)
{
    s32 u;

    D_800F0A2C = 0;
    D_800F0A38 = param_1;
    D_800F0A24 = 8;
    D_800F0A38 = param_1 + 4;
    func_8005FA18();
    D_800F0A18 = D_800EFDAC;
    D_800F0938 = func_80060204(8);
    u = func_80060204(8);
    D_800F0938 = D_800F0938 + u * 0x100;
    u = func_80060204(8);
    D_800F0938 = D_800F0938 + u * 0x10000;
    func_8000ED10(&D_800F0928[0], 0, 6);
    func_8000ED10(&D_800F0930[0], 0, 6);
    func_8000ED10(&D_800F0914[0], 0, 6);
}
