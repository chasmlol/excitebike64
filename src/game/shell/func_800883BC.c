#include "ctx.h"

extern u8 D_800CD79C[];
void func_8007AF38();
void func_8001102C(s32 param_1);

void func_800883BC(u8 *p, s32 x, u32 *f, void *s)
{
    s32 n;
    void *q;

    if ((*f & 0x50) != 0) {
        func_8007AF38(D_800CD79C, 0, 1, s);
        n = 1;
    } else {
        if ((*f & 0x20) == 0) {
            return;
        }
        q = *(void **)(p + 0x28);
        if (q == 0) {
            return;
        }
        func_8007AF38(q, 0, 0, s);
        n = 2;
    }
    func_8001102C(n);
}
