#include "ctx.h"

extern u32 D_800F13A8;
void func_800B1150();

void func_80065868(void *arg)
{
    s32 i;
    u32 *p;
    for (i = 0, p = &D_800F13A8; i < 4; i++, p++) {
        if (p[0] != 0) {
            func_800B1150(p[0], arg, 0);
        }
    }
}
