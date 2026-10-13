#include "ctx.h"
typedef struct {
    u32 flags;
    u8 pad04[0x38];
    s32 x3C;
    s32 x40;
    s32 x44;
    s32 x48;
    s32 x4C;
    u8 pad50[0x3C3 - 0x50];
    u8 x3C3;
    u8 pad3C4[0x43C - 0x3C4];
    u8 arr[6];
} G;
extern G D_800EFD70;
extern s32 D_800C2388;
extern char *D_800EFD64;
extern char D_800CC070[];

void func_8008802C(s32 p)
{
    s32 i;
    if (p != 0) {
        for (i = 0; i < 6; i++) {
            if (D_800EFD70.x3C < 2) {
                if (i == 0) {
                    D_800EFD70.arr[0] = D_800EFD70.x3C3;
                } else {
                    D_800EFD70.arr[i] = i;
                }
            } else {
                D_800EFD70.arr[i] = i;
            }
        }
        D_800EFD70.x44 = 0;
        D_800C2388 = 1;
        D_800EFD70.x4C = 7;
        D_800EFD64 = D_800CC070;
        D_800EFD70.flags &= ~0x80;
    }
}
