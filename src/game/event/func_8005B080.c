#include "ctx.h"

extern s32 D_800F05A8[];
extern s32 D_800E3770;
extern s32 D_800E36E0;
extern void func_800515A8(void);
extern void func_80050F7C(s32);
extern s32 func_8005AFFC(void);

void func_8005B080(void)
{
    s32 *g;

    func_800515A8();
    g = D_800F05A8;
    if (g[0x40 / 4] == 0) {
        if ((g[0x58 / 4] & 0x10) == 0) {
            s32 n = g[0x2C / 4] + 1;
            g[0x2C / 4] = n;
            if (n == 0x34BC0) {
                func_80050F7C(0);
                g[0x40 / 4] = 1;
            }
        }
    } else if ((D_800E3770 & 0x40) != 0 && (D_800E36E0 & 0x40) == 0) {
        func_8005AFFC();
    }
}
