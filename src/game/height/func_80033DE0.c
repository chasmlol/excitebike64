#include "ctx.h"

extern s32 D_800E8E90[];
extern s32 D_800E8E88[];

s32 func_80033DE0(s32 a, s32 idx) {
    s32 n = D_800E8E90[idx] + a;
    if (n >= 0x81) {
        return 0;
    }
    D_800E8E90[idx] = n;
    return D_800E8E88[idx] + (n - a) * 16;
}
