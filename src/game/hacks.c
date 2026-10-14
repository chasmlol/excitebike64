#include "game.h"

typedef struct {
    s32 x0, y0, x1, y1;
    s32 f10, f14, f18, f1C, f20, f24;
    s32 n;
    s32 f2C;
    u16 ang;
    s16 arr[3];
    u8 idx[4];
    void *buf;
} Track_14;

extern Track_14 D_800CCB90[2];

typedef struct Ent_61 {
    s32 key;
    void (*fn)(void);
    s32 pad[4];
    s32 n6;
    s32 n7;
    s32 **names;
} Ent_61;

extern Ent_61 *D_800F0B58_2;
extern Ent_61 *D_800CCB84[];

void func_800620A0(void)
{
    s32 key;
    Ent_61 **pp;
    s32 i;

    key = D_800EFD64_7 - D_800CB210_3;
    D_800F0B58_2 = NULL;
    for (i = 0; D_800CCB84[i] != NULL; i++) {
        if (D_800CCB84[i]->key == key) {
            D_800F0B58_2 = D_800CCB84[i];
            break;
        }
    }
    if (D_800F0B58_2 != NULL) {
        D_800F0B40[0] = NULL;
        D_800F0B40[1] = NULL;
        D_800F0B60 = NULL;
        if (D_800F0B58_2->n6 != 0) {
            D_800F0B40[0] = func_800BC908(D_800F0B58_2->n6 << 7, 0, D_800D8C10, 0x9f);
            D_800F0B40[1] = (u8 *)D_800F0B40[0] + (D_800F0B58_2->n6 << 6);
        }
        if (D_800F0B58_2->n7 != 0) {
            D_800F0B60 = func_800BC908(D_800F0B58_2->n7 << 2, 0, D_800D8C10, 0xa4);
            for (i = 0; i < D_800F0B58_2->n7; i++) {
                D_800F0B60[i] = func_800BF79C(D_800E7728, *D_800F0B58_2->names[i]);
            }
        }
        D_800F0B58_2->fn();
    }
}

typedef struct {
    s32 a, b, c;
    s32 data;
} Res_4;

void func_80062244(void)
{
    s32 i, j;
    s32 r, cx, cy, t;
    Track_14 *p;
    Res_4 *res;
    s32 sp18;
    s32 d, e;

    D_800CCB20 = 0;
    for (i = 0; i < 2; i++) {
        p = &D_800CCB90[i];
        func_8001254C(p->x1 - p->x0, p->y1 - p->y0, &p->ang, &sp18);
        cx = (p->x0 + p->x1) / 2;
        cy = (p->y0 + p->y1) / 2;
        sp18 = p->n * 0x2CEC;
        p->f10 = cx + D_800C2C60[(((s16)p->ang >> 4) - 0x400) & 0xFFF] * sp18 / 0x4000;
        p->f18 = cy + D_800C2C60[p->ang >> 4] * sp18 / 0x4000;
        p->f1C = cx - D_800C2C60[(((s16)p->ang >> 4) - 0x400) & 0xFFF] * sp18 / 0x4000;
        p->f24 = cy - D_800C2C60[p->ang >> 4] * sp18 / 0x4000;
        p->f14 = func_8002E920(cx, 50000, cy);
        p->f20 = func_8002E920(cx, 50000, cy);
        p->f2C = D_800E8958;
        for (j = 0; j < p->n; j++) {
            p->arr[j] = (j << 8) / p->n;
            p->idx[j] = j;
        }
        p->buf = func_800BC908(p->n << 9, 0, "hacks.c", 0x116);
    }
    for (i = 0; i < 4; i++) {
        res = func_800BF79C(D_800E7728, *D_800CCC10[i]);
        sp18 = res->data - (s32)res;
        t = func_800BD924(res);
        d = 0x1E000;
        memcpy(D_800C29E0 + d, res->data, t - sp18);
        d = 0;
        func_800BD798(res, sp18, d, "hacks.c", 0x124);
        e = 0x1E000;
        res->data = D_800C29E0 + e;
        func_80014B84(&D_800F0B48[i], res, D_800E7808, 0xC);
    }
}

typedef struct {
    u8 pad0[0x28];
    s32 count;
    s32 id;
    u8 pad30[2];
    s16 t[3];
    u8 v[8];
} Ent_62;

typedef struct {
    u8 pad0[0x3D0];
    s8 f3D0;
    u8 pad3D1[0x6E8 - 0x3D1];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player_20;

extern Ent_62 D_800CCB90_2[];
extern Player_20 D_800E37C0_77[];

void func_80062588(void)
{
    s32 j;
    s32 i;
    Ent_62 *e;

    for (j = 0; j < 2; j++) {
        e = &D_800CCB90_2[j];
        for (i = 0; i < e->count; i++) {
            if (++e->t[i] >= 0x100) {
                e->t[i] = 0;
                e->v[i] = func_8000EFE8() & 3;
            }
        }
    }

    for (i = 0; i < 8; i++) {
        if (D_800E37C0_77[i].f7A5 == 0 && D_800E37C0_77[i].f6E8 == 0 && D_800E37C0_77[i].f3D0 != -1 && D_800E37C0_77[i].f3D0 < 16) {
            if (D_800E15E0[i] == D_800CCB90_2[0].id || D_800E15E0[i] == D_800CCB90_2[1].id) {
                D_800CCB20 = 300;
            }
        }
    }

    if (D_800CCB20 > 0) {
        D_800CCB20--;
    }
}

typedef struct {
    u32 pad0[4];
    s32 x0;
    s32 y0;
    s32 z0;
    s32 x1;
    s32 y1;
    s32 z1;
    s32 cnt;
    s32 idx;
    s16 sh30;
    s16 sk[3];
    u8 bt[4];
    Mtx *base;
} Cell_6;

typedef struct {
    u8 pad[0x20];
    Obj e[1];
} Hdr_8;

extern Hdr_8 *D_800C9644_8;

Gfx *func_800626CC(Gfx *gdl, s32 unused, s32 p3, s32 p4)
{
    s32 i, k;
    s32 *v;
    s32 *tab;
    s32 sh;
    Cell_6 *c;
    Mtx *m;
    Gfx **tbl;
    s32 x, y, z;

    sh = p4 * 2;
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetRenderMode(gdl++, 0x00443078, 0);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);

    i = 0;
    tab = D_800E8090;
    v = tab + (sh + p4);
    for (; i < 2; i++) {
        c = &D_800CCB90[i];
        if ((D_800C9644_8->e[c->idx].mask >> sh) & 1) {
            k = 0;
            if (c->cnt > 0) {
                tbl = D_800F0B48;
                do {
                    x = c->x0 + (c->x1 - c->x0) * c->sk[k] / 256;
                    z = c->z0 + (c->z1 - c->z0) * c->sk[k] / 256;
                    y = c->y0;
                    m = &c->base[p3 * (c->cnt << 2) + p4 * c->cnt + k];
                    func_80011490(m, 0, 0, 0x8000 - c->sh30, x - v[0], y - v[1], z - v[2], 0x100);
                    gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(gdl++, tbl[c->bt[k]]);
                    k++;
                } while (k < c->cnt);
            }
        }
    }
    return gdl;
}

typedef struct {
    u8 pad0[0x458];
    s32 s458;
    u8 pad45C[0x494 - 0x45C];
    f32 f494;
    f32 f498;
    f32 f49C;
    u8 pad4A0[0x4FC - 0x4A0];
    s32 s4FC;
    u8 pad500[0x538 - 0x500];
    f32 f538;
    f32 f53C;
    f32 f540;
    u8 pad544[0x7A5 - 0x544];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike_37;

typedef struct { u8 pad[0x1A]; s16 h; u8 pad2[0x40 - 0x1C]; } Obj_44;

extern Bike_37 D_800E37C0_78[];
extern Obj_44 *D_800CCCD0;

void func_80062998(void)
{
    s32 c, s;
    s32 i, j, m;
    u32 k;
    Bike_37 *b, *bb, *base;
    V4 *p;
    Vec3i *e;
    Obj_44 *o;
    s32 dx, dz, a, v, t, r, w;
    s32 ang;

    k = ((u8 *)D_800F143C)[0xE] >> 4;
    c = D_800C2C60[k | 0xC00];
    s = D_800C2C60[k];
    for (m = 0; m < D_800F0B5C; m++) {
        D_800F0B64[m].z = 0;
    }
    for (m = 0; m < 8; m++) {
        if (D_800E37C0_78[m].b7A5 != 0 || D_800E37C0_78[m].s458 >= 0x15 || D_800E37C0_78[m].s4FC >= 0x15) continue;
        for (i = 0; i < D_800F0B5C; i++) {
            dx = (s32)(D_800E37C0_78[m].f538 * 1000.0f) - D_800F143C[i].x;
            if ((u32)(dx + 0x10000) > 0x20000) continue;
            dz = (s32)(D_800E37C0_78[m].f540 * 1000.0f) - D_800F143C[i].z;
            if ((u32)(dz + 0x10000) > 0x20000) continue;
            a = (dx * c + dz * s) >> 14;
            dx = (s32)(D_800E37C0_78[m].f494 * 1000.0f) - D_800F143C[i].x;
            if ((u32)(dx + 0x10000) > 0x20000) continue;
            dz = (s32)(D_800E37C0_78[m].f49C * 1000.0f) - D_800F143C[i].z;
            if ((u32)(dz + 0x10000) > 0x20000) continue;
            v = (dx * c + dz * s) >> 14;
            if (a > 0 && v < 0) {
                r = 0x100;
            } else if (a < 0 && v > 0) {
                r = 0x100;
            } else {
                if (a > 0) {
                    r = v;
                    if (a < r) r = a;
                    v = 400;
                    v -= r;
                    r = (v << 8) / 400;
                } else if (a < 0) {
                    r = v;
                    if (r < a) r = a;
                    v = r + 400;
                    r = (v << 8) / 400;
                } else {
                    goto store;
                }
                if (r < 0) r = 0;
                if (r >= 0x100) r = 0xFF;
            }
store:
            if (D_800F0B64[i].z < r) D_800F0B64[i].z = r;
        }
    }
    i = 0;
    if (D_800F0B5C > 0) {
        do {
            e = &D_800F0B64[i];
            t = e->z;
            w = (D_800C2C60[(((D_800E76D0 << 9) + i * 0x1000) >> 4) & 0xFFF] * (0xFF - t)) >> 15;
            w -= t;
            w /= 2;
            w -= 200;
            if (w < e->y - 16) {
                e->y = e->y - 16;
            } else {
                e->y = w;
            }
            D_800CCCD0[i].h = (e->x + e->y) >> 4;
            i++;
        } while (i < D_800F0B5C);
    }
}

void func_80062D0C(void)
{
    if (D_800F0B58 != 0) {
        ((void (*)(void))(*(u32 *)(D_800F0B58 + 0xC)))();
    }
}

typedef struct Obj_45 {
    s32 f0;
    s32 f4;
    void (*f8)(void);
    s32 fc;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
} Obj_45;

extern Obj_45 *D_800F0B58_3;

void func_80062D38(void) {
    if (D_800F0B58_3 != NULL) {
        if (D_800F0B58_3->f1c != 0) {
            func_800BCB04((void *)D_800F0B60);
        }
        if (D_800F0B58_3->f18 != 0) {
            func_800BCB04(D_800F0B40);
        }
        D_800F0B58_3->f8();
    }
}

s32 func_80062DA0(s32 a0, Dyn *arg1, s32 arg2, s32 arg3)
{
    if (D_800F0B58 != 0) {
        a0 = ((s32 (*)(s32))(*(u32 *)(D_800F0B58 + 0x14)))(a0);
    }
    return a0;
}

s32 func_80062DD4(s32 x, Dyn *arg1, s32 arg2, s32 arg3)
{
    if (D_800F0B58_4 != 0) {
        x = ((s32 (*)(s32))(D_800F0B58_4[0x10 / 4]))(x);
    }
    return x;
}

typedef struct {
    s32 val;
    s32 zero;
    s32 pad;
} Rec_34;

void func_80062E08(void)
{
    s32 i;
    s32 n;
    s32 cnt;
    Rec_34 *out;
    s32 *e;

    D_800F0B5C = D_800F1430[1];
    D_800F0B64 = (s32 *)func_800BC908(D_800F0B5C * 12, 0, D_800D8C10, 0x1B1);
    i = 0;
    n = D_800F0B5C;
    if (n > 0) {
        cnt = n;
        out = (Rec_34 *)D_800F0B64;
        e = D_800F143C;
        do {
            s32 v = e[1];
            e += 4;
            i++;
            out->zero = 0;
            out->val = v;
            out++;
        } while (i < cnt);
    }
}

void func_80062E9C(void)
{
    func_800BCB04(D_800F0B64);
}

s32 func_80062EC0(s32 x)
{
    return x;
}

void *func_80062EC8(void *p)
{
    return p;
}

typedef struct {
    u8 pad[0x3C];
    u32 v;
} Blk_17;

void func_80062ED0(void) {
    s32 i = 0;
    Blk_17 *p = D_800CCB90;
    u32 *q;

    do {
        u32 v = p->v;
        p++;
        i++;
        func_800BCB04(v);
    } while (i < 2);

    i = 0;
    q = D_800F0B48;
    do {
        u32 v = *q;
        q++;
        i++;
        func_800BCB04(v);
    } while (i < 4);
}

s32 func_80062F40(s32 x)
{
    return x;
}
