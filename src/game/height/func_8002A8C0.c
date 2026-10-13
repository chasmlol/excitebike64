#include "ctx.h"

typedef struct { u8 pad[0x20]; f32 pos; f32 vel; } Ent;
typedef struct { u32 flags; u8 pad4[0x48]; s32 mode; } Game;
typedef struct { s32 active; u8 pad4[0x22]; s16 t; } Rep;
typedef struct { u8 pad[0x28]; u32 flags; } Trk;

typedef struct {
    u8 pad0[0x34];
    u16 h34;
    u8 pad36[0x12];
    f32 pos[3];
    f32 v54[3];
    u8 pad60[0x48C - 0x60];
    f32 f48C;
    u8 pad490[0x4E4 - 0x490];
    Trk *p4E4;
    u8 pad4E8[0x5B0 - 0x4E8];
    u32 f5B0;
    u32 f5B4;
    u8 pad5B8[0x5CD - 0x5B8];
    u8 b5CD;
    u8 pad5CE[0x6F4 - 0x5CE];
    f32 f6F4;
    u8 pad6F8[0x744 - 0x6F8];
    s32 f744;
    u8 pad748[0x77C - 0x748];
    s16 h77C;
} Bike;

extern s32 D_800C9684;
extern f32 D_800C9688;
extern f32 D_800C968C;
extern f32 D_800C9690;
extern f32 D_800C9694;
extern s32 D_800E8010[];
extern Ent D_800E78C8[];
extern s32 D_800E7968[];
extern s16 D_800C2C60[];
extern Game D_800EFD70;
extern Rep D_800F05A8;
extern s32 D_800E3770;

f32 func_800292E4(Bike *p);
f32 func_800295E0(Bike *p);
f32 func_80013774(f32 x);
f32 func_800AEA30(f32 x);
void func_8002B5F8(s32 *p);
void func_8002B6F4(s32 *p, f32 *q);
void func_8002B348(s32 *p);
void func_80011A58(s32 *p, s32 a, s32 b, s32 c);
void func_80012DC8(s32 *a, s32 *b, s32 *c);

void func_8002A8C0(Bike *p, s32 *out, s32 z)
{
    s32 n, k, ang, tv, zero;
    s32 rot;
    f32 f, g, nv, fc, a, b, c, d;
    s32 dy, dz;
    s32 vec[3];
    s32 res[3];
    s32 mtx[10];
    f32 *pp;
    Ent *e, *eb;
    s32 *q, *cnt;
    s32 tb;
    s32 *q2;
    f32 cs;

    k = p->f744;
    cnt = D_800E8010;
    n = cnt[D_800C9684];
    if (p->f6F4 * 60.0f + (f32)k > 60.0f) {
        if (n < 0x20) {
            n++;
        }
    } else if (k < 0) {
        if (n == 0) goto first;
        n--;
    }
    if (n == 0) {
first:
        f = func_800292E4(p);
    } else if (n == 0x20) {
        f = func_800295E0(p);
    } else {
        a = func_800292E4(p);
        b = func_800295E0(p);
        f = (a * (f32)(0x20 - n) + b * (f32)n) * 0.03125f;
    }
    D_800E8010[D_800C9684] = n;
    eb = D_800E78C8;
    e = &eb[D_800C9684];
    f = f - e->pos;
    if (f > 0.0f) {
        nv = e->vel;
        nv += f * D_800C9688;
        fc = D_800C968C;
        e->vel = nv;
        do { d = nv; } while (0);
    } else {
        nv = e->vel;
        nv += f * D_800C9690;
        fc = D_800C9694;
        e->vel = nv;
        do { d = nv; } while (0);
    }
    d *= fc;
    e->pos += nv;
    do { e->vel = d; } while (0);
    if ((zero = D_800EFD70.mode, rot = p->h34, zero != 9) && !(D_800EFD70.flags & 0x8000)) {
        if ((p->f5B0 & 0x200) && D_800F05A8.active != 0) {
            rot += -0x8000;
        }
    }
    ang = D_800E78C8[D_800C9684].pos;
    if (p->h77C != 0) {
        ang = -500;
    }
    vec[0] = 0;
    vec[1] = 0;
    vec[2] = z;
    if (D_800F05A8.active == 0) {
        g = (f32)D_800F05A8.t * 0.00390625f;
        if (g < 0.5f) {
            g = 2.0f * (g * g);
        } else {
            g = 1.0f - g;
            g = 2.0f * (g * g);
            g = 1.0f - g;
        }
        tv = g * 36864.0f;
        tb = D_800C2C60[(tv >> 4) & 0xFFF] / 8;
        dy = 0;
        dz = -12000;
        if (!(D_800EFD70.flags & 0x8000)) {
            rot += tv;
            vec[0] = (f32)tb * g;
            vec[1] = g * (f32)dy;
            vec[2] = (f32)z + g * (f32)dz;
        }
    }
    func_80011A58(mtx, ang, 0, (s16)rot);
    func_80012DC8(vec, mtx, res);
    pp = p->pos;
    out[0] = (f32)res[0] + pp[0] * 1000.0f;
    out[1] = (f32)res[1] + pp[1] * 1000.0f + 600.0f;
    out[2] = (f32)res[2] + pp[2] * 1000.0f;
    out[3] = ang - 500;
    out[5] = (s16)rot;
    out[4] = 0;
    if (D_800EFD70.mode != 9) {
        if ((p->f5B0 & 0x200) || ((D_800E3770 & 0x10) && D_800F05A8.active == 0 && !(D_800EFD70.flags & 0x8000))) {
            func_8002B5F8(out);
        } else if (p->f5B4 & 0x200) {
            func_8002B6F4(out, p->v54);
        } else {
            goto other;
        }
    } else {
other:
        func_8002B348(out);
    }
    D_800E7968[D_800C9684] = 0;
    if (p->p4E4 != NULL && p->f744 < 6 && (p->p4E4->flags & 0x10)) {
        D_800E7968[D_800C9684] = -(s32)(p->f48C * 1000.0f) * 2;
    }
    if (p->b5CD != 0) {
        f = p->b5CD;
        cs = func_80013774(-f * 0.0625f);
        q2 = D_800E7968;
        f = f * 3.1415927f;
        q = &q2[D_800C9684];
        *q += (s32)(func_800AEA30(f / 6.0f) * 1024.0f * cs);
    }
}
