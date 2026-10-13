#include "ctx.h"

typedef struct {
    u8 pad[0x20];
    u8 a;
    u8 b;
    s8 c;
    s8 d;
    u8 pad2[0x0C];
} Ent;

extern u32 D_800FE8DC;
extern Ent *D_800FE8D4;

void func_800BBAD0(s32 i)
{
    s32 a, b, x, y;
    if (D_800FE8DC & 1) {
        a = D_800FE8D4[i].a;
        b = D_800FE8D4[i].b;

        x = 15 - b;
        x = x * a / 10;
        y = b * a / 10;
        if (x > a) x = a;
        if (y > a) y = a;
        D_800FE8D4[i].c = x;
        D_800FE8D4[i].d = y;
    }
}

