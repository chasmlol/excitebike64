#include "ctx.h"

typedef struct {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 f10;
    s32 f14;
    s32 pad18[5];
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    s32 pad3C[4];
} Ent; /* 0x4C */

extern s32 D_800EECCC;
extern Ent *D_800EFD24;
extern s32 D_800CA860;
extern s32 D_800CA85C;

s32 func_800BEF2C(s32 a0);
void func_800BEF8C(s32 a0);

void func_8004C3C4(void) {
    s32 i;
    s32 n;
    Ent *p;
    Ent *q;

    for (i = 0; i < D_800EECCC; i++) {
        D_800EFD24[i].f10 = -1;
        D_800EFD24[i].f14 = -1;
        D_800EFD24[i].f30 = -1;
        D_800EFD24[i].f2C = -1;
        D_800EFD24[i].f38 = -1;
        D_800EFD24[i].f34 = -1;
    }

    if (D_800CA860 != 0) {
        D_800CA85C = 1;
        for (i = 0; i < D_800EECCC; i++) {
            p = &D_800EFD24[i];
            for (;;) {
                if (p->f04 < 0) break;
                if (func_800BEF2C(p->f04) == 3) {
                    func_800BEF8C(p->f04);
                    p->f04 = -1;
                    break;
                }
            }
        }
        D_800CA85C = 0;
        D_800CA860 = 0;
    }
}
