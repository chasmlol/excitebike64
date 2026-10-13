#include "ctx.h"

extern s32 D_800CCCD0;
extern s32 D_800F142C;
extern s32 D_800F1430;
extern s32 D_800F1424;
void func_800BCB04(s32);

void func_80066268(void)
{
    u32 i;
    if (D_800CCCD0 == 0) {
        return;
    }
    i = 0;
    func_800BCB04(D_800CCCD0);
    func_800BCB04(D_800F142C);
    if (*(u32 *)D_800F1430 != 0) {
        do {
            func_800BCB04(((s32 *)D_800F1424)[i]);
            i++;
        } while (i < *(u32 *)D_800F1430);
    }
    func_800BCB04(D_800F1424);
    D_800CCCD0 = 0;
}
