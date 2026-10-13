#include "ctx.h"

extern s32 D_800CA770;
extern s32 D_800C29E0;
extern s32 D_800D5870;
extern u32 *D_800CA774[];
extern char D_800CA79C[];
extern void func_8000ECB8(s32 x) __attribute__((noreturn));
extern u32 func_800C1DEC(void);
extern s32 func_80065858(void);
extern s32 func_800C06D0(void *arc, u32 **ids, char *name);

void func_80048230(void *arc)
{
    if (D_800CA770 != 2) {
        if (D_800CA770 != 0 && func_800C1DEC() != 0) {
            func_8000ECB8(1);
        }
        while (func_80065858() != 0) {
        }
        D_800D5870 = D_800C29E0;
        if (func_800C06D0(arc, D_800CA774, D_800CA79C) != 0) {
            func_8000ECB8(1);
        }
        D_800D5870 = 0;
        D_800CA770 = 2;
    }
}
