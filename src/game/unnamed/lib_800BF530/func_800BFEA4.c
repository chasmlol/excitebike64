#include "ctx.h"

void func_800BCB04(u32 v);

void func_800BFEA4(u32 *p) {
    u32 v = *p;
    if (v != 0) {
        if ((v & 2) == 0) {
            func_800BCB04(v);
            *p = 0;
        } else {
            *p = 0;
        }
    }
}
