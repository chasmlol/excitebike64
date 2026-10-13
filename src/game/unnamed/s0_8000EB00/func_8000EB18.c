#include "ctx.h"

u16 func_8000EB18(u32 r)
{
    s32 v;
    s32 t;
    u32 k;

    k = 0x40000000;
    v = 0;
    if (r >= k) {
        r = r - k;
        v = k;
    }
    t = v + 0x10000000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x10000000;
    }
    t = v + 0x4000000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x4000000;
    }
    t = v + 0x1000000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x1000000;
    }
    t = v + 0x400000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x400000;
    }
    t = v + 0x100000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x100000;
    }
    t = v + 0x40000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x40000;
    }
    t = v + 0x10000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x10000;
    }
    t = v + 0x4000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x4000;
    }
    t = v + 0x1000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x1000;
    }
    t = v + 0x400;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x400;
    }
    t = v + 0x100;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x100;
    }
    t = v + 0x40;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x40;
    }
    t = v + 0x10;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x10;
    }
    t = v + 0x4;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x4;
    }
    t = v + 0x1;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x1;
    }
    return (u16)v;
}
