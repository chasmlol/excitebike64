#include "ctx.h"

typedef struct { s32 m[9]; } Mtx3;

typedef struct Node {
    Vec3f pos;          /* 0x00 */
    Vec3f ppos;         /* 0x0C */
    Vec3f vel;          /* 0x18 */
    Vec3f acc;          /* 0x24 */
    s32 pad30;
    s32 pad34;
    struct Node *prev;  /* 0x38 */
    struct Node *next;  /* 0x3C */
    f32 len40;
    f32 len44;
} Node;

typedef struct {
    s32 f0;
    s32 pos[3];         /* 0x04 */
    u8 pad10[0x24];
    s32 p[3];           /* 0x34 */
    s32 m[9];           /* 0x40 */
    u8 pad64[0x80 - 0x64];
} Skel;

typedef struct {
    u8 pad0[0xA48];
    Node *nodes;        /* 0xA48 */
    s32 padA4C;
    Skel *skel;         /* 0xA50 */
    u8 padA54[0xA80 - 0xA54];
} Rider;

typedef struct {
    s32 pos[3];         /* 0x00 */
    s32 alt[3];         /* 0x0C */
    u8 pad18[0x44 - 0x18];
    u8 f44;
    u8 pad45[0x54 - 0x45];
    Vec3f vel;          /* 0x54 */
    f32 f60;
    f32 f64;
    f32 f68;
    u8 pad6C[0x420 - 0x6C];
    f32 f420;
    u8 pad424[0x7E0 - 0x424];
} Player;

typedef struct {
    u8 pad0[0xB4];
    s32 pos[3];         /* 0xB4 */
    Mtx3 mC0;           /* 0xC0 */
    u8 padE4[0x134 - 0xE4];
    s32 off[3];         /* 0x134 */
    Mtx3 m140;          /* 0x140 */
} Obj;

extern Rider D_800DC150[];
extern Player D_800E37C0[];
extern s16 D_800C2C60[];
extern s32 D_800E76D0;
extern f32 D_800CCFD0[];

void func_80012D94(s32 *p);
void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3);
void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio);

void func_80070600(s32 idx, Obj *o) {
    Vec3f prev;
    s32 v[3];
    s32 w[3];
    Mtx3 m;
    s32 c[3];
    s32 d[3];
    s16 a1;
    s32 r1;
    s16 a2;
    s32 r2;
    Node *n;
    Node *nd;
    Skel *sk;
    s32 bx, by, bz;
    s32 i;
    s32 k;
    f32 dx, dy, dz, len, t, g;
    s32 iy;
    s32 sn;
    s16 cs;

    n = D_800DC150[idx].nodes;
    prev = n->pos;
    v[0] = 0;
    v[1] = 10;
    v[2] = -88;
    func_80012DC8(v, o->m140.m, w);
    w[0] += o->off[0];
    w[1] += o->off[1];
    w[2] += o->off[2];
    if (D_800E37C0[idx].f44 == 0) {
        bx = D_800E37C0[idx].pos[0];
        by = D_800E37C0[idx].pos[1];
        bz = D_800E37C0[idx].pos[2];
    } else {
        bx = D_800E37C0[idx].alt[0];
        by = D_800E37C0[idx].alt[1];
        bz = D_800E37C0[idx].alt[2];
    }
    n->pos.x = (f32)(w[0] + bx) / 1000.0f;
    n->pos.y = (f32)(w[1] + by) / 1000.0f;
    n->pos.z = (f32)(w[2] + bz) / 1000.0f;
    dx = n->pos.x - prev.x;
    dy = n->pos.y - prev.y;
    dz = n->pos.z - prev.z;
    for (i = 1; i < 4; i++) {
        n[i].pos.x += dx * 0.7f;
        n[i].pos.y += dy * 0.7f;
        n[i].pos.z += dz * 0.7f;
    }

    g = 9.8f;
    prev = D_800E37C0[idx].vel;
    {
        f32 l = __builtin_sqrtf(prev.x * prev.x + prev.y * prev.y + prev.z * prev.z);
        if (l != 0.0f) {
            prev.x /= l;
            prev.y /= l;
            prev.z /= l;
        }
    }
    t = D_800E37C0[idx].f60 * prev.z - D_800E37C0[idx].f68 * prev.x;
    if (t < 2.0f && t > -2.0f) {
        t = 0.0f;
    }
    prev.x *= t;
    prev.y *= t;
    prev.z *= t;
    for (i = 0; i < 4; i++) {
        n[i].acc.x = 0.0f;
        n[i].acc.y = -g * 0.05f;
        n[i].acc.z = 0.0f;
        n[i].acc.x -= prev.z * 0.1f;
        n[i].acc.z += prev.x * 0.1f;
        n[i].acc.x -= D_800E37C0[idx].vel.x * 0.05f;
        n[i].acc.y -= D_800E37C0[idx].vel.y * 0.05f;
        n[i].acc.z -= D_800E37C0[idx].vel.z * 0.05f;
    }

    for (k = 1; k < 4; k++) {
        if (n[k].prev != NULL) {
            dx = n[k].prev->pos.x - n[k].pos.x;
            dy = n[k].prev->pos.y - n[k].pos.y;
            dz = n[k].prev->pos.z - n[k].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len != 0.0f) {
                dx /= len;
                dy /= len;
                dz /= len;
            }
            len -= n[k].len40;
            n[k].acc.x += dx * len;
            n[k].acc.y += dy * len;
            n[k].acc.z += dz * len;
        }
        if (n[k].next != NULL) {
            dx = n[k].next->pos.x - n[k].pos.x;
            dy = n[k].next->pos.y - n[k].pos.y;
            dz = n[k].next->pos.z - n[k].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len != 0.0f) {
                dx /= len;
                dy /= len;
                dz /= len;
            }
            len -= n[k].len44;
            n[k].acc.x += dx * len;
            n[k].acc.y += dy * len;
            n[k].acc.z += dz * len;
        }
    }

    for (i = 1; i < 4; i++) {
        n[i].vel.x += n[i].acc.x / 0.05f / 60.0f;
        n[i].vel.y += n[i].acc.y / 0.05f / 60.0f;
        n[i].vel.z += n[i].acc.z / 0.05f / 60.0f;
        n[i].vel.x *= 0.96f;
        n[i].vel.y *= 0.96f;
        n[i].vel.z *= 0.96f;
        n[i].pos.x += n[i].vel.x / 60.0f;
        n[i].pos.y += n[i].vel.y / 60.0f;
        n[i].pos.z += n[i].vel.z / 60.0f;
    }

    for (i = 0; i < 4; i++) {
        if (n[i].next != NULL) {
            dx = n[i].next->pos.x - n[i].pos.x;
            dy = n[i].next->pos.y - n[i].pos.y;
            dz = n[i].next->pos.z - n[i].pos.z;
            len = __builtin_sqrtf(dx * dx + dy * dy + dz * dz);
            if (len > n[i].len44 + 0.05f) {
                n[i].next->pos.x = n[i].pos.x + dx / len * (n[i].len44 + 0.05f);
                n[i].next->pos.y = n[i].pos.y + dy / len * (n[i].len44 + 0.05f);
                n[i].next->pos.z = n[i].pos.z + dz / len * (n[i].len44 + 0.05f);
            }
            if (len < n[i].len44 - 0.05f) {
                n[i].next->pos.x = n[i].pos.x + dx / len * (n[i].len44 - 0.05f);
                n[i].next->pos.y = n[i].pos.y + dy / len * (n[i].len44 - 0.05f);
                n[i].next->pos.z = n[i].pos.z + dz / len * (n[i].len44 - 0.05f);
            }
        }
    }

    m = o->mC0;
    func_80012D94(m.m);
    for (i = 1; i < 4; i++) {
        v[0] = (s32)(n[i].pos.x * 1000.0f) - (bx + o->pos[0]);
        v[1] = (s32)(n[i].pos.y * 1000.0f) - (by + o->pos[1]);
        v[2] = (s32)(n[i].pos.z * 1000.0f) - (bz + o->pos[2]);
        func_80012DC8(v, m.m, w);
        if (w[2] > -100) {
            w[2] = -100;
        }
        func_80012DC8(w, o->mC0.m, v);
        n[i].pos.x = (f32)(v[0] + bx + o->pos[0]) / 1000.0f;
        n[i].pos.y = (f32)(v[1] + by + o->pos[1]) / 1000.0f;
        n[i].pos.z = (f32)(v[2] + bz + o->pos[2]) / 1000.0f;
    }

    if (idx) i = 5;
    sk = D_800DC150[idx].skel;
    sk->pos[0] = (s32)(n->pos.x * 1000.0f) - bx;
    sk->pos[1] = (s32)(n->pos.y * 1000.0f) - by;
    sk->pos[2] = (s32)(n->pos.z * 1000.0f) - bz;
    n->ppos = n->pos;
    for (i = 0; i < 3; i++) {
        sk[i].p[0] = n[i].ppos.x * 1000.0f - bx;
        sk[i].p[1] = n[i].ppos.y * 1000.0f - by;
        sk[i].p[2] = n[i].ppos.z * 1000.0f - bz;
        iy = (n[i + 1].pos.y - n[i].ppos.y) * 1000.0f;
        func_8001254C((n[i + 1].pos.x - n[i].ppos.x) * 1000.0f, (n[i + 1].pos.z - n[i].ppos.z) * 1000.0f, &a1, &r1);
        func_8001254C(iy, r1, &a2, &r2);
        if (i != 0) {
            a1 = a1 + D_800C2C60[((u32)(D_800E76D0 * 4000 + i * 10000) >> 4) & 0xFFF] / 16 * D_800E37C0[idx].f420 / 25.0f;
            a2 = a2 + (s16)((s16)D_800C2C60[(((D_800E76D0 * 4400 + i * 12000) >> 4) - 0x400) & 0xFFF] / 12) * D_800E37C0[idx].f420 / 25.0f;
        }
        func_80011A58(sk[i].m, a2, 0, 0x8000 - a1);
        c[0] = 0;
        c[1] = 0;
        c[2] = D_800CCFD0[i + 1] * 1000.0f;
        func_80012DC8(c, sk[i].m, d);
        n[i + 1].ppos.x = n[i].pos.x + (f32)d[0] / 1000.0f;
        n[i + 1].ppos.y = n[i].pos.y + (f32)d[1] / 1000.0f;
        n[i + 1].ppos.z = n[i].pos.z + (f32)d[2] / 1000.0f;
    }
}
