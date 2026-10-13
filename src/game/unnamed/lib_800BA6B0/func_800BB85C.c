#include "ctx.h"

extern s32 D_800D5608;
extern s32 D_800FE8DC;
extern u8 D_800DB170[];
extern u8 *D_800FE870;
extern s32 D_800FE878;
extern s32 D_800FE874;
extern u8 D_80100500[];
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
u8 *func_800BC908(s32 a, s32 b, void *c, s32 d);

s32 func_800BB85C(s32 size)
{
    s32 i;
    u8 *q;

    if (D_800D5608 != 0) {
        return -1;
    }
    if (D_800FE8DC & 1) {
        size <<= 1;
    }
    D_800FE870 = q = func_800BC908(size, 0, D_800DB170, 0x601);
    if (q == NULL) {
        return 1;
    }
    func_8000ED10(q, 0, size);
    D_800FE878 = size;
    i = 0xFF;
    {
        u8 *pp = D_80100500 + i;
        do {
            *pp = i;
            i--;
            pp--;
        } while (i >= 0);
    }
    D_800FE874 = 0;
    D_800D5608 = 1;
    return 0;
}
