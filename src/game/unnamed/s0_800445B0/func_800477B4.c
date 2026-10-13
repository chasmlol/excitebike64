#include "ctx.h"

typedef struct {
    f32 a, b, c, d;
} S;

extern S D_800CA440[];

void func_800477B4(f32 x)
{
    s32 i;

    for (i = 0; i < 11; i++) {
        D_800CA440[i].d = x;
    }
    D_800CA440[3].d = 0.1f;
    D_800CA440[4].d = 0.1f;
}
