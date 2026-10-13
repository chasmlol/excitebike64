#include "ctx.h"

typedef struct {
    u32 w00;
    u8 p04[0x38];
    u32 w3C;
    u8 p40[0x0C];
    u32 w4C;
    u8 p50[0x370];
    u32 w3C0;
    u8 p3C4[0x4C];
    u32 w410;
    u8 p414[0x28];
    u8 arr43C[6];
} Blk;

extern Blk D_800EFD70;
extern const char *D_800EFD64;

void func_80087FB0(s32 p) {
    s32 i;

    if (p != 0) {
        i = 0;
        do {
            if (i == 0) {
                D_800EFD70.arr43C[0] = (u8)D_800EFD70.w3C0;
            } else {
                D_800EFD70.arr43C[i] = i;
            }
            i++;
        } while (i < 6);
        D_800EFD70.w4C = 6;
        D_800EFD64 = "Excite 3D";
        D_800EFD70.w3C = 1;
        D_800EFD70.w410 = 2;
        D_800EFD70.w00 &= 0xFFFFFF7F;
    }
}
