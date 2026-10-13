#include "ctx.h"

typedef struct {
    u8 pad0[0x1C];
    u32 tab[12];
    s32 mode;
} Blk;

u32 func_8000EFE8(void);

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

extern s32 D_800CA8F4[];

void func_8004DBF8(u32 param_1)
{
    u32 r;

    if (D_800CA858 != 0) {
        return;
    }
    if (chk(&param_1) == 0) {
        r = func_8000EFE8() % 3;
        if (r == 0) {
            push(D_800CA864[param_1 + 0x12]);
        } else {
            push(D_800CA864[param_1 + 6]);
        }
        push(D_800CA8F4[r]);
    }
}
