#include "ctx.h"

extern s32 D_800CEC18;
extern s32 D_800F5A70;
extern s32 D_800F5A74;
extern s32 D_800F5A78;
extern s32 D_800F5A80[];
extern u8 D_800DAF38[];

s32 func_800AD600(s32 x);
void *func_800BC908(s32 size, s32 fill, u8 *src, s32 n);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_800A1484(void)
{
    s32 i;
    void *q;

    D_800F5A70 = func_800AD600(0x5622);
    D_800F5A78 = ((D_800CEC18 * D_800F5A70) / 60 + 7) & -8;
    for (i = 0; i < 2; i++) {
        q = func_800BC908(D_800F5A78 << 2, 0, D_800DAF38, 0x396);
        D_800F5A80[i] = (s32)q;
        func_8000ED10(q, 0, D_800F5A78 << 2);
    }
    D_800F5A74 = 1;
}
