#include "ctx.h"

void func_800C2054(char *dst, char *src)
{
    do {
        *dst++ = *src++;
        *dst++ = '.';
        *dst++ = ' ';
        while (*src != 0) {
            if (*src == ' ' || *src == '.') {
                break;
            }
            src++;
        }
        while (*src != 0) {
            if (*src != ' ' && *src != '.') {
                break;
            }
            src++;
        }
    } while (*src != 0);
    dst[-1] = 0;
}
