#include "ctx.h"

typedef struct {
    u8 pad[0xA0];
} Track;

typedef struct {
    u8 pad[0x1C];
} Slot;

extern Track *D_800EFD64;
extern Track D_800CB210[];
extern s32 D_800C2594;
extern Slot *D_800CCFFC;
extern s32 D_800CCFF8;
extern u32 *D_800E7728;
extern s32 D_800D1000;
extern u32 D_800F219C;
extern u8 D_800D9190[];

void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
u32 func_800BF79C(u32 *pa0, s32 pa1);
void func_800729D0(Slot *p);

void func_80071990(void)
{
    s32 i;

    if (D_800EFD64 - D_800CB210 == 0x13 && D_800C2594 == 1) {
        D_800CCFFC = func_800BC908(0xE00, 0, (s32)D_800D9190, 0x6B);
        D_800F219C = func_800BF79C(D_800E7728, D_800D1000);
        for (i = 0; i < 0x80; i++) {
            func_800729D0(&D_800CCFFC[i]);
        }
        D_800CCFF8 = 0;
    }
}
