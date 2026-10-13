#include "ctx.h"

extern u8 *D_800CEA40;

void func_8009EB4C(void) {
    u8 *p = D_800CEA40;
    u8 *n;
    if (*(u8 **)(p + 0x190) != 0) {
        return;
    }
    n = *(u8 **)(p + 0x17C);
    if (n == 0) {
        return;
    }
    do {
        if (*(u16 *)(*(u8 **)(n + 0x14) + 0xC) & 2) {
            *(u8 **)(p + 0x190) = n;
            return;
        }
        n = *(u8 **)(n + 4);
        if (n == *(u8 **)(p + 0x17C)) {
            return;
        }
    } while (n != 0);
}
