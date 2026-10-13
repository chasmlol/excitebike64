#include "ctx.h"

extern u32 D_800FA850;
extern u32 D_800FA844;

typedef struct {
    u8 pad[0x28];
    s32 a;
    u32 q;
} Obj;

void func_800B9C64(Obj *o, s32 a1)
{
    u32 q = (u32)(a1 * D_800FA850) / D_800FA844;
    o->a = a1;
    o->q = q;
}
