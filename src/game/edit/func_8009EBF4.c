#include "ctx.h"

typedef struct {
    u8 pad[0x172C];
    s32 a;
    s32 b;
    s32 c;
} Obj;

extern Obj *D_800CEA40;

s32 func_80013010(s32 x);

void func_8009EBF4(void)
{
    f32 x, y;
    s32 d;

    x = D_800CEA40->a;
    D_800CEA40->c = ((x - 20000.0f) * 38000.0f) / 165000.0f + 40000.0f;
    y = D_800CEA40->c;
    d = __builtin_sqrtf(y * y + x * x);
    D_800CEA40->b = func_80013010((D_800CEA40->c << 14) / d & 0x3FFF) & 0xFFFF;
}
