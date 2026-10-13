#include "ctx.h"

typedef struct {
    s32 f0;
    s32 pad[12];
    s32 f34;
} Ent;

extern s32 D_800EA680;

void func_8003C834(Ent *base, s32 val, s32 n)
{
    s32 i;

    while (n != 0) {
        i = D_800EA680;
        base[i].f0 = val;
        if (base[i].f34 != 0) {
            D_800EA680++;
            func_8003C834(base, i, base[D_800EA680 - 1].f34);
        } else {
            D_800EA680++;
        }
        n--;
    }
}
