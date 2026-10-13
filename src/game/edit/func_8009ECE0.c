#include "ctx.h"

typedef struct Gs {
    u8 pad[0x1738];
    s32 f1738;
    s32 f173c;
    s32 f1740;
} Gs;

extern Gs *D_800CEA40;
s32 func_80013010(s32 x);

void func_8009ECE0(void) {
    Gs *g;
    f32 x;
    s32 n;
    f32 hyp;

    g = D_800CEA40;
    x = (f32)g->f1738;
    n = (s32)((x - 20000.0f) * 32000.0f / 100000.0f + 8000.0f);
    g->f1740 = n;
    hyp = __builtin_sqrtf((f32)n * (f32)n + x * x);
    D_800CEA40->f173c = func_80013010((D_800CEA40->f1740 << 14) / (s32)hyp & 0x3FFF) & 0xFFFF;
}
