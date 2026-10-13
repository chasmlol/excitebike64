#include "ctx.h"

u32 func_8000EB18(u32 r);

typedef struct {
    u8 flag;
    u8 p6;
    u16 pad;
    s32 p1;
    s32 cx;
    s32 cz;
    s32 dx;
    s32 dz;
} Rec;

typedef struct { s16 x, y, z; s16 pad[5]; } Pt;
typedef struct { s32 f0, f4, a, b, c, d; s32 pad[2]; } Box;
typedef struct { s32 pad[11]; s32 mode; } Ctl;

extern Rec *D_800F0798[4];
extern s32 D_800F0770;
extern Rec *D_800F0774;
extern Ctl *D_800EFD64;
extern Box *D_800E8948;
extern Pt *D_800F07F8;

Rec *func_8005DCFC(s32 p1, s32 *p2, s32 *p3, s32 *p4, s32 *p5, s32 p6)
{
    s32 i, j, mode;
    s32 sx, sy;
    s32 cz, dx, dz;
    u32 r;
    Box *b;
    Pt *pt;
    Pt *q;
    Rec *rec;
    Rec *r2;
    Rec *rb;

    for (i = 0; i < 4; i++) {
        if (&D_800F0774[D_800F0770] == D_800F0798[i]) D_800F0798[i] = 0;
    }

    mode = D_800EFD64->mode;
    if (mode == 1) {
        b = &D_800E8948[p1];
        sx = (b->a + b->c) / 2;
        sy = (b->b + b->d) / 2;
    } else if (mode == 2) {
        sx = (s16)(p1 >> 16) * 8000 + 4000;
        sy = (s16)p1 * 8000 + 4000;
    } else {
        sx = sy = 0;
    }

    j = D_800F0770 * 4;
    pt = (Pt *)((u32)(j * 16) + (u32)D_800F07F8);
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
    rb = D_800F0774;
    rec = (Rec *)((s32)(j * 24) + (s32)rb);
    rec->cx = (p4[0] + p5[0]) / 2;
    rec->cz = cz = (p4[2] + p5[2]) / 2;
    dx = rec->cx - (p2[0] + p3[0]) / 2;
    dz = cz - (p2[2] + p3[2]) / 2;
    r = func_8000EB18(dx * dx + dz * dz);
    if (r != 0) {
        dx = (dx * 0x4000) / (s32)r;
        dz = (dz * 0x4000) / (s32)r;
    }

    r2 = (Rec *)((u32)(j * 24) + (u32)D_800F0774);
    r2->flag = 1;
    r2->dx = dx;
    r2->dz = dz;
    r2->p1 = p1;
    D_800F0774[j].p6 = p6;

    D_800F0770 = (D_800F0770 + 1) % 128;
    return &D_800F0774[j];
}
