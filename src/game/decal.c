#include "game.h"

typedef struct {
    u8 pad[8];
    s16 a;
    s16 b;
    u8 pad2[4];
} E16;

extern E16 *D_800F07F8;

typedef struct {
    u8 pad0[0x44C];
    f32 f44C;
    f32 f450;
    u8 pad1[0x7E0 - 0x454];
} Ent_58;

typedef struct {
    u8 pad0[0x20];
    f32 f20[1];
} Ply;

typedef struct {
    u8 pad0[0x410];
    s32 f410;
} Gm;

extern Ply *D_800EFD64_31;
extern Gm D_800EFD70_44;
extern Ent_58 D_800E37C0_73[];

void func_8005C910(void)
{
    s32 i;
    char buf[0x28];

    if (D_800E3770_2 & 0x80) {
        if (D_800E36E0 == 0) {
            D_800CB178++;
            if (D_800CB178 == 4) {
                D_800CB178 = 0;
            }
        }
    }
    for (i = 0x13; i >= 0; i--) {
    }
    switch (D_800CB178) {
    case 1:
        if (*(s32 *)((u8 *)D_800EFD64_31 + 0x2C) == 2) {
            break;
        }
        if (D_800C8D08 != 0 || D_800C8D10 != 0 || D_800C8D00 != 0 || D_800C8D18 != 0) {
            break;
        }
        func_8005D004(D_800D8B10, D_800E76E8);
        func_8005D004(D_800D8B18, D_800E76EC);
        break;
    case 0:
        func_8005D004(D_800D8B20, D_800E76E8);
        func_8005D004(D_800D8B24, D_800E76EC);
        func_8005D004(D_800D8B28, D_800E76F0);
        func_8005D004(D_800D8B2C, D_800E76F4);
        func_8005D004(D_800D8B30, D_800E76F8);
        func_8005D004(D_800D8B34, D_800E76FC);
        break;
    case 2:
        for (i = 0; i < 6; i++) {
            func_8005CD30(buf, D_800E37C0_73[i].f44C / D_800EFD64_31->f20[D_800EFD70_44.f410 - 1], 4);
            func_8005D004(buf, D_800E37C0_73[i].f450);
        }
        break;
    }
}

typedef struct {
    u8 b10[16];
    u8 b20[16];
    u8 b30[16];
    u8 b40[16];
} Frame5CB20;

void func_8005CB20(void)
{
    Frame5CB20 f;
    s32 k;
    u32 u;

    func_8000EFB0(D_800CB17C);
    func_8000EE30(D_800F0750, f.b40);
    func_8000EFB0(f.b40);
    func_8000EFB0(D_800CB1D4);
    for (k = 1; k <= D_800F0750; k++) {
        func_8000EE30((s32)(D_800F0754[k].x * 1000.0f), f.b10);
        func_8000EE30((s32)(D_800F0754[k].z * 1000.0f), f.b30);
        u = func_8002E920((s32)(D_800F0754[k].x * 1000.0f), (s32)(D_800F0754[k].y * 1000.0f), (s32)(D_800F0754[k].z * 1000.0f));
        func_8000EE30(u, f.b20);
        func_8000EFB0(f.b10);
        func_8000EFB0(D_800D8B44);
        func_8000EFB0(f.b30);
        func_8000EFB0(D_800D8B48);
    }
    func_8000EFB0(D_800CB1F0);
}

struct Obj_40 {
    u8 pad[0x48];
    Vec3f pos;
};

void func_8005CC84(Obj_40 *o)
{
    s32 n;
    s32 ni;
    Vec3f **gp;
    Vec3f *p;
    Vec3f *q;
    f32 dx;
    f32 dy;
    f32 dz;

    if (D_800F0750 < 2000) {
        gp = &D_800F0754;
        n = D_800F0750;
        p = (Vec3f *)((u32)(n * 12) + (u32)*gp);
        dx = p->x - o->pos.x;
        dy = p->y - o->pos.y;
        dz = p->z - o->pos.z;
        if (1.0f < dx * dx + dy * dy + dz * dz) {
            ni = n + 1;
            q = (Vec3f *)((u32)(ni * 12) + (u32)*gp);
            D_800F0750 = ni;
            *q = o->pos;
        }
    }
}

typedef struct {
    u8 c[4];
} S4_2;

extern S4_2 D_800D8B64;

void func_8005CD30(char *p, f32 x, s32 n)
{
    s32 ip;
    s32 pw;
    s32 d;

    if (x < 0.0f) {
        *p++ = '-';
        x = -x;
    } else {
        *p++ = ' ';
    }
    if (x >= 1.0f) {
        if (x > 1000000.0f) {
            *(S4_2 *)p = D_800D8B64;
            return;
        }
        pw = 10;
        ip = (s32)x;
        x -= (f32)ip;
        while (ip >= pw) {
            pw *= 10;
        }
        pw /= 10;
        do {
            n--;
            *p++ = ip / pw + '0';
            ip -= (ip / pw) * pw;
            pw /= 10;
        } while (pw != 0);
    } else {
        *p++ = '0';
    }
    if (n > 0) {
        if (x > 0.0001f) {
            *p++ = '.';
        }
        do {
            n--;
            {
                f32 t = x;
                if (!(t > 0.0001f)) break;
            }
            ip = (s32)(x * 10.0f + 0.0001f);
            *p++ = ip + '0';
            x = x * 10.0f - (f32)ip;
        } while (n > 0);
    }
    *p = '\0';
}

f32 func_8005CEFC(f32 x)
{
    if (!(0.0f < x)) {
        x = -x;
    }
    return x / 200.0f;
}

f32 func_8005CF20(f32 x)
{
    f32 a;
    f32 b;
    f32 sx;
    f32 res;

    sx = x;
    a = x;
    if (0.0f < sx) {
        a = x;
    } else {
        a = -x;
    }
    b = 1.0f;
    for (; b < a; b = b * 10.0f) {
    }
    for (; a < b; b = b / 10.0f) {
    }
    res = a;
    if (b == 0.0f) {
        return res;
    }
    a = ((f32)(s32)((a * 100.0f) / b + 0.5f) * b) / 100.0f;
    if (0.0f < sx) {
        return a;
    }
    return -a;
}

void func_8005D004(char *param_1, f32 param_2)
{
    char buf[40];
    s32 len;
    func_8000EF8C(buf, param_1);
    len = strlen(buf);
    func_8005CD30(buf + len, param_2, 3);
}

void func_8005D050(char *param_1, f32 param_2, f32 param_3)
{
    char buf[40];
    s32 n;
    s32 i;

    func_8000ED10((u8 *)buf, 0x20, 0x1E);
    func_8000EF8C(buf, param_1);
    n = strlen(buf);
    func_8005CD30(buf + n, param_2, 3);
    i = 0;
    if (0 < n + 6) {
        do {
            if (buf[i] == 0) {
                buf[i] = ' ';
            }
            i++;
        } while (i < n + 6);
    }
    func_8005CD30(buf + 6 + n, param_3, 3);
}

void func_8005D118(s32 arg0, f32 *v)
{
    char bx[24];
    char by[24];
    char bz[24];

    strcpy(bx, "\nx=");
    func_8005CD30(bx + strlen(bx), v[0], 6);
    strcpy(by, "y=");
    func_8005CD30(by + strlen(by), v[1], 6);
    strcpy(bz, "z=");
    func_8005CD30(bz + strlen(bz), v[2], 6);
}

void func_8005D1D0(char *param_1, s32 param_2) {
    char pad[16];
    char buf[40];
    char *b;

    b = buf;
    func_8000EF8C(b, param_1);
    func_8000EE30(param_2, b + strlen(b));
}

void func_8005D220(char *param_1, u32 param_2) {
    char buf[56];
    char *s = buf + 16;
    func_8000EF8C(s, param_1);
    func_8000EF0C(param_2, s + strlen(s));
}

void func_8005D270(void)
{
    s32 i;

    D_800F0774 = func_800BC908(0xC00, 0, (s32)D_800D8B90, 0x67);
    D_800F07F8 = func_800BC908(0x2000, 0, (s32)D_800D8B90, 0x68);
    D_800F0770 = 0;
    func_8000ED10(D_800F0798, 0, 0x20);
    func_8000ED10(D_800F07D8, 0, 0x20);
    func_8000ED10(D_800F0778, 0, 0x20);
    func_8000ED10(D_800F0828, 0, 0x20);
    func_8000ED10(D_800F0774, 0, 0xC00);
    func_8000ED10(D_800F07B8, 0, 0x20);
    func_8000ED10(D_800F0800, 0, 0x20);
    for (i = 0; i < 0x80; i++) {
        D_800F0774[i * 0x18] = 0;
    }
    for (i = 0; i < 0x200; i += 4) {
        D_800F07F8[i].a = 0;
        D_800F07F8[i].b = 0;
        D_800F07F8[i + 1].a = 0x200;
        D_800F07F8[i + 1].b = 0;
        D_800F07F8[i + 2].a = 0;
        D_800F07F8[i + 2].b = 0;
        D_800F07F8[i + 3].a = 0x200;
        D_800F07F8[i + 3].b = 0;
    }
    D_800F0820 = func_800BC908(8, 0, (s32)D_800D8B90, 0x85);
    D_800F0820[0] = 0xFF;
    D_800F0820[1] = 0xDC;
    D_800F0820[2] = 0x88;
    D_800F0820[3] = 0x58;
    D_800F0820[4] = 0x38;
    D_800F0820[5] = 0x58;
    D_800F0820[6] = 0x8C;
    D_800F0820[7] = 0x1F;
}

typedef struct Rec_32 {
    s32 f0;
    s32 key;
    s32 x, z;
    s32 nx, nz;
} Rec_32;

typedef struct Seg_14 {
    u8 pad[0x28];
    s32 flags;
    u8 pad2[0x40 - 0x2C];
} Seg_14;

typedef struct Gate_2 {
    u8 pad[0x20];
    s16 a[3];
    u8 pad26[0x30 - 0x26];
    s16 b[3];
    u8 pad36[0x40 - 0x36];
} Gate_2;

typedef struct Cell_4 {
    s32 f0, f4;
    s32 x0, z0, x1, z1;
    u8 pad[0x20 - 0x18];
} Cell_4;

typedef struct Bike_35 {
    u8 pad0[0x44];
    u8 f44;
    u8 pad45[0x370 - 0x45];
    s32 f370[1];
    u8 pad374[0x458 - 0x374];
    s32 f458;
    u8 pad45C[0x494 - 0x45C];
    f32 x, y, z;          /* 0x494 */
    u8 pad4A0[0x4AC - 0x4A0];
    f32 f4AC;
    f32 f4B0;
    f32 f4B4;
    u8 pad4B8[0x4E4 - 0x4B8];
    Seg_14 *seg;             /* 0x4E4 */
    u8 pad4E8[0x4F0 - 0x4E8];
    f32 f4F0;
    u8 pad4F4[0x7A5 - 0x4F4];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike_35;

extern Bike_35 D_800E37C0_74[];
extern Cell_4 *D_800E8948_5;
extern Seg_14 *D_800E9298_2;
extern Rec_32 *D_800F0774_2;
extern Rec_32 *D_800F0798_2[];
extern Seg_14 *D_800F07B8_2[];
extern Gate_2 *D_800F07F8_2;
extern Seg_14 *D_800F0800_2[];

void func_8005D444(void)
{
    s32 pos[3];
    s32 a[3];
    s32 b[3];
    s32 d[3];
    s32 m[10];
    s32 c[3];
    s32 e[3];
    s32 qr[4];
    s32 i, chg, key, idx, ox, oz, dx, dz, nx, nz, len, k;
    Bike_35 *p;
    Seg_14 *t;
    Rec_32 *r;
    s32 n;
    s32 l2;
    Cell_4 *ce;

    for (i = 0; i < 8; i++) {
        if (D_800E37C0_74[i].f7A5 != 0) continue;
        p = &D_800E37C0_74[i];
        if (D_800F0828_2[i] != 0) D_800F0828_2[i]--;
        if (D_800E37C0_74[i].f4F0 != 0.0f) {
            D_800F07D8[i] = 1;
        } else if (D_800F07D8[i] != 0) {
            D_800F07D8[i]--;
        }
        if (!(p->seg->flags & 1)) D_800F07D8[i] = 0;
        t = D_800F07B8_2[i];
        if (p->seg != t) {
            chg = 1;
            if (t != NULL && (t->flags & 0x40) && p->f458 <= 0) {
                D_800F0800_2[i] = t;
                D_800F07D8[i] = 45;
                D_800F0828_2[i] = 45;
            }
        } else {
            chg = 0;
        }
        D_800F07B8_2[i] = p->seg;
        if (p->f44 != 0) D_800F07D8[i] = 0;
        if (D_800EE140 != 0) D_800F07D8[i] = 0;
        if (D_800F07D8[i] == 0) {
            D_800F0798_2[i] = NULL;
            continue;
        }
        pos[0] = p->x * 1000.0f;
        pos[1] = p->y * 1000.0f;
        pos[2] = p->z * 1000.0f;
        func_80013650(m, p->f370);
        d[0] = -p->f4B4 * 1000.0f;
        d[2] = p->f4AC * 1000.0f;
        len = func_8000EB18_2(d[0] * d[0] + d[2] * d[2]);
        if (len == 0) {
            d[1] = 0;
            d[0] = m[0];
            d[2] = m[2];
            len = func_8000EB18_2(m[0] * m[0] + m[2] * m[2]);
        }
        if (len != 0) {
            d[0] = d[0] * 50 / len;
            d[2] = d[2] * 50 / len;
        }
        a[0] = pos[0] - d[0];
        a[2] = pos[2] - d[2];
        b[0] = pos[0] + d[0];
        b[2] = pos[2] + d[2];
        a[1] = func_8002E920(a[0], pos[1], a[2]);
        b[1] = func_8002E920(b[0], pos[1], b[2]);
        if (D_800E8508 == 0) continue;
        key = D_800E8958;
        if (D_800EFD64_5->mode == 2) {
            func_8004262C(pos[0], 128000, &qr[0], &qr[1]);
            func_8004262C(pos[2], 128000, &qr[2], &qr[3]);
            key = (qr[0] << 16) | (u16)qr[2];
        }
        if (D_800F0798_2[i] != NULL && (u32)(a[1] - D_800F0778_2[i] + 500) > 1000) {
            D_800F0798_2[i] = NULL;
            D_800F07D8[i] = 0;
            continue;
        }
        idx = p->seg - D_800E9298_2;
        if (D_800F0828_2[i] != 0) idx = D_800F0800_2[i] - D_800E9298_2;
        if (D_800F0798_2[i] != NULL) {
            k = D_800F0798_2[i]->key;
            if (D_800EFD64_5->mode == 1) {
                ce = &D_800E8948_5[k];
                ox = (ce->x0 + ce->x1) / 2;
                oz = (ce->z0 + ce->z1) / 2;
            } else if (D_800EFD64_5->mode == 2) {
                ox = (k >> 16) * 8000 + 4000;
                oz = (s16)k * 8000 + 4000;
            } else {
                ox = oz = 0;
            }
            r = D_800F0798_2[i];
            dx = pos[0] - r->x;
            dz = pos[2] - r->z;
            nx = r->nx;
            nz = r->nz;
            l2 = func_8000EB18_2(dx * dx + dz * dz);
            if (l2 != 0) {
                dx = (dx << 14) / l2;
                dz = (dz << 14) / l2;
            }
            if ((nx * dx + nz * dz) >> 14 < 0x3F07 || key != D_800F0798_2[i]->key || chg) {
                Gate_2 *g;
                s16 *q, *q2;
                len = D_800F0798_2[i] - D_800F0774_2;
                g = (Gate_2 *)((len << 6) + (u32)D_800F07F8_2);
                q = g->a;
                c[0] = q[0] + ox;
                c[1] = q[1];
                c[2] = q[2] + oz;
                q2 = g->b;
                e[0] = q2[0] + ox;
                e[1] = q2[1];
                e[2] = q2[2] + oz;
                c[0] *= 16;
                c[1] *= 16;
                c[2] *= 16;
                e[0] *= 16;
                e[1] *= 16;
                e[2] *= 16;
                D_800F0798_2[i] = func_8005DCFC(key, c, e, a, b, idx);
                D_800F0778_2[i] = a[1];
            } else {
                D_800F0798_2[i]->x = pos[0];
                D_800F0798_2[i]->z = pos[2];
                {
                Gate_2 *g;
                len = D_800F0798_2[i] - D_800F0774_2;
                g = &D_800F07F8_2[len];
                g->a[0] = a[0] / 16 - ox;
                g->a[1] = a[1] / 16;
                g->a[2] = a[2] / 16 - oz;
                g->b[0] = b[0] / 16 - ox;
                g->b[1] = b[1] / 16;
                g->b[2] = b[2] / 16 - oz;
                }
                D_800F0778_2[i] = a[1];
            }
        } else {
            D_800F0798_2[i] = func_8005DCFC(key, a, b, a, b, idx);
            D_800F0778_2[i] = a[1];
        }
    }
}

struct Rec_33 {
    u8 flag;
    u8 p6;
    u16 pad;
    s32 p1;
    s32 cx;
    s32 cz;
    s32 dx;
    s32 dz;
};

typedef struct { s32 f0, f4, a, b, c, d; s32 pad[2]; } Box_2;
typedef struct { s32 pad[11]; s32 mode; } Ctl_17;

extern Ctl_17 *D_800EFD64_32;
extern Box_2 *D_800E8948_6;

Rec_33 *func_8005DCFC(s32 p1, s32 *p2, s32 *p3, s32 *p4, s32 *p5, s32 p6)
{
    s32 i, j, mode;
    s32 sx, sy;
    s32 cz, dx, dz;
    u32 r;
    Box_2 *b;
    Vtx16 *pt;
    Vtx16 *q;
    Rec_33 *rec;
    Rec_33 *r2;
    Rec_33 *rb;

    for (i = 0; i < 4; i++) {
        if (&D_800F0774_3[D_800F0770] == D_800F0798[i]) D_800F0798[i] = 0;
    }

    mode = D_800EFD64_32->mode;
    if (mode == 1) {
        b = &D_800E8948_6[p1];
        sx = (b->a + b->c) / 2;
        sy = (b->b + b->d) / 2;
    } else if (mode == 2) {
        sx = (s16)(p1 >> 16) * 8000 + 4000;
        sy = (s16)p1 * 8000 + 4000;
    } else {
        sx = sy = 0;
    }

    j = D_800F0770 * 4;
    pt = (Vtx16 *)((u32)(j * 16) + (u32)D_800F07F8);
    pt[0].x = (p2[0] >> 4) - sx;
    pt[0].y = p2[1] >> 4;
    pt[0].z = (p2[2] >> 4) - sy;
    pt[1].x = (p3[0] >> 4) - sx;
    q = &pt[1];
    q->y = p3[1] >> 4;
    q->z = (p3[2] >> 4) - sy;
    pt[2].x = (p4[0] >> 4) - sx;
    q = &pt[2];
    q->y = p4[1] >> 4;
    q->z = (p4[2] >> 4) - sy;
    pt[3].x = (p5[0] >> 4) - sx;
    pt = &pt[3];
    pt->y = p5[1] >> 4;
    pt->z = (p5[2] >> 4) - sy;

    j = j / 4;
    rb = D_800F0774_3;
    rec = (Rec_33 *)((s32)(j * 24) + (s32)rb);
    rec->cx = (p4[0] + p5[0]) / 2;
    rec->cz = cz = (p4[2] + p5[2]) / 2;
    dx = rec->cx - (p2[0] + p3[0]) / 2;
    dz = cz - (p2[2] + p3[2]) / 2;
    r = func_8000EB18_3(dx * dx + dz * dz);
    if (r != 0) {
        dx = (dx * 0x4000) / (s32)r;
        dz = (dz * 0x4000) / (s32)r;
    }

    r2 = (Rec_33 *)((u32)(j * 24) + (u32)D_800F0774_3);
    r2->flag = 1;
    r2->dx = dx;
    r2->dz = dz;
    r2->p1 = p1;
    D_800F0774_3[j].p6 = p6;

    D_800F0770 = (D_800F0770 + 1) % 128;
    return &D_800F0774_3[j];
}

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 active; u8 col; u8 pad[2]; s32 id; u8 rest[0x10]; } Ent_59;
typedef struct { u8 pad[0x34]; u8 r, g, b; u8 pad2[9]; } Col_2;
typedef struct { u32 count; u8 pad[0x1C]; Obj obj[1]; } ObjList;

extern Ent_59 *D_800F0774_4;
extern Col_2 *D_800E9298_3;
extern ObjList *D_800E8948_7;

Gfx *func_8005E090(Gfx *gdl, s32 arg1, Mtx *arg2, s32 arg3) {
    s32 i, j, k, m, a, idx, started, v, hi, lo;
    Col_2 *c;

    if (D_800EE140 != 0) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    RAW(gdl++, 0xE200001C, 0x00404A50);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    RAW(gdl++, 0xFC119623, 0xFF2FFFFF);
    gDPLoadTextureBlock(gdl++, D_800F0820, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 1, 0,
                        G_TX_CLAMP, G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    if (D_800EFD64_5->mode == 2) {
        for (i = 0; i < D_800EB9EC; i++) {
            started = 0;
            for (j = 0; j < 128; j++) {
                if (!D_800F0774_4[j].active) continue;
                v = D_800F0774_4[j].id;
                hi = v >> 16;
                lo = (s16)v;
                if (hi == D_800ED6A8[i] && lo == D_800ED728[i]) {
                    k = j + 1;
                    if (!started) {
                        gSPMatrix(gdl++, &D_800EA9E8[arg1][i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        started = 1;
                    }
                    for (; k < 128; k++) {
                        if (k >= j + 8) break;
                        if (!D_800F0774_4[k].active) break;
                        v = D_800F0774_4[k].id;
                        hi = v >> 16;
                        lo = (s16)v;
                        if (hi != D_800ED6A8[i] || lo != D_800ED728[i]) break;
                    }
                    k -= j;
                    gSPVertex(gdl++, &D_800F07F8[j * 4], k * 4, 0);
                    for (m = 0; m < k; m++) {
                        idx = m + j;
                        a = idx - D_800F0770;
                        if (a < 0) a += 128;
                        if (a >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, D_800E9298_3[D_800F0774_4[idx].col].r, D_800E9298_3[D_800F0774_4[idx].col].g, D_800E9298_3[D_800F0774_4[idx].col].b, a);
                            gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                        }
                    }
                    j += k - 1;
                }
            }
        }
    } else if (D_800EFD64_5->mode == 1) {
        ObjList *l = D_800E8948_7;
        u32 bit;
        {
            for (i = 0; i < D_800E8948_7->count; i++) {
                if (l->obj[i].mask & (1 << (arg3 * 2))) {
                started = 0;
                for (j = 0; j < 128; j++) {
                    if (D_800F0774_4[j].active && D_800F0774_4[j].id == i) {
                        k = j + 1;
                        if (!started) {
                            gSPMatrix(gdl++, D_800E77D4[i * 4 + arg3], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                            started = 1;
                        }
                        for (; k < 128; k++) {
                            if (k >= j + 8) break;
                            if (!D_800F0774_4[k].active) break;
                            if (D_800F0774_4[k].id != i) break;
                        }
                        k -= j;
                        gSPVertex(gdl++, &D_800F07F8[j * 4], k * 4, 0);
                        for (m = 0; m < k; m++) {
                            idx = m + j;
                            a = idx - D_800F0770;
                            if (a < 0) a += 128;
                            if (a >= 16) {
                                gDPSetPrimColor(gdl++, 0, 0, D_800E9298_3[D_800F0774_4[idx].col].r, D_800E9298_3[D_800F0774_4[idx].col].g, D_800E9298_3[D_800F0774_4[idx].col].b, a);
                                gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                            }
                        }
                        j += k - 1;
                    }
                }
                }
            }
        }
    } else {
        for (i = 0; i < 128; i += 8) {
            gSPVertex(gdl++, &D_800F07F8[i * 4], 32, 0);
            for (j = 0; j < 8; j++) {
                idx = i + j;
                if (D_800F0774_4[idx].active) {
                    if (D_800E8948_7->obj[D_800F0774_4[idx].id].mask & (1 << (arg3 * 2))) {
                        a = idx - D_800F0770;
                        if (a < 0) a += 128;
                        if (a >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, D_800E9298_3[D_800F0774_4[idx].col].r, D_800E9298_3[D_800F0774_4[idx].col].g, D_800E9298_3[D_800F0774_4[idx].col].b, a);
                            gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 2, 0, j * 4 + 2, j * 4 + 1, j * 4 + 3, 0);
                        }
                    }
                }
            }
        }
    }
    gSPMatrix(gdl++, arg2 + 16, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(gdl++);
    return gdl;
}

void func_8005E960(void)
{
    func_800BCB04(D_800F0820);
    func_800BCB04(D_800F07F8);
    func_800BCB04(D_800F0774);
}

void func_8005E9A4(s32 a0)
{
    D_800F0798[a0] = 0;
}

void func_8005E9BC(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        D_800F0798[i] = 0;
        D_800F07D8[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        D_800F0774[i * 0x18] = 0;
    }
}
