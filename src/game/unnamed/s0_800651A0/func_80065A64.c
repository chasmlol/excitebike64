#include "ctx.h"

extern u32 D_80000300;
extern u8 D_800D3210[];
extern u8 D_800D3170[];
extern u8 D_800D3350[];
extern u8 D_800D32B0[];
void func_8000E9B8(void);

void *func_80065A64(s32 a0) {
    void *s0;
    if (D_80000300 == 1) {
        if (a0 == 2) {
            s0 = D_800D3210;
        } else {
            s0 = D_800D3170;
        }
    } else if (D_80000300 == 2) {
        if (a0 == 2) {
            s0 = D_800D3350;
        } else {
            s0 = D_800D32B0;
        }
    } else {
        func_8000E9B8();
    }
    return s0;
}
