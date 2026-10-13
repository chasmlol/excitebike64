#include "ctx.h"

extern s32 D_801054E0[];
extern s32 D_80105440[];
extern u8 D_80101368[];
extern u8 *D_8010536C;
extern u8 *D_80101360;

s32 func_800C2224(void) {
    u32 i = 0;
    s32 *a = D_801054E0;
    s32 *b = D_80105440;
    D_8010536C = D_80101368;
    D_80101360 = D_80101368 + 0x3FE0;
    do {
        *b = 0;
        *a = 0;
        a++;
        i++;
        b++;
    } while (i < 0x28);
    return 0;
}
