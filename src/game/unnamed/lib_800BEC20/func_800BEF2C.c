#include "ctx.h"

extern void (*D_800D5710)(void);
extern void func_800BED20(void);
extern s32 D_80100720[];

s32 func_800BEF2C(s32 a0) {
    if (D_800D5710 != NULL) {
        D_800D5710();
    }
    func_800BED20();
    return D_80100720[a0 * 19];
}
