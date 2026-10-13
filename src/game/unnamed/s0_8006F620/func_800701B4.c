#include "ctx.h"

typedef struct Tbl {
    u32 flags;
    u8 pad4[8];
    s32 *f0C;
    u8 pad10[0x18];
    s32 f28;
    u8 pad2C[4];
    s32 f30;
    s32 f34;
    u8 pad38[4];
} Tbl;

typedef struct Obj {
    u8 pad0[0x22];
    u16 f22;
    u8 pad24[0x10];
    u16 f34;
    u8 pad36[0x3AC - 0x36];
    s32 f3AC;
    union {
        s32 w;
        struct { u16 hi; u16 lo; } h;
    } f3B0;
    s32 f3B4;
    s16 f3B8;
    s16 f3BA;
    u8 pad3BC[0x3D0 - 0x3BC];
    s8 f3D0;
    u8 pad3D1[2];
    s8 f3D3;
    u8 pad3D4[0x6F8 - 0x3D4];
    f32 f6F8;
    u8 pad6FC[0x744 - 0x6FC];
    s32 f744;
} Obj;

extern Tbl D_800C9A40[];
extern void func_800477F0(Obj *p, s32 v);

s32 func_800701B4(Obj *p, s32 k)
{
    s32 i;
    s32 top;
    Tbl *e;
    s32 v;
    u32 fl;

    i = p->f3D0;
    if (i < 0) {
        goto ret0;
    }
    e = &D_800C9A40[i];
    top = e->f0C[1] << 8;
    fl = e->flags;

    if (i == 15) {
        s32 d = p->f6F8 * 60.0f;
        s32 n;
        if (d == 0) {
            d = 1;
        }
        n = top - p->f3AC - 0x100;
        v = n / d;
        if (v > 0x80) {
            v = 0x80;
        }
        p->f3AC += v;
        if (d != 1 && p->f744 > 0) {
            goto ret0;
        }
        {
            s32 df = (s16)(p->f22 - p->f34);
            if (df < 0) {
                df = -df;
            }
            if (df > 0x1000) {
                func_800477F0(p, -0x22);
            }
        }
        return 1;
    }

    if (p->f3B8 != 0) {
        if (p->f3B8 < e->f28 || k != 0) {
            u16 o = p->f3B8;
            s32 x;
            s32 y;
            p->f3B8 = o + p->f3B0.h.lo;
            p->f3BA = p->f3BA + 1;
            x = ((u16)p->f3B8 >> 8) & 7;
            if (x & 4) {
                x = 8 - x;
            }
            y = (x << 8) + 0x100;
            p->f3AC = top - y;
            goto ret0;
        }
        p->f3B0.w = -e->f34;
        p->f3B8 = 0;
        goto ret0;
    } else {
        s32 b = p->f3B0.w;
        if (b > 0) {
            s32 n = p->f3AC + b;
            p->f3AC = n;
            if (n < top) {
                goto ret0;
            }
            if (fl & 2) {
                p->f3AC = 0;
                return 1;
            }
            if ((fl & 8) && k != 0) {
                s32 q = e->f30;
                p->f3B8 = 1;
                p->f3BA = 1;
                p->f3B4 = 1;
                p->f3B0.w = q / 2;
                goto ret0;
            }
            if (!(fl & 1)) {
                goto ret0;
            }
            p->f3B8 = 0;
            p->f3B0.w = -e->f34;
            goto ret0;
        }
        {
            s32 vv;
            s32 w;
            if (p->f744 > 0) {
                s32 d = p->f6F8 * 60.0f;
                s32 qq;
                if (d == 0) {
                    d = 1;
                }
                qq = -p->f3AC / d;
                if (qq < b * 2) {
                    qq = b * 2;
                }
                if (p->f3D3 < 0) {
                    s32 h = b / 2;
                    vv = (h < qq) ? h : qq;
                } else {
                    vv = b;
                }
            } else {
                vv = b;
            }
            w = p->f3AC + vv;
            p->f3AC = w;
            if (w > 0) {
                goto ret0;
            }
            p->f3AC = 0;
            return 1;
        }
    }
ret0:
    return 0;
}
