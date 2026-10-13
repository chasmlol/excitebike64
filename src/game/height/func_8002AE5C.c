#include "ctx.h"

typedef struct {
    s32 x, y, z, a, b, c;
} Rec;

typedef struct {
    u8 pad0[4];
    s32 y;
    u8 pad1[0x44C - 8];
    f32 f44C;
    u8 pad2[0x624 - 0x450];
    s16 h624;
    u8 pad3[0x7E0 - 0x626];
} Ent;

extern Rec D_800E8030[];
extern Ent D_800E37C0[];
extern s32 D_800EFDBC;
extern void func_8002B5F8(Rec *p);

void func_8002AE5C(Ent *e, Rec *r)
{
    Rec *b;
    s32 i;
    s32 d;

    b = D_800E8030;
    i = e - D_800E37C0;
    r->x = b[i].x / 16;
    r->y = b[i].y / 16;
    r->z = b[i].z / 16;
    d = e->y - r->y;
    r->a = b[i].a;
    r->c = b[i].c;
    r->b = 0;
    func_8002B5F8(r);
    if (D_800EFDBC == 7) {
        e->f44C = 0.5f;
        if (d < -12000) {
            e->h624 = 10000;
            e->f44C = 1.0f;
        }
    }
}
