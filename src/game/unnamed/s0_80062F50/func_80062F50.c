#include "ctx.h"

typedef struct { f32 x, y, z, w; } Quat;

typedef struct {
    s16 _0;
    s16 track;
    s32 x;
    s32 z;
    s16 scale;
    u16 spd;
} Spawn;

typedef struct {
    u8 pad0[0x44];
    u8 b44;
    u8 b45;
    u8 pad46[2];
    Vec3f pos;          /* 0x48 */
    f32 sx, sy, sz;     /* 0x54 */
    f32 v60, v64, v68;  /* 0x60 */
    u8 pad6C[0xC];
    Quat quat;          /* 0x78 */
    f32 f88, f8C, f90, f94; /* 0x88 */
    u8 pad98[8];
    f32 mtx[6];         /* 0xA0 */
    f32 mx, my, mz;     /* 0xB8 */
    u8 padC4[4];
    Vec3f hist[20];     /* 0xC8 */
    f32 f1B8;           /* 0x1B8 */
    u8 pad1BC[8];
    u8 b1C4;
    u8 pad1C5[0x36C - 0x1C5];
    u8 b36C;
    u8 pad36D[3];
    Quat quat2;         /* 0x370 */
    u8 pad380[0x3D0 - 0x380];
    s8 b3D0;
    s8 b3D1;
    u8 pad3D2[0x416 - 0x3D2];
    s16 h416;
    u8 pad418[0x42C - 0x418];
    f32 f42C;
    f32 f430, f434, f438;
    u8 pad43C[0x44C - 0x43C];
    f32 f44C;
    u8 pad450[8];
    s32 w458;
    u8 pad45C[0x478 - 0x45C];
    f32 f478;
    u8 pad47C[0x4FC - 0x47C];
    s32 w4FC;
    u8 pad500[0x51C - 0x500];
    f32 f51C;
    u8 pad520[0x5CD - 0x520];
    u8 b5CD;
    u8 pad5CE[0x624 - 0x5CE];
    s16 h624;
    u8 pad626[2];
    s16 h628;
    u8 pad62A[0x63C - 0x62A];
    f32 f63C;
    u8 pad640[8];
    s32 w648;
    u8 pad64C[8];
    u8 b654;
    u8 pad655[0x6E8 - 0x655];
    u8 b6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 w744;
    s32 w748;
    u8 pad74C[0x7A5 - 0x74C];
    u8 b7A5;
    u8 pad7A6[0x7B0 - 0x7A6];
    s32 w7B0;
    u8 pad7B4[0x7E0 - 0x7B4];
} Bike;

extern u8 D_800F1BC4;
extern s32 D_800F0600;
extern Bike D_800E37C0[8];
extern s32 D_800E76D0;
extern s32 D_800EFDBC;
extern f32 D_800EE4A0;
extern f32 D_800EE484;
extern f32 D_800EE480;
extern f32 D_800F0C08[];
extern f32 D_800C8E14;

void func_8002DA48(s32 i, s32 v);
void func_8005E9A4(s32 a0);
f32 func_800228A0(Vec3f *p);
s32 func_8004B5F0(Vec3f *pos, s32 start, s32 count, u8 mode);
f32 func_8004A944(Vec3f *p, s16 i);
f32 func_80013098(f32 p1, f32 p2);
void func_80013438(Quat *out, f32 x, f32 y, f32 z);
void func_80013568(f32 *m, Quat *q);
void func_8001C780(Bike *b);
void func_8001B808(Bike *b);
void func_8001BC98(Bike *b);
void func_8001FE1C(Bike *b);
void func_8001AEB0(Bike *b, s32 a1);

void func_80062F50(Spawn *sp)
{
    Bike *b;
    s32 track;
    s32 i, idx;
    f32 f;
    s32 spd;
    f32 y, k;
    Vec3f unused;

    if (D_800F1BC4 & 0x10) return;
    if (D_800F0600 & 0x200) return;

    b = &D_800E37C0[6];
    track = sp->track;
    sp->track = -1;
    for (; b < &D_800E37C0[8]; b++) {
        if (b->b7A5 != 0) break;
    }
    if (b == &D_800E37C0[8]) return;

    b->b3D1 = -1;
    b->b7A5 = 0;
    b->b36C = 0;
    b->b5CD = 0;
    b->b45 = 0;
    b->b44 = 0;
    b->b654 = 0;
    b->w748 = 0;
    b->w744 = 0;
    b->h416 = 0;
    b->h624 = 0;
    b->w648 = 0;
    b->w4FC = 0;
    b->w458 = 0;
    b->w7B0 = D_800E76D0;
    b->pos.x = (f32)sp->x / 1000.0f;
    b->pos.z = (f32)sp->z / 1000.0f;
    b->pos.y = 0.0f;
    b->pos.y = func_800228A0(&b->pos);
    for (i = 0; i < 20; i++) {
        b->hist[i] = b->pos;
    }
    b->h628 = func_8004B5F0(&b->pos, track, 15, 0);
    b->f42C = func_8004A944(&b->pos, b->h628);
    b->f63C = D_800EE4A0;
    f = func_8004A944(&b->pos, b->h628);
    b->f42C = f;
    D_800F0C08[b - &D_800E37C0[6]] = f;
    func_80013438(&b->quat, 0.0f, 0.0f, -func_80013098(D_800EE484, D_800EE480));
    func_8001C780(b);
    func_80013568(b->mtx, &b->quat);
    b->sx = b->mx * ((f32)sp->scale / 1000.0f);
    b->sy = b->my * ((f32)sp->scale / 1000.0f);
    b->sz = b->mz * ((f32)sp->scale / 1000.0f);
    b->f51C = 0.0f;
    b->f478 = 0.0f;
    b->f90 = 0.0f;
    b->f8C = 0.0f;
    b->f88 = 0.0f;
    b->f94 = 0.0f;
    b->v68 = 0.0f;
    b->v64 = 0.0f;
    b->v60 = 0.0f;
    b->f430 = 0.0f;
    b->f434 = 0.0f;
    b->f438 = 0.0f;
    b->f1B8 = 1.0f;
    b->b3D0 = -1;
    b->quat2 = b->quat;
    b->pos.y = func_800228A0(&b->pos) + 0.0f + 0.5f
        - (D_800EFDBC == 6 ? 3675.0002f / (2.0f * (D_800C8E14 * 20000.0f)) : 0.12250001f);
    b->w744 = 4;
    func_8001B808(b);
    func_8001B808(b);
    func_8001BC98(b);
    idx = b - D_800E37C0;
    func_8005E9A4(idx);
    func_8002DA48(idx, 0);
    spd = sp->spd;
    b->b6E8 = 2;
    b->b1C4 = 1;
    b->f44C = (f32)spd / 29491.0f;
    func_8001FE1C(b);
    func_8001AEB0(b, 0);
}
