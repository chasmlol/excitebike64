#include "ctx.h"

typedef struct Bike Bike;

typedef struct { f32 q[4]; } Quat;

typedef struct {
    Vec3f v;
    f32 w;
} Pt;

typedef struct Body {
    Vec3f pos;          /* 0x00 */
    Vec3f vel;          /* 0x0C */
    Vec3f acc;          /* 0x18 */
    u8 pad24[0xC];
    Quat q;             /* 0x30 */
    Vec3f angvel;       /* 0x40 */
    Vec3f angacc;       /* 0x4C */
    f32 m[9];           /* 0x58 */
    s32 f7C;            /* 0x7C */
    Vec3f pts[20];      /* 0x80 */
    f32 f170;           /* 0x170 */
    u8 pad174[8];
    u8 type;            /* 0x17C */
    u8 flags;           /* 0x17D */
    s8 timer;           /* 0x17E */
    u8 pad17F;
    Bike *owner;        /* 0x180 */
} Body;

struct Bike {
    u8 pad0[0x44];
    u8 f44;
    s8 f45;
    u8 pad46[2];
    Body b0;            /* 0x48 */
    Body b1;            /* 0x1CC */
    s32 f350;           /* 0x350 */
    Vec3f f354;         /* 0x354 */
    u8 pad360[0x380 - 0x360];
    f32 f380;
    f32 f384;
    u8 pad388[0x3D0 - 0x388];
    u8 f3D0;
    u8 pad3D1[0x416 - 0x3D1];
    s16 f416;
    u8 pad418[0x41D - 0x418];
    u8 f41D;
    u8 pad41E[0x420 - 0x41E];
    f32 f420;
    u8 pad424[0x43C - 0x424];
    Vec3f f43C;
    u8 pad448[0x478 - 0x448];
    f32 f478;
    f32 f47C;
    u8 pad480[0x494 - 0x480];
    Vec3f f494;
    u8 pad4A0[0x4C0 - 0x4A0];
    Vec3f f4C0;
    u8 pad4CC[0x51C - 0x4CC];
    f32 f51C;
    f32 f520;
    u8 pad524[0x538 - 0x524];
    Vec3f f538;
    u8 pad544[0x564 - 0x544];
    Vec3f f564;
    u8 pad570[0x6E8 - 0x570];
    u8 f6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 f744;
    u8 pad748[0x764 - 0x748];
    u8 f764;
    u8 pad765[0x768 - 0x765];
    s32 f768;
    u8 pad76C[0x774 - 0x76C];
    s32 f774;
    u8 pad778[0x7A5 - 0x778];
    u8 f7A5;
    u8 pad7A6[0x7B4 - 0x7A6];
    f32 f7B4;
    u8 pad7B8[0x7D4 - 0x7B8];
    f32 f7D4;
    u8 pad7D8[0x7E0 - 0x7D8];
};

typedef struct { u8 pad[0x2C]; s32 f2C; } Game;

extern s32 D_800C2350;
extern s32 D_800C238C[];
extern Bike D_800E37C0[];
extern Body D_800E3808;
extern u8 D_800E76C7;
extern f32 D_800E76C8;
extern s32 D_800E76D0;
extern s32 D_800E8D70;
extern f32 D_800EE1FC;
extern f32 D_800EE260;
extern s32 D_800EE298;
extern Vec3f D_800EE300;
extern Game *D_800EFD64;
extern s32 D_800EFDBC;
extern Body D_800F1510;

f32 func_80013408(f32 *a, f32 *b);
void func_80010E28(s32 a0, s32 a1);
void func_8003A744(Vec3f *src, s32 val);
void func_800133A4(Vec3f *a, Vec3f *b, Vec3f *r);
void func_8001327C(f32 *v, f32 *m, f32 *o);
void func_80013568(f32 *m, f32 *q);
void func_80013C20(f32 *o, f32 *p, f32 *q);
f32 func_80046B28(f32 *a, f32 *b, f32 *h, Vec3f *v, s32 n, f32 thr);
void func_80030ECC(s32 a0, s32 a1, s32 a2, s32 a3);
s32 func_8002E920(s32 x, s32 y, s32 z);
void func_8002F4B0(Vec3f *v);
u8 func_80030658(Vec3f *a, Vec3f *b, Vec3f *n, f32 *t, s32 e);
void func_800467C4(Body *b, Vec3f *n, Vec3f *p, f32 k, Vec3f *v, f32 e);
void func_8001B808(Bike *o);
void func_8004DBF8(s32 a0);
void func_8004DDD4(s32 a0);
void func_80061B90(Body *a, Body *b);
f32 func_800AEA30(f32 x);
f32 func_800ADFC0(f32 x);

#define HEIGHT(v) ((f32)func_8002E920((s32)((v)->x * 1000.0f), (s32)((v)->y * 1000.0f), (s32)((v)->z * 1000.0f)) / 1000.0f)

s32 func_800453D8(Body *b, Pt *t, s32 n, u8 m)
{
    Vec3f p;
    Vec3f dv;
    Vec3f rot[40];
    Vec3f world[40];
    Quat dq;
    Quat q;
    f32 depth[42];
    Vec3f w;
    Vec3f savepos;
    Quat saveq;
    Vec3f d;
    Vec3f r;
    f32 tmax;
    f32 hx, lx, hz, lz;
    f32 a, e, x, gk, dt, len, half, f, sn;
    s32 i, k;
    u8 hit;
    Bike *o;
    s32 idx;

    if (b->vel.x > 0.0f) {
        hx = b->vel.x * D_800E76C8 + 1.0f;
        lx = -1.0f;
    } else {
        hx = 1.0f;
        lx = b->vel.x * D_800E76C8 - 1.0f;
    }
    if (b->vel.z > 0.0f) {
        hz = b->vel.z * D_800E76C8 + 1.0f;
        lz = -1.0f;
    } else {
        hz = 1.0f;
        lz = b->vel.z * D_800E76C8 - 1.0f;
    }
    if ((D_800C2350 & 0x200) && b == &D_800E3808) {
        hx = 4.0f;
        lx = -4.0f;
        hz = hx;
        lz = lx;
    }
    func_80030ECC((s32)((b->pos.x + lx) * 1000.0f), (s32)((b->pos.z + lz) * 1000.0f),
                  (s32)((b->pos.x + hx) * 1000.0f), (s32)((b->pos.z + hz) * 1000.0f));
    if (b->timer != 0) {
        b->timer -= D_800E76C7;
        if (b->timer < 0) {
            b->timer = 0;
        }
    }
    dt = D_800E76C8;
    do {
        w.x = b->vel.x + b->acc.x * dt * 0.5f;
        w.y = b->vel.y + b->acc.y * dt * 0.5f;
        w.z = b->vel.z + b->acc.z * dt * 0.5f;
        p.x = w.x * dt;
        p.y = w.y * dt;
        p.z = w.z * dt;
        p.x = b->pos.x + p.x;
        p.y = b->pos.y + p.y;
        p.z = b->pos.z + p.z;
        w.x = b->angvel.x + b->angacc.x * dt * 0.5f;
        w.y = b->angvel.y + b->angacc.y * dt * 0.5f;
        w.z = b->angvel.z + b->angacc.z * dt * 0.5f;
        tmax = 1.0f;
        len = __builtin_sqrtf(w.x * w.x + w.y * w.y + w.z * w.z);
        half = len * dt * 0.5f;
        if (len != 0.0f) {
            sn = func_800AEA30(half) / len;
        } else {
            sn = 0.0f;
        }
        dq.q[0] = func_800ADFC0(half);
        dq.q[1] = w.x * sn;
        dq.q[2] = w.y * sn;
        dq.q[3] = w.z * sn;
        func_80013C20(q.q, dq.q, b->q.q);
        func_80013568(b->m, q.q);
        for (i = 0; i < n; i++) {
            func_8001327C(&t[i].v.x, b->m, &rot[i].x);
            world[i].x = rot[i].x + p.x;
            world[i].y = rot[i].y + p.y;
            world[i].z = rot[i].z + p.z;
        }
        if (m != 0) {
            if (b->type == 4) {
                x = p.y - (f32)func_8002E920((s32)(p.x * 1000.0f), (s32)(p.y * 1000.0f), (s32)(p.z * 1000.0f)) / 1000.0f - 1.0f;
                if (x < 0.0f) {
                    w.x = 0.0f;
                    w.y = -1.0f;
                    w.z = 0.0f;
                    p.y += 0.005f - x;
                    func_800133A4(&b->angvel, &w, &dv);
                    dv.x = b->vel.x + dv.x;
                    dv.y = b->vel.y + dv.y;
                    dv.z = b->vel.z + dv.z;
                    func_8002F4B0(&D_800EE300);
                    func_800467C4(b, &D_800EE300, &w, 0.3f, &dv, 0.7f);
                }
            } else {
                x = 0.0f;
                for (i = 0; i < n; i++) {
                    depth[i] = HEIGHT(world + i) - world[i].y;
                    if (x < depth[i]) {
                        x = depth[i];
                        k = i;
                        func_8002F4B0(&D_800EE300);
                    }
                }
                if (x > 0.0f) {
                    if (func_80046B28(&b->vel.x, &b->angvel.x, depth, rot, n, b->f170) < 0.0f) {
                        b->flags &= ~1;
                        return 0;
                    }
                    p.y += depth[k] + 0.01f;
                    dv.x = (world[k].x - b->pts[k].x) / dt;
                    dv.y = (world[k].y - b->pts[k].y) / dt;
                    dv.z = (world[k].z - b->pts[k].z) / dt;
                    gk = t[k].w;
                    if (b->owner != NULL && b->owner->f44 != 0) {
                        gk /= b->owner->f7B4;
                    }
                    func_800467C4(b, &D_800EE300, &rot[k], gk, &dv, 0.1f);
                    e = dv.x * dv.x + dv.y * dv.y + dv.z * dv.z;
                    o = b->owner;
                    if (b->type == 2 && o->f6E8 == 0 && e > 4.0f && t[k].w > 0.15f) {
                        if (o != NULL) {
                            if (D_800EE260 > o->f384) {
                                o->f384 = D_800EE260;
                            }
                            if (D_800EE1FC > o->f380) {
                                o->f380 = D_800EE1FC;
                            }
                            if (D_800C238C[o - D_800E37C0] == 0xFF) {
                                func_8003A744(&world[k], o - D_800E37C0);
                            }
                        }
                        if (o->f6E8 == 0 && D_800E76D0 - o->f774 >= 0x10) {
                            o->f768 = e;
                            o->f774 = D_800E76D0;
                            o->f764 = 0xFF;
                        }
                    }
                    for (i = 0; i < n; i++) {
                        world[i].x = p.x + rot[i].x;
                        world[i].y = p.y + rot[i].y;
                        world[i].z = p.z + rot[i].z;
                    }
                }
            }
        }
        hit = 0;
        for (i = 0; i < n; i++) {
            if (func_80030658(&b->pts[i], &world[i], &D_800EE300, &tmax, 0)) {
                dv.x = (world[i].x - b->pts[i].x) / dt;
                dv.y = (world[i].y - b->pts[i].y) / dt;
                k = i;
                dv.z = (world[i].z - b->pts[i].z) / dt;
                if (D_800E8D70 & 1) {
                    hit = 1;
                }
            }
        }
        if (tmax < 1.0f) {
            a = func_80013408(&D_800EE300.x, &dv.x);
            x = 0.3f;
            if (b->type < 2) {
                o = b->owner;
                if (D_800EE300.y > 0.75f && (k > 5 || o->f744 > 60)) {
                    o->f768 = 0x200;
                    o->f774 = D_800E76D0;
                    o->f764 = 0xFF;
                    o->f45 = (t[k].v.z > 0.0f) ? -0x19 : -0x1C;
                    o->f43C = o->b0.vel;
                } else if (a < -15.0f) {
                    if (D_800EFD64->f2C == 0 && D_800EFDBC != 10 && D_800EFDBC != 2) {
                        D_800EE298 = D_800E76D0;
                        if ((f32)func_8002E920((s32)(o->b0.pos.x * 1000.0f), (s32)(o->b0.pos.y * 1000.0f), (s32)(o->b0.pos.z * 1000.0f)) / 1000.0f - o->b0.pos.y > 2.0f) {
                            func_8004DBF8(o - D_800E37C0);
                        } else {
                            func_8004DDD4(o - D_800E37C0);
                        }
                    }
                    o->f768 = 0x200;
                    o->f774 = D_800E76D0;
                    o->f764 = 0xFF;
                    o->f45 = -0x1C;
                    o->f43C = o->b0.vel;
                }
                if (o->f420 < 10.0f) {
                    x = 0.2f;
                } else {
                    x = o->f7D4;
                }
            }
            func_800467C4(b, &D_800EE300, &rot[k], x, &dv, 0.05f);
            if (b->type < 2) {
                Bike *o = b->owner;
                if ((o->f416 != 0 || (o->f3D0 > 0 && o->f3D0 < 16)) && o->f744 <= 0) {
                    o->f45 = -0x19;
                    o->f43C = o->b0.vel;
                }
            }
            {
            f32 ln = __builtin_sqrtf(dv.x * dv.x + dv.y * dv.y + dv.z * dv.z);
            if (ln > 2.0f && hit) {
                if (t[k].w > 0.15f) {
                    if (b->type < 2) {
                        b->owner->f354 = t[k].v;
                        b->owner->f350 = 10;
                        func_80010E28(b->owner - D_800E37C0, (s32)(ln * 16.0f));
                    } else if (b->type == 2) {
                        Bike *o = b->owner;
                        if (D_800EE260 > o->f384) {
                            o->f384 = D_800EE260;
                        }
                        if (D_800EE1FC > o->f380) {
                            o->f380 = D_800EE1FC;
                        }
                    }
                }
            }
            }
            dt *= 0.8f;
            b->timer = 15;
        } else {
            f32 ln;
            savepos = b->pos;
            saveq = b->q;
            b->pos = p;
            b->q = q;
            if (b->type < 2 && m == 0) {
                o = b->owner;
                func_8001B808(o);
                if (o->f564.y < 0.5f && o->f51C <= o->f520 + 0.01f) {
                    if (o->f744 > 60 || func_80013408(&o->f564.x, &o->b0.m[3]) < 0.3f) {
                        o->f45 = -0x1C;
                        o->f43C = o->b0.vel;
                    } else {
                        d.x = o->f538.x - p.x;
                        d.y = o->f538.y - p.y;
                        d.z = o->f538.z - p.z;
                        func_800133A4(&d, &b->angvel, &r);
                        r.x = b->vel.x + r.x;
                        r.y = b->vel.y + r.y;
                        r.z = b->vel.z + r.z;
                        ln = o->f564.x * o->f564.x + o->f564.z * o->f564.z;
                        ln = __builtin_sqrtf(ln);
                        w.x = o->f564.x / ln;
                        w.y = 0.0f;
                        w.z = o->f564.z / ln;
                        f = func_80013408(&w.x, &r.x);
                        if (f > 0.0f) {
                            r.x -= f * w.x;
                            r.z -= f * w.z;
                        }
                        dt *= 0.5f;
                        func_800467C4(b, &w, &d, 0.0f, &r, 0.1f);
                        b->pos = savepos;
                        b->q = saveq;
                        func_80013568(b->m, b->q.q);
                        func_8001B808(o);
                        continue;
                    }
                }
                if (o->f4C0.y < 0.5f && o->f478 <= o->f47C + 0.01f) {
                    if (o->f744 > 60 || func_80013408(&o->f4C0.x, &o->b0.m[3]) < 0.3f) {
                        o->f45 = -0x1C;
                        o->f43C = o->b0.vel;
                    } else {
                        d.x = o->f494.x - p.x;
                        d.y = o->f494.y - p.y;
                        d.z = o->f494.z - p.z;
                        func_800133A4(&d, &b->angvel, &r);
                        r.x = b->vel.x + r.x;
                        r.y = b->vel.y + r.y;
                        r.z = b->vel.z + r.z;
                        ln = o->f4C0.x * o->f4C0.x + o->f4C0.z * o->f4C0.z;
                        w.x = o->f4C0.x / ln;
                        w.y = 0.0f;
                        w.z = o->f4C0.z / ln;
                        f = func_80013408(&w.x, &r.x);
                        if (f > 0.0f) {
                            r.x -= f * w.x;
                            r.z -= f * w.z;
                        }
                        func_800467C4(b, &w, &d, 0.0f, &r, 0.1f);
                        b->pos = savepos;
                        b->q = saveq;
                        w.x *= 0.125f;
                        w.y *= 0.125f;
                        w.z *= 0.125f;
                        b->vel.x = w.x + b->vel.x;
                        b->vel.y = w.y + b->vel.y;
                        b->vel.z = w.z + b->vel.z;
                        dt *= 0.5f;
                        func_80013568(b->m, b->q.q);
                        func_8001B808(o);
                        dt -= 0.25f;
                        continue;
                    }
                }
            }
            dt = 0.0f;
        }
    } while (dt > 0.0002f);

    for (i = 0; i < n; i++) {
        b->pts[i] = world[i];
    }
    if (b->type != 4) {
        o = b->owner;
        if (o == NULL || o->f41D != 0) {
            return 0;
        }
        if (o->f44 != 0) {
            if (b->f7C != 0) {
                func_80061B90(b, &o->b0);
                func_80061B90(b, &o->b1);
            }
            return 0;
        }
    }
    if (b->f7C != 0) {
        for (i = 0; i < 8; i++) {
            if (D_800E37C0[i].f7A5 == 0 && D_800E37C0[i].f41D == 0 && D_800E37C0[i].f44 == 0) {
                func_80061B90(b, &D_800E37C0[i].b0);
            }
        }
        if (D_800EFDBC == 2) {
            func_80061B90(b, &D_800F1510);
        }
    }
    return 0;
}
