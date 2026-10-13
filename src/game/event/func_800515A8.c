#include "ctx.h"

typedef struct {
    s32 state;
    u8 pad4[0x26 - 4];
    s16 timer;
    u8 pad28[0x58 - 0x28];
    s32 flags;
} Ctl;

extern Ctl D_800F05A8;

s32 func_8004F270(s32 param_1, s32 param_2, s32 param_3);

void func_800515A8(void)
{
    if (D_800F05A8.timer != 0 && --D_800F05A8.timer == 0) {
        switch (D_800F05A8.state) {
        case 0:
            D_800F05A8.timer = 150;
            D_800F05A8.state = 1;
            break;
        case 1:
            D_800F05A8.timer = 36;
            D_800F05A8.state = 2;
            func_8004F270(0x6F, 5, 1);
            break;
        case 2:
            D_800F05A8.timer = 36;
            D_800F05A8.state = 3;
            func_8004F270(0x6D, 6, 1);
            break;
        case 3:
            D_800F05A8.timer = 36;
            D_800F05A8.state = 4;
            func_8004F270(0x6E, 7, 1);
            break;
        case 4:
            D_800F05A8.timer = 15;
            D_800F05A8.state = 5;
            D_800F05A8.flags &= ~0x10;
            func_8004F270(0x70, 8, 1);
            break;
        }
    }
}
