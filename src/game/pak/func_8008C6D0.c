#include "ctx.h"

extern void *D_800F4A10[];
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_8008C6D0(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        D_800F4A10[i] = func_800BC908(0x740, 0, (s32)"pak.c", 0x75F);
        func_8000ED10(D_800F4A10[i], 0x77, 0x740);
    }
}
