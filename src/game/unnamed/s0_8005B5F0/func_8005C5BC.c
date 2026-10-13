#include "ctx.h"

#define FLD(t, p, o) (*(t *)((u8 *)(p) + (o)))

extern f32 D_800C9050[];
extern f32 D_800C901C[];
extern f32 D_800C8FD8[];
extern f32 D_800C8E00;
extern f32 D_800CB130;
extern s32 D_800EFDBC;

void func_8006BC14(u8 *p);

void func_8005C5BC(u8 *p, s32 m)
{
    s32 dir, i;
    u8 b;
    f32 x, lim, r, w, T, S, g;

    dir = FLD(s8, p, 0x5d0);
    if (FLD(s16, p, 0x5aa) == 4) {
        func_8006BC14(p);
    } else {
        b = (s32)(FLD(f32, p, 0x48) / 2.178f);
        FLD(u8, p, 0x7a9) = b;
        if (FLD(u8, p, 0x44) != 0) {
            FLD(s8, p, 0x7aa) = 0;
        } else {
            FLD(s8, p, 0x7aa) = (s32)((FLD(f32, p, 0x48) - b * 2.178f) * (D_800C8E00 / 2.178f));
            if (FLD(u8, p, 0x7a9) == 0 && FLD(s8, p, 0x7aa) < 0) {
                FLD(s8, p, 0x7aa) = 0;
            }
        }
    }
    if (FLD(s32, p, 0x744) >= 8) {
        dir = 0;
    }
    x = FLD(f32, p, 0x420);
    i = 0;
    while (D_800C9050[i + 1] < x) {
        i++;
    }
    if (x < 0.0f) {
        r = 0.0f;
    } else {
        r = (D_800C9050[i + 1] - x) / (D_800C9050[i + 1] - D_800C9050[i]);
    }
    S = 0.012566371f;
    T = dir * S;
    w = (D_800C901C[i] * r + D_800C901C[i + 1] * (1.0f - r)) * FLD(f32, p, 0x7d0);
    T *= w;
    lim = 0.18849556f;
    lim *= w;
    S = (D_800C8FD8[i] * r + D_800C8FD8[i + 1] * (1.0f - r)) * 0.327f * m * S;
    if (FLD(u8, p, 0x6e9) != 0) {
        T *= 0.25f;
    }
    if (T == 0.0f || T * FLD(f32, p, 0x5a0) < 0.0f) {
        S *= 5.0f;
    }
    if (FLD(u8, p, 0x41c) != 0) {
        T = D_800CB130;
        if (FLD(s8, p, 0x5d0) <= 0) {
            T = -T;
        }
    } else if (lim < T) {
        S = 0.1f;
        T = lim;
    } else {
        w = -lim;
        if (T < w) {
            S = 0.1f;
            T = w;
        }
    }
    if (D_800EFDBC == 7) {
        T += (FLD(f32, p, 0xb8) * FLD(f32, p, 0x56c) - FLD(f32, p, 0xc0) * FLD(f32, p, 0x564)) * 16.0f * S;
    }
    g = FLD(f32, p, 0x5a0);
    if (T < g) {
        FLD(f32, p, 0x5a0) = g - S;
        if (FLD(f32, p, 0x5a0) < T) {
            FLD(f32, p, 0x5a0) = T;
        }
    } else {
        FLD(f32, p, 0x5a0) = g + S;
        if (T < FLD(f32, p, 0x5a0)) {
            FLD(f32, p, 0x5a0) = T;
        }
    }
}
