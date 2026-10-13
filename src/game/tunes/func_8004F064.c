#include "ctx.h"

typedef struct { u8 pad[0x1C]; u32 tab[12]; u32 mode; } Game;

extern s32 D_800C8CD8;
extern u32 D_800E3700;
extern Game D_800EFD70;
extern u32 D_800EFD28[];
extern u32 D_800E76D0;
extern s32 *D_800EFD24;
extern s32 D_800EECD4;
extern s32 *D_800EFD4C;
extern s32 D_800EECC0;
extern s32 D_800C2B58;
extern s32 D_800CA854[];

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

void func_8004F064(u32 param_1)
{
    s32 *b;
    s32 *s;

    if (chk(&param_1) != 0) {
        return;
    }
    if (param_1 == 0 || param_1 == 3) {
        return;
    }
    b = D_800EFD24;
    b[61] = 0x73;
    b[62] = 0;
    s = D_800EFD4C;
    b[65] = s[577] + D_800EECD4;
    b[66] = s[578];
    D_800EECC0 = 1;
    b[64] = s[579];
    b[73] = 0;
    D_800CA854[0] = D_800C2B58;
}
