#include "ctx.h"

extern s32 D_800F5BB8;

void func_800A3328(void) {
    u8 *nes = (u8 *)0x80390000;
    s32 a = nes[0x4104];
    s32 b = D_800F5BB8;
    if (b < a) {
        nes[0x4108] = 0;
        nes[0x410C] = 1;
        nes[0x410B] = 0;
        return;
    }
    if (a == b) {
        nes[0x4108] = 0;
        nes[0x410C] = 1;
        nes[0x410B] = 1;
        return;
    }
    nes[0x4108] = 1;
    nes[0x410C] = 0;
    nes[0x410B] = 0;
}
