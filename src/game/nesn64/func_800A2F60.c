#include "ctx.h"

extern s32 D_800D0670;
extern s32 D_800D0660;

s32 func_800A2F60(s32 param_1, s32 param_2) {
    u8 *nes = (u8 *)0x80390000;
    u8 *ram = (u8 *)0x80388000;
    D_800D0670 = param_1;
    D_800D0660 = param_2;
    nes[0x4106] = 0;
    nes[0x4107] = 0;
    nes[0x4109] = 0;
    nes[0x410A] = 1;
    nes[0x410B] = 0;
    nes[0x410C] = 0;
    *(u16 *)(nes + 0x4100) = (u16)(ram[0x7FFC] | (ram[0x7FFD] << 8));
    return 0;
}
