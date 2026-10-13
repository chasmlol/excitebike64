#include "ctx.h"

extern s32 D_800F5A80[];
extern s32 D_800F5A74;
extern s32 D_800F5A78;
extern s32 D_800CEC18;

void func_800AD7E0(s32 a, s32 b);
s32 func_800AD5F0(void);
void func_800A1570(s16 *p);
void func_800AD910(s16 *p, s32 n);
void func_800AD720(s16 *p, s32 n);

void func_800A1898(void)
{
    s32 i;
    s16 *p;

    func_800AD7E0(0, 0x400000);
    if (func_800AD5F0() >= 0) {
        p = (s16 *)D_800F5A80[D_800F5A74];
        for (i = 0; i < D_800CEC18; i++) {
            func_800A1570(p);
            p += (D_800F5A78 / D_800CEC18) * 2;
        }
        func_800AD910((s16 *)D_800F5A80[D_800F5A74], D_800F5A78 << 2);
        func_800AD720((s16 *)D_800F5A80[D_800F5A74], D_800F5A78 << 2);
        D_800F5A74 = D_800F5A74 ^ 1;
    }
}
