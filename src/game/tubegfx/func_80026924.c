#include "ctx.h"

typedef struct { s32 f0, f4, f8, fC, f10, f14, f18, f1C; } Rec;
typedef struct { u32 count; Rec r[1]; } List;
typedef struct { s32 x, y, z; } Pos;
typedef struct { u8 pad[0x5AA]; s16 f5AA; u8 pad2[0x7E0 - 0x5AC]; } Player;
typedef struct { u8 pad[0xA0]; } Track;
typedef struct { s32 a, b, c, d; } Quad;

extern s32 D_800C9650;
extern s32 D_800C964C;
extern s32 D_800E1898;
extern Track *D_800EFD64;
extern Track D_800CB210[];
extern s16 D_800E3D6A;
extern s32 D_800E15E0[];
extern Pos D_800E8090[];
extern Player D_800E37C0[];
extern s32 D_800E8508;
extern s32 D_800E8958;
extern s32 D_800E77FC;
extern u16 **D_800C963C;
extern s32 D_800E7770[];
extern s32 D_800E8000[];
extern s16 D_800C2C60[];
extern Quad *D_800E7814;

s32 func_8002E920(s32 x, s32 y, s32 z);

void func_80026924(List *l, s32 n)
{
    s32 k;
    s32 i;
    s32 base;
    s32 px, py, pz;
    s32 cnt;
    s32 bit;
    u16 *e;
    s32 n1, tot;
    s32 h;
    s32 lim;
    s32 ang;
    s32 sn, cs;
    s32 d[4];
    s32 w[4];

    k = n;
    if (D_800C9650 != -1) {
        k = D_800C9650;
    }
    D_800C964C = 0x68;
    if (D_800E1898 == 2) {
        D_800C964C = 0x9C;
    }
    if (D_800EFD64 - D_800CB210 == 20 && D_800E3D6A == 4) {
        n1 = D_800E15E0[0];
        if (n1 >= 0 && n1 < l->count) {
            s32 ref = l->r[n1].f8 + l->r[n1].f10;
            for (i = 0; i < l->count; i++) {
                s32 v = l->r[i].f1C & (-4 << (n * 2));
                s32 dd = l->r[i].f8 + l->r[i].f10;
                l->r[i].f1C = v;
                if (dd >= ref - 7500 && dd <= ref + 2500) {
                    l->r[i].f1C = v | (1 << (n * 2));
                }
            }
        }
    } else {
        base = 0;
        if (D_800E1898 != 1) {
            base = l->count;
        }
        px = D_800E8090[n].x;
        py = D_800E8090[n].y;
        pz = D_800E8090[n].z;
        cnt = 0;
        if (D_800E37C0[n].f5AA == 9) {
            D_800E77FC = D_800E15E0[k];
        } else {
            D_800E8508 = 0;
            func_8002E920(px, py, pz);
            if (D_800E8508 == 0) {
                D_800E77FC = D_800E15E0[k];
                D_800E8508 = 1;
            } else {
                D_800E77FC = D_800E8958;
            }
        }
        bit = 1 << (n * 2);
        for (i = 0; i < l->count; i++) {
            l->r[i].f1C &= ~(3 << (n * 2));
        }
        e = D_800C963C[D_800E77FC + base];
        n1 = e[0];
        tot = n1 + e[1];
        for (i = 0; i < tot; i++) {
            l->r[e[i + 2]].f1C |= bit;
        }
        for (i = 0; i < n1; i++) {
            l->r[e[i + 2]].f1C |= 2 << (n * 2);
        }
        px >>= 4;
        pz >>= 4;
        l->r[D_800E77FC].f1C |= bit;
        lim = 0xC48A / (0x3EE - D_800E7770[n]);
        ang = D_800E8000[n] >> 4;
        sn = D_800C2C60[ang & 0xFFF];
        cs = D_800C2C60[(ang - 0x400) & 0xFFF];
        for (i = 0; i < l->count; i++) {
            Rec *r = &l->r[i];
            Quad *q = &D_800E7814[i];
            if (r->f1C & bit) {
                cnt++;
                if (D_800E8508 == 0 && cnt >= 16) {
                    r->f1C &= ~bit;
                    cnt--;
                } else {
                    s32 t, u, f, x, z;
                    x = r->f4;
                    z = r->f8;
                    t = (sn * (z - pz) - cs * (x - px)) >> 14;
                    x = r->fC - x;
                    h = (-cs * x) >> 14;
                    f = (sn * (r->f10 - z)) >> 14;
                    u = t + h;
                    d[0] = t;
                    d[1] = u;
                    d[2] = t + f;
                    d[3] = u + f;
                    if (d[0] < 11 && d[1] < 11 && d[2] < 11 && d[3] < 11) {
                        cnt--;
                        r->f1C &= ~(3 << (n * 2));
                    } else {
                        if (d[0] >= lim || d[1] >= lim || d[2] >= lim || d[3] >= lim) {
                            r->f1C |= 0x100 << n;
                        } else {
                            r->f1C &= ~(0x100 << n);
                        }
                        if (D_800EFD64 - D_800CB210 != 20 && d[0] > 300 && d[1] > 300 && d[2] > 300 && d[3] > 300) {
                            s32 y = q->a;
                            s32 u2;
                            t = (sn * (y - px) + cs * (q->c - pz)) >> 14;
                            y = q->b - y;
                            h = (sn * y) >> 14;
                            u2 = t + h;
                            f = (cs * (q->d - q->c)) >> 14;
                            w[0] = (t << 8) / d[0];
                            w[1] = (u2 << 8) / d[1];
                            w[2] = ((t + f) << 8) / d[2];
                            w[3] = ((u2 + f) << 8) / d[3];
                            if ((w[0] < -D_800C964C && w[1] < -D_800C964C && w[2] < -D_800C964C && w[3] < -D_800C964C) ||
                                (D_800C964C < w[0] && D_800C964C < w[1] && D_800C964C < w[2] && D_800C964C < w[3])) {
                                cnt--;
                                r->f1C &= ~(3 << (n * 2));
                            }
                        }
                    }
                }
            }
        }
    }
}
