#include "ctx.h"

extern u8 D_800CDE74[];

void func_8008C5E8(u8 *p, s32 n)
{
    u8 *q;
    u8 c;
    s32 j;
    s32 i;

    for (i = 0; i < n; i++, p++) {
        j = 0;
        if (D_800CDE74[0] == 0) goto nf;
        q = p;
        c = *p;
        while (D_800CDE74[j] != 0) {
            if (c == D_800CDE74[j]) break;
            j++;
        }
        if (D_800CDE74[j] != 0) goto st;
    nf:
        if (*p != 0) goto nz;
        *p = 0;
        goto next;
    nz:
        *p = 0xF;
        goto next;
    st:
        *q = j + 0xF;
    next:
        ;
    }
}
