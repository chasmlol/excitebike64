#include "ctx.h"

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
u32 func_8000EFE8(void);

typedef struct {
    u16 f0;
    u16 step;
    s32 f4;
    s32 vol;
    s32 dec;
    u16 pos;
    u16 lim;
    s32 c;
    s32 d;
    s32 e;
    u32 flags;
} Chan;

typedef struct {
    u8 pad[0x1412C];
    Chan ch[4];
} SndMem;

#define SND ((SndMem *)0x80380000)

extern s32 D_800F5A78;
extern s32 D_800CEC18;
extern s32 D_800D0670;

void func_800A1570(s16 *p)
{
    s32 n, i, j, v;
    Chan *c;
    s32 old;

    n = D_800F5A78 / D_800CEC18;
    if (D_800D0670 == 0) {
        return;
    }
    if (!(SND->ch[2].flags & 8) || SND->ch[2].d >= SND->ch[2].c || SND->ch[2].vol == 0) {
        func_8000ED10((u8 *)p, 0, n * 4);
    } else {
        for (j = 0; j < n * 2; j += 2) {
            SND->ch[2].pos += SND->ch[2].step;
            v = SND->ch[2].pos;
            v -= 0x4000;
            if (v > 0x4000) {
                v = 0x8000 - v;
            }
            v >>= 1;
            p[j] = v;
            p[j + 1] = v;
        }
    }
    for (i = 0; i < 2; i++) {
        if ((SND->ch[i].flags & 8) && SND->ch[i].d < SND->ch[i].c && SND->ch[i].vol != 0) {
                for (j = 0; j < n * 2; j += 2) {
                SND->ch[i].pos += SND->ch[i].step;
                if (SND->ch[i].lim < SND->ch[i].pos) {
                    v = 0x1000;
                } else {
                    v = -0x1000;
                }
                v = (v * SND->ch[i].vol) >> 16;
                p[j] += v;
                p[j + 1] += v;
            }
        }
    }
    if ((SND->ch[3].flags & 8) && SND->ch[3].d < SND->ch[3].c && SND->ch[3].vol != 0) {
        for (j = 0; j < n * 2; j += 2) {
            old = SND->ch[3].pos;
            SND->ch[3].pos += SND->ch[3].step;
            if (SND->ch[3].pos < old) {
                v = ((s32)func_8000EFE8() - 0x80) << 4;
                v = v * SND->ch[3].vol >> 16;
            }
            p[j] += v;
            p[j + 1] += v;
        }
    }
    for (i = 0; i < 4; i++) {
        SND->ch[i].d += SND->ch[i].e;
        SND->ch[i].vol -= SND->ch[i].dec;
        if (SND->ch[i].vol < 0) {
            if ((SND->ch[i].flags & 0x10) && i < 2) {
                SND->ch[i].vol = 0x10000;
            } else {
                SND->ch[i].vol = 0;
                SND->ch[i].dec = 0;
            }
        }
    }
}
