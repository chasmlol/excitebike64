#include "ctx.h"

void func_800BE7F4(s32 *o, s32 v, s32 mode) {
    s32 t;
    if (mode == 1) {
        t = o[3];
    } else {
        if (mode == 0) {
            o[3] = v;
            return;
        }
        if (mode != 2) {
            return;
        }
        t = ((s32 *)o[0])[7];
    }
    o[3] = v + t;
}
