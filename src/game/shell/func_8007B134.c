#include "ctx.h"

typedef struct { s32 x, y, w, h; } Rect;

typedef struct { s32 f0; s32 pad[14]; s32 mode; s32 pad2[13]; s32 *slot[64]; } G;
extern G D_800EFD70;
extern u8 D_800E1718[];
typedef struct { s32 f0; s32 f4; s32 pad[16]; s32 a48[4]; } Tb;
extern Tb D_800CDB78[];
extern s32 D_800CDD88[];
extern s32 D_800CDD98[];

s32 *func_800C1E60(s32 *p, s32 a, s32 b, s32 c, s32 d);
char *func_800C1F2C(s32 *p, char *s);
void func_800C0ACC(s32 *p, s32 a, s32 b);

void func_8007B134(s32 x, s32 y, s32 p3, s32 p4)
{
    G *g = &D_800EFD70;
    s32 mode = g->mode;
    s32 yy, v, i, j, k, sy, t;
    Rect *q;
    u8 *base;

    if (mode == 1) {
        return;
    }
    yy = y;
    if (D_800EFD70.f0 & (0x400 << p4)) {
        yy += 600;
    }
    base = D_800E1718;
    q = (Rect *)(base + (p4 * 16 + (mode - 1) * 64));
    v = q->y + q->h + 5;
    y = yy;
    if (v < yy) {
        y = v;
    }
    func_800C1E60(D_800EFD70.slot[p4 * 9], 0x40, 1, 6, 0x21);
    for (i = 1; i < 9; i++) {
        func_800C1E60(D_800EFD70.slot[i + p4 * 9], 0x40, 1, 2, 0x20);
    }
    func_800C1F2C(D_800EFD70.slot[p4 * 9], (char *)D_800CDB78[p3].f4);
    func_800C0ACC(D_800EFD70.slot[p4 * 9], x, y - 0x1F);
    i = 0;
    do {
        k = i + 1;
        func_800C1F2C(D_800EFD70.slot[p4 * 9 + k], (char *)D_800CDD88[i]);
        t = i * 14 - 11;
        sy = yy + t;
        func_800C0ACC(D_800EFD70.slot[p4 * 9 + k], x - 0x46, sy);
        j = i + 5;
        func_800C1F2C(D_800EFD70.slot[p4 * 9 + j], (char *)D_800CDD98[D_800CDB78[p3].a48[i]]);
        func_800C0ACC(D_800EFD70.slot[p4 * 9 + j], x + 0x12, sy);
        i = k;
    } while (i < 4);
}
