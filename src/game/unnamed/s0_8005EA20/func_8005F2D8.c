#include "ctx.h"

typedef struct {
    s8 a;
    s8 b;
    u8 c;
    u8 d;
} Cell;

extern s32 D_800F0A18;
extern s32 D_800F0A24;
extern u8 *D_800F0A38;
extern Cell D_800F0A40[][8];
extern s8 D_800F0928[];
extern s8 D_800F0930[];
extern s8 D_800F0914[];
extern u32 D_800F0958[][8];
extern s32 D_800F0A2C;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);

static inline s32 rb(void)
{
    u8 *p = D_800F0A38;
    s32 b = *p;
    s32 c = D_800F0A24 - 1;
    s32 bit;

    D_800F0A24 = c;
    bit = (b >> c) & 1;
    if (c == 0) {
        D_800F0A38 = p + 1;
        D_800F0A24 = 8;
    }
    return bit;
}
#define RB() rb()

void func_8005F2D8(void)
{
    s32 i, j, k, n, s;
    u32 v, f;
    s32 x, y, z;

    for (i = 0; i < D_800F0A18; i++) {
        func_8000ED10((u8 *)D_800F0A40[i], 0, 0x20);
        if (RB()) {
            for (j = 0; j < 8; j++) {
                while (RB()) {
                    k = RB() << 2;
                    k |= RB() << 1;
                    k |= RB();
                    D_800F0A40[i][j].c |= 1 << k;
                }
            }
        }
        if (RB()) {
            if (RB()) {
                for (j = 0; j < 8; j++) {
                    if (!RB()) continue;
                    s = RB();
                    if (RB()) {
                        n = 0;
                        for (k = 0; k < 5; k++) { n <<= 1; n |= RB(); }
                    } else {
                        n = 1;
                        while (RB()) n++;
                    }
                    if (s == 0) n = -n;
                    D_800F0A40[i][j].a = n;
                }
            }
            if (RB()) {
                for (j = 0; j < 8; j++) {
                    if (!RB()) continue;
                    s = RB();
                    if (RB()) {
                        n = 0;
                        for (k = 0; k < 5; k++) { n <<= 1; n |= RB(); }
                    } else {
                        n = 1;
                        while (RB()) n++;
                    }
                    if (s == 0) n = -n;
                    D_800F0A40[i][j].b = n;
                }
            }
        }
    }

    for (i = 0; i < D_800F0A18; i++) {
        for (j = 0; j < 8; j++) {
            x = D_800F0928[i] + D_800F0A40[i][j].a;
            y = D_800F0930[i] + D_800F0A40[i][j].b;
            z = D_800F0914[i] ^ D_800F0A40[i][j].c;
            D_800F0928[i] = x;
            D_800F0930[i] = y;
            D_800F0914[i] = z;
            k = (z & 1) << 4;
            if (z & 2) k |= 0x20;
            if (z & 4) k |= 0x20000;
            if (z & 8) k |= 0x100;
            if (z & 0x10) k |= 0x10000;
            if (x < 0) {
                x = -x;
                k |= 4;
                k |= (x & 0xF) << 24;
            } else if (x != 0) {
                k |= 8;
                k |= (x & 0xF) << 24;
            }
            if (y < 0) {
                y = -y;
                k |= 2;
                k |= (y & 0xF) << 28;
            } else if (y != 0) {
                k |= 1;
                k |= (y & 0xF) << 28;
            }
            D_800F0958[i][j] = k;
        }
    }
    D_800F0A2C = 8;
}
