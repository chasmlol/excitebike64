#include "boot.h"

/* Writes `n` in decimal to `s`, with a '-' when it is negative. */
void boot_int_to_str(s32 n, char *s)
{
    s32 place;

    if (n < 0) {
        *s = '-';
        s = s + 1;
        n = -n;
    }
    /* The power of ten of n's first digit. */
    place = 1;
    if (n > 0) {
        do {
            place = place * 10;
        } while (place <= n);
    }
    if (n != 0) {
        place = place / 10;
    }
    for (; place > 0; place = place / 10) {
        *s = (char)(n / place) + '0';
        s = s + 1;
        n = n - (n / place) * place;
    }
    *s = '\0';
}
