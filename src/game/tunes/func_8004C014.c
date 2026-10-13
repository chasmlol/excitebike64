#include "ctx.h"

typedef struct {
    u16 time;
    u8 note;
    u8 ch;
} Cmd;

typedef struct {
    s32 smp;
    s32 f04;
} Ent;

typedef struct {
    s32 x;
    s32 smp;
} Pair;

typedef struct {
    s32 count;
    Ent ent[1];
} Song;

typedef struct {
    u8 pad0[8];
    s32 f08;
    u8 pad0C[4];
    s32 f10;
} Smp;

typedef struct {
    u8 pad0[0x10];
    s32 note;
    s32 f14;
    u8 pad18[4];
    s32 f1C;
    s32 f20;
    s32 f24;
    u8 pad28[0x48 - 0x28];
    s32 vol;
} Chan;

extern Song *D_800CA860;
extern Cmd *D_800EFD48;
extern s32 D_800EFCF4;
extern s32 D_800CA848[];
extern Chan *D_800EFD24;
extern Smp *D_800EFD4C;
extern s32 D_800EECD4;
extern s32 D_800E32A0;

void func_8004C014(void)
{
    Song *s;
    Cmd *c;
    Smp *sm;
    Pair *e;
    Song *s0;

    if (D_800CA860 == NULL) {
        return;
    }
    while (D_800EFD48->time <= D_800EFCF4) {
        c = D_800EFD48;
        s = D_800CA860;
        if (c->note == 0xFE) {
            D_800EFCF4 = 0;
            D_800EFD48 = (Cmd *)&s->ent[s->count];
        } else {
            if (c->note == 0xFF) {
                D_800EFD24[c->ch & 0xF].note = -1;
            } else {
                D_800EFD24[c->ch & 0xF].note = c->note;
                D_800EFD24[c->ch & 0xF].f14 = 0;
                e = &((Pair *)s)[c->note];
                D_800EFD24[c->ch & 0xF].f20 = (sm = &D_800EFD4C[e->smp])->f08 + D_800EECD4;
                D_800EFD24[c->ch & 0xF].f24 = s->ent[c->note].f04;
                D_800EFD24[c->ch & 0xF].f1C = sm->f10;
                D_800EFD24[c->ch & 0xF].vol = D_800CA848[c->ch & 0xF] * D_800E32A0 >> 8;
            }
            D_800EFD48++;
        }
    }
}
