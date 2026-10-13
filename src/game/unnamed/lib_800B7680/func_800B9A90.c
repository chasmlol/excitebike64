#include "ctx.h"

extern s32 *func_800BC908(s32 size, s32 zero, s32 *pool, s32 line);
extern void func_800BCB04(s32 *p);
extern void func_8000EE20(void);
extern void func_8000EE28(void);
extern s32 *D_800D33E0;
extern s32 D_800DB160[];

s32 *func_800B9A90(s32 p1)
{
    s32 *a;
    s32 *b;
    s32 *old;

    a = func_800BC908(0x34, 0, D_800DB160, 0x6AC);
    if (a != NULL) {
        b = func_800BC908(p1, 0, D_800DB160, 0x6AF);
        a[6] = (s32)b;
        if (b != NULL) {
            a[5] = p1;
            a[7] = 0;
            a[8] = 0;
            a[9] = 0;
            a[1] = -1;
            a[2] = -1;
            a[3] = -1;
            func_8000EE20();
            old = D_800D33E0;
            D_800D33E0 = a;
            a[0] = (s32)old;
            func_8000EE28();
            return a;
        }
        func_800BCB04(a);
    }
    return NULL;
}
