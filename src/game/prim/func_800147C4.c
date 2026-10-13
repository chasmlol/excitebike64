#include "ctx.h"

extern s32 D_800C8C74;
extern s32 D_800C8CA8[];

void func_800147C4(u32 *param_1, s32 param_2)
{
    u32 i;
    s32 a2;
    s32 a3;
    s32 j;
    u32 *dst;
    s32 jn;
    
    u32 idx;
    s32 g;
    s32 *tab;
    u16 t1;
    u32 t0;

    if (param_1[0] == 0) {
        return;
    }
    i = 0;
    tab = D_800C8CA8;
    g = D_800C8C74;
    do {
        a2 = *(u8 *)((s32)param_1 + (i << 3) + 0xA) & 0x34;
        if (a2 != 0) {
            idx = ((u32)a2 >> 5) << 2;
            if (a2 & 0x10) {
                idx = idx | 2;
            }
            a2 = (((u32)a2 >> 2) & 1) | idx;
            a2 = tab[a2];
            if (param_2 % a2 == 0) {
                a3 = i + 1;
                while (*(u8 *)((s32)param_1 + (a3 << 3) + 0xA) & 0x34) {
                    a3++;
                }
                if (g != 0) {
                    t1 = ((u16 *)param_1[1])[i];
                    for (a2 = i; a2 < a3; a2++) {
                        ((u16 *)param_1[1])[a2] = ((u16 *)param_1[1])[a2 + 1];
                    }
                    ((u16 *)param_1[1])[a3] = t1;
                }
                t0 = *(u32 *)((s32)param_1 + (i << 3) + 0xC);
                for (a2 = i; a2 < a3; a2++) {
                    *(u32 *)((s32)param_1 + (a2 << 3) + 0xC) = *(u32 *)((s32)param_1 + ((a2 + 1) << 3) + 0xC);
                }
                *(u32 *)((s32)param_1 + (a3 << 3) + 0xC) = t0;
                i = a3;
            }
        }
        i = i + 1;
    } while (i < param_1[0]);
}
