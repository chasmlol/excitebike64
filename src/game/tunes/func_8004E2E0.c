#include "ctx.h"

typedef struct {
    s32 pad[7];
    u32 map[12];
    s32 mode;
} Cfg;

extern s32 D_800CA858;
extern s32 D_800C8CD8;
extern u32 D_800E3700;
extern s32 D_800E76D0;
extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800EFD00[];
extern s32 D_800EFD28[];
extern Cfg D_800EFD70;

static inline s32 check(u32 *p)
{
    if (D_800EFD70.mode == 2) {
        return 1;
    }
    if (*p >= 6) {
        return 1;
    }
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70.map[*p];
    if (*p >= 6) {
        return 1;
    }
    return (D_800EFD70.mode == 9) * 2;
}

void func_8004E2E0(u32 a, s32 b)
{
    s32 r;
    u32 *pa;

    if (D_800CA858 < 7) {
        pa = &a;
        if (D_800C8CD8 != 0 && a == D_800E3700) {
            r = 1;
        } else {
            r = check(pa);
        }
        if (r != 1 && D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
            D_800EFD00[D_800CA858++] = b;
        }
    }
}
