#include "ctx.h"

extern s32 D_800D33F0;
extern s32 *D_800D33E8;
extern s32 *D_800FE860;
void func_8000EE20(void);
void func_8000EE28(void);

void func_800B93E8(s32 p1, u32 p2)
{
    s32 *n;
    s32 *r;
    u32 v;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (n = D_800D33E8; n != 0; n = (s32 *)n[1]) {
            if (n[4] == p1) {
                func_8000EE28();
                r = n;
                goto found;
            }
        }
        func_8000EE28();
        r = 0;
    found:
        if (r != 0) {
            v = p2 << 8;
            r[0x11] = v;
            if (r[2] != -1) {
                ((void (*)(s32, s32))D_800FE860[14])(r[2], v >> 8);
            }
        }
    }
}
