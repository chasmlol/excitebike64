#include "ctx.h"

typedef struct { s32 a; s32 id; } Snd;
typedef struct { u8 pad[0x28]; s32 vol; u8 pad2[0x38]; } Ent100;

extern s32 D_800C2B64;
extern s32 D_800C2B58;
extern Snd D_800C2C10[];
extern Ent100 D_800E32A8[];

void func_800B8E08(s32 id, s32 *buf);
s32 func_800B7990(s32, s32, s32, s32, s32, s32);

void func_80011378(s32 i, s32 j, s32 vol, s32 pan)
{
    s32 buf[4];
    s32 v;

    if (D_800C2B64 != 0) {
        Snd *tab = D_800C2C10;
        Snd *s = &tab[i];
        func_800B8E08(s->id, buf);
        v = D_800C2B58 * D_800E32A8[j].vol / 256;
        v = v * vol / 255;
        if (v >= 0x20) {
            if (v > 0xff) v = 0xff;
            func_800B7990(s->id, buf[3] * pan / 256, v, 7, 1, 1);
        }
    }
}

