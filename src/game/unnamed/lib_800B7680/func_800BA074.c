#include "ctx.h"

extern s32 D_800FA840;
extern s32 **D_800FA854;
extern s32 D_800D33F8;

void func_800BA074(s32 *out) {
    s32 i;
    for (i = 0; i < D_800FA840; i++) {
        if (D_800FA854[i] != NULL) {
            out[i] = D_800FA854[i][4];
        } else {
            out[i] = -1;
        }
    }
    out[D_800FA840] = D_800D33F8;
}
