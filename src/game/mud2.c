#include "game.h"

typedef struct Part {
    s16 life;
    s16 w2;
    u8 typ;
    u8 pad5[7];
    f32 px, py, pz;
    f32 vx, vy, vz;
} Part;

extern Part *D_800E9214;

void func_80036230(void)
{
    Part *p;
    Part *base;
    s32 i;
    s32 t;
    s32 *q;

    if (D_800EE140 == 0 && D_800E1898 < 4 && D_800E3710 == 0) {
        func_80037290();
        i = 0;
        base = D_800E9214;
        do {
            p = (Part *)((s32)base + i * 0x24);
            if (p->life >= 0) {
                p->life = p->life - 1;
                t = p->typ;
                p->px = p->px + p->vx;
                p->py = p->py + p->vy;
                p->pz = p->pz + p->vz;
                if (t == 1) {
                    p->vy = p->vy - 0.008166667f;
                } else if (t == 3) {
                    p->vx = p->vx * 0.99f;
                    p->vz = p->vz * 0.99f;
                } else if (t == 2) {
                    p->vy = p->vy - 0.0020416668f;
                }
            }
            i = i + 1;
        } while (i < 0x60);
        i = 0;
        q = D_800E9228;
        do {
            i = i + 1;
            if (*q != 0) {
                *q = *q - 1;
            }
            q = q + 1;
        } while (i < 0x10);
    }
}

typedef struct {
    s16 life;
    s16 life0;
    u8 type;
    u8 b;
    u8 col[3];
    u8 pad;
    V3 pos;
    V3 vel;
} Part_2;

extern Part_2 *D_800E9214_2;

void func_80036374(s32 type, u8 b, V3 *pos, V3 *vel, u8 *col)
{
    s32 i;
    Part_2 *p;

    for (i = D_800E9210 + 1; ; i++) {
        if (i >= 0x60) i = 0;
        if (D_800E9214_2[i].life <= 0) {
            D_800E9214_2[i].pos = *pos;
            D_800E9214_2[i].vel = *vel;
            D_800E9214_2[i].b = b;
            if (type == 1) {
                D_800E9214_2[i].life = (s16)(vel->y * 122.448975f + 1.0f) * 2;
            } else if (type == 2) {
                D_800E9214_2[i].life = 15;
            } else if (type == 5) {
                D_800E9214_2[i].life = 10;
            } else if (type == 3) {
                D_800E9214_2[i].life = 20;
            } else if (type == 4) {
                D_800E9214_2[i].life = 2;
            } else if (type == 6) {
                D_800E9214_2[i].life = 120;
            } else {
                D_800E9214_2[i].life = 15;
            }
            p = (Part_2 *)(i * sizeof(Part_2) + (u32)D_800E9214_2);
            p->type = type;
            p->life0 = p->life;
            D_800E9214_2[i].col[0] = col[0];
            D_800E9214_2[i].col[1] = col[1];
            D_800E9214_2[i].col[2] = col[2];
            D_800E9210 = i;
            return;
        }
        if (i == D_800E9210) return;
    }
}

struct Obj_29 {
    u8 pad0[0x48];
    f32 v48[3];
    f32 v54[3];
    u8 pad60[0x40];
    f32 vA0[3];
    u8 padAC[0x2A4];
    s32 cnt350;
    f32 v354[3];
};

void func_80036544(Obj_29 *param_1)
{
    f32 o[3];
    f32 t[3];
    u32 r;
    s32 iv;

    if (D_800E1898 == 1 && param_1->cnt350 != 0) {
        param_1->cnt350 = param_1->cnt350 - 1;
        if ((void *)param_1 == (void *)D_800E37C0) {
            func_8001327C(param_1->v354, param_1->vA0, o);
            o[0] = param_1->v48[0] + o[0];
            o[1] = param_1->v48[1] + o[1];
            o[2] = param_1->v48[2] + o[2];
            t[0] = param_1->v54[0] * 0.016666668f;
            t[1] = param_1->v54[1] * 0.016666668f;
            t[2] = param_1->v54[2] * 0.016666668f;
            r = func_8000EFE8();
            if (0.0f < param_1->v354[0]) {
                iv = r + 0x80;
            } else {
                iv = r - 0x80;
            }
            t[0] = t[0] + param_1->vA0[0] * ((f32)iv * 0.00012207031f);
            t[2] = t[2] + param_1->vA0[2] * ((f32)iv * 0.00012207031f);
            r = func_8000EFE8();
            t[1] = t[1] + (f32)(s32)(r - 0x40) * 0.02f * 0.0078125f;
            func_80036374(2, 0, o, t, D_800C9994);
        }
    }
    return;
}

typedef struct { u8 a; u8 b; u16 z; u8 *p; } Rec_19;
typedef struct { u32 n; u32 pad; Rec_19 r[1]; } Tab_3;

extern Tab_3 *D_800E92A0;
extern Tab_3 *D_800E9290;

void func_800366AC(void *param_1)
{
    s32 iVar4;
    s32 j;
    s32 i;
    u8 *pb;

    D_800E92A0 = (Tab_3 *)func_800BF79C(param_1, D_800D0E0C);
    iVar4 = D_800E92A0->r[0].a * (D_800E92A0->r[0].b << 3);
    if (D_800E92A0->n != 0) {
        i = 0;
        do {
            j = 0;
            if (iVar4 != 0) {
                                do {
                    pb = D_800E92A0->r[i].p;
                    pb = pb + j;
                    *pb = *pb >> 4;
                    pb = D_800E92A0->r[i].p;
                    pb = pb + j;
                    *pb = *pb & 0xf;
                    pb = D_800E92A0->r[i].p;
                    pb = pb + j;
                    j++;
                    *pb = *pb | 0xf0;
                } while (j < iVar4);
            }
            i++;
        } while (i < D_800E92A0->n);
    }
    D_800E926C = func_800BF79C(param_1, D_800D0DB4);
    D_800E9290 = (Tab_3 *)func_800BF79C(param_1, D_800D0D20);
    i = 0;
    if (D_800E9290->n != 0) {
        do {
            iVar4 = D_800E9290->r[i].a * (D_800E9290->r[i].b << 3);
            j = 0;
            if (iVar4 != 0) {
                do {
                    pb = D_800E9290->r[i].p;
                    pb = pb + j;
                    j++;
                    *pb = *pb | 0xf0;
                } while (j < iVar4);
            }
            i++;
        } while (i < D_800E9290->n);
    }
    D_800E9270 = func_800BF79C(param_1, D_800D0924);
    func_80014948(D_800E9270);
    D_800E929C = func_800BF79C(param_1, D_800D0DAC);
    func_80014948(D_800E929C);
    D_800E92C8_2 = func_800BF79C(param_1, D_800D10B4);
    func_80014948(D_800E92C8_2);
}

typedef struct {
    u8 pad0[4];
    s32 f458;
    u8 pad8[0x40 - 8];
    V3 pos;
} Sub_9;

typedef struct {
    u8 pad0[0x34];
    u16 ang;
    u8 pad36[0x44 - 0x36];
    u8 f44;
    u8 pad45[0x54 - 0x45];
    V3 f54;
    V3 vel;
    u8 pad6C[0xB8 - 0x6C];
    V3 fB8;
    u8 padC4[0x420 - 0xC4];
    f32 f420;
    u8 pad424[0x454 - 0x424];
    Sub_9 sub;
    u8 pad4A0[0x4E4 - 0x4A0];
    Trk *model;
    u8 pad4E8[0x5B0 - 0x4E8];
    u32 f5B0;
    u8 pad5B4[4];
    s32 f5B8;
    u8 pad5BC[0x5CF - 0x5BC];
    u8 f5CF;
    u8 pad5D0[0x6E8 - 0x5D0];
    u8 f6E8;
    u8 pad6E9[0x774 - 0x6E9];
    s32 f774;
    u8 pad778[0x7A5 - 0x778];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike_20;

typedef struct {
    u8 b[4];
} Col;

extern Col D_800DC0E8_2[];
extern Bike_20 D_800E37C0_35[];
extern Col D_800E1B50_2[];

void func_800368A4(void)
{
    V3 pad0;
    V3 A;
    V3 B;
    V3 pad1;
    V3 pad2;
    s32 i, j;
    s32 bA;
    s32 bB, bC;
    s32 ang;
    f32 s, c, d;
    f32 kr;

    if (D_800EE140 != 0 || D_800E1898 >= 4 || D_800E3710 != 0) {
        return;
    }
    if (++D_800C999C & 3) {
        return;
    }
    kr = 12800.0f;
    for (i = 0; i < 8; i++) {
        Bike_20 *b = &D_800E37C0_35[i];
        if (b->f7A5 != 0) continue;
        if (D_800E18A0[i] == 0) continue;
        bA = D_800E1B50_2[i].b[3] >> 4;
        if (b->f44 != 0) continue;
        if (b->sub.f458 <= 0 && (b->model->flags & 2)) {
            A.x = 0.0f;
            A.y = 0.0f;
            A.z = 0.0f;
            B = b->sub.pos;
            B.y += 0.3f;
            if (b->f420 < 8.0f) {
                B.x -= (8.0f - b->f420) * b->fB8.x * 0.125f;
                B.z -= (8.0f - b->f420) * b->fB8.z * 0.125f;
            }
            bC = 0;
            bB = 0;
            if ((b->f5B0 & 0x20000) || b->f5CF == 1) {
                bB = 1;
            }
            if (D_800E76D0 - b->f774 < 15) {
                bC = 1;
            } else if (D_800E76D0 - b->f5B8 < 10) {
                bC = 1;
            } else if (b->f5CF == 2) {
                bC = 1;
            }
            if (bB && (D_800E7FC8 & 1)) {
                bC = 1;
            }
            if (bC) {
                if (D_800F05A8_4 >= 5 || (D_800E76D0 & 1)) {
                    A.y = 0.07f;
                    A.z += (s32)(func_8000EFE8() - 0x80) / kr;
                    A.x += (s32)(func_8000EFE8() - 0x80) / kr;
                    ang = (s16)b->ang >> 4;
                    s = -D_800C2C60[(ang - 0x400) & 0xFFF] * 6.1035156e-05f;
                    c = D_800C2C60[ang & 0xFFF] * 6.1035156e-05f;
                    d = b->vel.x * c - b->vel.z * s;
                    A.x -= c * d / 250.0f;
                    A.z += s * d / 250.0f;
                    func_80036374_2(0, bA, &B, &A, D_800DC0E8_2[i].b);
                }
            } else if (bB) {
                func_80036374_2(0, bA, &B, &A, D_800DC0E8_2[i].b);
            }
        }
        if (((b->f5B0 & 0x20000) || D_800E76D0 - b->f774 < 10) && (b->model->flags & 4)) {
            if (b->f6E8 == 0 || (func_8000EFE8() & 1)) {
                for (j = 0; j < 2; j++) {
                    Sub_9 *sp;
                    A = D_800E37C0_35[i].f54;
                    A.x = A.x / 80.0f + (((s32)func_8000EFE8() - 128.0f) * 0.01f) * 0.0078125f;
                    A.y = ((s32)func_8000EFE8() / 50.0f + 5.0f) / 100.0f;
                    A.z = A.z / 80.0f + (((s32)func_8000EFE8() - 128.0f) * 0.01f) * 0.0078125f;
                    sp = &D_800E37C0_35[i].sub;
                    func_80036374_2(1, bA, &sp->pos, &A, D_800DC0E8_2[i].b);
                }
            }
        }
    }
}

void func_80036DF4(u8 *p)
{
    volatile s32 pad[4];
    f32 pt[3];
    f32 A[3];
    f32 B[3];
    f32 C[3];
    s32 loc[2];
    s32 idx;
    s32 mv;
    f32 lim;
    f32 t;

    if (p[0x7a5] != 0) goto done;
    idx = ((s32)p - (s32)D_800E37C0) * -0x41041041 >> 5;
    mv = *(s32 *)(p + 0x780);
    if (!(1.0f < *(f32 *)(p + 0x420))) goto done;
    if (!(D_800C9080_2[mv].flags & 8)) goto done;

    *(T3 *)pt = *(T3 *)(p + 0x48);
    if (!func_8002733C((s32)(pt[0] * 1000.0f), (s32)(pt[2] * 1000.0f), loc)) goto done;

    lim = (f32)loc[0] / 1000.0f;
    A[0] = 0.0f;
    A[1] = 0.8f;
    A[2] = 1.0f;
    func_8001327C(A, (f32 *)(p + 0xA0), B);
    B[0] = B[0] + *(f32 *)(p + 0x48);
    B[1] = B[1] + *(f32 *)(p + 0x4C);
    B[2] = B[2] + *(f32 *)(p + 0x50);
    if (B[1] < lim) goto done;

    A[0] = 0.0f;
    A[1] = -0.3f;
    A[2] = 1.0f;
    func_8001327C(A, (f32 *)(p + 0xA0), C);
    C[0] = C[0] + *(f32 *)(p + 0x48);
    C[1] = C[1] + *(f32 *)(p + 0x4C);
    C[2] = C[2] + *(f32 *)(p + 0x50);
    if (lim < C[1]) {
        A[0] = 0.0f;
        A[1] = -0.3f;
        A[2] = -1.0f;
        func_8001327C(A, (f32 *)(p + 0xA0), B);
        B[0] = B[0] + *(f32 *)(p + 0x48);
        B[1] = B[1] + *(f32 *)(p + 0x4C);
        B[2] = B[2] + *(f32 *)(p + 0x50);
        if (lim < B[1]) goto done;
    }

    t = (lim - C[1]) / (B[1] - C[1]);
    pt[0] = C[0] + t * (B[0] - C[0]);
    pt[1] = lim;
    pt[2] = C[2] + t * (B[2] - C[2]);

    if (D_800E18A0[idx] != 0 && (D_800E76D0 & 3) == 0) {
        A[0] = (f32)*(s16 *)((u8 *)D_800C2C60 + ((*(s32 *)(p + 0x20) >> 3) & 0x1FFE)) / 655360.0f;
        A[1] = 0.0f;
        A[2] = (f32)*(s16 *)((u8 *)D_800C2C60 + ((((*(s32 *)(p + 0x20) >> 4) - 0x400) & 0xFFF) << 1)) / 655360.0f;
        func_80036374_3(3, mv, pt, A, D_800C9994);
        A[0] = -(f32)*(s16 *)((u8 *)D_800C2C60 + ((*(s32 *)(p + 0x20) >> 3) & 0x1FFE)) / 655360.0f;
        A[1] = 0.0f;
        A[2] = -(f32)*(s16 *)((u8 *)D_800C2C60 + ((((*(s32 *)(p + 0x20) >> 4) - 0x400) & 0xFFF) << 1)) / 655360.0f;
        func_80036374_3(3, mv, pt, A, D_800C9994);
    }

    if ((D_800C9080_2[mv].flags & 0x180) != 0) {
        if (D_800E18A0[idx] != 0) {
            func_8002845C(idx, (s32)(pt[0] * 1000.0f), loc[0], (s32)(pt[2] * 1000.0f));
        }
        if (*(s8 *)(p + 0x45) == 0) {
            *(s8 *)(p + 0x45) = 0x1c;
            func_8002DA48(idx, 1);
        }
    } else {
        if ((s32)(D_800E76D0 - D_800E92A8[idx]) > 0x78) {
            func_80010994(3, idx, 0xff);
            D_800E92A8[idx] = D_800E76D0;
        }
    }

done:
    return;
}

void func_80037290(void)
{
    s32 i;
    s32 z;
    s32 v;
    f32 lf[3];
    s32 li[3];

    if (D_800E1898 < 4) {
        if (D_800E76D0 < D_800E9294) {
            D_800E9294 = -1000;
        }
        if (D_800E76D0 - D_800E9294 < 0x1F) {
            z = 0;
            i = z;
            li[0] = 0;
            li[1] = 0;
            li[2] = 0;
            do {
                i = i + 1;
                v = func_8000EFE8();
                lf[0] = D_800E9278[0] + ((f32)v - 128.0f) * 0.00390625f;
                v = func_8000EFE8();
                lf[1] = D_800E9278[1] + ((f32)v - 128.0f) * 0.0009765625f;
                v = func_8000EFE8();
                lf[2] = D_800E9278[2] + ((f32)v - 128.0f) * 0.00390625f;
                func_80036374_4(0, z, lf, li, &D_800DC0E8[D_800C9998]);
                *(u16 *)(D_800E9214_3 + D_800E9210 * 0x24 + 2) = 0x1E;
            } while (i <= 0);
        }
    }
}

typedef struct {
    s16 life;   /* 0x00 */
    s16 max;    /* 0x02 */
    u8 type;    /* 0x04 */
    u8 color;   /* 0x05 */
    u8 r, g, b; /* 0x06 */
    u8 pad9[3];
    Vec3f pos;  /* 0x0C */
    Vec3f vel;  /* 0x18 */
} Part_3;

typedef struct {
    u32 n;
    u32 pad;
    TexInfo e[1];
} TexTab_2;

typedef struct {
    u8 pad[0x2C];
    u8 r1, g1, b1, a1; /* 0x2C */
    u8 r2, g2, b2;     /* 0x30 */
    u8 pad33[0xD];
} ColDef;

typedef struct {
    s32 x, y, z;
    u8 pad0C[0xC];
    s32 rx, ry, rz; /* 0x18 */
    u8 pad24[0x5B0 - 0x24];
    u32 flags; /* 0x5B0 */
    u8 pad5B4[0x654 - 0x5B4];
    u8 f654;
    u8 pad655[0x7E0 - 0x655];
} Bike_21;

extern Part_3 *D_800E9214_4;
extern TexTab_2 *D_800E92A0_2;
extern TexTab_2 *D_800E9270_2;
extern TexTab_2 *D_800E929C_2;
extern TexTab_2 *D_800E926C_2;
extern TexTab_2 *D_800E9290_2;
extern Bike_21 D_800E37C0_36[];
extern ColDef D_800C9080_6[];

static inline s32 project(Vec3f *p, s32 view, s32 arg2, s32 *sx, s32 *sy, s32 *sz, s32 *w) {
    if (D_800EFD64_4->f2C == 0) {
        return func_80033AC8(p->x * 1000.0f * 0.0625f, p->y * 1000.0f * 0.0625f, p->z * 1000.0f * 0.0625f, sx, sy,
                             sz, w, arg2, view);
    } else {
        return func_80033AC8(p->x * 1000.0f * 0.0625f - (D_800E8090[view].x >> 4),
                             p->y * 1000.0f * 0.0625f - (D_800E8090[view].y >> 4),
                             p->z * 1000.0f * 0.0625f - (D_800E8090[view].z >> 4), sx, sy, sz, w, arg2, view);
    }
}

Gfx *func_80037414(Gfx *gdl, s32 view, s32 arg2) {
    s32 mtx[9];
    s32 v[3];
    s32 out[3];
    Vec3f pos;
    s32 sx, sy, sz, w;
    s32 i, size, k, hh, dsdx, dtdy, alpha, a, f, last;
    Bike_21 *b;

    if (D_800E1898 >= 4) {
        return gdl;
    }

    gDPPipeSync(gdl++);
    gDPSetColorDither(gdl++, G_CD_NOISE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x30B261, 0xFF67FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    gDPLoadTextureBlock(gdl++, D_800E92A0_2->e[0].img, G_IM_FMT_IA, G_IM_SIZ_8b, D_800E92A0_2->e[0].w * 8,
                        D_800E92A0_2->e[0].h, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    for (i = 0; i < 96; i++) {
        if (D_800E9214_4[i].life >= 0 && D_800E9214_4[i].type == 0) {
            if (project(&D_800E9214_4[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (w < 5001 && w != 0) {
                size = 15000 / w;
                if (D_800C2594 == 2) {
                    size = size * 2 / 3;
                } else if (D_800C2594 >= 3) {
                    size /= 2;
                }
                if (size > 48) {
                    size = 48;
                }
                if (D_800E9214_4[i].vel.y > 0.0f) {
                    k = ((D_800E9214_4[i].max - D_800E9214_4[i].life) << 10) / D_800E9214_4[i].max + 0x80;
                    if (k > 0x400) {
                        k = 0x400;
                    }
                    if (D_800E3710 == 0) {
                        D_800E9214_4[i].vel.y -= 0.005f;
                    }
                } else {
                    k = ((D_800E9214_4[i].max - D_800E9214_4[i].life) << 9) / D_800E9214_4[i].max + 0x100;
                    if (k > 0x200) {
                        k = 0x200;
                    }
                }
                size = size * k / 256;
                if (size < 3) {
                    size = 3;
                }
                dsdx = (D_800E92A0_2->e[0].w << 13) / size;
                a = D_800E9214_4[i].color;
                hh = size * 2 / 3;
                alpha = 0xFF - D_800C9080_6[a].a1;
                alpha = (((D_800E9214_4[i].life << 7) / D_800E9214_4[i].max + 0x7F) * alpha) >> 8;
                dtdy = (D_800E92A0_2->e[0].h << 10) / hh;
                if (alpha != 0) {
                    gDPSetPrimColor(gdl++, 0, 0, (D_800C9080_6[a].r1 * D_800E9214_4[i].r) >> 8,
                                    (D_800C9080_6[a].g1 * D_800E9214_4[i].g) >> 8, (D_800C9080_6[a].b1 * D_800E9214_4[i].b) >> 8,
                                    alpha);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (D_800E9214_4[i].life >= 0 && D_800E9214_4[i].type == 6) {
            if (project(&D_800E9214_4[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (w != 0) {
                k = ((D_800E9214_4[i].max - D_800E9214_4[i].life) * 0x2800) / D_800E9214_4[i].max + 0x500;
                size = 15000 / w;
                if (k > 0x400) {
                    k = 0x400;
                }
                size = size * k / 256;
                if (size < 20) {
                    size = 20;
                }
                if (size > 100) {
                    size = 100;
                }
                dsdx = (D_800E92A0_2->e[0].w << 13) / size;
                alpha = (D_800E9214_4[i].life * 0xC0) / D_800E9214_4[i].max + 0x3F;
                hh = size * 2 / 3;
                a = 0xFF - (w * 0xFF) / 15000;
                dtdy = (D_800E92A0_2->e[0].h << 10) / hh;
                if (a < 0) {
                    a = 0;
                }
                if (a > 255) {
                    a = 255;
                }
                alpha = (alpha * a) >> 8;
                if (alpha != 0) {
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, alpha);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (D_800E9214_4[i].life >= 0 && D_800E9214_4[i].type == 3) {
            if (project(&D_800E9214_4[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (w != 0) {
                size = 7500 / w;
                if (D_800C2594 >= 3) {
                    size /= 2;
                }
                if (size > 48) {
                    size = 48;
                }
                k = ((D_800E9214_4[i].max - D_800E9214_4[i].life) << 10) / D_800E9214_4[i].max + 0x100;
                if (k > 0x400) {
                    k = 0x400;
                }
                size = size * k / 256;
                if (size < 3) {
                    size = 3;
                }
                dsdx = (D_800E9270_2->e[0].w << 13) / size;
                hh = size * 2 / 3;
                dtdy = (D_800E9270_2->e[0].h << 10) / hh;
                gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, (D_800E9214_4[i].life * 245) / D_800E9214_4[i].max + 20);
                k = (u32)(i * 123 + D_800E9214_4[i].max - D_800E9214_4[i].life) % D_800E9270_2->n;
                gDPLoadTextureBlock(gdl++, D_800E9270_2->e[k].img, G_IM_FMT_IA, G_IM_SIZ_8b, D_800E9270_2->e[k].w * 8,
                                    D_800E9270_2->e[k].h, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
                gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                    (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
            }
        }
    }

    for (i = 0; i < 16; i++) {
        if (D_800E9228[i] != 0) {
            k = ((15 - D_800E9228[i]) * D_800E929C_2->n) / 15;
            if (k < 0) {
                k = 0;
            }
            if (k >= D_800E929C_2->n) {
                k = D_800E929C_2->n - 1;
            }
            gDPLoadTextureBlock(gdl++, D_800E929C_2->e[k].img, G_IM_FMT_IA, G_IM_SIZ_8b, D_800E929C_2->e[k].w * 8,
                                D_800E929C_2->e[k].h, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
            b = &D_800E37C0_36[i / 2];
            func_80011A58(mtx, b->rx, b->ry, b->rz);
            v[0] = 100;
            v[1] = 700;
            v[2] = (D_800E9228[i] - 15) * 40 - 650;
            func_80012DC8(v, mtx, out);
            pos.x = (f32)(out[0] + b->x) / 1000.0f;
            pos.y = (f32)(out[1] + b->y) / 1000.0f;
            pos.z = (f32)(out[2] + b->z) / 1000.0f;
            if (project(&pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (w != 0) {
                size = 10000 / w;
                if (D_800C2594 >= 2) {
                    size /= 2;
                }
                k = ((15 - D_800E9228[i]) << 8) / 15;
                k += 0x100;
                size = size * k / 256;
                if (size > 48) {
                    size = 48;
                }
                if (size < 3) {
                    size = 3;
                }
                if (D_800E37C0_36[i / 2].f654 != 0) {
                    size = size * 3 / 2;
                    k = (D_800E9228[i] * 63) / 15 + 0xC0;
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, k);
                } else {
                    if (D_800E37C0_36[i / 2].flags & 0x20000) {
                        size = size * 3 / 2;
                    }
                    k = (D_800E9228[i] * 127) / 15 + 0x80;
                    gDPSetPrimColor(gdl++, 0, 0, 0x60, 0x60, 0x80, k);
                }
                dsdx = (D_800E929C_2->e[0].w << 13) / size;
                hh = size * 2 / 3;
                dtdy = (D_800E929C_2->e[0].h << 10) / hh;
                gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                    (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (D_800E9214_4[i].life >= 0 && D_800E9214_4[i].type == 1) {
            if (project(&D_800E9214_4[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else {
                gDPLoadTextureBlock(gdl++, D_800E926C_2->e[i & 3].img, G_IM_FMT_IA, G_IM_SIZ_8b,
                                    D_800E926C_2->e[i & 3].w * 8, D_800E926C_2->e[i & 3].h, 0, G_TX_CLAMP, G_TX_CLAMP, 5,
                                    5, G_TX_NOLOD, G_TX_NOLOD);
                if (w != 0) {
                    size = 15000 / w;
                    size /= 6;
                    if (D_800C2594 >= 3) {
                        size /= 2;
                    }
                    if (size > 24) {
                        size = 24;
                    }
                    k = ((D_800E9214_4[i].max - D_800E9214_4[i].life) << 9) / D_800E9214_4[i].max + 0x100;
                    if (k > 400) {
                        k = 400;
                    }
                    size = size * k / 256;
                    if (size < 3) {
                        size = 3;
                    }
                    dsdx = (D_800E926C_2->e[0].w << 13) / size;
                    hh = size * 2 / 3;
                    dtdy = (D_800E926C_2->e[0].h << 10) / hh;
                    a = D_800E9214_4[i].color;
                    gDPSetPrimColor(gdl++, 0, 0, (D_800C9080_6[a].r2 * D_800E9214_4[i].r) >> 8,
                                    (D_800C9080_6[a].g2 * D_800E9214_4[i].g) >> 8, (D_800C9080_6[a].b2 * D_800E9214_4[i].b) >> 8,
                                    (D_800E9214_4[i].life << 7) / D_800E9214_4[i].max + 0x40);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    last = -1;
    if (D_800C2594 == 1) {
        gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        for (i = 0; i < 96; i++) {
            if (D_800E9214_4[i].life >= 0 && D_800E9214_4[i].type == 5) {
                if (project(&D_800E9214_4[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                    if (D_800C2594 == 1) {
                        D_800E9214_4[i].life = -1;
                    }
                } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                    if (D_800C2594 == 1) {
                        D_800E9214_4[i].life = -1;
                    }
                } else {
                    alpha = (D_800E9214_4[i].max - D_800E9214_4[i].life) / 2;
                    if (alpha >= 3) {
                        alpha = 5 - alpha;
                    }
                    if (alpha >= 0) {
                        if (last != alpha) {
                            last = alpha;
                            gDPLoadTextureBlock(gdl++, D_800E9290_2->e[alpha].img, G_IM_FMT_IA, G_IM_SIZ_8b,
                                                D_800E9290_2->e[alpha].w * 8, D_800E9290_2->e[alpha].h, 0, G_TX_CLAMP, G_TX_CLAMP,
                                                5, 5, G_TX_NOLOD, G_TX_NOLOD);
                        }
                        if (w != 0) {
                            size = 30000 / w;
                            size /= 6;
                            if (D_800C2594 >= 3) {
                                size /= 2;
                            }
                            if (size > 24) {
                                size = 24;
                            }
                            if (size < 15) {
                                size = 15;
                            }
                            dsdx = (D_800E9290_2->e[alpha].w << 13) / size;
                            hh = size * 2 / 3;
                            dtdy = (D_800E9290_2->e[alpha].h << 10) / hh;
                            gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                            gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                                (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                        }
                    }
                }
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    for (i = 0; i < 96; i++) {
        if (D_800E9214_4[i].life >= 0 && D_800E9214_4[i].type == 2) {
            if (project(&D_800E9214_4[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214_4[i].life = -1;
                }
            } else {
                w = 0x7F - (w << 7) / 2000;
                if (w < 0) {
                    w = 0;
                }
                if (w > 127) {
                    w = 127;
                }
                gDPSetPrimColor(gdl++, 0, 0, w + 0x80, w + 0x80, 0, 255);
                gDPSetPrimDepth(gdl++, sz - 0x100, 0);
                gDPFillRectangle(gdl++, sx, sy, sx + 1, sy + 1);
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    return gdl;
}

#define SIN(a) D_800C2C60[((a) >> 4) & 0xFFF]
#define COS(a) D_800C2C60[(((a) >> 4) - 0x400) & 0xFFF]

Gfx *func_80039E04(Gfx *gdl, s32 mtx, s32 buf) {
    s32 dx, dz, a, s, c;
    s32 fwd, side, turn;
    s32 i, j, k, row;
    s32 h, w, y, r, q, t, u;
    s32 rad, off, x, z;
    Vtx *src, *dst, *end;
    Vec3f pos;
    Vec3i vel;
    volatile s32 vv;

    if (D_800C9990 == 0 || (!(D_800E7FC8 & 1) && (D_800E7FF0 & 0x40))) {
        return gdl;
    }
    gdl = func_80002418_2(gdl, mtx, 0);

    t = D_800E8090_2.x - D_800E9218.x;
    r = D_800E8090_2.z - D_800E9218.z;
    fwd = (((t) * SIN(D_800E8000[0])) >> 14) + (((r) * COS(D_800E8000[0])) >> 14);
    fwd = -fwd;
    side = (((r) * SIN(D_800E8000[0])) >> 14) - (((t) * COS(D_800E8000[0])) >> 14);
    side *= 2;
    turn = D_800E8000[0] - D_800E9268;
    if (fwd > 0x200) fwd = 0x200;
    if (fwd < -0x200) fwd = -0x200;
    if (side > 0x200) side = 0x200;
    if (side < -0x200) side = -0x200;
    if (turn > 0x200) turn = 0x200;
    if (turn < -0x200) turn = -0x200;

    fwd = turn;
    if (D_800E3710 == 0 && (D_800C2358 == 0 || D_800E1B70 != 0x100)) {
        for (i = 0; i < 64; i++) {
            x = func_8000EFE8_2() * 2;
            x -= 0x100;
            x = x * D_800F13EC / 512;
            row = i >> 3;
            r = row * 32;
            h = r * 10 / 256 + 5;
            h = h * D_800F13F0 / 240;
            w = ((r >> 1) + 0x80) * D_800F13F0;
            if (w < 0) w += 255;
            w >>= 8;
            r = func_8000EFE8_2() * w;
            if (r < 0) r += 255;
            y = r >> 8;
            y = D_800F13F0 / 2 - y;
            q = -(x * 32) / D_800F13EC;
            w = 0xFBFF - (row << 8);
            D_800E9288[buf][i * 4].v.ob[0] = x;
            D_800E9288[buf][i * 4].v.ob[1] = y;
            D_800E9288[buf][i * 4].v.ob[2] = w;
            t = (h * (-fwd * 16 / 500)) >> 4;
            t += (q * side) >> 9;
            D_800E9288[buf][i * 4 + 1].v.ob[0] = x + t;
            D_800E9288[buf][i * 4 + 1].v.ob[1] = y + h;
            D_800E9288[buf][i * 4 + 2].v.ob[0] = x + 2;
            D_800E9288[buf][i * 4 + 2].v.ob[1] = y;
            u = t + 2 + (((u32)h & 1) >> 1);
            D_800E9288[buf][i * 4 + 3].v.ob[0] = x + u;
            D_800E9288[buf][i * 4 + 3].v.ob[1] = y + h;
        }
    } else {
        src = D_800E9288[buf ^ 1];
        dst = D_800E9288[buf];
        end = src + 256;
        do {
            *dst++ = *src++;
        } while (src != end);
    }

    gDPPipeSync(gdl++);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xE200001C;
        g->words.w1 = 0x404A50;
    }
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xDA380007;
        g->words.w1 = mtx + 0x300;
    }
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    for (i = 0; i < 64; i += 8) {
        gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, i * 5 * 4 / 64 + 16);
        gDPSetPrimDepth(gdl++, D_800E9288[buf][i * 4].v.ob[2], 0);
        gSPVertex(gdl++, &D_800E9288[buf][i * 4], 32, 0);
        for (j = 0; j < 8; j++) {
            gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 3, 0, j * 4, j * 4 + 3, j * 4 + 2, 0);
        }
    }
    gDPPipeSync(gdl++);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xDA380007;
        g->words.w1 = mtx;
    }
    D_800E9218 = D_800E8090_2;
    D_800E9268 = D_800E8000[0];
    if (D_800E3710 == 0) {
        w = (func_8000EFE8_2() << 7) + 8000;
        x = func_8000EFE8_2() * w / 256 - w / 2;
        t = ((x * SIN(D_800E8000[0])) >> 14) - ((w * COS(D_800E8000[0])) >> 14);
        r = ((w * SIN(D_800E8000[0])) >> 14) + ((x * COS(D_800E8000[0])) >> 14);
        x = t + D_800E8090_2.x;
        w = r + D_800E8090_2.z;
        y = func_8002E920(x, 0x8000, w);
        r = (D_800E84F8 >> 4) & 0xF;
        if (D_800E8508 != 0 && (D_800EFD64_2 - D_800CB210 != 11 || r != 6)) {
            vel.x = 0;
            vel.y = 0;
            vel.z = 0;
            pos.x = (f32)x / 1000.0f;
            pos.y = (f32)(y + 10) / 1000.0f;
            pos.z = (f32)w / 1000.0f;
            func_80036374(3, 0, &pos, &vel, D_800C9994);
        }
    }
    gSPSetGeometryMode(gdl++, D_800E19CC);
    return gdl;
}

void func_8003A694(s32 p)
{
    D_800E9214 = func_800BC908(0xD80, 0, D_800D6D00, 0x60);
    func_8000ED10((u8 *)D_800E9214, 0, 0xD80);
    D_800E9210 = 0;
    func_8000ED10(D_800E9228, 0, 0x40);
    func_8000ED10(D_800E92A8, 0, 0x20);
    D_800E9298 = p;
    func_80036230();
}

void func_8003A720(void) {
    func_800BCB04(D_800E9214);
}

void func_8003A744(Vec3f *src, s32 val)
{
    Vec3f *dst = &D_800E9278;
    *dst = *src;
    D_800C9998 = val;
    D_800E9294 = D_800E76D0;
}

void func_8003A780(void)
{
    s32 i;

    D_800C9990 = 0;
    if ((((D_800EFD64_6 - (s32)D_800CB210) * -0x33333333) >> 5) == 0xb &&
        (i = 0, D_800E1898 == 1)) {
        for (i = 0; i < 2; i++) {
            D_800E9288[i] = func_800BC908(0x1000, 0, D_800D6D00, 0x512);
        }
        D_800C9990 = 1;
    }
}

void func_8003A84C(void)
{
    s32 i;
    u32 *p;
    if (D_800C9990 != 0) {
        i = 0;
        p = &D_800E9288;
        do {
            u32 v = p[0];
            p++;
            i++;
            func_800BCB04(v);
        } while (i < 2);
        D_800C9990 = 0;
    }
}

void func_8003A8A8(u8 *p)
{
    f32 buf[8];

    if (D_800E1898 == 1) {
        buf[0] = D_800F0B20.x - *(f32 *)(p + 0x48);
        buf[1] = D_800F0B20.y - *(f32 *)(p + 0x4C);
        buf[2] = D_800F0B20.z - *(f32 *)(p + 0x50);
        func_80013310(buf, (f32 *)(p + 0xA0), (f32 *)(p + 0x354));
        *(s32 *)(p + 0x350) = 0xF;
    }
}

void func_8003A928(f32 *v, s32 *a1, s32 *a2, s32 *a3, s32 *a4, s32 a5, s32 idx)
{
    s32 x, y, z;
    Vec3i *t;

    if (D_800EFD64_3[11] == 0) {
        x = v[0] * 1000.0f * 0.0625f;
        y = v[1] * 1000.0f * 0.0625f;
        z = v[2] * 1000.0f * 0.0625f;
        func_80033AC8(x, y, z, a1, a2, a3, a4, a5, idx);
    } else {
        x = v[0] * 1000.0f * 0.0625f - (f32)(D_800E8090[idx].x >> 4);
        y = v[1] * 1000.0f * 0.0625f - (f32)(D_800E8090[idx].y >> 4);
        z = v[2] * 1000.0f * 0.0625f - (f32)(D_800E8090[idx].z >> 4);
        func_80033AC8(x, y, z, a1, a2, a3, a4, a5, idx);
    }
}

void func_8003AA8C(s32 param_1)
{
    u32 r = func_8000EFE8();
    D_800E9228[param_1 * 2 + (r & 1)] = 0xF;
}
