#include "ctx.h"

extern s32 D_800D5638;
extern s32 D_800D5620;
extern s32 D_800D5630;
extern s32 D_800D5628;
extern s32 D_800D562C;
extern s32 D_80100624;
extern s32 D_80100628;
extern s32 D_80100618;
extern s32 D_80100614;
extern s32 D_80100610;
extern s32 *D_80100620;
extern s32 *D_8010061C;
extern char D_800DB1F0[];

u32 func_800AD600(s32 a0);
void *func_800BC908(s32 a0, s32 a1, void *a2, s32 a3);
s32 func_800BA6B0(s32 a0, s32 a1, s32 a2, s32 a3);
void func_800BCB04(s32 a0);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800AD720(s32 a0, s32 a1);

s32 func_800BBBC0(s32 param_1, s32 param_2, s32 param_3, s32 param_4, u32 *param_5, s32 *param_6)
{
    s32 iVar1;
    s32 r;

    D_800D5638 = 0;
    D_800D5620 = 0;
    if ((param_2 & 3) != 3) {
        return 1;
    }
    {
        D_80100624 = param_2;
        D_80100628 = (s32)func_800AD600(param_3);
        iVar1 = 1;
        *param_6 = D_80100628;
        if (param_2 & 1) {
            iVar1 = 2;
        }
        if (param_2 & 2) {
            iVar1 = iVar1 << 1;
        }
        D_80100618 = ((((u32)(D_80100628 * param_4) / 1000 + 0x3f) >> 6) << 6);
        *param_5 = D_80100618;
        D_80100618 = D_80100618 * 2;
        D_80100614 = iVar1 * D_80100618;
        D_80100610 = (s32)func_800BC908(D_80100614 * 4, 0, D_800DB1F0, 0xd7);
        D_8010061C = (s32 *)func_800BC908(D_80100614 * 2 + 0x10, 0, D_800DB1F0, 0xd9);
        D_80100620 = D_8010061C;
        if (((u32)D_80100620 & 0xff) == 0) {
            D_8010061C = D_80100620 + 4;
        }
        if (D_8010061C == 0) {
            return 1;
        }
        {
            r = func_800BA6B0(param_1, param_2, param_3, param_4);
            if (r != 0) {
                func_800BCB04((s32)D_80100620);
                func_800BCB04(D_80100610);
                return r;
            }
            D_800D5630 = 0;
            D_800D5628 = 0;
            func_8000ED10((u8 *)D_8010061C, 0, D_80100614 << 1);
            func_800AD720((s32)D_8010061C, D_80100614);
            func_800AD720((s32)D_8010061C + D_80100614, D_80100614);
            D_800D562C = 1;
            return 0;
        }
    }
}
