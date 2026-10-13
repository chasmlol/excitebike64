#include "ctx.h"

extern u8 D_80100708[];
extern u8 D_80100980[];
extern u8 D_80100724[];
extern s32 D_801009A4;
extern s32 D_80100700;

void func_800AC920(s32, void *, void *, s32);
void func_800B05E0(void *, void *, s32);
s32 func_800BF100(void);
void *func_800BF3C0(void);

s32 func_800BEE78(void)
{
    s32 i;
    u8 *b;
    s32 *z;
    u8 *q;
    u8 *p;

    func_800AC920(0x96, D_80100708, D_80100980, 8);
    i = 0;
    b = D_80100724;
    z = (s32 *)(b - 4);
    q = b + 0x18;
    p = b;
    do {
        func_800B05E0(p, q, 1);
        *z = 0;
        z = (s32 *)((u8 *)z + 0x4C);
        q += 0x4C;
        i++;
        p += 0x4C;
    } while (i < 8);
    D_801009A4 = func_800BF100();
    D_80100700 = (s32)func_800BF3C0();
    return 0;
}
