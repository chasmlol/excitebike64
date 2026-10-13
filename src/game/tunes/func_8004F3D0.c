#include "ctx.h"

extern u32 *D_800EFD4C;

u32 func_8004F3D0(u32 key)
{
    u32 *p;
    u32 i = 0;
    u32 n;

    p = D_800EFD4C;
    if (p[0] != 0) {
        n = p[0];
        do {
            if (p[5] == key) {
                return i;
            }
            i++;
            p += 5;
        } while (i < n);
    }
    return 0xFFFFFFFF;
}
