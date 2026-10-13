#include "ctx.h"

typedef struct Node Node;
typedef struct {
    u8 pad0[0x34];
    s32 x34, x38, x3C;
    u8 rot[1];
} Model;

typedef struct Bike {
    s32 p0[3];                 /* 0x000 */
    u8 pad0C[0x44 - 0x0C];
    u8 active;                 /* 0x044 */
    s8 x45;                    /* 0x045 */
    u8 pad46[2];
    Vec3f pos;                 /* 0x048 */
    Vec3f vel;                 /* 0x054 */
    u8 pad60[0x88 - 0x60];
    Vec3f p88;                 /* 0x088 */
    u8 pad94[0xA0 - 0x94];
    Vec3f mtx[3];              /* 0x0A0 */
    f32 xC4;
    Vec3f pts[11];           /* 0x0C8 */
    u8 pad14C[0x1B8 - 0x14C];
    f32 x1B8;                  /* 0x1B8 */
    u8 pad1BC[0x1C4 - 0x1BC];
    u8 x1C4;                   /* 0x1C4 */
    u8 x1C5;                   /* 0x1C5 */
    u8 pad1C6[0x1CC - 0x1C6];
    Vec3f p1CC;                /* 0x1CC */
    Vec3f p1D8;                /* 0x1D8 */
    u8 pad1E4[0x1FC - 0x1E4];
    f32 x1FC[4];               /* 0x1FC */
    Vec3f p20C;                /* 0x20C */
    u8 pad218[0x248 - 0x218];
    Node *x248;                /* 0x248 */
    Vec3f pts2[8];             /* 0x24C */
    u8 pad2AC[0x33C - 0x2AC];
    f32 x33C;                  /* 0x33C */
    u8 pad340[0x349 - 0x340];
    u8 x349;                   /* 0x349 */
    u8 pad34A[0x36C - 0x34A];
    u8 x36C;                   /* 0x36C */
    u8 x36D;                   /* 0x36D */
    u8 pad36E[0x3AC - 0x36E];
    s32 x3AC;                  /* 0x3AC */
    u8 pad3B0[0x3CC - 0x3B0];
    s32 x3CC;                  /* 0x3CC */
    u8 x3D0;                   /* 0x3D0 */
    u8 x3D1;                   /* 0x3D1 */
    u8 pad3D2[0x3FB - 0x3D2];
    u8 x3FB;                   /* 0x3FB */
    u8 pad3FC[0x416 - 0x3FC];
    s16 x416;                  /* 0x416 */
    u8 pad418[0x420 - 0x418];
    f32 x420;                  /* 0x420 */
    u8 pad424[4];
    f32 x428;                  /* 0x428 */
    u8 pad42C[0x43C - 0x42C];
    Vec3f p43C;                /* 0x43C */
    u8 pad448[0x5A8 - 0x448];
    u16 x5A8;                  /* 0x5A8 */
    u16 x5AA;                  /* 0x5AA */
    u8 pad5AC[0x624 - 0x5AC];
    s16 x624;                  /* 0x624 */
    s16 x626;                  /* 0x626 */
    s16 x628;                  /* 0x628 */
    u8 pad62A[0x644 - 0x62A];
    s32 x644;                  /* 0x644 */
    u8 pad648[0x655 - 0x648];
    u8 x655;                   /* 0x655 */
    u8 pad656[0x658 - 0x656];
    s32 x658;                  /* 0x658 */
    Model *x65C;               /* 0x65C */
    u8 pad660[0x6E8 - 0x660];
    u8 x6E8;                   /* 0x6E8 */
    u8 pad6E9[0x744 - 0x6E9];
    s32 x744;                  /* 0x744 */
    u8 pad748[0x7A8 - 0x748];
    u8 x7A8;                   /* 0x7A8 */
    u8 pad7A9[0x7E0 - 0x7A9];
} Bike;

typedef struct { u8 pad[0x58]; u32 flags; } Ctl;

extern Bike D_800E37C0[];
extern s32 D_800EE298;
extern s32 D_800E76D0;
extern s32 D_800EFDBC;
extern Ctl D_800F05A8;
extern f32 D_800CA440[][4];
extern f32 D_800CA4F0[][8][4];

void func_8006B190(s32 x);
void func_80061D54(Node *p);
void func_800133A4(Vec3f *a, Vec3f *b, Vec3f *r);
void func_8004F410(s32 p);
void func_8001327C(f32 *v, f32 *m, f32 *o);
s32 func_8004B5F0(Vec3f *pos, s32 start, s32 count, u8 mode);
void func_80013438(f32 *out, f32 x, f32 y, f32 z);
void func_80012EBC(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4);
void func_8004DBF8(s32 i);
void func_8003EAC4(s32 a, s32 i);
void func_8003D4BC(Model *m, s32 i);
void func_8006FE84(Bike *b, s32 n);

void func_800445B0(Bike *b, s32 n)
{
    s16 rx, ry, rz;
    Vec3f v;
    Vec3f c;
    Vec3f w;
    s32 idx;
    s32 i;
    f32 s;
    Vec3f *src;

    if (b->active != 0) {
        return;
    }
    if (b->x6E8 == 0 && D_800EE298 != D_800E76D0) {
        func_8004DBF8(b - D_800E37C0);
    }
    b->x644 = 0;
    b->x655 = 0;
    if (b == &D_800E37C0[1]) {
        D_800F05A8.flags &= ~0x1000;
    }
    b->x3FB = 0;
    b->x7A8 = 0;
    b->x5A8 = b->x5AA;
    if (b->x6E8 == 0) {
        func_8006B190(0x3C);
        func_8004F410(0);
    }
    if (D_800EFDBC == 6) {
        b->x626 = func_8004B5F0(&b->pos, b->x628, 10, 1);
    }
    idx = b - D_800E37C0;
    b->x416 = 0;
    b->x744 = 0;
    b->x1C4 = 2;
    b->x36C = 1;
    b->x36D = (n - 0x19) / 3;
    func_8003EAC4(b->x658, idx);
    func_8003D4BC(b->x65C, idx);
    func_80012EBC((s32 *)b->x65C->rot, (u16 *)&rx, (u16 *)&ry, (u16 *)&rz);
    func_80013438(b->x1FC, (f32)rx * 3.1415927f * 3.0517578e-05f,
                  (f32)ry * 3.1415927f * 3.0517578e-05f,
                  (f32)rz * 3.1415927f * 3.0517578e-05f);
    b->active = 1;
    b->x624 = 0;
    b->x3D1 = b->x3D0;
    b->x3CC = b->x3AC;
    b->x428 = b->x420 + 5.0f;
    if (b->x428 > 32.0f) {
        b->x428 = 32.0f;
    }
    b->x420 = 0.0f;
    for (i = 0; i < 11; i++) {
        func_8001327C(D_800CA440[i], (f32 *)b->mtx, &v.x);
        b->pts[i].x = v.x + b->pos.x;
        b->pts[i].y = v.y + b->pos.y;
        b->pts[i].z = v.z + b->pos.z;
    }
    if (n == 0x22) {
        s = 2.0f;
        goto adj;
    }
    if (n == 0x1F) {
        s = -2.0f;
    adj:
        if (b->x420 < 5.0f) {
            s *= 0.25f;
        }
        b->p88.x += s * b->mtx[2].x;
        b->p88.y += s * b->mtx[2].y;
        b->p88.z += s * b->mtx[2].z;
    }
    b->p1CC.x = (f32)(b->x65C->x34 + b->p0[0]) / 1000.0f;
    b->p1CC.y = (f32)(b->x65C->x38 + b->p0[1]) / 1000.0f;
    b->p1CC.z = (f32)(b->x65C->x3C + b->p0[2]) / 1000.0f;
    s = __builtin_sqrtf(b->vel.x * b->vel.x + b->vel.y * b->vel.y + b->vel.z * b->vel.z) / 5.0f;
    if (s < 1.0f) {
        s = 1.0f;
    }
    if (s > 5.0f) {
        s = 5.0f;
    }
    src = &b->vel;
    if (b->x45 < 0) {
        src = &b->p43C;
    }
    func_800133A4(&b->p88, &b->mtx[1], &c);
    b->p1D8.x = src->x + c.x + b->mtx[1].x * s;
    b->p1D8.y = src->y + c.y + b->mtx[1].y * s;
    b->p1D8.z = src->z + c.z + b->mtx[1].z * s;
    b->p20C = b->p88;
    if (n == 0x19) {
        b->p20C.x -= b->mtx[0].x + b->mtx[0].x;
        b->p20C.y -= b->mtx[0].y + b->mtx[0].y;
        b->p20C.z -= b->mtx[0].z + b->mtx[0].z;
    }
    b->x3AC = 0;
    b->x349 |= 1;
    b->x1C5 |= 1;
    for (i = 0; i < 8; i++) {
        func_8001327C(D_800CA4F0[b->x36D][i], (f32 *)b->mtx, &w.x);
        b->pts2[i].x = w.x + b->p1CC.x;
        b->pts2[i].y = w.y + b->p1CC.y;
        b->pts2[i].z = w.z + b->p1CC.z;
    }
    b->x1B8 = 1.0f;
    b->x33C = 1.0f;
    func_8006FE84(b, n);
    func_80061D54(b->x248);
}
