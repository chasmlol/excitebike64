#include "ctx.h"

void func_80070600(s32 a, s32 b);
void func_80013ACC(s32 a, void *b, s32 c);

void func_80071678(s32 a0, s32 a1, s32 a2, void *a3) {
    func_80070600(a0, a1);
    func_80013ACC(a2, a3, 3);
}
