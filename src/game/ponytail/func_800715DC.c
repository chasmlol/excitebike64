#include "ctx.h"

extern u8 D_800DC150[];
void func_800BCB04();

void func_800715DC(s32 a0)
{
    u8 *base;
    u8 *p;

    base = D_800DC150;
    p = base + a0 * 0xA80;
    if (*(s32 *)(p + 0xA58) != 0) {
        func_800BCB04(*(s32 *)(p + 0xA58));
    }
    if (*(s32 *)(p + 0xA50) != 0) {
        func_800BCB04(*(s32 *)(p + 0xA50));
    }
    if (*(s32 *)(p + 0xA4C) != 0) {
        func_800BCB04(*(s32 *)(p + 0xA4C));
    }
    if (*(s32 *)(p + 0xA48) == 0) {
        *(s32 *)(p + 0xA58) = 0;
    } else {
        func_800BCB04(*(s32 *)(p + 0xA48));
        *(s32 *)(p + 0xA58) = 0;
    }
    *(s32 *)(p + 0xA50) = 0;
    *(s32 *)(p + 0xA4C) = 0;
    *(s32 *)(p + 0xA48) = 0;
}
