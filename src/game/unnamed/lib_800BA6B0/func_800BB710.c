#include "ctx.h"

typedef struct { s32 f0; s32 f1; s32 f2; s32 f3; s32 f4; s32 f5; s32 f6; } Row;
typedef struct { u8 pad0[4]; u32 flags; u8 pad8[0x10]; s32 val; u8 pad1c[0xC]; s32 idx; u8 pad2c[4]; } Ent;

extern Ent *D_800FE8D4;
extern Row D_800FE8F0[];

void func_800BB710(s32 n, u32 flags)
{
    Ent *base = D_800FE8D4;
    Ent *e = (Ent *)((u32)(n * 0x30) + (u32)base);
    e->flags = flags;
    if (flags & 2) {
        e->val = D_800FE8F0[e->idx].f3;
        return;
    }
    e->val = D_800FE8F0[e->idx].f1;
}
