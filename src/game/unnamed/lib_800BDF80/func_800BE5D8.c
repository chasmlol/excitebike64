#include "ctx.h"

extern s32 D_80100670;
extern s32 D_80100674;
extern s32 D_80100680[];
extern s32 D_80100694;
extern s32 D_801006A4;
extern s32 D_801006B0[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800BCB04(void *p);

s32 func_800BE5D8(void)
{
    s32 i;
    s32 *p;
    s32 *base;

    if (D_80100670 == 0) {
        return 0;
    }
    i = 3;
    base = D_801006B0;
    p = base + 12;
    do {
        *p = 0;
        i = i - 1;
        p = p - 4;
    } while (i >= 0);
    if (D_80100680[2] != 0) {
        func_800BCB04((void *)D_80100680[2]);
    }
    D_80100694 = 0;
    func_8000ED10((u8 *)D_80100680, 0, 0x14);
    func_800BCB04((void *)D_801006A4);
    func_800BCB04((void *)D_80100674);
    D_801006A4 = 0;
    D_80100674 = 0;
    D_80100670 = 0;
    return 0;
}
