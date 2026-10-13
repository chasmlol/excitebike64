#include "ctx.h"

extern s32 D_800ED7B4;
extern s32 D_800ED7B8;
extern s32 D_800ED7BC;
extern s32 D_800ED7C0;

void func_80042694(s32 param_1, s32 param_2)
{
    if (param_1 < D_800ED7B4) {
        D_800ED7B4 = param_1;
    }
    if (param_2 < D_800ED7B8) {
        D_800ED7B8 = param_2;
    }
    if (D_800ED7BC < param_1) {
        D_800ED7BC = param_1;
    }
    if (D_800ED7C0 < param_2) {
        D_800ED7C0 = param_2;
    }
}
