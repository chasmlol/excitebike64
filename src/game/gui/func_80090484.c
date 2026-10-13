#include "ctx.h"

extern s32 *D_800CE200;
void func_800BCB04(s32 x);
void func_800BFDD4(s32 *param_1);

void func_80090484(void)
{
    s32 i;

    if (D_800CE200 != 0) {
        i = 0;
        do {
            func_800BCB04(*(s32 *)((u8 *)D_800CE200 + (i << 2) + 0x24));
            i++;
        } while (i < 9);
        i = 0;
        do {
            func_800BCB04(*(s32 *)((u8 *)D_800CE200 + (i << 2) + 0x48));
            i++;
        } while (i < 4);
        i = 0;
        do {
            func_800BCB04(*(s32 *)((u8 *)D_800CE200 + (i << 2) + 0x58));
            i++;
        } while (i < 3);
        func_800BFDD4((s32 *)D_800CE200[0]);
        func_800BCB04((s32)D_800CE200);
        D_800CE200 = 0;
    }
}
