#include "ctx.h"

typedef struct {
    s32 x, y;
    u32 z;
} V3;

s32 func_8009EDCC(s32 a0, s32 a1, s32 a2, s32 a3);

extern s32 D_800F56A8;
extern V3 D_800CE234;
extern V3 D_800CE228;
extern V3 D_800CE21C;
extern V3 D_800CE210;
extern s32 D_800CE240;

s32 func_80090560(void)
{
    D_800CE228.x += func_8009EDCC(1, D_800CE234.x, 0x23, D_800F56A8);
    D_800CE228.y += func_8009EDCC(1, D_800CE234.y, 0x23, D_800F56A8);
    D_800CE228.z += func_8009EDCC(2, D_800CE234.z, 0x23, D_800F56A8);
    D_800CE21C.x = D_800CE228.x >> 10;
    D_800CE21C.y = D_800CE228.y >> 10;
    D_800CE21C.z = (u16)(D_800CE228.z >> 10);
    D_800F56A8++;
    if (D_800F56A8 == 0x23) {
        D_800CE240 = 0;
        D_800CE21C.x = D_800CE210.x;
        D_800CE21C.y = D_800CE210.y;
        D_800CE21C.z = ((u32)(D_800CE21C.z + 0x800) >> 13) << 13;
    }
}
