#include "ctx.h"

typedef struct {
    u32 w00;
    u8 pad[0x4C - 4];
    u32 w4C;
} Blk;

typedef struct {
    s32 a;
    s32 b;
    s32 c;
} Trip;

extern Blk D_800EFD70;
extern Trip D_800CA924;
extern const char *D_800EFD64;

void func_80087E30(s32 p) {
    if (p != 0) {
        D_800EFD64 = "Desert";
        D_800EFD70.w4C = 5;
        D_800CA924.b = 0;
        D_800CA924.a = 1;
        D_800CA924.c = 0;
        D_800EFD70.w00 &= 0xFFFFFF7F;
    }
}
