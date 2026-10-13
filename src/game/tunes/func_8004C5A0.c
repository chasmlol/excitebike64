#include "ctx.h"

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 f10;
} Ent;

typedef struct {
    s32 pad[0x3D];
    s32 f0F4;
    s32 f0F8;
    s32 f0FC;
    s32 f100;
    s32 f104;
    s32 f108;
    s32 pad2[6];
    s32 f124;
} Ctl;

extern s32 D_800EECC0;
extern Ctl *D_800EFD24;
extern s32 D_800CA854;
extern Ent *D_800EFD4C;
extern s32 D_800EECD4;

void func_8004C5A0(s32 param_1, s32 param_2)
{
    if (D_800EECC0 != 0) {
        return;
    }
    if (param_2 == 0) {
        if (D_800EFD24->f0F4 != -1) {
            D_800EFD24->f0F4 = -1;
            D_800EFD24->f124 = 0;
            D_800CA854 = 0;
        }
        return;
    }
    if (D_800EFD24->f0F4 != param_1) {
        Ent *t;
        s32 v10;
        D_800EFD24->f0F4 = param_1;
        t = D_800EFD4C + param_1;
        D_800EFD24->f0F8 = 0;
        D_800EFD24->f104 = t->f08 + D_800EECD4;
        D_800EFD24->f108 = t->f0C;
        v10 = t->f10;
        D_800EFD24->f124 = 1;
        D_800EFD24->f100 = v10;
    }
    D_800CA854 = param_2;
}
