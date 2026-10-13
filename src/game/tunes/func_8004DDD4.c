#include "ctx.h"

typedef struct {
    u8 pad0[0x1C];
    u32 tab[12];
    s32 mode;
} Blk;

void func_8004DBF8(s32 a0);
u32 func_8000EFE8(void);

extern u8 *D_800EFD64;
extern s32 D_800CA858;
extern s32 D_800C8CD8;
extern u32 D_800E3700;
extern Blk D_800EFD70;
extern s32 D_800E76D0;
extern s32 D_800EFD28[];
extern s32 D_800CA864[];
extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800EFD00[];
extern s32 D_800CA900[];

static __inline__ s32 chk(u32 *p)
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
    return (D_800EFD70.mode == 9) << 1;
}

static __inline__ void push(s32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004DDD4(u32 param_1)
{
    if (*(s32 *)(D_800EFD64 + 0x2c) != 0) {
        func_8004DBF8(param_1);
        return;
    }
    if (D_800CA858 != 0) {
        return;
    }
    if (chk(&param_1) == 0) {
        push(D_800CA864[param_1 + 6]);
        push(D_800CA900[func_8000EFE8() % 3]);
    }
}
