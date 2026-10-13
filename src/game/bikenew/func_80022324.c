#include "ctx.h"

extern s8 D_800EFD70[];

u32 func_80022324(u32 a0, s32 a1)
{
    s8 *base;
    s32 c;
    u32 u2;

    if (a0 & 0x8000) a0 |= 0x20000;
    base = (s8 *)D_800EFD70;
    if (a1 < *(s32 *)(base + 0x3C))
        c = (s8)base[(s8)base[a1 + 0x2C8] + 0x50];
    else
        c = 0;

    switch (c) {
    case 0:
        return a0;
    case 1:
        u2 = a0 & 0xfffcffff;
        if (a0 & 0x10000) u2 |= 0x20000;
        if (a0 & 0x20000) u2 |= 0x10000;
        return u2;
    case 2:
        u2 = a0 & 0xfffdffdf;
        if (a0 & 0x20) u2 |= 0x20000;
        if (a0 & 0x20000) u2 |= 0x20;
        return u2;
    case 3:
        u2 = a0 & 0xfffcffdf;
        if (a0 & 0x20) u2 |= 0x10000;
        if (a0 & 0x20000) u2 |= 0x20;
        if (a0 & 0x10000) u2 |= 0x20000;
        return u2;
    }
    return a0;
}
