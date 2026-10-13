#include "ctx.h"

extern u8 D_80017E60[];
extern u32 D_80017CA8;
int func_80008030();
void func_80000704();
void func_80006CE0();

void boot_func_80001A6C(void)
{
    u32 buf[2];
    if (func_80008030(D_80017E60, buf, 0) == 0) {
        func_80000704(D_80017CA8, 0, 0x25800);
        func_80006CE0(0, 0x400000);
        while (1) {}
    }
}
