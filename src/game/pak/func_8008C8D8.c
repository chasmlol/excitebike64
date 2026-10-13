#include "ctx.h"

extern s32 D_800CE164;
void func_800C1E60(s32 a, s32 b, s32 c, s32 d, s32 e);
void func_800C1F2C(s32 a, char *b);

void func_8008C8D8(char *param_1, s32 param_2, s32 param_3) {
    func_800C1E60(D_800CE164, 0x80, param_2, param_3, 0);
    func_800C1F2C(D_800CE164, param_1);
}
