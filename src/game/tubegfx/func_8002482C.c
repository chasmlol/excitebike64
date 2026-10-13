#include "ctx.h"

extern void *D_800C9640;
extern void *D_800C9644;
extern void *D_800C9648;
extern void *D_800E7714;
extern s32 D_800E7718;
extern s32 D_800E7724;
extern s32 D_800EFDBC;

void func_8002400C(void *a0, s32 a1, s32 a2, void *a3, s32 a4);

void func_8002482C(s32 param_1)
{
    s32 f = (D_800E7718 == 0);
    void *p3 = D_800E7714;

    if (p3 != NULL && param_1 == 0) {
        func_8002400C(D_800C9644, 0, f, p3, D_800E7724);
    } else {
        func_8002400C(D_800C9644, param_1, f, NULL, 0);
    }
    func_8002400C(D_800C9640, param_1, D_800EFDBC != 2, NULL, 0);
    if (D_800C9648 != NULL) {
        func_8002400C(D_800C9648, param_1, 1, NULL, 0);
    }
}
