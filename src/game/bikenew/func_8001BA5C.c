#include "ctx.h"

typedef struct {
    s32 f00;
    s32 n04;
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    f32 f1c;
    f32 f20;
    f32 f24;
    f32 f28;
    f32 f2c;
} Rec;

extern s32 D_800EFDBC;
extern s32 D_800E76C4;
extern f32 D_800E76C8;
extern f32 D_800C8E14;
extern f32 D_800C8E18;
extern f32 D_800C94B0;
extern f32 D_800C94B4;

#define MAXRPM (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f : 15000.0f)
#define RATE (D_800EFDBC == 6 ? D_800C8E18 * 2000.0f : 2200.0f)

void func_8001BA5C(Rec *p, f32 x)
{
    f32 a, k, r, v;

    if (p->n04 > 0) {
        p->f1c = 0.0f;
        a = p->f28 - p->f24;
        if (p->f1c < a) {
            p->n04 = 0;
            k = a / (a - p->f20);
        }
        p->f20 = a;
    } else {
        k = 1.0f;
    }

    if (p->n04 <= 0) {
        r = (p->f28 + 0.05f + p->f2c * 0.5f - x) * (MAXRPM * D_800C94B0);
        v = p->f2c / D_800E76C8 * RATE * D_800C94B4;
        p->f1c = r + v;
        if (p->f1c < 0.0f) {
            p->f1c = 0.0f;
            p->n04 = D_800E76C4;
            p->f20 = p->f2c;
        } else {
            p->n04 -= D_800E76C4;
            p->f24 = p->f28;
        }
        p->f1c *= k;
    } else {
        p->f24 = p->f24 - p->f24 * (MAXRPM / RATE) * D_800E76C8;
        p->n04 += D_800E76C4;
    }
}
