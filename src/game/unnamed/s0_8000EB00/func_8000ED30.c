#include "ctx.h"

void func_8000ED30(char *dst, char *src)
{
    while (*dst != '\0') {
        dst++;
    }
loop:
    *dst = *src;
    dst++;
    if (*src == '\0') {
        goto done;
    }
    src++;
    goto loop;
done:
    return;
}
