#include "ctx.h"

extern s32 D_800E2004;
extern s32 D_800E2008;
extern s32 D_800F4A68[];

s32 func_8008B7B4(s32 p);
s32 func_8008C068(u32 p);
void func_80088500(s32 p, s32 z);

void func_80089E30(s32 p)
{
    s32 n;
    s32 r;
    s32 bit;
    s32 *q;

    n = 3;
    do {
        if (func_8008B7B4(p) == 0) {
            break;
        }
        n = n - 1;
    } while (n != 0);
    if (n == 0) {
        D_800E2008 = D_800E2008 | (1 << p);
        D_800E2004 = D_800E2004 & ~(1 << p);
    }
    n = 3;
    do {
        r = func_8008C068(p);
        if (r < 0) {
            r = r + 0xFF;
        }
        D_800F4A68[p] = r >> 8;
    } while ((r >> 8) < 0 && (n = n - 1) != 0);
    if (n == 0) {
        D_800E2008 = D_800E2008 | (1 << p);
        D_800E2004 = D_800E2004 & ~(1 << p);
    }
    func_80088500(p, 0);
}
