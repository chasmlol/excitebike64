#include "ctx.h"

typedef struct {
    s32 unk0;
    u8 *obj;
    f32 thr;
} Ent;

extern s32 D_800F0BDC;
extern Ent D_800F0C30;
extern s32 D_800F0C04;
extern Ent D_800F0B70;

void func_80063B60(u8 *a0, void *a1, f32 a2, s32 *a3);

void func_80064F30(u8 *p)
{
    f32 a, b, d;
    if (D_800F0BDC > 0) {
        b = *(f32 *)(D_800F0C30.obj + 0x630);
        a = *(f32 *)(p + 0x630);
        d = b - a;
        if (d < D_800F0C30.thr) {
            func_80063B60(p, &D_800F0C30, 70.0f, &D_800F0BDC);
        }
    }
    if (D_800F0C04 > 0) {
        b = *(f32 *)(p + 0x630);
        a = *(f32 *)(D_800F0B70.obj + 0x630);
        d = b - a;
        if (d < D_800F0B70.thr) {
            func_80063B60(p, &D_800F0B70, -30.0f, &D_800F0C04);
        }
    }
}
