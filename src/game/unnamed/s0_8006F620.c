#include "game.h"

typedef struct { u8 r0; u8 key[7]; } Rec_43;

extern Rec_43 *D_800F2110[];
extern Rec_43 *D_800F2128[];

extern Rec_43 D_800CCF20[13];
extern Rec_43 D_800CCF88[8];

static inline u8 cmp(Rec_43 *a, Rec_43 *b)
{
    s32 k;

    for (k = 0; k < 7; k++) {
        if (a->key[k] > b->key[k]) {
            return 0;
        }
        if (a->key[k] < b->key[k]) {
            return 1;
        }
    }
    return 0;
}

void func_8006F620(void)
{
    u8 flag;
    s32 i;
    u32 m;
    s32 off;
    Rec_43 *t, *pa, *pb, *p1;
    Rec_43 tmp;

    t = D_800CCF20;
    p1 = &t[1];
    do {
        flag = 0;
        i = 0;
        pa = t;
        off = 8;
        pb = p1;
        for (; i < sizeof(D_800CCF20) / sizeof(Rec_43) - 2; i++) {
            if (cmp(pa, pb)) {
                flag = 1;
                tmp = *pa;
                *pa = *(Rec_43 *)(off + (u32)t);
                *(Rec_43 *)(off + (u32)t) = tmp;
            }
            pa++;
            off += 8;
            pb++;
        }
    } while (flag);
    for (i = 0; i < 5; i++) {
        for (m = 0; m < sizeof(D_800CCF20) / sizeof(Rec_43); m++) {
            if (D_800CCF20[m].key[0] <= i + 2) {
                break;
            }
        }
        D_800F2110[i] = (Rec_43 *)(m * 8 + (u32)D_800CCF20);
    }

    t = D_800CCF88;
    p1 = &t[1];
    do {
        flag = 0;
        i = 0;
        pa = t;
        off = 8;
        pb = p1;
        for (; i < sizeof(D_800CCF88) / sizeof(Rec_43) - 2; i++) {
            if (cmp(pa, pb)) {
                flag = 1;
                tmp = *pa;
                *pa = *(Rec_43 *)(off + (u32)t);
                *(Rec_43 *)(off + (u32)t) = tmp;
            }
            pa++;
            off += 8;
            pb++;
        }
    } while (flag);
    for (i = 0; i < 5; i++) {
        for (m = 0; m < sizeof(D_800CCF88) / sizeof(Rec_43); m++) {
            if (D_800CCF88[m].key[0] <= i + 2) {
                break;
            }
        }
        D_800F2128[i] = (Rec_43 *)(m * 8 + (u32)D_800CCF88);
    }
}

struct Ent_78 {
    u8 pad0[0x3D3];
    s8 f3D3;
    s8 hist[8];
    u8 pad1[0x3FC - 0x3DC];
    u8 idx;
    u8 cnt;
    u8 pad2[0x408 - 0x3FE];
    s32 f408;
    u8 pad3[0x5B0 - 0x40C];
    u32 btn;
    u32 prev;
    u8 pad4[0x7E0 - 0x5B8];
};

typedef struct {
    u8 pad[0x1C];
    s32 mask;
    u8 rest[0x1C];
} Move;

typedef struct {
    u8 pad[0x1C];
    s32 sh[1];
} Cfg_11;

extern Move D_800C9A40_12[];
extern Ent_78 D_800E37C0_89[];
extern Cfg_11 D_800EFD70_51;

void func_8006F8D4(Ent_78 *e)
{
    u32 w;
    s32 x, y, dir;
    u8 flag;
    s32 k;
    s32 i;
    u8 **tab;
    u8 *p;

    if ((e->prev & 0x100) && !(e->btn & 0x100)) {
        e->f3D3 = -1;
        e->hist[e->idx] = 1;
        return;
    }
    w = e->btn;
    y = w >> 28;
    if (w & 2) y = -y;
    x = ((s32)w >> 24) & 0xF;
    if (w & 8) x = -x;
    if (abs(x) < 8 && abs(y) < 8) {
        dir = 2;
    } else {
        dir = 3;
        if (y < x) dir = 5;
        if (-y < x) dir++;
    }
    flag = 0;
    if (e->hist[e->idx] == dir && e->cnt < 0xF0) {
        e->cnt++;
    } else {
        flag = 1;
        e->idx = (e->idx - 1) & 7;
        e->hist[e->idx] = dir;
        e->cnt = 0;
    }
    if ((e->btn & 0x100) && (!(e->prev & 0x100) || flag)) {
        if (e->btn & 0x10000) {
            p = D_800F2128[dir - 2];
        } else {
            p = D_800F2110[dir - 2];
        }
        do {
            for (i = 0; i < 7; i++) {
                if (p[i + 1] == 0 &&
                    (((D_800C9A40_12[p[0]].mask >> D_800EFD70_51.sh[e - D_800E37C0_89]) & 1) ||
                     (D_800C2280 & 0x200))) {
                    e->f408 = D_800E76D0;
                    e->f3D3 = p[0];
                }
                if ((p + i)[1] != e->hist[(e->idx + i) & 7]) break;
            }
            if ((p + i)[1] < e->hist[(e->idx + i) & 7]) return;
            p += 8;
        } while (i != 0);
    }
}

struct Obj_56 {
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
};

typedef struct {
    s32 pad[3];
    s32 *p;
    s32 rest[11];
} TblEnt_2;

extern TblEnt_2 D_800C9A40_13[];

void func_8006FB3C(Obj_56 *o)
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
                lim = D_800C9A40_13[o->s3D0].p[1] << 8;
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

typedef struct {
    s32 f0;
    u8 p4[0x1C];
    s32 f20;
    u8 p24[8];
    s32 f2C;
    u8 p30[8];
    s32 f38;
} Tab_7;

struct Ent_79 {
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
};

extern Tab_7 D_800C9A40_14[];

void func_8006FE84(Ent_79 *o, s32 n)
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
    } else if (!(D_800C9A40_14[n].f0 & 0x10)) {
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
    u = D_800C9A40_14[n].f2C;
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
    if (D_800C9A40_14[n].f38 == -1) {
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
        v = D_800C9A40_14[a2].f20;
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
        func_8004E2E0(id, D_800C9A40_14[n].f38);
    }
    c2 = 0;
    for (j = 0; j < 4; j++) {
        if ((s8)o->f3D0 == o->hist[j]) {
            c2++;
        }
    }
    if (c2 < 2) {
        if (D_800C9A40_14[n].f20 < 0x3E9) {
            c2 = 1;
        } else {
            c2 = 2;
        }
    } else {
        c2 = 0;
    }
    func_8004F410(c2);
}

typedef struct Tbl_6 {
    u32 flags;
    u8 pad4[8];
    s32 *f0C;
    u8 pad10[0x18];
    s32 f28;
    u8 pad2C[4];
    s32 f30;
    s32 f34;
    u8 pad38[4];
} Tbl_6;

extern Tbl_6 D_800C9A40_15[];

s32 func_800701B4(Obj_57 *p, s32 k)
{
    s32 i;
    s32 top;
    Tbl_6 *e;
    s32 v;
    u32 fl;

    i = p->f3D0;
    if (i < 0) {
        goto ret0;
    }
    e = &D_800C9A40_15[i];
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
                func_800477F0_3(p, -0x22);
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

struct Obj_58 {
    u8 pad0[0x3BA];
    s16 f3ba;
    u8 pad1[0x3C4 - 0x3BC];
    s32 f3c4;
    u8 pad2[0x3D0 - 0x3C8];
    u8 f3d0;
    u8 pad3[0x3E4 - 0x3D1];
    u8 f3e4[4];
};

typedef struct {
    u8 pad[0x20];
    s32 f20;
    u8 rest[0x3C - 0x24];
} Ent_80;

extern Ent_80 D_800C9A40_16[];

void func_8007047C(Obj_58 *o) {
    s32 base;
    s8 idx;
    s32 k;
    s32 t;
    if (o->f3d0 >= 16) {
        o->f3c4 = 0;
        return;
    }
    idx = (s8)o->f3d0;
    base = D_800C9A40_16[idx].f20;
    base += o->f3ba * 2;
    for (k = 0; k < 4; k++) {
        if (idx == o->f3e4[k]) {
            break;
        }
    }
    base = base * (k + 4);
    t = base;
    if (base < 0) {
        t = base + 7;
    }
    base = t >> 3;
    o->f3c4 += base;
}

s32 func_80070514(u8 *a, u8 *b) {
    s32 i;

    for (i = 0; i < 7; i++) {
        u8 *pa = a + i;
        u8 *pb = b + i;

        if (pa[1] > pb[1]) {
            return 0;
        }
        if (pb[1] > pa[1]) {
            return 1;
        }
    }
    return 0;
}

void func_80070560(s32 arg0)
{
}

struct Obj_59 {
    u8 pad[0x3D2];
    s8 cur;
    u8 pad2[0x3E4 - 0x3D3];
    u8 hist[6];
    u8 n;
    s8 list[16];
};

s32 func_80070568(Obj_59 *p)
{
    s32 ret = 0;
    s32 i;

    if (p->cur < 16) {
        for (i = 0; i < p->n; i++) {
            if (p->cur == p->list[i]) {
                break;
            }
        }
        if (i == p->n) {
            ret = 1;
            p->list[i] = p->cur;
            p->n = i + 1;
        }
        for (i = 4; i >= 0; i--) { p->hist[i + 1] = p->hist[i]; }
        p->hist[0] = p->cur;
    }
    return ret;
}
