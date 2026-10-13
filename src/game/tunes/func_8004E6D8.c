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
extern s32 D_800E76D0;
extern s32 D_800EE4B0;

static inline s32 chk(u32 p, u32 *q)
{
    if (D_800C8CD8 != 0 && p == D_800E3700) {
        return 1;
    }
    if (D_800EFD70.mode == 2) {
        return 1;
    }
    if (*q >= 6) {
        return 1;
    }
    D_800EFD28[*q] = D_800E76D0;
    *q = D_800EFD70.tab[*q];
    if (*q >= 6) {
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

void func_8004E6D8(u32 param_1, s32 param_2, u32 param_3)
{
    u32 t1 = param_1;
    u32 uVar2;
    u32 sh;
    if (chk(t1, &param_1) != 0) {
        return;
    }
    if (D_800C8CD8 != 0) {
        return;
    }
    if ((s32)(D_800E76D0 - D_800EE4B0) < 900) {
        return;
    }
    D_800EE4B0 = D_800E76D0;
    if (D_800CA858 >= 6) {
        return;
    }
    push(D_800CA864[param_1 + 0xc]);
    if (param_2 < (s32)param_3) {
        push(0x97);
        sh = t1 << 8;
        uVar2 = 0x80010000;
    } else {
        push(0x98);
        sh = t1 << 8;
        uVar2 = 0x80020000;
    }
    push(sh | (param_3 | uVar2));
}
