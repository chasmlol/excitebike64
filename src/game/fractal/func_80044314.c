#include "ctx.h"

extern s32 D_800EE12C;
extern s32 D_800EE138;
extern s32 D_800D6FE0;
s32 func_800BC908(s32 a, s32 b, s32 c, s32 d);

void func_80044314(s32 param_1) {
    s32 *dst = &D_800EE138;
    D_800EE12C = param_1;
    *dst = func_800BC908(0x4040, 0, (s32)&D_800D6FE0, 0x266);
}
