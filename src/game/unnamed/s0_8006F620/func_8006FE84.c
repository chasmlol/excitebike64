#include "ctx.h"

typedef struct {
    s32 f0;
    u8 p4[0x1C];
    s32 f20;
    u8 p24[8];
    s32 f2C;
    u8 p30[8];
    s32 f38;
} Tab;

typedef struct {
    u8 p0[0x45];
    s8 f45;
    u8 p46[0x3AC - 0x46];
    s32 f3AC;
    s32 f3B0;
    s32 f3B4;
    s16 f3B8;
    s16 f3BA;
    u8 p3BC[0x3C4 - 0x3BC];
    s32 f3C4;
    u8 p3C8[0x3D0 - 0x3C8];
    u8 f3D0;
    u8 p3D1;
    s8 f3D2;
    s8 f3D3;
    u8 p3D4[0x3E4 - 0x3D4];
    u8 hist[6];
    u8 cnt;
    s8 list[0x10];
    u8 f3FB;
    u8 p3FC[0x41B - 0x3FC];
    u8 f41B;
    u8 p41C[0x622 - 0x41C];
    s16 f622;
    u8 p624[0x65C - 0x624];
    s32 f65C;
    u8 p660[0x6F4 - 0x660];
    f32 f6F4;
    u8 p6F8[0x744 - 0x6F8];
    s32 f744;
    u8 p748[0x7A8 - 0x748];
    u8 f7A8;
    u8 p7A9[0x7E0 - 0x7A9];
} Ent;

extern Tab D_800C9A40[];
extern s32 D_800E3700;
extern s32 D_800C8CD8;
extern Ent D_800E37C0[];
void func_8003F564(s32 a, s32 b);
void func_8004E2E0(u32 a, s32 b);
void func_8004F410(s32 a);

void func_8006FE84(Ent *o, s32 n)
{
    s32 u;
    s32 d, k, j, cnt, v, c2; s8 a2;
    u8 c;
    s32 id;

    if (o->f744 > 0) {
        if (n < 0x19) {
            if (o->f6F4 < 0.5f) {
                return;
            }
            if (o->f45 != 0) {
                return;
            }
        }
    } else if (!(D_800C9A40[n].f0 & 0x10)) {
        return;
    }
    o->f41B = 0;
    func_8003F564(o->f65C, 0xF);
    if (n == 0xF) {
        if (o->f744 < 3) {
            return;
        }
        if (o->f744 >= 5) {
            o->f3D3 = -1;
            return;
        }
    }
    o->f3D0 = n;
    o->f3AC = 0;
    u = D_800C9A40[n].f2C;
    o->f3D3 = -1;
    o->f3B4 = 0;
    o->f7A8 = 0;
    o->f3B8 = 0;
    o->f3BA = 0;
    o->f622 = 0;
    o->f3B0 = u;
    if (n >= 0x10) {
        return;
    }
    if (D_800C9A40[n].f38 == -1) {
        return;
    }
    if (o->f3FB != 0) {
        d = o->f3D2;
        o->f3FB = 0;
        if (d < 0x10) {
            cnt = o->cnt;
            for (k = 0; k < cnt; k++) {
                if (d == o->list[k]) {
                    break;
                }
            }
            if (k == o->cnt) {
                o->list[k] = o->f3D2;
                o->cnt = k + 1;
            }
            for (k = 4; k >= 0; k--) {
                o->hist[k + 1] = o->hist[k];
            }
            o->hist[0] = o->f3D2;
        }
    }
    c = o->f3D0;
    d = 0;
    if (c >= 0x10) {
        o->f3C4 = 0;
    } else {
        a2 = (s8)c;
        v = D_800C9A40[a2].f20;
        v += o->f3BA * 2;
        for (; d < 4; d++) {
            if (a2 == o->hist[d]) {
                break;
            }
        }
        v = v * (d + 4);
        v = v / 8;
        o->f3C4 += v;
    }
    id = ((s32)o - (s32)D_800E37C0) * -0x41041041 >> 5;
    if (id != D_800E3700 || D_800C8CD8 == 0) {
        func_8004E2E0(id, D_800C9A40[n].f38);
    }
    c2 = 0;
    for (j = 0; j < 4; j++) {
        if ((s8)o->f3D0 == o->hist[j]) {
            c2++;
        }
    }
    if (c2 < 2) {
        if (D_800C9A40[n].f20 < 0x3E9) {
            c2 = 1;
        } else {
            c2 = 2;
        }
    } else {
        c2 = 0;
    }
    func_8004F410(c2);
}
