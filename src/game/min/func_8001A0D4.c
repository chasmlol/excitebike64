#include "ctx.h"

extern void func_800BCB04(void *);
extern void func_800C204C(void *);
extern void *D_800C2354;

void func_8001A0D4(void)
{
    s32 i;
    void **p;

    i = 0;
    do {
        p = (void **)D_800C2354 + i;
        func_800C204C(*p);
        p = (void **)D_800C2354 + i;
        func_800BCB04(*p);
        i++;
    } while (i < 0x4a);
    func_800BCB04(D_800C2354);
}
