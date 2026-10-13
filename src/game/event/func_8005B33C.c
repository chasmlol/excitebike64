#include "ctx.h"

extern s32 D_800EFD70[];
extern s32 D_800F06B0;
extern s32 D_800F0730[];
extern s32 D_800F0530[];
extern s32 D_800F06B8[];
void func_800C204C();
void func_800BCB04(s32);

void func_8005B33C(void)
{
    s32 i;
    s32 *p1;
    s32 *p2;

    if (D_800EFD70[0x13] == 9) {
        func_800BCB04(D_800F06B0);
    }
    if (D_800EFD70[0xF] == 1) {
        func_800BCB04(D_800F0730[0]);
        func_800BCB04(D_800F0730[1]);
    }
    i = 0;
    p2 = D_800F06B8;
    p1 = D_800F0530;
    do {
        i++;
        func_800C204C(*p1);
        func_800BCB04(*p1);
        p1++;
        func_800C204C(*p2);
        func_800BCB04(*p2);
        p2++;
    } while (i < 0x1e);
    return;
}
