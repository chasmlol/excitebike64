#include "ctx.h"

extern s32 func_800BDB44(s32, s32, s32);
extern s32 D_80100640;
extern s32 D_80100644;
extern s32 D_80100648;

s32 func_800BD8E0(void) {
    s32 r = func_800BDB44(D_80100640, D_80100644, D_80100648);

    if (r < 0) {
        while (1) {
        }
    }
    return 0;
}
