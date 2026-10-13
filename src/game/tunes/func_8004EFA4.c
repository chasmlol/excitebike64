#include "ctx.h"

typedef struct { u8 pad[0x2C]; s32 f2C; } Obj;

extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800CA858;
extern s32 D_800CA840;
extern u32 D_800EFD00[];
extern Obj *D_800EFD64;
extern s32 D_800EFCF8;
extern s32 D_800EFCE0[];

static inline void push(u32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004EFA4(void)
{
    push(0x94);
    if (D_800EFD64->f2C == 0) {
        s32 k = D_800EFCF8;
        s32 v = D_800CA840;
        if (k < 2) {
            D_800EFCE0[k * 2] = v;
            D_800EFCE0[k * 2 + 1] = 0x1E;
            D_800EFCF8 = k + 1;
        }
    }
}
