#include "ctx.h"

typedef struct {
    u8 pad0[0x630];
    f32 f630;
    u8 pad634[0x6E8 - 0x634];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Ent;

typedef struct {
    s16 idx;
    Ent *e;
    f32 d;
} Rec;

extern s32 D_800F0C04;
extern Rec D_800F0B70[];
extern Ent *D_800F0C10[];
extern Ent D_800E37C0[];

void func_800638E8(Ent *p, Ent *p2, s32 p3)
{
    s32 i;

    if (p->f6E8 == 2) {
        p->f7A5 = 1;
        return;
    }
    if (D_800F0C10[p3 - 1]->f6E8 == 1 && D_800F0C10[p3 - 1]->f7A5 == 0) {
        p->f7A5 = 1;
        p->f630 = -1000000.0f;
        for (i = D_800F0C04; i > 0; i--) {
            D_800F0B70[i] = D_800F0B70[i - 1];
        }
        D_800F0B70[0].idx = p - D_800E37C0;
        D_800F0B70[0].e = D_800F0C10[p3 - 1];
        D_800F0B70[0].d = p2->f630 - D_800F0C10[p3 - 1]->f630 - 20.0f;
        D_800F0C04++;
    }
}
