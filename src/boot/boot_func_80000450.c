#include "ctx.h"

extern OSThread D_80011600;
extern u8 D_800137B0[];

void func_8000C2D8(void);
void func_8000C5B0(void);
void func_800084E0(OSThread *t, OSId id, void (*entry)(void *), void *arg, void *sp, OSPri pri);
void func_80008780(OSThread *t);
void D_800004A8(void *arg);

void boot_func_80000450(void)
{
    func_8000C2D8();
    func_8000C5B0();
    func_800084E0(&D_80011600, 1, D_800004A8, NULL, D_800137B0, 8);
    func_80008780(&D_80011600);
    for (;;) {
    }
}
