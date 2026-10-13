#include "ctx.h"

typedef struct {
    u8 p000[0x1C];
    s32 w01C;
    u8 p020[0x44 - 0x20];
    u8 b044;
    u8 p045[0x3AC - 0x45];
    s32 w3AC;
    s32 w3B0;
    u8 p3B4[0x3C4 - 0x3B4];
    s32 w3C4;
    s32 w3C8;
    u8 p3CC[0x3D0 - 0x3CC];
    s8 s3D0;
    u8 p3D1;
    s8 s3D2;
    s8 s3D3;
    u8 p3D4[0x3E4 - 0x3D4];
    u8 b3E4[6];
    u8 b3EA;
    s8 b3EB[4];
    u8 p3EF[0x3FB - 0x3EF];
    u8 b3FB;
    u8 p3FC[0x3FE - 0x3FC];
    u8 b3FE;
    u8 p3FF[0x408 - 0x3FF];
    s32 w408;
    u8 p40C[0x420 - 0x40C];
    f32 f420;
    u8 p424[0x5B0 - 0x424];
    u32 w5B0;
    u8 p5B4[0x648 - 0x5B4];
    s32 w648;
    u8 p64C[0x6F4 - 0x64C];
    f32 f6F4;
    u8 p6F8[0x744 - 0x6F8];
    s32 w744;
} Obj;

void func_8006F8D4(Obj *o);
void func_800445B0(Obj *o, s32 k);
s32 func_800701B4(Obj *o, s32 k);
void func_8006FE84(Obj *o, s32 k);

extern s32 D_800F05E8;
extern s32 D_800E76D0;
extern s32 D_800C2280;
typedef struct {
    s32 pad[3];
    s32 *p;
    s32 rest[11];
} TblEnt;
extern TblEnt D_800C9A40[];

void func_8006FB3C(Obj *o)
{
    s32 found;
    s32 k;
    s32 t;
    s32 v;
    s32 lim;
    s8 d2;
    s8 d3;
    s8 s;

    func_8006F8D4(o);
    if (o->b3FB != 0) {
        if (--o->b3FB == 0) {
            if ((o->w744 < 0 || 30.0f < o->f6F4) && D_800F05E8 == 0) {
                found = 0;
                t = o->w3C4;
                o->w3C4 = 0;
                o->w3C8 += t;
                d2 = o->s3D2;
                if (d2 < 16) {
                    for (k = 0; k < o->b3EA; k++) {
                        if (d2 == o->b3EB[k]) break;
                    }
                    if (k == o->b3EA) {
                        found = 1;
                        o->b3EB[k] = o->s3D2;
                        o->b3EA = k + found;
                    }
                    for (k = 4; k >= 0; k--) {
                        o->b3E4[k + 1] = o->b3E4[k];
                    }
                    o->b3E4[0] = o->s3D2;
                }
                if (found && (D_800C2280 & 0x10)) {
                    o->w648 = 0;
                }
            } else {
                o->b3FB = 1;
            }
        }
    }
    s = o->s3D0;
    if (s < 0) {
        d3 = o->s3D3;
        if (d3 >= 0) {
            if (D_800E76D0 - o->w408 < 60) {
                func_8006FE84(o, d3);
            } else {
                o->s3D3 = -1;
            }
        }
        return;
    }
    if (o->s3D0 == 16 || o->s3D0 == 17) {
        if (o->w744 > 0) {
            v = (s32)(o->f6F4 * 60.0f);
            if (o->w3B0 > 0 && v >= 16) {
                o->w3AC += o->w3B0;
                lim = D_800C9A40[o->s3D0].p[1] << 8;
                if (o->w3AC >= lim) {
                    o->w3AC = lim - 0x100;
                    o->w3B0 = -0x100;
                }
            } else {
                o->w3B0 = -0x100;
                v = (s32)(o->f6F4 * 60.0f);
                if (v < 15) {
                    if (v < 2) {
                    neg:
                        o->s3D0 = -1;
                    } else {
                        o->w3AC -= o->w3AC / v;
                    }
                }
            }
        } else {
            goto neg;
        }
        return;
    }
    if (o->w744 < 0 && o->f420 < 5.0f && s < 16) {
        if (o->w01C > 0) {
            func_800445B0(o, 0x22);
        } else {
            func_800445B0(o, 0x1F);
        }
        return;
    }
    if (func_800701B4(o, (o->w5B0 >> 8) & 1) != 0) {
        u8 pv = o->s3D0;
        o->b3FB = 0x20;
        o->s3D0 = -1;
        o->s3D2 = pv;
    }
    if (o->b044 == 0 && o->s3D0 < 16 && o->s3D2 < 16) {
        o->b3FE = 15;
    }
}
