#include "ctx.h"

void func_800C2124(u8 *s) {
    u8 *p = s;
    u8 c;

    if (s[0] != 0) {
        do {
            c = *p;
            if ((u32)(c - 0x61) < 0x1A) {
                *p = c - 0x20;
            }
            p++;
        } while (*p != 0);
    }
    if (s[0] == 0x4D) {
        if (s[1] == 0x43) {
            s[1] = 0x63;
        }
    }
    if (s[0] == 0x44 && s[1] == 0x45 && s[2] == 0x43 && s[3] == 0x4C) {
        s[1] = 0x65;
    }
}
