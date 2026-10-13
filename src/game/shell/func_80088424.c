#include "ctx.h"

typedef struct {
    u8 pad[0x2BF];
    u8 bits;
    u8 mode;
} Save;

extern Save D_800F1900;

s32 func_80088424(s32 p)
{
    s32 v;
    if (p == 0) goto one;
    switch (D_800F1900.mode) {
    case 1:
        if (p >= 2) goto zero;
        v = (D_800F1900.bits >> (p - 1)) & 1;
        break;
    one:
        return 1;
    case 2:
        if (p < 2) goto one;
        if (p != 2) goto zero;
        v = (D_800F1900.bits >> (p - 1)) & 1;
        break;
    case 3:
        if (p < 3) goto one;
        if (p != 3) goto zero;
        v = (D_800F1900.bits >> (p - 1)) & 1;
        break;
    default:
        goto zero;
    }
    if (v) goto one;
zero:
    return 0;
}
