#include "game.h"

s32 func_800BDCB0(s32 *p1, u32 *p2)
{
    u32 r;
    u32 t;
    u8 *q;
    u8 c;


    r = func_800BDF18(1);
    if (r == 0) {
        q = D_8010065C;
        c = *q;
        *p2 = c;
        D_8010065C = q + 1;
        *p1 = 1;
        return 1;
    }
    r = func_800BDF18(1);
    if (r == 0) {
        *p1 = 2;
    } else if ((r = func_800BDF18(2)) != 0) {
        *p1 = r + 2;
    } else if ((r = func_800BDF18(4)) != 0) {
        *p1 = r + 5;
    } else {
        r = *D_8010065C;
        D_8010065C = D_8010065C + 1;
        if (r == 0) {
            *p2 = 0;
            *p1 = 0;
            return 0;
        }
        *p1 = r + 0x14;
    }
    r = func_800BDF18(2);
    switch (r) {
    case 0:
        r = func_800BDF18(5);
        *p2 = r + 1;
        break;
    case 1:
        r = func_800BDF18(7);
        *p2 = r + 0x21;
        break;
    case 2:
        r = *D_8010065C;
        D_8010065C = D_8010065C + 1;
        t = func_800BDF18(1);
        r = r * 2 + t + 0xa1;
        *p2 = r;
        break;
    case 3:
        r = *D_8010065C;
        D_8010065C = D_8010065C + 1;
        t = func_800BDF18(2);
        r = r * 4 + t + 0x2a1;
        *p2 = r;
        break;
    default:
        break;
    }
    return *p1;
}

s32 func_800BDE38(void)
{
    s32 cnt;
    s32 val;
    u8 *src;
    u8 *dst;
    u8 *p;

    func_800BDF08();
    for (;;) {
        if (func_800BDCB0(&cnt, &val) < 0) {
            return -1;
        }
        if (cnt == 0) {
            break;
        }
        if (cnt == 1) {
            p = D_80100660;
            *p = (u8)val;
            D_80100660 = p + 1;
        } else {
            src = D_80100660 - val;
            for (cnt = cnt - 1; cnt != -1; cnt = cnt - 1) {
                dst = D_80100660;
                *dst = *src;
                src = src + 1;
                D_80100660 = dst + 1;
            }
        }
    }
    return 0;
}

void func_800BDF08(void)
{
    D_80100650 = 1;
}

u32 func_800BDF18(s32 param_1)
{
    u32 r = 0;
    s32 n = param_1;
    n -= 1;

    if (param_1 != 0) {
        do {
            n = n - 1;
            if (D_80100650_2 == 1) {
                u32 b = *D_8010065C;
                D_8010065C = D_8010065C + 1;
                D_80100650_2 = b + 0x100;
            }
            r = (r << 1) | (D_80100650_2 & 1);
            D_80100650_2 = D_80100650_2 >> 1;
        } while (n != -1);
    }
    return r;
}
