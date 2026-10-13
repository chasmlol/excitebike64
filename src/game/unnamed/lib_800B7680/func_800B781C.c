#include "ctx.h"

extern s32 D_800D33F0;
extern u8 *D_800FE860;
extern u8 D_800FA860[];

s32 func_800B781C(void)
{
    s32 idx;
    s32 buf[4];
    u8 *base;
    u8 *fp;
    s32 *p;

    if (D_800D33F0 != 0) {
        idx = (*(s32 (**)(void))(D_800FE860 + 0x10))();
        if (idx < 0) {
            return idx;
        }
        if (idx < 0x100) {
            base = D_800FA860;
            p = (s32 *)(base + idx * 0x18);
            fp = D_800FE860;
            p[0] = 0;
            p[5] = 1;
            (*(void (**)(s32, s32 *))(fp + 0x1C))(idx, buf);
            p[1] = buf[0];
            p[2] = buf[1];
            p[3] = buf[2];
            p[4] = buf[3];
            return idx;
        }
        (*(void (**)(s32))(D_800FE860 + 0x18))(idx);
    }
    return -1;
}
