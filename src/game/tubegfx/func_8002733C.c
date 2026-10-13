#include "ctx.h"

typedef struct Obj {
    s32 f0;
    u8 *base;
    s32 f8;
    u8 *bytes;
} Obj;

typedef struct Ent {
    Obj *obj;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;
    s32 pad[3];
} Ent;

typedef struct Tbl {
    u32 n;
    Ent e[1];
} Tbl;

extern Tbl *D_800C9648;
extern s32 *D_800EFD64;
s32 func_80031D54(s32 *p1, s32 *p2, s32 x, s32 y);

s32 func_8002733C(s32 x, s32 y, s32 *out)
{
    u32 i;
    s32 k;
    s32 la[4];
    s32 lb[4];
    s32 fx, fy;
    Ent *e;
    u8 *pb;
    u8 *base;

    if (D_800C9648 == NULL) {
        return 0;
    }
    x >>= 4;
    y >>= 4;
    for (i = 0; i < D_800C9648->n; i++) {
        e = &D_800C9648->e[i];
        if (x < e->x0 || e->x1 < x || y < e->y0 || e->y1 < y) {
            continue;
        }
        if (D_800EFD64[0xb] == 1) {
            fx = x - (e->x0 + e->x1) / 2;
            fy = y - (e->y0 + e->y1) / 2;
        } else {
            fx = x;
            fy = y;
        }
        for (pb = e->obj->bytes; pb[0] != 0; pb += 4) {
            if ((pb[0] >> 4) == 3) {
                for (k = 0; k < 3; k++) {
                    la[k] = *(s16 *)((u32)(pb[k + 1] << 4) + (u32)base);
                    lb[k] = *(s16 *)((u32)(pb[k + 1] << 4) + (u32)base + 4);
                }
                if (func_80031D54(la, lb, fx, fy)) {
                    *out = *(s16 *)((u32)(pb[1] << 4) + (u32)base + 2) << 4;
                    return 1;
                }
            } else if ((pb[0] >> 4) == 1) {
                base = e->obj->base + (*(u16 *)(pb + 2) << 4);
            }
        }
    }
    return 0;
}
