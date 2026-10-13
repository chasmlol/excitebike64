#include "ctx.h"

void func_800B05E0(void *dst, void *src, int n);
void func_800651A0(int a, int b);

extern u8 D_800E3090[];
extern u8 D_800E2068[];

void func_8000E980(void)
{
    func_800B05E0(D_800E3090, D_800E2068, 0x10);
    func_800651A0(0xD, 1);
}
