#include "ctx.h"

extern s32 D_800CCD60;
extern s32 D_800F14C0;
extern s32 D_800F14C8[];
extern s32 D_800F14D0;
extern s32 D_800F14D8;
extern s32 D_800F14E0;
extern u8 D_800D8E10[];

void *func_800BC908();

void func_8006A8B0(s32 p)
{
    D_800CCD60 = 0;
    D_800F14C0 = 0;
    D_800F14C8[0] = -1;
    D_800F14C8[1] = -1;
    D_800F14E0 = -1;
    if (p != 0) {
        D_800F14D8 = 0x10;
    } else {
        D_800F14D8 = 8;
    }
    D_800F14D0 = (s32)func_800BC908(D_800F14D8 * 0x14, 0, D_800D8E10, 0x74);
}
