#include "ctx.h"

void func_8004F8A4(void);

extern s32 D_800C2B58;
extern s32 D_800CA848;
extern s32 D_800CA84C;
extern s32 D_800CA850;
extern s32 D_800CA854;

void func_80010E5C(s32 mode, s32 v)
{
    s32 *p;

    if (mode == 1) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        goto end;
    }
    if (mode == 2) {
        goto m2;
    }
    goto end;
m0:
    D_800C2B58 = v;
    D_800CA854 = v;
    goto end;
m1:
    D_800CA848 = v;
    p = &D_800CA848;
    p[1] = v;
    goto end;
m2:
    D_800CA850 = v;
end:
    func_8004F8A4();
}
