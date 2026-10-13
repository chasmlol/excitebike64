#include "ctx.h"

typedef struct {
    u8 pad[0x2BF];
    u8 mask;
    u8 cup;
} Save;

extern s32 D_800F0180;
extern Save D_800F1900;

s32 func_80077000(s32 n)
{
    if (D_800F0180 == D_800F1900.cup) {
        s32 r = 0;
        if (n == 0 || ((D_800F1900.mask >> (n - 1)) & 1)) {
            r = 1;
        }
        return r;
    }
    switch (D_800F1900.cup) {
    case 1:
        if (n < 2 && ((D_800F1900.mask >> (n - 1)) & 1)) return 1;
        break;
    case 2:
        if (n < 2 || (n == 2 && ((D_800F1900.mask >> (n - 1)) & 1))) return 1;
        break;
    case 3:
        if (n < 3 || (n == 3 && ((D_800F1900.mask >> (n - 1)) & 1))) return 1;
        break;
    }
    return 0;
}
