#include "ctx.h"

extern u16 D_800C6C64[];

s32 func_80013010(s32 x)
{
    s32 t;
    u16 *tbl;
    u16 *tbl2;
    if (x > 0) {
        if (x >= 0x4000) {
            return 0x4000;
        }
        t = x + 2;
        tbl = D_800C6C64;
        if (t < 0) {
            t = x + 5;
        }
        return ((s16 *)tbl)[t >> 2];
    }
    if (x < -0x3FFF) {
        return -0x4000;
    }
    x = 2 - x;
    tbl2 = D_800C6C64;
    if (x < 0) {
        x = x + 3;
    }
    return (s16)(-tbl2[x >> 2]);
}
