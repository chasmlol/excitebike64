#include "ctx.h"

extern s32 *D_800CE200;
extern s32 D_800D1440;
extern s32 D_800D1408;
extern s32 D_800E7728;
extern char D_800DA880[];
extern char D_800DA888[];

extern s32 *func_800BC908(s32, s32, void *, s32);
extern void func_800BF530(void *, s32 *);
extern s32 func_800BF79C(s32 *, s32);
extern s32 *func_800C1E60(s32 *, s32, s32, s32, s32);
extern void func_800482D0(s32);

void func_8008F0B0(void)
{
    s32 i;
    s32 *p;
    s32 r;

    D_800CE200 = func_800BC908(100, 0, D_800DA880, 0x3E);
    func_800BF530(D_800DA888, D_800CE200);
    D_800CE200[6] = 0;
    D_800CE200[3] = 3;
    r = func_800BF79C((s32 *)D_800CE200[0], D_800D1440);
    D_800CE200[2] = r;
    r = func_800BF79C((s32 *)D_800CE200[0], D_800D1408);
    D_800CE200[1] = r;
    i = 0;
    do {
        p = func_800C1E60(0, 0x20, 2, 2, 0x21);
        { s32 *t = D_800CE200 + i; t[9] = (s32)p; }
        i++;
    } while (i < 9);
    i = 0;
    do {
        p = func_800C1E60(0, 0x20, 2, 2, 0x21);
        { s32 *t = D_800CE200 + i; t[18] = (s32)p; }
        i++;
    } while (i < 4);
    i = 0;
    do {
        p = func_800C1E60(0, 0x40, 2, 2, 0x21);
        { s32 *t = D_800CE200 + i; t[22] = (s32)p; }
        i++;
    } while (i < 3);
    *(u16 *)(((s32 *)D_800CE200[2])[1]) = 0;
    func_800482D0(D_800E7728);
}
