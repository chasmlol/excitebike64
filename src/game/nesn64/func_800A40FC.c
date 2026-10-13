#include "ctx.h"
extern u32 D_800F5BB0;
#define RAM ((u8 *)0x80380000)
static inline u32 rd(u32 addr) { return RAM[addr]; }
void func_800A40FC(void)
{
    u8 *st = (u8 *)0x80390000;
    u32 a = rd(*(u16 *)(st + 0x4100) + 1);
    D_800F5BB0 = a;
    D_800F5BB0 = (rd(a) | (rd(a + 1) << 8)) + st[0x4104];
}
