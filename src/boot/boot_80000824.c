#include "boot.h"

void boot_int_to_str(s32 n, char *s)
{
    s32 p;

    if (n < 0) {
        *s = '-';
        s = s + 1;
        n = -n;
    }
    p = 1;
    if (n > 0) {
        do {
            p = p * 10;
        } while (p <= n);
    }
    if (n != 0) {
        p = p / 10;
    }
    for (; p > 0; p = p / 10) {
        *s = (char)(n / p) + '0';
        s = s + 1;
        n = n - (n / p) * p;
    }
    *s = '\0';
}
