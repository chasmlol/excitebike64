#include "ctx.h"

typedef struct {
    u8 pad0[0x8];
    s32 f8;             /* 0x8 */
    u8 padC[0x14 - 0xC];
    s32 f14;            /* 0x14 */
    u8 pad18[0x44 - 0x18];
    u8 f44;             /* 0x44 */
    u8 f45;             /* 0x45 */
    u8 pad46[0x48 - 0x46];
    f32 f48;            /* 0x48 */
    f32 f4C;            /* 0x4C */
    f32 f50;            /* 0x50 */
    s32 f54;            /* 0x54 */
    f32 f58;            /* 0x58 */
    f32 f5C;            /* 0x5C */
    u8 pad60[0x74 - 0x60];
    f32 f74;            /* 0x74 */
    u8 pad78[0xBC - 0x78];
    f32 fBC;            /* 0xBC */
    f32 fC0;            /* 0xC0 */
    u8 padC4[0x1C4 - 0xC4];
    u8 f1C4;            /* 0x1C4 */
    u8 pad1C5[0x36C - 0x1C5];
    u8 f36C;            /* 0x36C */
    u8 pad36D[0x3D0 - 0x36D];
    s8 f3D0;            /* 0x3D0 */
    s8 f3D1;            /* 0x3D1 */
    u8 pad3D2[0x416 - 0x3D2];
    s16 f416;           /* 0x416 */
    u8 pad418[0x41D - 0x418];
    u8 f41D;            /* 0x41D */
    u8 pad41E[0x420 - 0x41E];
    f32 f420;           /* 0x420 */
    u8 pad424[0x42C - 0x424];
    f32 f42C;           /* 0x42C */
    u8 pad430[0x5CD - 0x430];
    u8 f5CD;            /* 0x5CD */
    u8 pad5CE[0x620 - 0x5CE];
    u8 f620;            /* 0x620 */
    u8 pad621[0x624 - 0x621];
    s16 f624;           /* 0x624 */
    u8 pad626[0x628 - 0x626];
    s16 f628;           /* 0x628 */
    u8 pad62A[0x630 - 0x62A];
    f32 f630;           /* 0x630 */
    u8 pad634[0x648 - 0x634];
    s32 f648;           /* 0x648 */
    u8 pad64C[0x654 - 0x64C];
    u8 f654;            /* 0x654 */
    u8 pad655[0x662 - 0x655];
    s16 f662;           /* 0x662 */
    u8 pad664[0x6E8 - 0x664];
    u8 f6E8;            /* 0x6E8 */
    u8 pad6E9[0x6ED - 0x6E9];
    s8 f6ED;            /* 0x6ED */
    u8 pad6EE[0x744 - 0x6EE];
    s32 f744;           /* 0x744 */
    s32 f748;           /* 0x748 */
    u8 pad74C[0x7A4 - 0x74C];
    u8 f7A4;            /* 0x7A4 */
    u8 f7A5;            /* 0x7A5 */
    u8 pad7A6[0x7A9 - 0x7A6];
    u8 f7A9;            /* 0x7A9 */
    u8 pad7AA[0x7B0 - 0x7AA];
    s32 f7B0;           /* 0x7B0 */
    u8 pad7B4[0x7D8 - 0x7B4];
    f32 f7D8;           /* 0x7D8 */
    u8 pad7DC[0x7E0 - 0x7DC];
} Bike;

typedef struct {
    f32 f0;
    f32 f4;
    f32 f8;
    u8 padC[0x6C - 0xC];
} Seg;

typedef struct {
    u8 pad0[0x22];
    s16 n;
} Race;

extern Bike D_800E37C0[8];
extern Bike *D_800F0C10[];
extern Race D_800F05A8;
extern Seg *D_800EE498;
extern s32 D_800EE490;
extern s32 D_800E76D0;
extern s32 D_800F1504;

void func_8005E9A4(s32 a0);
u8 func_800138A4(void);
float func_800228A0(float *p);
void func_80061E20(u8 *p);
void func_8002D384(s32 p, s32 q);
void func_800635A4(void);
void func_8006B2AC(u8 *p);
void func_8001C780(Bike *b);
void func_800511B8(s32 a0);

void func_8006B4EC(void)
{
    Bike *p;
    Bike *a;
    Bike *b;
    Bike *q;
    Seg *u;
    s16 m;
    Bike *e;
    Seg *t;
    s32 i;
    s32 j;
    s32 k;
    s16 n;
    u8 sw;
    u8 cnt;
    u8 r;
    s32 now;
    f32 z;
    f32 h;
    f32 dist;
    f32 d;
    f32 v;
    f32 lo;

    for (p = D_800E37C0; p < &D_800E37C0[8]; p++) {
        if (p->f7A4 == 0 && p->f6E8 != 0 && p->f44 == 0 && p->f744 < 3) {
            func_8006B2AC((u8 *)p);
        }
    }

    do {
        sw = 0;
        for (i = 0; i < D_800F05A8.n - 1; i++) {
            a = D_800F0C10[i];
            b = D_800F0C10[i + 1];
            if (a->f630 < b->f630 && (a->f7A5 | b->f7A5) == 0) {
                sw = 1;
                D_800F0C10[i] = b;
                D_800F0C10[i + 1] = a;
            }
        }
    } while (sw);

    for (i = 0; i < D_800F05A8.n; i++) {
        D_800F0C10[i]->f662 = i;
    }

    func_800635A4();

    if (D_800E37C0[0].f50 < -970.375f) {
        D_800E37C0[0].f50 -= -997.15717f;
        D_800E37C0[0].f8 = D_800E37C0[0].f8 - -997157.2f;
        D_800E37C0[0].f14 = D_800E37C0[0].f14 - -997157.2f;
        D_800E37C0[0].f74 -= -997.15717f;
        func_8002D384(0, 0xF3725);
        q = D_800E37C0;
        j = 0;
        while (q->f50 < D_800EE498[j].f8) {
            j++;
        }
        q->f628 = j;
        func_8005E9A4(0);
        func_800511B8(0);
    }

    for (e = &D_800E37C0[1]; e < &D_800E37C0[8]; e++) {
        if (e->f7A4 == 0 && e->f50 < -970.375f) {
            if (e->f6ED == 0) {
                if (e - D_800E37C0 < 4) {
                    func_800511B8(e - D_800E37C0);
                }
                e->f6ED = 1;
            }
            d = e->f50 - D_800E37C0[0].f50;
            if (d < -70.0f) {
                e->f6ED = 0;
                e->f50 -= -997.15717f;
                e->f74 -= -997.15717f;
                {
                    s32 jj = 0;
                    t = D_800EE498;
                    while (e->f50 < t[jj].f8) {
                        jj++;
                    }
                    e->f628 = jj;
                }
                func_8005E9A4(e - D_800E37C0);
            }
        }
    }

    cnt = 0;
    for (e = D_800E37C0; e < &D_800E37C0[8]; e++) {
        if (e->f6E8 == 0 && e->f7A5 == 0) {
            cnt++;
            z = e->f50;
        }
    }

    if (cnt != 0) {
        k = D_800E37C0[0].f628 + 4;
        dist = D_800E37C0[0].f50 - D_800EE498[k].f8;
        h = D_800E37C0[0].f4C;
        if (!(dist > 100.0f)) goto body;
        h = 4.0f;
        goto found;
        do {
        body:
            v = D_800EE498[k].f4;
            if (h < v) {
                h = v;
            }
            if (h - D_800E37C0[0].f4C > 1.5f && h - v > 1.5f) {
                goto found;
            }
            k = (k + 1) % D_800EE490;
            dist = D_800E37C0[0].f50 - D_800EE498[k].f8;
        } while (!(dist > 100.0f));
        h = 4.0f;
    found:
        lo = -10.0f;
        for (p = &D_800E37C0[6]; p < &D_800E37C0[8]; p++) {
            if (!(h < p->f4C) && D_800E76D0 - D_800F1504 > 60 && p->f6E8 != 0 &&
                D_800E76D0 - p->f7B0 > 179 && p->f7A5 == 0) {
                d = z - p->f50;
                if (d < lo || dist < d) {
                    now = D_800E76D0;
                    D_800F1504 = now;
                    if (now & 1) {
                        p->f7D8 = 0.2f;
                        p->f50 = z - dist;
                    } else {
                        p->f7D8 = 0.2f;
                        p->f50 = z - lo;
                    }
                    func_80061E20((u8 *)&p->f48);
                    p->f1C4 = 1;
                    p->f7A9 = func_800138A4() & 3;
                    p->f4C = 40.0f;
                    p->f48 = p->f7A9 * 2.178f;
                    p->f4C = func_800228A0(&p->f48);
                    func_8001C780(p);
                    p->f420 = 10.0f;
                    p->f42C = -p->f48;
                    p->f58 = p->f420 * p->fBC;
                    p->f5C = p->f420 * p->fC0;
                    p->f7B0 = D_800E76D0;
                    p->f54 = 0;
                    p->f620 = 0;
                    p->f41D = 0;
                    p->f36C = 0;
                    p->f748 = 0;
                    p->f744 = 0;
                    p->f5CD = 0;
                    p->f416 = 0;
                    p->f45 = 0;
                    p->f624 = 0;
                    p->f44 = 0;
                    p->f648 = 0;
                    p->f654 = 0;
                    p->f7A4 = 0;
                    p->f3D0 = -1;
                    p->f3D1 = -1;
                }
            }
        }
    }
}
