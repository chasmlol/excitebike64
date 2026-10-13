#include "ctx.h"

extern u32 D_800D56C0[];

void func_8000ED30(char *a, char *b);

void func_800BDC2C(char *a0, u32 a1)
{
    u32 *a2;
    char *a3;
    char buf[9];
    s32 q;

    a2 = D_800D56C0;
    a3 = buf;
    do {
        q = (s32)(a1 / *a2);
        if (q < 10) {
            *a3 = (char)(q + '0');
        } else {
            *a3 = (char)(q + '7');
        }
        a1 = a1 - q * *a2;
        a2 = a2 + 1;
        a3 = a3 + 1;
    } while ((s32)a2 < (s32)(D_800D56C0 + 8));
    buf[8] = 0;
    func_8000ED30(a0, buf);
}
