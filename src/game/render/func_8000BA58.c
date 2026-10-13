#include "ctx.h"

extern s32 D_800C2384;
extern s32 D_800C2380;

void func_8000B028(void);

void func_8000BA58(void) {
    D_800C2384 = 1;
    D_800C2380 = 0;
    func_8000B028();
}
