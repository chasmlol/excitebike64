#include "ctx.h"

extern s32 *D_800EFD64;
extern s32 D_800CA838[];
extern s32 D_800EFCF8;
extern s32 D_800EFCE0[];

static inline void push(s32 v, s32 w) {
    s32 n = D_800EFCF8;
    if (n < 2) {
        do {
            do {
                D_800EFCE0[n * 2] = v;
            } while (0);
        } while (0);
        D_800EFCE0[n * 2 + 1] = w;
        D_800EFCF8 = n + 1;
    }
}

void func_8004F410(s32 p) {
    if (D_800EFD64[11] == 0) {
        push(D_800CA838[p], 0x1E);
    }
}
