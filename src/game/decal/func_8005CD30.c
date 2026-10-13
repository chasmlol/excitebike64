#include "ctx.h"

typedef struct {
    u8 c[4];
} S4;

extern S4 D_800D8B64;

void func_8005CD30(char *p, f32 x, s32 n)
{
    s32 ip;
    s32 pw;
    s32 d;

    if (x < 0.0f) {
        *p++ = '-';
        x = -x;
    } else {
        *p++ = ' ';
    }
    if (x >= 1.0f) {
        if (x > 1000000.0f) {
            *(S4 *)p = D_800D8B64;
            return;
        }
        pw = 10;
        ip = (s32)x;
        x -= (f32)ip;
        while (ip >= pw) {
            pw *= 10;
        }
        pw /= 10;
        do {
            n--;
            *p++ = ip / pw + '0';
            ip -= (ip / pw) * pw;
            pw /= 10;
        } while (pw != 0);
    } else {
        *p++ = '0';
    }
    if (n > 0) {
        if (x > 0.0001f) {
            *p++ = '.';
        }
        do {
            n--;
            {
                f32 t = x;
                if (!(t > 0.0001f)) break;
            }
            ip = (s32)(x * 10.0f + 0.0001f);
            *p++ = ip + '0';
            x = x * 10.0f - (f32)ip;
        } while (n > 0);
    }
    *p = '\0';
}
