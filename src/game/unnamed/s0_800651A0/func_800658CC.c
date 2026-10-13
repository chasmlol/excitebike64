#include "ctx.h"

extern s32 D_800F13E8;
void func_800B1350(void *p);
void func_800B155C(void *p);

void func_800658CC(u8 *param_1) {
    void *p = param_1 + 0x10;
    D_800F13E8 = 0;
    func_800B1350(p);
    func_800B155C(p);
}
