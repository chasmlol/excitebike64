#include "ctx.h"

void func_8000EE20(void);
void func_8000EE28(void);
extern s32 D_800D33F0;
extern s32 D_800D33E8;
extern s32 D_800FA84C;

void func_800B9548(s32 id, s32 v)
{
    s32 *p;
    s32 *q;
    s32 iv;

    if (D_800D33F0 == 0) {
        return;
    }
    func_8000EE20();
    for (p = (s32 *)D_800D33E8; p != 0; p = (s32 *)p[1]) {
        if (p[4] == id) {
            func_8000EE28();
            q = p;
            goto common;
        }
    }
    func_8000EE28();
    q = 0;
common:
    if (q != 0) {
        iv = v * D_800FA84C;
        q[14] = q[9] << 16;
        q[13] = (iv * 0x10000) / 1000;
    }
}
