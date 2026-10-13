#include "ctx.h"

extern u32 D_800D6010;

void func_8000EFB0();

void func_8000EA2C(void) {
    func_8000EFB0();
    func_8000EFB0(&D_800D6010);
}
