#include "ctx.h"

typedef struct {
    u32 key;
    u32 val;
} Pair;
typedef struct {
    u32 n;
    Pair e[1];
} Tab;
typedef struct {
    u32 n;
    u32 key;
    u32 x;
    u32 val;
    u32 y;
} Rec;
typedef struct {
    u8 pad[0x2C];
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    u8 pad2[0x4C - 0x3C];
} Slot;

extern s32 *D_800CA860;
extern u32 *D_800D5604;
extern Tab D_800EE4B8;
extern Rec *D_800EFD4C;
extern Slot *D_800EFD24;
extern u32 D_800EFCF4;
extern s32 *D_800EFD48;

extern void func_8004C3C4(void);
extern s32 *func_800BF79C();
extern s32 func_800BD924(s32 *param);
extern void func_800AEC34();
extern void func_800BFEA4(u32 *p);
extern void func_8004C014(void);

void func_8004C22C(s32 param_1)
{
    s32 n;
    u32 j;
    Rec *r;
    s32 k;
    Tab *tab;
    Tab *t;
    u32 *pw;
    u32 key;

    if (D_800CA860 != 0) {
        func_8004C3C4();
    }
    if (param_1 >= 0) {
        D_800CA860 = func_800BF79C(D_800D5604, param_1);
        n = func_800BD924(D_800CA860);
        if (n > 0x800) {
            while (1) ;
        }
        tab = &D_800EE4B8;
        func_800AEC34(tab, D_800CA860, n);
        func_800BFEA4((u32 *)D_800D5604[1] + param_1);
        n = 0;
        D_800CA860 = (s32 *)tab;
        if (D_800EE4B8.n != 0) {
            r = D_800EFD4C;
            t = tab;
            pw = (u32 *)t;
            do {
                key = pw[n * 2 + 1];
                for (j = 0; j < r->n; j++) {
                    if (r[j].key == key) {
                        pw[n * 2 + 1] = j;
                        pw[n * 2 + 2] = r[j].val;
                        break;
                    }
                }
                if (j == r->n) {
                    while (1) ;
                }
                n++;
            } while (n < t->n);
        }
        D_800EFCF4 = 0;
        D_800EFD48 = D_800CA860 + (D_800CA860[0] * 2 + 1);
        for (n = 0; n < 2; n++) {
            D_800EFD24[n].f30 = -1;
            D_800EFD24[n].f2C = -1;
            D_800EFD24[n].f38 = -1;
            D_800EFD24[n].f34 = -1;
        }
        func_8004C014();
    }
}
