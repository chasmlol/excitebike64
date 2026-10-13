#include "ctx.h"

typedef struct {
    u32 flags;
    s32 a1, a2, a3;
    s32 p10;
    s32 v14, v18, v1C;
    s32 p20;
} Rec;

typedef struct {
    s32 h0;
    s32 count;
    Rec r[1];
} Tab;

typedef struct {
    s32 A, B, C, D, E;
} Locs;

extern Tab *D_800C9860;
extern s32 D_800F13EC;
extern s32 D_800F13F0;

s32 func_800353EC(s32 *a, s32 *b, s32 c, s32 d);
s32 func_80033AC8(s32 a0, s32 a1, s32 a2, s32 *a3, s32 *a4, s32 *a5, s32 *a6, s32 a7, s32 a8);

void func_8003240C(s32 param_1, s32 param_2)
{
    s32 A, B, C, D, E;
    s32 i;
    Rec *rp;
    u32 fl;

    if (D_800C9860 != NULL) {
        for (i = 0; i < D_800C9860->count; i++) {
            rp = &D_800C9860->r[i];
            fl = rp->flags;
            rp->flags = fl & ~8;
            if (fl & 1) {
                if (func_800353EC(&A, &B, param_1, param_2) == 0) {
                    rp->v1C = 0x7FFF;
                    rp->flags |= 8;
                    rp->v14 = A;
                    rp->v18 = B;
                }
            } else if (fl & 2) {
                if (func_80033AC8(0, 0x14, -0x12C, &A, &B, &C, &D, param_1, 0) == 0) {
                    rp->v1C = 0x7FFF;
                    rp->flags |= 8;
                    rp->v14 = A;
                    rp->v18 = B;
                }
            } else {
                if (func_80033AC8(rp->a1, rp->a2, rp->a3, &A, &B, &C, &E, param_1, param_2) == 0) {
                    rp->v14 = A;
                    rp->v18 = B;
                    rp->v1C = C;
                    rp->flags |= 8;
                }
            }
            if (rp->v14 < -0x32 || rp->v18 < -0x32 ||
                D_800F13EC + 0x32 < rp->v14 || D_800F13F0 + 0x32 < rp->v18) {
                rp->flags &= ~8;
            }
        }
    }
}
