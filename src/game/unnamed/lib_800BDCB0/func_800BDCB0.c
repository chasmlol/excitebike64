#include "ctx.h"

extern u8 *D_8010065C;
s32 func_800BDF18();

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
