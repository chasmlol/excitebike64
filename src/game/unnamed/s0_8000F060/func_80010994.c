#include "ctx.h"

typedef struct {
    s32 w0[10];
    s32 f28;
    s32 w1[4];
    s32 f3c;
    s32 w2[9];
} Rec25;

typedef struct {
    s32 a;
    s32 id;
} Snd;

extern s32 D_800C2B64;
extern s32 D_800C2C54;
extern s32 D_800C2B58;
extern s32 D_800E76D0;
extern Rec25 D_800E32A8[];
extern Snd D_800C2C10[];

void func_800B8E08(s32 a, s32 *b);
s32 func_800B7990(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);

void func_80010994(s32 k, s32 n, s32 vol)
{
    s32 buf[4];
    Snd *s, *tb;
    s32 t, x;

    if (D_800C2B64 != 0) {
        if (k == 8 && D_800C2C54 == -1) {
            k = 4;
        }
        if (k == 4 || k == 8) {
            if (D_800E76D0 - D_800E32A8[n].f3c < 20) {
                return;
            }
            D_800E32A8[n].f3c = D_800E76D0;
        }
        tb = D_800C2C10;
        s = &tb[k];
        func_800B8E08(s->id, buf);
        t = D_800C2B58 * D_800E32A8[n].f28 / 256;
        x = t * vol;
        t = x / 255;
        if (t >= 32) {
            if (t > 255) {
                t = 255;
            }
            func_800B7990(s->id, buf[3], t, 7, 1, 0);
        }
    }
}
