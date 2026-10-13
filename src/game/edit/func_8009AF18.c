#include "ctx.h"

extern u8 *D_800CEA40;
void func_8008D3B4(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i);

#define PTRF(b, off) (*(u8 **)((u8 *)(b) + (off)))
#define S16F(b, off) (*(s16 *)((u8 *)(b) + (off)))
#define U16F(b, off) (*(u16 *)((u8 *)(b) + (off)))
#define S32F(b, off) (*(s32 *)((u8 *)(b) + (off)))

void func_8009AF18(u8 *p)
{
    s16 *tbl2;
    s32 ix;
    s32 iy;
    s32 x;
    s32 y;

    if (PTRF(p, 4) != 0) {
        if (PTRF(p, 8) != 0) {
            S32F(PTRF(p, 8), 4) = 0;
            S32F(PTRF(p, 4), 8) = 0;
            if ((U16F(p, 0x10) & 4) == 0) {
                S32F(D_800CEA40, 0x17C) = (s32)PTRF(p, 4);
                S32F(D_800CEA40, 0x180) = (s32)PTRF(p, 8);
            }
            goto L_d4;
        }
        goto L_c;
    }
    if (PTRF(p, 8) != 0) {
        S32F(PTRF(p, 8), 4) = 0;
        goto L_8c;
    }
L_c:
    if (PTRF(p, 4) != 0) {
        S32F(PTRF(p, 4), 8) = (s32)PTRF(p, 8);
    }
L_8c:
    if ((U16F(p, 0x10) & 4) == 0) {
        if (S32F(D_800CEA40, 0x17C) == (s32)p) {
            S32F(D_800CEA40, 0x17C) = (s32)PTRF(p, 4);
        }
        if (S32F(D_800CEA40, 0x180) == (s32)p) {
            S32F(D_800CEA40, 0x180) = (s32)PTRF(p, 8);
        }
    }
L_d4:
    {
        u8 *gg = D_800CEA40;
        *(u16 *)p = 0;
        if (S32F(gg, 0x190) == (s32)p) {
            S32F(gg, 0x190) = 0;
        }
    }
    for (iy = 0; iy < *(s16 *)((u8 *)PTRF(p, 0x14) + (((S16F(p, 0x12) + 1) & 1) << 1)); iy++) {
        for (ix = 0; ix < *(s16 *)((u8 *)PTRF(p, 0x14) + ((U16F(p, 0x12) & 1) << 1)); ix++) {
            x = S16F(p, 0xC) + ix;
            if (x >= 0) {
                tbl2 = (s16 *)PTRF(D_800CEA40, 0x16AC);
                if (x < tbl2[0]) {
                    y = S16F(p, 0xE) + iy;
                    if (y >= 0 && y < tbl2[1]) {
                        func_8008D3B4(x, y, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        }
    }
}
