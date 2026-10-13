#include "ctx.h"

extern u8 *D_800F0924;
extern s32 D_800F094C;
extern u8 *D_800F0954;
extern s32 D_800F0A28;
extern s32 D_800F0A30;
extern s32 D_800F0A34;
extern u8 *D_800F0A3C;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8005EB68(s32 v, s32 n);
void func_8005EC14(u32 *p);

s32 func_8005F188(void)
{
    u32 buf[6];
    s32 i;
    s32 n;
    s32 acc;
    u8 *p;

    D_800F0924[0] = D_800F094C;
    D_800F0924[1] = D_800F094C >> 8;
    D_800F0924[2] = D_800F094C >> 16;
    func_8000ED10((u8 *)buf, 0, 0x18);
    while (D_800F0A30 != 0) {
        func_8005EC14(buf);
    }
    if (D_800F0A28 != 8) {
        func_8005EB68(0, D_800F0A28);
    }
    acc = 0;
    n = D_800F0954 - D_800F0A3C - 4;
    p = D_800F0A3C + 4;
    for (i = 0; i < n; i++) {
        acc += p[i] << (i % 8);
    }
    acc &= 0xFFFFFF;
    D_800F0A3C[1] = acc;
    D_800F0A3C[2] = acc >> 8;
    D_800F0A3C[3] = acc >> 16;
    if (D_800F0A34 != 0) {
        return -1;
    }
    return D_800F0954 - D_800F0A3C;
}
