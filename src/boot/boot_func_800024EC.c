#include "ctx.h"

extern u8 D_80018430;
extern u8 D_80018432;
extern u32 D_80000318;
extern u32 D_8000F4A8;
extern u32 D_8000F4B8;
u32 func_80001B50(void);
void func_80002D20(void);
void func_80001BB8(u32 a, u32 b);
void func_80001B88(u32 a);

void boot_func_800024EC(void)
{
    u32 r;
    u32 x;

    r = func_80001B50();
    func_80002D20();
    func_80001BB8(0xB1FFFFF0, 0);
    func_80001BB8(0xB1FFFFFC, 0);
    D_80018430 = 1;
    D_80018432 = 0xFF;
    x = D_80000318;
    if (x > 0x3FFFFF && (x & 0x1FFFF) == 0 && x < 0x2000001) {
        x = x - 1;
        D_8000F4A8 = x - 0x80000000;
        D_8000F4B8 = x + 0xA0000000;
    }
    func_80001B88(r);
}
