#include "ctx.h"

void func_8000EF8C(char *a0, char *a1)
{
    while (1) {
        *a0 = *a1;
        a0 = a0 + 1;
        if (*a1 == 0) {
            break;
        }
        a1 = a1 + 1;
    }
}
