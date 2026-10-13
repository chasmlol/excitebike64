#include "ctx.h"

extern s32 func_8004B890();
extern s32 D_800CA844;

void func_8004F7E8(void) {
    if (D_800CA844 != 0) {
        while (func_8004B890() != 0) {
        }
    }
}
