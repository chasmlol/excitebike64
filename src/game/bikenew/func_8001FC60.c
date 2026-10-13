#include "ctx.h"

typedef struct Bike {
    u8 pad0[0x48];
    f32 f48[3];
    f32 pos[3];          /* 54 */
    u8 pad1[0xA0 - 0x60];
    f32 dir[3];          /* A0 */
    u8 pad2[0xC8 - 0xAC];
    f32 pts[8][3];       /* C8 */
    u8 pad3[0x1C5 - 0x128];
    u8 flags;            /* 1C5 */
    u8 pad4[0x438 - 0x1C6];
    f32 f438;
    u8 pad5[0x458 - 0x43C];
    s32 f458;
    u8 pad6[0x4FC - 0x45C];
    s32 f4FC;
    u8 pad7[0x7E0 - 0x500];
} Bike;

typedef struct {
    f32 loc[3];
    f32 pad0;
    f32 zv[3];
    f32 pad1[5];
    f32 one[2];
} Locals;

extern f32 D_800E76C8;
extern s32 D_800E76D0;
extern Bike D_800E37C0[];
extern u8 D_800C94C8[];
extern u8 D_800C9548[];
extern u8 D_800EE300[];

s32 func_800453D8(f32 *a, void *b, s32 c, s32 d);
u8 func_80030658(f32 *a, f32 *b, void *c, f32 *d, s32 e);
void func_800445B0(Bike *p, s32 n);

void func_8001FC60(Bike *b)
{
    Locals L;
    f32 c;
    f32 k;
    s32 n;
    s32 id;
    s32 i;

    c = D_800E76C8 * 0.5f;
    L.zv[0] = L.zv[1] = L.zv[2] = 0.0f;
    k = -b->f438 * c;
    L.loc[0] = b->dir[0] * k;
    L.loc[1] = b->dir[1] * k;
    L.loc[2] = b->dir[2] * k;
    b->pos[0] = L.loc[0] + b->pos[0];
    b->pos[1] = L.loc[1] + b->pos[1];
    b->pos[2] = L.loc[2] + b->pos[2];
    n = 8;
    if (b->f4FC <= 0 && b->f458 <= 0) {
        n = 6;
    }
    func_800453D8(b->f48, D_800C94C8, n, 0);
    b->flags |= 1;
    id = b - D_800E37C0;
    b->pos[0] -= L.loc[0];
    b->pos[1] -= L.loc[1];
    {
        f32 t2 = b->pos[2];
        L.one[0] = 1.0f;
        b->pos[2] = t2 - L.loc[2];
    }
    if (id == (D_800E76D0 & 7)) {
        u8 *tb = D_800C9548;
        u8 *t = (u8 *)(((D_800E76D0 >> 2) & 6) + (s32)tb);
        if (func_80030658(b->pts[t[0]], b->pts[t[1]], D_800EE300, L.one, 0)) {
            func_800445B0(b, 0x1C);
        }
    }
}
