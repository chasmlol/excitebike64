#include "ctx.h"

typedef struct {
    s32 a;
    s32 buf[4];
    s32 flag;
} Ent;

extern s32 D_800D33F0;
extern s32 *D_800FE860;
extern Ent D_800FA860[];

s32 func_800B8CBC(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5)
{
    s32 r;
    s32 buf[4];
    Ent *e;
    Ent *base;

    if (D_800D33F0 != 0) {
        r = ((s32 (*)(s32, s32, s32, s32, s32))D_800FE860[5])(p1, p2, p3, p4, p5);
        if (r < 0) {
            return r;
        }
        if (r < 0x100) {
            base = D_800FA860;
            e = base + r;
            e->a = 0;
            e->flag = 1;
            ((void (*)(s32, s32 *))D_800FE860[7])(r, buf);
            e->buf[0] = buf[0];
            e->buf[1] = buf[1];
            e->buf[2] = buf[2];
            e->buf[3] = buf[3];
            return r;
        }
        ((void (*)(s32))D_800FE860[6])(r);
    }
    return -1;
}
