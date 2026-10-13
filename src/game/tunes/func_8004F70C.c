#include "ctx.h"

extern s32 D_800D7338;
s32 func_800BC908(s32 a, s32 b, s32 c, s32 d);

void func_8004F70C(s32 *p) {
    if (p[2] == 0) {
        p[2] = func_800BC908(0x220, 0, (s32)&D_800D7338, 0x128);
        p[3] = func_800BC908(0x800, 0, (s32)&D_800D7338, 0x129);
        p[0] = 0;
        p[1] = -1;
        p[4] = -1;
        p[0xC] = -1;
    }
}
