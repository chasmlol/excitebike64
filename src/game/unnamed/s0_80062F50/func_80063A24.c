#include "ctx.h"

extern u8 D_800E37C0[];
extern u8 *D_800F0C10[];
extern s32 D_800F0BDC;

typedef struct {
    s32 w0;
    s32 w1;
    f32 f2;
} Rec;
extern Rec D_800F0C30[];

void func_80063A24(u8 *a, u8 *b, s32 n)
{
    u8 *p;
    s32 i;

    if (a[0x6E8] == 2) {
        a[0x7A5] = 1;
        return;
    }
    p = D_800F0C10[n + 1];
    if (p[0x6E8] == 1 && p[0x7A5] == 0) {
        a[0x7A5] = 1;
        i = D_800F0BDC;
        *(f32 *)(a + 0x630) = 1000000.0f;
        while (i > 0) {
            D_800F0C30[i] = D_800F0C30[i - 1];
            i = i - 1;
        }
        *(s16 *)&D_800F0C30[0] = (s16)(((s32)a - (s32)D_800E37C0) * -0x41041041 >> 5);
        D_800F0C30[0].w1 = (s32)D_800F0C10[n + 1];
        D_800F0C30[0].f2 = (*(f32 *)(D_800F0C10[n + 1] + 0x630) - *(f32 *)(b + 0x630)) - 20.0f;
        D_800F0BDC = D_800F0BDC + 1;
    }
}
