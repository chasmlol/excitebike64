#include "ctx.h"

extern void func_800BCB04(void *);
extern void *D_800F55A0;
extern void *D_800CE1FC;
extern void *D_800F4F9C;
extern void *D_800F557C;
extern void *D_800F4FF0;

void func_8008ED48(void)
{
    s32 i;
    void **tbl;

    i = 0;
    do {
        tbl = (void **)D_800F55A0;
        func_800BCB04(tbl[i]);
        i++;
    } while (i < 0x1c2);
    func_800BCB04(D_800F55A0);
    func_800BCB04(D_800CE1FC);
    func_800BCB04(D_800F4F9C);
    func_800BCB04(D_800F557C);
    func_800BCB04(D_800F4FF0);
    D_800CE1FC = 0;
}
