#include "ctx.h"

extern void func_800BDF08(void);
extern s32 func_800BDCB0(s32 *cnt, s32 *val);
extern u8 *D_80100660;

s32 func_800BDE38(void)
{
    s32 cnt;
    s32 val;
    u8 *src;
    u8 *dst;
    u8 *p;

    func_800BDF08();
    for (;;) {
        if (func_800BDCB0(&cnt, &val) < 0) {
            return -1;
        }
        if (cnt == 0) {
            break;
        }
        if (cnt == 1) {
            p = D_80100660;
            *p = (u8)val;
            D_80100660 = p + 1;
        } else {
            src = D_80100660 - val;
            for (cnt = cnt - 1; cnt != -1; cnt = cnt - 1) {
                dst = D_80100660;
                *dst = *src;
                src = src + 1;
                D_80100660 = dst + 1;
            }
        }
    }
    return 0;
}
