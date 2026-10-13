#include "ctx.h"

extern s32 D_800ED7AC;
extern s32 D_800EA990;
extern s32 D_800EBDF8[];
extern s32 D_800EE138;
extern s32 D_800EDBB4;
extern s32 D_800EA9A8[];
void func_800BCB04(s32 x);

void func_80042584(void)
{
    s32 i;
    s32 *p;

    func_800BCB04(D_800ED7AC);
    func_800BCB04(D_800EA990);
    func_800BCB04(D_800EBDF8[0]);
    func_800BCB04(D_800EBDF8[1]);
    i = 0;
    func_800BCB04(D_800EE138);
    func_800BCB04(D_800EDBB4);
    p = D_800EA9A8;
    do {
        s32 v = *p;
        p++;
        i++;
        func_800BCB04(v);
    } while (i < 16);
}
