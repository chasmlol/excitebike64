#include "ctx.h"

extern s32 D_800C9840;
extern s32 D_800C9860;
extern s32 D_800E8E88[];
void func_800BCB04(s32 a);

void func_80033D40(void) {
    if (D_800C9840 != 0) {
        func_800BCB04(D_800C9840);
        D_800C9840 = 0;
    }
    if (D_800E8E88[0] != 0) {
        func_800BCB04(D_800E8E88[0]);
        func_800BCB04(D_800E8E88[1]);
    }
    D_800C9860 = 0;
}
