#include "ctx.h"

s32 boot_func_80002808(char *s)
{
    char c;
    char *p;
    c = *s;
    p = s;
    while (p = p + 1, c != 0) {
        c = *p;
    }
    return p - s;
}
