#include "ctx.h"

extern s32 D_800F4AC8[];
extern u8 D_800E1C78[];
extern u32 D_800E2004;

void func_8008BC30(void);
s32 func_800B4C20(void *p, s32 *a, s32 *b);
s32 func_800B4B10(void *p, s32 *c);
void func_8000CD00(void);

void func_8008BF60(void)
{
    s32 i;
    s32 a;
    s32 b;
    s32 c;
    s32 x;

    func_8008BC30();
    for (i = 0; i < 4; i++) {
        D_800F4AC8[i] = -2;
        if ((D_800E2004 & (1 << i)) && func_800B4C20(&D_800E1C78[i * 0x68], &a, &b) == 0) {
            if (!(D_800E2004 & (1 << i))) {
                x = -1;
            } else {
                func_8000CD00();
                if (func_800B4B10(&D_800E1C78[i * 0x68], &c) != 0) {
                    x = -2;
                } else {
                    x = c;
                }
            }
            x &= ~0xFF;
            if (b == 0x10) {
                D_800F4AC8[i] = -1;
            } else {
                D_800F4AC8[i] = x / 256;
            }
        }
    }
}
