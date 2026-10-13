#include "ctx.h"

extern s32 D_800EFD54;
extern u16 D_800EECE0[];

void func_8004F818(s16 *p, s32 n) {
    s32 v;
    s32 i2;
    s32 t;
    for (; n != 0; n--) {
        v = *p + ((s16)D_800EECE0[D_800EFD54] >> 1);
        if (v > 0x7FFF) {
            v = 0x7FFF;
        }
        if (v < -0x7FFF) {
            v = -0x7FFF;
        }
        D_800EECE0[D_800EFD54] = v;
        *p = v;
        p++;
        i2 = D_800EFD54 + 1;
        t = i2;
        if (i2 < 0) {
            t = D_800EFD54 + 0x800;
        }
        D_800EFD54 = i2 - (t >> 11) * 0x800;
    }
}
