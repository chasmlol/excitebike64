#include "ctx.h"

typedef struct { f32 x, y, z, w; } Ent;
extern Ent D_800CCDE0[];
extern f32 sqrtf(f32 x);
extern f32 func_800ADFC0(f32 x);
extern f32 func_800AEA30(f32 x);

void func_8006BFC0(void)
{
    s32 i;
    f32 sq, f6;

    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f;
        D_800CCDE0[i].z = 0.9f;
        sq = sqrtf(0.19000006f);
        D_800CCDE0[i].x = sq * func_800ADFC0(f6);
        D_800CCDE0[i].y = sq * func_800AEA30(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f;
        D_800CCDE0[i + 5].z = 0.1f;
        sq = sqrtf(0.99f);
        D_800CCDE0[i + 5].x = sq * func_800ADFC0(f6);
        D_800CCDE0[i + 5].y = sq * func_800AEA30(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f + 3.1415927f;
        D_800CCDE0[i + 10].z = -0.1f;
        sq = sqrtf(0.99f);
        D_800CCDE0[i + 10].x = sq * func_800ADFC0(f6);
        D_800CCDE0[i + 10].y = sq * func_800AEA30(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f + 3.1415927f;
        D_800CCDE0[i + 15].z = -0.9f;
        sq = sqrtf(0.19000006f);
        D_800CCDE0[i + 15].x = sq * func_800ADFC0(f6);
        D_800CCDE0[i + 15].y = sq * func_800AEA30(f6);
    }
}
