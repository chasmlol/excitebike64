#include "ctx.h"

extern u8 D_80017E60[];
extern u8 D_800180B0[];

void func_800074B0(void *a, void *b, s32 c);
void func_80008290(s32 a, void *b, s32 c);

void boot_func_80001A24(void) {
    func_800074B0(D_80017E60, D_800180B0, 8);
    func_80008290(0xE, D_80017E60, 0);
}
