#include "ctx.h"

extern u32 D_800E1C60;
extern u8 D_800E1C48[];
extern u8 D_800E1E60[];
extern u32 D_800E2010[];
extern u32 D_800E2020;

void func_8000CD00(void);
void func_800B3704(u8 *a0, u8 *a1, s32 a2);
void func_800B35C0(u8 *a0, s32 a1);

void func_8000D0D8(void)
{
    s32 i;
    u32 bit;

    func_8000CD00();
    for (i = 0; i < 4; i++) {
        if ((D_800E1C60 & (1u << i)) != 0) {
            bit = 1u << i;
            func_800B3704(D_800E1C48, D_800E1E60 + i * 0x68, i);
            func_800B35C0(D_800E1E60 + i * 0x68, 0);
            D_800E2010[i] = 0;
            D_800E2020 = D_800E2020 & ~bit;
        }
    }
}
