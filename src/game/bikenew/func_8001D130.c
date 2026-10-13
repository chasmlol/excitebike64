#include "ctx.h"

typedef struct {
    u8 pad0[0x8C];
    f32 f8C;
    u8 pad90[0x3D0 - 0x90];
    u8 f3D0;
    u8 pad3D1[0x416 - 0x3D1];
    s16 f416;
    u8 pad418[3];
    u8 f41B;
    u8 f41C;
    u8 pad41D[3];
    f32 f420;
    u8 pad424[0x448 - 0x424];
    f32 f448;
    u8 pad44C[0x458 - 0x44C];
    s32 f458;
    u8 pad45C[0x4BC - 0x45C];
    f32 f4BC;
    u8 pad4C0[0x4FC - 0x4C0];
    s32 f4FC;
    u8 pad500[0x560 - 0x500];
    f32 f560;
    u8 pad564[0x5AA - 0x564];
    s16 f5AA;
    u8 pad5AC[4];
    s32 f5B0;
    s32 f5B4;
    s32 f5B8;
    s32 f5BC;
    s32 f5C0;
    u8 pad5C4[0x5CF - 0x5C4];
    u8 f5CF;
    u8 f5D0;
    u8 pad5D1;
    u8 f5D2;
    u8 pad5D3[0x6E8 - 0x5D3];
    u8 f6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 f744;
} Obj;

extern s32 D_800EFDBC;
extern s32 D_800E76D0;
extern u16 D_800E37E2;
void func_8003F004(Obj *p, s32 n);
void func_80046CC0(Obj *p);

void func_8001D130(Obj *o, s32 a1)
{
    f32 fs0;
    f32 fs1;
    f32 fv0;
    s32 w;
    s32 n;
    s16 t;
    s32 x;
    s32 v;

    w = o->f5B0;
    fs1 = (s32)((u32)w >> 28);
    if (w & 2) {
        fs1 = -fs1;
    }
    fs0 = (w >> 24) & 0xF;
    if (w & 8) {
        fs0 = -fs0;
    }
    if (o->f5AA == 4) {
        v = fs0;
        fs0 = fs1;
        fs1 = -v;
    }
    if ((u32)(o->f744 + 14) < 15) {
        n = o->f458;
        if (n < o->f4FC) {
            n = o->f4FC;
        }
        if (n >= -6) {
            fs0 *= 0.25f;
        }
    }
    if (D_800EFDBC == 6) {
        x = D_800E37E2 - 0x8000;
        t = x;
        if (t > 0x2000 && 0.0f < fs0) {
            fs0 = 0.0f;
            o->f8C = fs0;
        } else if (t < -0x2000 && fs0 < 0.0f) {
            fs0 = 0.0f;
            o->f8C = fs0;
        }
        if (o->f744 <= 0) {
            if (t > 0x4000) {
                goto call;
            }
            if (t < -0x4000) {
call:
                func_80046CC0(o);
            }
        }
    }
    if (o->f744 < 0) {
        if (o->f5B0 & 0x100) {
            fs0 = 0.0f;
        }
    } else if ((o->f5B0 & 0x100) || o->f3D0 < 0x10) {
        fs1 = 0.0f;
        fs0 *= 0.25f;
    }
    if (o->f5B0 & 0x20000) {
        if (!(o->f5B4 & 0x20000)) {
            o->f5B8 = D_800E76D0;
        }
    } else if (o->f5B4 & 0x20000) {
        o->f5BC = D_800E76D0;
    }
    if ((o->f5B0 & 0x10000) && o->f41B == 0) {
        if (o->f5C0 >= 0) {
            o->f5C0++;
        } else {
            o->f5C0 = 0;
        }
    } else {
        if (o->f5C0 < 0) {
            o->f5C0--;
        } else {
            o->f5C0 = -1;
        }
    }
    if (o->f416 == 0) {
        if (fs1 < -8.0f && o->f744 < 0 && (8.0f < fs0 || fs0 < -8.0f)) {
            o->f41C = 1;
            if (0.0f < fs0) {
                fs0 = 15.0f;
            } else {
                fs0 = -15.0f;
            }
        } else {
            o->f41C = 0;
        }
        if (o->f6E8 == 0) {
            if ((0.0f < fs0) ? (10.0f <= fs0) : (10.0f <= -fs0)) {
                if ((o->f5B0 & 0x10020) == 0x10020 && 4.0f < o->f420 && (s8)o->f3D0 != -1) {
                    o->f41B = 1;
                    goto L37c;
                }
            }
            if (o->f41B == 1) {
                o->f41B = 2;
            } else {
                o->f41B = 0;
            }
        }
L37c:
        if (o->f5C0 >= -14 && o->f744 < 0) {
            fv0 = (o->f448 - o->f4BC - o->f560) * 15.625f / 1323.0001f + 4.0f;
            if (13.0f < fv0) {
                fv0 = 13.0f;
            }
            if (fv0 < 0.0f) {
                fv0 = 0.0f;
            }
            fs1 = (s32)fv0;
        }
    }
    o->f5D0 = (s32)fs0;
    o->f5D2 = (s32)fs1;
    if (o->f458 < 0) {
        o->f5CF = 0;
        if (o->f41B != 0) {
            goto two;
        }
        if (o->f5B0 & 0x20000) {
            if (D_800E76D0 - o->f5B8 < 30) {
two:
                o->f5CF = 2;
            } else {
                o->f5CF = 1;
            }
        } else if (o->f5B0 & 0x10000) {
            if ((0.0f < fs0) ? (8.0f < fs0) : (8.0f < -fs0)) {
                if (5.0f < o->f420) {
                    o->f5CF = 1;
                }
            }
        }
    }
end:
    func_8003F004(o, a1);
}
