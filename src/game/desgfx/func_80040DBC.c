#include "ctx.h"

extern s16 D_800C2C60[];
extern u16 D_800E8002;
extern s32 D_800EA7C0;
extern s32 D_800EA7C4;
extern u8 D_800EB9F0[];
extern s32 D_800ED698;
extern s32 D_800ED6A0;
extern s32 D_800ED7A8;
extern s32 D_800ED7B0;
extern s32 D_800EDBB0;
extern s32 D_800EDBB8;
extern s32 D_800EDBBC;
extern s32 D_800EDBC0;
extern s32 D_800EDBC4;
extern s32 D_800EDBC8;
extern u8 D_800EDBD0[];

#define G D_800EB9F0
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_80040DBC(void) {
    s32 i, j, a, k;
    s32 x, z, u, v, h;
    s32 s, c, n, xx;
    s32 d = 4000;
    s32 w, m;

    a = D_800ED698 >> 4;
    h = D_800ED6A0 >> 4;
    D_800EA7C0 = xx = a / d;
    D_800ED7A8 = a - D_800EA7C0 * d + (((u32)a & 1) >> 1);
    w = D_800E8002;
    w >>= 4;
    s = D_800C2C60[w] + (((u32)&D_800C2C60[w] & 1) >> 1);
    c = D_800C2C60[(w - 0x400) & 0xFFF];
    if (D_800ED7A8 < 0) {
        D_800ED7A8 += d;
        D_800EA7C0 -= 1;
    }
    D_800EA7C4 = h / d;
    xx = D_800EA7C4 * d;
    D_800ED7B0 = h - xx + (((u32)h & 1) >> 1);
    if (D_800ED7B0 < 0) {
        D_800ED7B0 += d;
        D_800EA7C4 -= 1;
    }
    n = 0;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            G[n] = 0;
            x = ((j - 15) * 8000 - 4000) >> 4;
            k = ((u32)(D_800ED7A8 >> 31) & 1) >> 1;
            z = (i * 8000 - 0x1E460) >> 4;
            x -= D_800ED7A8 % 500;
            z -= D_800ED7B0 % 500;
            k = s * x;
            v = (s * z - c * x) >> 14;
            u = (k + c * z) >> 14;
            if (v < 5000) G[n] = 3;
            if (v < 4000) G[n] |= 4;
            if (i < 8 || i >= 24 || j < 8 || j >= 24) G[n] &= 0xFB;
            if (v < 1500) G[n] &= 0xF1;
            if (v >= -359 && u >= -359 && u < 360 && v < 360) {
                G[n] |= 1;
            } else if (v < 0) {
                G[n] = 0;
            } else {
                v = v / 2;
                if (u < 0) {
                    if (u + 360 < -v) G[n] &= 0xFE;
                    if (u + 720 < -v) G[n] &= 0xF1;
                } else {
                    if (v < u - 360) G[n] &= 0xFE;
                    if (v < u - 720) G[n] &= 0xF1;
                }
            }
            n++;
        }
    }
    D_800ED7A8 /= 500;
    D_800ED7B0 /= 500;
    D_800EDBB0 = D_800ED7A8 & 1;
    D_800EDBBC = D_800ED7B0 & 1;
    D_800EDBB8 = (D_800ED7A8 & 2) >> 1;
    D_800EDBC4 = (D_800ED7B0 & 2) >> 1;
    D_800EDBC0 = D_800ED7A8 & 4;
    D_800EDBC0 /= 4;
    D_800EDBC8 = D_800ED7B0 & 4;
    D_800EDBC8 /= 4;
    for (i = 2; i < 32; i += 2) {
        for (j = 2; j < 32; j += 2) {
            s32 r, kb, kc;
            r = ((i / 2 + 8) << 5) + 8;
            m = r + j / 2;
            n = ((i - D_800EDBBC) << 5) + (j - D_800EDBB0);
            kb = ((i - D_800EDBC4) << 5) + (j - D_800EDBB8);
            kc = ((i - D_800EDBC8) << 5) + (j - D_800EDBC0);
            if (G[n] & G[n + 1] & G[n + 32] & G[n + 33] & 1) G[m] &= 0xFD;
            if (G[kb] & G[kb + 1] & G[kb + 32] & G[kb + 33] & 2) G[m] &= 0xFB;
            if (G[kc] & G[kc + 1] & G[kc + 32] & G[kc + 33] & 4) G[m] &= 0xF7;
            if (G[m] & 2) { G[n] &= 0xFE; G[n + 1] &= 0xFE; G[n + 32] &= 0xFE; G[n + 33] &= 0xFE; }
            if (G[m] & 4) { G[kb] &= 0xFD; G[kb + 1] &= 0xFD; G[kb + 32] &= 0xFD; G[kb + 33] &= 0xFD; }
            if (G[m] & 8) { G[kc] &= 0xFB; G[kc + 1] &= 0xFB; G[kc + 32] &= 0xFB; G[kc + 33] &= 0xFB; }
        }
    }
    func_8000ED10(D_800EDBD0, 0, 0x400);
    for (i = 2; i < 32; i += 2) {
        for (j = 2; j < 32; j += 2) {
            s32 r = (((i >> 1) + 8) << 5) + 8;
            m = r + (j >> 1);
            n = (i - D_800EDBBC) * 32 + (j - D_800EDBB0);
            if (D_800EB9F0[m + 32] & 2) {
                if (D_800EB9F0[n + 32] & 1) D_800EDBD0[n + 32] |= 0x10;
                if (D_800EB9F0[n + 33] & 1) D_800EDBD0[n + 33] |= 0x10;
            }
            if (D_800EB9F0[m - 32] & 2) {
                if (D_800EB9F0[n] & 1) D_800EDBD0[n] |= 0x20;
                if (D_800EB9F0[n + 1] & 1) D_800EDBD0[n + 1] |= 0x20;
            }
            if (D_800EB9F0[m + 1] & 2) {
                if (D_800EB9F0[n + 1] & 1) D_800EDBD0[n + 1] |= 0x40;
                if (D_800EB9F0[n + 33] & 1) D_800EDBD0[n + 33] |= 0x40;
            }
            if (D_800EB9F0[m - 1] & 2) {
                if (D_800EB9F0[n] & 1) D_800EDBD0[n] |= 0x80;
                if (D_800EB9F0[n + 32] & 1) D_800EDBD0[n + 32] |= 0x80;
            }
            n = (i - D_800EDBC4) * 32 + (j - D_800EDBB8);
            if (D_800EB9F0[m + 32] & 4) {
                if (D_800EB9F0[n + 32] & 2) D_800EDBD0[n + 32] |= 1;
                if (D_800EB9F0[n + 33] & 2) D_800EDBD0[n + 33] |= 1;
            }
            if (D_800EB9F0[m - 32] & 4) {
                if (D_800EB9F0[n] & 2) D_800EDBD0[n] |= 2;
                if (D_800EB9F0[n + 1] & 2) D_800EDBD0[n + 1] |= 2;
            }
            if (D_800EB9F0[m + 1] & 4) {
                if (D_800EB9F0[n + 1] & 2) D_800EDBD0[n + 1] |= 4;
                if (D_800EB9F0[n + 33] & 2) D_800EDBD0[n + 33] |= 4;
            }
            if (D_800EB9F0[m - 1] & 4) {
                if (D_800EB9F0[n] & 2) D_800EDBD0[n] |= 8;
                if (D_800EB9F0[n + 32] & 2) D_800EDBD0[n + 32] |= 8;
            }
        }
    }
}
