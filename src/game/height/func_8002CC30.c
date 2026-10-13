#include "ctx.h"

typedef struct Rec {
    f32 x;
    f32 y;
    f32 z;
    u8 p0c[8];
    f32 f14;
    f32 f18;
    u8 p1c[0x2a];
    s16 s46;
    u8 p48[10];
    s16 s52;
    s16 s54;
    s16 s56;
    u8 p58[12];
    u8 b64;
    u8 p65[7];
} Rec;

typedef struct Blk {
    u8 p0[0x6c];
    f32 f6c;
    f32 f70;
    f32 f74;
    u8 p78[0x628 - 0x78];
    s16 s628;
    u8 p62a[0x744 - 0x62a];
    s32 w744;
    u8 p748[0x7e0 - 0x748];
} Blk;

typedef struct Ent {
    s32 f00, f04, f08, f0c, f10, f14;
} Ent;

typedef struct { s32 x, y, z; } V3i;
typedef struct { u8 p0[0x2C]; s32 f2C; u8 p30[0xA0 - 0x30]; } Trk;

extern s32 D_800EE140;
extern Trk *D_800EFD64;
extern Rec *D_800EE498;
extern Blk D_800E37C0[];
extern V3i D_800E8090[];
extern Ent D_800E8030[];
extern Ent D_800E7998[][16];
extern s32 D_800E7978[];
extern s32 D_800E7FF0[];
extern s32 D_800E7F98;
extern Trk D_800CB210[];

void func_8004AB24(Vec3f *param_1, s32 *param_2, f32 *param_3, s32 param_4);
void func_8002D1F0(s32 param_1, s32 param_2);
void func_8002CA70(s32 p1, s32 p2, s32 p3, s32 p4);
void func_8002C7C0(s32 a, s32 b, s32 c);
void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio);

#define REC(i) ((Rec *)((i) * sizeof(Rec) + (u32)D_800EE498))
#define REC2(i) ((Rec *)((u32)D_800EE498 + (i) * sizeof(Rec)))
static inline f32 lerp(f32 a, f32 b, f32 t)
{
    return a + (b - a) * t;
}

static inline f32 lerpi(s32 a, s32 b, f32 t)
{
    return (f32)a + (f32)(b - a) * t;
}

void func_8002CC30(s32 m, s32 n)
{
    Vec3f v;
    s32 idx;
    f32 t;
    s16 a0;
    s32 ratio;
    s16 a1;
    s32 lim;
    Blk *pb;
    V3i *q;
    Rec *pr, *pn;
    s32 nx;
    Rec *tab;
    f32 ax, az, bx, bz, qx, qz;
    s32 k, h, top, d, r, dz;
    f32 f, g, sx, sz;

    if (D_800EE140 != 0) {
        return;
    }
    if (D_800EFD64->f2C == 2) {
        func_8002D1F0(m, n);
        return;
    }
    {
        v.x = (f32)D_800E8090[n].x / 1000.0f;
        v.y = (f32)D_800E8090[n].y / 1000.0f;
        v.z = (f32)D_800E8090[n].z / 1000.0f;
        func_8004AB24(&v, &idx, &t, D_800E37C0[m].s628);
        tab = D_800EE498;
        pr = (Rec *)(idx * sizeof(Rec) + (u32)tab);
        nx = pr->s46;
        D_800E7FF0[n] = pr->b64;
        if (D_800EFD64->f2C == 0) {
            s32 id = D_800EFD64 - D_800CB210;
            if (id == 0x18 || id == 0x15 || id == 0x16) {
                func_8002C7C0(m, n, 1);
            }
            return;
        }
        D_800E7F98 = 0;
        if (pr->s56 < 0) {
            func_8002C7C0(m, n, 0);
            return;
        }
        if (pr->s56 > 0) {
            pn = (Rec *)(nx * sizeof(Rec) + (u32)tab);
            if (pn->s56 > 0) {
                f32 ya, yb;
                D_800E7F98 = 1;
                ya = pr->y * 1000.0f + (f32)pr->s56 * 16.0f;
                yb = pn->y * 1000.0f + (f32)pn->s56 * 16.0f;
                lim = (s32)(lerpi(pr->s52, pn->s52, t) * 16.0f);
                top = (s32)(lerpi(pr->s54, pn->s54, t) * 16.0f);
                h = (s32)lerp(ya, yb, t);
                if (h < D_800E8090[n].y && D_800E37C0[m].w744 < 0x1e) {
                    f32 ex, ez;
                    ex = D_800E37C0[m].f6c * 1000.0f - (f32)D_800E8090[n].x;
                    ez = D_800E37C0[m].f74 * 1000.0f - (f32)D_800E8090[n].z;
                    ex = __builtin_sqrtf(ex * ex + ez * ez);
                    g = D_800E37C0[m].f70;
                    g *= 1000.0f;
                    r = (s32)ex;
                    g -= (f32)D_800E8090[n].y;
                    func_8001254C((s32)g, r, &a0, &ratio);
                    g += (f32)(D_800E8090[n].y - h);
                    func_8001254C((s32)g, r, &a1, &ratio);
                    D_800E7978[n] -= a1 - a0;
                    for (k = 0; k < 16; k++) {
                        D_800E7998[n][k].f04 += h - D_800E8090[n].y;
                        D_800E7998[n][k].f0c -= a1 - a0;
                    }
                    D_800E8030[n].f04 += (h - D_800E8090[n].y) * 16;
                    D_800E8090[n].y = h;
                }
                {
                Rec *pr2, *pn2;
                Rec *tab2;
                f32 ax, az;
                tab2 = D_800EE498;
                pr2 = (Rec *)(idx * sizeof(Rec) + (u32)tab2);
                nx = pr2->s46;
                pn2 = (Rec *)(nx * sizeof(Rec) + (u32)tab2);
                g = lerp(pr2->f14, pn2->f14, t);
                f = lerp(pr2->f18, pn2->f18, t);
                ax = pr2->x * 1000.0f;
                az = pr2->z * 1000.0f;
                dz = (s32)(lerp((f32)D_800E8090[n].x - ax, (f32)D_800E8090[n].x - pn2->x * 1000.0f, t) * g +
                           lerp((f32)D_800E8090[n].z - az, (f32)D_800E8090[n].z - pn2->z * 1000.0f, t) * f);
                }
                if (top < dz) {
                    d = top - dz;
                } else if (dz < -lim) {
                    d = -lim - dz;
                } else {
                    func_8002D1F0(m, n);
                    return;
                }
                func_8002CA70(n, m, (s32)((f32)d * g), (s32)((f32)d * f));
            }
        }
    }
    func_8002D1F0(m, n);
}
