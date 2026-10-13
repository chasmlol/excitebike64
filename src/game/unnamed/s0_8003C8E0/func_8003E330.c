#include "ctx.h"

typedef struct Ent {
    u8 p00[0x18];
    s32 f18;
    s32 f1C;
    s32 f20;
    u8 p24[0x44 - 0x24];
    u8 f44;
    u8 p45[0x1FC - 0x45];
    f32 f1FC[4];
    u8 p20C[0x36C - 0x20C];
    u8 f36C;
    u8 p36D[0x3AC - 0x36D];
    s32 f3AC;
    u8 p3B0[0x3B8 - 0x3B0];
    s16 f3B8;
    u8 p3BA[0x3D0 - 0x3BA];
    s8 f3D0;
    u8 p3D1[0x7E0 - 0x3D1];
} Ent;

typedef struct Iv3 { s32 w[3]; } Iv3;
typedef struct Blk9 { s32 w[9]; } Blk9;

typedef struct Bone {
    s32 f00;
    Iv3 pos;
    Blk9 mtx;
    Iv3 ppos;
    Blk9 pmtx;
    u16 rot[3];
    s16 base[3];
    u8 p70[0x10];
} Bone;

typedef struct Anim {
    u8 p00[4];
    s32 count;
    u8 p08[8];
    s16 *data;
} Anim;

typedef struct Tab {
    u32 flags;
    u8 p04[8];
    Anim *keys;
    Anim *sub;
    u8 p14[0x3C - 0x14];
} Tab;

extern Ent D_800E37C0[];
extern Tab D_800C9A40[];

void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
void func_80012D94(s32 *param_1);
void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3);
void func_80011C1C(s32 *a, s32 *b, s32 *c);
void func_80012EBC(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4);
void func_8003C4A4(s32 *param_1, s32 param_2);
void func_80013650(s32 *a, f32 *b);

#define LERPROT(b, x, y, z, t)                                              {                                                                           u16 cx = (b)->rot[0];                                                   u16 cy;                                                                 u16 cz;                                                                 cx += (s16)((x) - cx) * (t) / 256;                                      cy = (b)->rot[1];                                                       cy += (s16)((y) - cy) * (t) / 256;                                      cz = (b)->rot[2];                                                       cz += (s16)((z) - cz) * (t) / 256;                                      func_80011A58((b)->mtx.w, (s16)cx, (s16)cy, (s16)cz);               }

static inline void lerpRot(Bone *b, s32 x, s32 y, s32 z, s32 t)
{
    u16 cx = b->rot[0];
    u16 cy;
    u16 cz;
    cx += (s16)(x - cx) * t / 256;
    cy = b->rot[1];
    cy += (s16)(y - cy) * t / 256;
    cz = b->rot[2];
    cz += (s16)(z - cz) * t / 256;
    func_80011A58(b->mtx.w, (s16)cx, (s16)cy, (s16)cz);
}

static inline s16 *getFrame(s16 *d, s32 n, s32 size)
{
    return d + n * size;
}

void func_8003E330(Bone *bones, s32 param_2, s32 param_3)
{
    s32 m0[10];
    s32 m1[10];
    s32 v0[4];
    s32 v1[3];
    s32 v2[3];
    s32 m2[14];
    u16 rx;
    u16 ry;
    u16 rz;
    Anim *sub;
    Tab *t;
    Anim *keys;
    s32 s4;
    s16 *fr;
    s16 *sf;
    s8 *rot0;
    s8 *r;
    s32 i;
    s32 w;
    Ent *base;
    s32 off;

    {
        s8 ix = D_800E37C0[param_2].f3D0;
        Tab *tb = D_800C9A40;
        t = &tb[ix];
    }
    keys = t->keys;
    sub = t->sub;
    s4 = D_800E37C0[param_2].f3AC >> 8;
    if ((t->flags & 8) != 0 && !(s4 < keys->count)) {
        s4 -= 5;
        s4 += (s16)(D_800E37C0[param_2].f3B8 % 5);
    }
    if (s4 < 0) {
        s4 = 0;
    }
    if (s4 >= keys->count) {
        s4 = keys->count - 1;
    }
    fr = getFrame(keys->data, s4, 26);
    rot0 = (s8 *)fr + 6;

    if (param_3 == 0x100) {
        for (i = 1; i < 15; i++) {
            func_80011A58(bones[i].mtx.w, rot0[i * 3] << 8, rot0[i * 3 + 1] << 8, rot0[i * 3 + 2] << 8);
        }
    } else {
        w = param_3;
        for (i = 1; i < 15; i++) {
            lerpRot(&bones[i], rot0[i * 3] << 8, rot0[i * 3 + 1] << 8, rot0[i * 3 + 2] << 8, w);
        }
    }

    if (sub != 0) {
        sf = getFrame(sub->data, s4, 6);
        v0[0] = fr[0] - sf[0];
        v0[1] = fr[1] - sf[1];
        v0[2] = fr[2] - sf[2];
        r = (s8 *)sf + 6;
        func_80011A58(m0, r[0] << 8, r[1] << 8, r[2] << 8);
        func_80012D94(m0);
        func_80012DC8(v0, m0, v1);
        v2[0] = v1[0];
        v2[1] = v1[1];
        v2[2] = v1[2];
        func_80011A58(m1, rot0[0] << 8, rot0[1] << 8, rot0[2] << 8);
        func_80011C1C(m1, m0, m2);
    } else {
        func_80011A58(m2, rot0[0] << 8, rot0[1] << 8, rot0[2] << 8);
        v2[0] = fr[0];
        v2[1] = fr[1];
        v2[2] = fr[2];
    }

    base = D_800E37C0;
    off = param_2 * sizeof(Ent);
    if (((Ent *)((u8 *)base + off))->f44 == 0) {
        func_80011A58(m0, D_800E37C0[param_2].f18, D_800E37C0[param_2].f1C, D_800E37C0[param_2].f20);
        func_80012DC8(v2, m0, v0);
        func_80011C1C(m2, m0, bones->mtx.w);
    } else {
        if (param_3 == 0) {
            w = 1;
            i = 2;
        } else {
            w = 3;
            i = 4;
        }
        func_80013650(m1, ((Ent *)((u8 *)base + off))->f1FC);
        func_80011C1C(m2, m1, bones->mtx.w);
        func_80012DC8(v2, m1, v0);
    }

    if (param_3 != 0x100 && D_800E37C0[param_2].f36C != 1) {
        for (i = 0; i < 3; i++) {
            bones->pos.w[i] = (v0[i] * param_3 + bones->base[i] * (0x100 - param_3)) / 256;
        }
        func_80012EBC(bones->mtx.w, &rx, &ry, &rz);
        w = param_3;
        LERPROT(bones, rx, ry, rz, w);
    } else {
        bones->pos.w[0] = v0[0];
        bones->pos.w[1] = v0[1];
        bones->pos.w[2] = v0[2];
    }

    if (D_800E37C0[param_2].f44 != 0) {
        bones->pos.w[0] = 0;
        bones->pos.w[1] = 0;
        bones->pos.w[2] = 0;
    }
    bones->pmtx = bones->mtx;
    bones->ppos = bones->pos;
    func_8003C4A4((s32 *)bones, 0xf);
}
