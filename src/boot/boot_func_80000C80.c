#include "ctx.h"

extern s32 D_80017968[];
extern s32 D_80017BE0[];
extern s32 D_80017984[];
extern s32 D_80017C04;

void func_80005D00(s32 a, void *b, void *c, s32 d);
void func_800074B0(void *a, void *b, s32 c);
s32 func_80006210(void);

s32 boot_func_80000C80(void) {
    s32 n;
    s32 *base;
    s32 *s0;
    s32 *s1;
    s32 *s2;

    func_80005D00(0x96, D_80017968, D_80017BE0, 8);
    n = 0;
    base = D_80017984;
    s2 = base - 1;
    s1 = base + 6;
    s0 = base;
    do {
        func_800074B0(s0, s1, 1);
        *s2 = 0;
        s2 += 19;
        s1 += 19;
        n++;
        s0 += 19;
    } while (n < 8);
    D_80017C04 = func_80006210();
    return 0;
}
