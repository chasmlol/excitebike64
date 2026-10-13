#include "ctx.h"

typedef struct Ent {
    u8 p0[8];
    f32 f8;
    u8 rest[0x60];
} Ent;

typedef struct Obj {
    u8 p0[0x48];
    f32 f48;
    u8 p1[4];
    f32 f50;
    u8 p2[0x628 - 0x54];
    s16 s628;
    u8 p3[0x630 - 0x62A];
    f32 f630;
    u8 p4[4];
    f32 f638;
    u8 p5[0x660 - 0x63C];
    s16 s660;
} Obj;

extern Ent *D_800EE498;

void func_8006BF04(Obj *o)
{
    Ent *t = D_800EE498;
    s32 count = 0;
    f32 f;
    if (o->f50 < t->f8) {
        do {
            count++;
            t++;
        } while (o->f50 < t->f8);
    }
    f = (f32)o->s660 * -997.15717f;
    o->s628 = count;
    o->f638 = -o->f48;
    o->f630 = -f - o->f50;
}
