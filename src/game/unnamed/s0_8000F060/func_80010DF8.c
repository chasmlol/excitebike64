#include "ctx.h"

extern u32 D_800E32A0;

void func_8004F8A4(void);

void func_80010DF8(u32 a) {
    D_800E32A0 = a;
    func_8004F8A4();
}
