#include "ctx.h"

extern s32 D_800D33F0;
extern void *D_800D33F4;
extern s32 *D_800D33E0;
void func_800B8760(s32 *p);
void func_800B7BE0(void);

void func_800B9F74(void) {
    s32 *p;

    if (D_800D33F0 != 0) {
        if (D_800D33F4 != 0) {
            ((void (*)(void))D_800D33F4)();
        }
        p = D_800D33E0;
        while (p != 0) {
            func_800B8760(p);
            p = (s32 *)*p;
        }
        func_800B7BE0();
    }
}
