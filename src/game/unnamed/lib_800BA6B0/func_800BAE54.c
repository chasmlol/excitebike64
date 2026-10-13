#include "ctx.h"

typedef struct { u32 w[12]; } RecA;
typedef struct { u32 w[7]; } RecB;

extern RecA *D_800FE8D4;
extern RecB D_800FE8F0[];
void func_8000EE20(void);
void func_8000EE28(void);

void func_800BAE54(s32 param_1, s32 param_2)
{
    RecA *a;
    RecB *b;
    RecA *ab;
    RecB *bb;
    u32 t;
    u32 fl;
    func_8000EE20();
    ab = D_800FE8D4;
    a = (RecA *)((s32)(param_1 * 0x30) + (s32)ab);
    bb = D_800FE8F0;
    b = (RecB *)((s32)(param_2 * 0x1c) + (s32)bb);
    a->w[2] = a->w[2] % b->w[1];
    a->w[7] = b->w[2];
    a->w[9] = b->w[3];
    t = b->w[0];
    fl = a->w[1];
    a->w[10] = param_2;
    a->w[0] = t;
    if (fl & 2) {
        a->w[6] = b->w[3];
    } else {
        a->w[6] = b->w[1];
    }
    func_8000EE28();
}
