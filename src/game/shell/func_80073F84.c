#include "ctx.h"

extern char D_800CBFD0[];
extern char *D_800EFD64;
extern s32 D_800C2388;
extern u8 D_800C292C;
extern u8 D_800EFD70[];
extern s32 w_800EFD70;

void func_80073F84(s32 param_1)
{
    s32 uVar1;
    s32 uVar2;
    s32 iVar3;
    u8 *base;

    if (param_1 != 0) {
        uVar1 = 0;
        do {
            uVar2 = uVar1 + 1;
            D_800EFD70[0x43C + uVar1] = (u8)(((s32)(uVar1 & 3) >> ((*(s32 *)(D_800EFD70 + 0x3C) - 1) >> 1)) << 1);
            uVar1 = uVar2;
        } while (uVar2 < 6);
        uVar1 = 0;
        iVar3 = uVar1;
        base = D_800EFD70;
        *(s32 *)(base + 0x4C) = 2;
        D_800EFD64 = D_800CBFD0;
        D_800C2388 = 0;
        *(s32 *)(base + 0x1C) = 4;
        *(s32 *)(base + 0x20) = 4;
        *(s32 *)(base + 0x24) = 4;
        *(s32 *)(base + 0x28) = 4;
        *(s32 *)D_800EFD70 = *(s32 *)D_800EFD70 & 0xffffff7f;
        for (; uVar1 < 4; uVar1++) {
            if (((s32)D_800C292C >> uVar1 & 1) != 0) {
                D_800EFD70[0x2C8 + iVar3] = (u8)uVar1;
                iVar3 = iVar3 + 1;
                if (iVar3 == *(s32 *)(D_800EFD70 + 0x3C)) {
                    return;
                }
            }
        }
    }
}
