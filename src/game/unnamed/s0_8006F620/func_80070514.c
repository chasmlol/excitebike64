#include "ctx.h"

s32 func_80070514(u8 *a, u8 *b) {
    s32 i;

    for (i = 0; i < 7; i++) {
        u8 *pa = a + i;
        u8 *pb = b + i;

        if (pa[1] > pb[1]) {
            return 0;
        }
        if (pb[1] > pa[1]) {
            return 1;
        }
    }
    return 0;
}
