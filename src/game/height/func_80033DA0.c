#include "ctx.h"

extern u8 D_800C9864[];
extern u8 D_800C9884[];

s32 func_80033DA0(s32 x) {
    s32 off = x >> 11;
    s32 v = (x >> 2) & 0x7FF;
    u8 *p1 = D_800C9864;
    s32 sh = *(s32 *)(p1 + (off & 0x1C));
    s32 add = *(s32 *)(D_800C9884 + (off & 0x1C));
    return ((v << sh) + add) >> 3;
}
