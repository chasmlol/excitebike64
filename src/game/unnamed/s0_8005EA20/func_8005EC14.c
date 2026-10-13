#include "ctx.h"

typedef struct {
    s8 x;
    s8 y;
    u8 bits;
    u8 pad;
} Ent;

extern s32 D_800F094C;
extern s32 D_800F0A1C;
extern s32 D_800F0A34;
extern s32 D_800F0A30;
extern s32 D_800F0954;
extern s32 D_800F0A3C;
extern s32 D_800F0950;
extern s32 D_800F0A20;
extern Ent D_800F0850[][8];
extern u8 D_800F091C[];
extern u8 D_800F093C[];
extern u8 D_800F0944[];

void func_8005EB68(s32 v, s32 n);

void func_8005EC14(u32 *p)
{
    s32 i, j, k;
    s32 x, y, bits, c, v, a, n;
    u32 w;

    if (D_800F0A34 != 0) {
        return;
    }
    D_800F094C++;
    for (i = 0; i < D_800F0A1C; i++) {
        w = p[i];
        x = (w >> 24) & 0xF;
        if (w & 4) x = -x;
        y = w >> 28;
        if (w & 2) y = -y;
        bits = (w >> 4) & 1;
        if (w & 0x20) bits |= 2;
        if (w & 0x20000) bits |= 4;
        if (w & 0x100) bits |= 8;
        c = bits;
        if (w & 0x10000) c |= 0x10;
        D_800F0850[i][D_800F0A30].x = x - D_800F093C[i];
        D_800F0850[i][D_800F0A30].y = y - D_800F0944[i];
        D_800F0850[i][D_800F0A30].bits = D_800F091C[i] ^ c;
        D_800F093C[i] = x;
        D_800F0944[i] = y;
        D_800F091C[i] = c;
    }
    D_800F0A30++;
    if (D_800F0A30 != 8) {
        return;
    }
    D_800F0A30 = 0;
    for (i = 0; i < D_800F0A1C; i++) {
        for (j = 0; j < 8; j++) {
            if (D_800F0850[i][j].bits != 0) break;
        }
        if (j == 8) {
            func_8005EB68(0, 1);
        } else {
            func_8005EB68(0x80, 1);
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 5; k++) {
                    if ((D_800F0850[i][j].bits >> k) & 1) {
                        func_8005EB68(((k << 4) | ~0x7F) & 0xF0, 4);
                    }
                }
                func_8005EB68(0, 1);
            }
        }
        for (j = 0; j < 8; j++) {
            if (D_800F0850[i][j].x != 0 || D_800F0850[i][j].y != 0) break;
        }
        if (j == 8) {
            func_8005EB68(0, 1);
            continue;
        }
        func_8005EB68(0x80, 1);
        for (j = 0; j < 8; j++) {
            if (D_800F0850[i][j].x != 0) break;
        }
        if (j == 8) {
            func_8005EB68(0, 1);
        } else {
            func_8005EB68(0x80, 1);
            for (j = 0; j < 8; j++) {
                if (D_800F0850[i][j].x == 0) {
                    a = 0;
                    n = 1;
                } else {
                    func_8005EB68(0x80, 1);
                    v = D_800F0850[i][j].x;
                    if (v > 0) {
                        func_8005EB68(0x80, 1);
                        v &= 0x1F;
                    } else {
                        func_8005EB68(0, 1);
                        v = -v;
                        v &= 0x1F;
                    }
                    if (v == 1) { a = 0; n = 2; }
                    else if (v == 2) { a = 0x40; n = 3; }
                    else if (v == 3) { a = 0x60; n = 4; }
                    else { a = v << 2; a |= ~0x7F; a &= 0xFC; n = 6; }
                }
                func_8005EB68(a, n);
            }
        }
        for (j = 0; j < 8; j++) {
            if (D_800F0850[i][j].y != 0) break;
        }
        if (j == 8) {
            func_8005EB68(0, 1);
        } else {
            func_8005EB68(0x80, 1);
            for (j = 0; j < 8; j++) {
                if (D_800F0850[i][j].y == 0) {
                    a = 0;
                    n = 1;
                } else {
                    func_8005EB68(0x80, 1);
                    v = D_800F0850[i][j].y;
                    if (v > 0) {
                        func_8005EB68(0x80, 1);
                        v &= 0x1F;
                    } else {
                        func_8005EB68(0, 1);
                        v = -v;
                        v &= 0x1F;
                    }
                    if (v == 1) { a = 0; n = 2; }
                    else if (v == 2) { a = 0x40; n = 3; }
                    else if (v == 3) { a = 0x60; n = 4; }
                    else { a = v << 2; a |= ~0x7F; a &= 0xFC; n = 6; }
                }
                func_8005EB68(a, n);
            }
        }
    }
    D_800F0A20 = ((D_800F0954 - D_800F0A3C) * 100) / D_800F0950;
}
