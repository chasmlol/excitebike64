#include "ctx.h"

void func_8000EF50(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        c = *s;
        if ((u8)(c - 0x61) < 0x1A) {
            c -= 0x20;
        }
        *s = c;
        s++;
        c = *s;
    }
}
