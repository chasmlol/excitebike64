#include "ctx.h"

typedef struct { u8 pad[0x1C]; u32 tab[12]; u32 mode; } Game;

extern s32 D_800C8CD8;
extern u32 D_800E3700;
extern Game D_800EFD70;
extern u32 D_800EFD28[];
extern u32 D_800EFD00[];
extern u32 D_800CA864[];
extern s32 D_800CA858;
extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern u32 D_800E76D0;

extern s32 D_800F05D4;

static inline s32 chk(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700) {
        return 1;
    }
    if (D_800EFD70.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70.tab[*p];
    if (*p >= 6) {
        return 1;
    }
    return (D_800EFD70.mode == 9) * 2;
}

static inline void push(u32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004E3EC(u32 a, u32 b, s32 c)
{
    if (D_800F05D4 < 0x168) {
        return;
    }
    if (D_800CA858 >= 6) {
        return;
    }
    if (chk(&a) != 0) {
        return;
    }
    if (chk(&b) != 0) {
        return;
    }
    push(D_800CA864[a + 12]);
    if (c != 0) {
        push(0x95);
    } else {
        push(0x96);
    }
    push(D_800CA864[b + 18]);
}
