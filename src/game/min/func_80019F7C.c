#include "ctx.h"

typedef struct {
    u32 flags;
    u8 pad0[0x3C - 4];
    s32 f3c;
    u8 pad1[0x4C - 0x40];
    s32 f4c;
} Blk70;

extern Blk70 D_800EFD70;
extern s32 *D_800EFD64;
extern u8 D_800F1BC4;
extern s32 D_800C2940;
extern s32 D_800CCCA4;
extern volatile s32 D_800CCCA8;
extern void func_80000F0C(void);

void func_80019F7C(void)
{
    if ((D_800EFD70.flags & 0x81) == 0 && (D_800F1BC4 & 2) != 0 && D_800C2940 != 0 &&
        (D_800EFD70.f3c == 1 || D_800EFD70.f4c == 8) &&
        D_800EFD64[11] != 2 && (D_800EFD70.flags & 0x4040000) == 0) {
        D_800CCCA4 = 2;
    } else {
        D_800CCCA4 = 1;
    }
    while (D_800CCCA8 != D_800CCCA4) {
    }
    func_80000F0C();
}
