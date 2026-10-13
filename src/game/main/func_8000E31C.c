#include "ctx.h"

void func_8000E31C(OSContPad *pad, u32 *out)
{
    s32 x;
    s32 y;
    u32 v;
    u32 u;
    u32 t;

    *out = 0;
    x = pad->stick_x;
    x = x * 127 / 80;
    y = pad->stick_y;
    y = y * 127 / 80;
    if (x < -127) {
        x = -127;
    }
    if (x > 127) {
        x = 127;
    }
    if (y < -127) {
        y = -127;
    }
    if (y > 127) {
        y = 127;
    }
    if (x < -5) {
        u = (-x >> 3) << 24;
        if (u != 0) {
            *out = u | 4;
        }
    } else if (x > 5) {
        u = (x >> 3) << 24;
        if (u != 0) {
            *out = u | 8;
        }
    }
    if (y < -5) {
        v = (-y >> 3) << 28;
        if (v == 0) {
            goto done;
        }
        t = *out | 2;
    } else {
        if (y < 6 || (v = (y >> 3) << 28) == 0) {
            goto done;
        }
        t = *out | 1;
    }
    *out = t | v;
done:
    if (pad->button & 0x8000) {
        *out |= 0x10;
    }
    if (pad->button & 0x4000) {
        *out |= 0x20;
    }
    if (pad->button & 0x1000) {
        *out |= 0x40;
    }
    if (pad->button & 8) {
        *out |= 0x80;
    }
    if (pad->button & 4) {
        *out |= 0x100;
    }
    if (pad->button & 2) {
        *out |= 0x200;
    }
    if (pad->button & 1) {
        *out |= 0x400;
    }
    if (pad->button & 0x800) {
        *out |= 0x800;
    }
    if (pad->button & 0x400) {
        *out |= 0x1000;
    }
    if (pad->button & 0x200) {
        *out |= 0x2000;
    }
    if (pad->button & 0x100) {
        *out |= 0x4000;
    }
    if (pad->button & 0x20) {
        *out |= 0x8000;
    }
    if (pad->button & 0x10) {
        *out |= 0x10000;
    }
    if (pad->button & 0x2000) {
        *out |= 0x20000;
    }
}
