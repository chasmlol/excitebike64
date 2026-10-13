#include "ctx.h"

typedef struct {
    u8 pad0[8];
    f32 f8;
    u8 pad1[0x6C - 12];
} Ent;

typedef struct {
    u8 pad0[0x50];
    f32 f50;
    u8 pad1[0x628 - 0x54];
    s16 s628;
} Obj;

extern Ent *D_800EE498;

void func_8006BF78(Obj *o) {
    s16 c = 0;
    Ent *p = D_800EE498;
    if (o->f50 < p->f8) {
        do {
            p++;
            c++;
        } while (o->f50 < p->f8);
    }
    o->s628 = c;
}
