#include "ctx.h"

typedef struct {
    s32 f0;
    s32 w[16];
} Ent;

typedef struct {
    s32 a, b, c, d;
} Quad;

extern Ent D_800F48E0[];
extern s32 D_800F4AE0[];
extern u8 D_800CE168[];

void func_8008C790(s32 idx)
{
    Quad *dst;
    Quad *src;
    Quad *end;

    if (D_800F48E0[idx].f0 == (s32)D_800CE168 && D_800F4AE0[idx] > 0) {
        dst = (Quad *)&D_800F48E0[idx];
        src = (Quad *)D_800F48E0[idx].w;
        end = src + 4;
        do {
            *dst = *src;
            dst++;
            src++;
        } while (src != end);
        D_800F4AE0[idx] = D_800F4AE0[idx] - 1;
    }
}
