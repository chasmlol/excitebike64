#include "ctx.h"

typedef struct { f32 x, y; } V2;
typedef struct { u8 pad[0x628]; s16 obj; u8 pad2[0x63C - 0x62A]; f32 t; u8 pad3[0x7E0 - 0x640]; } Player;
typedef struct { u8 pad[0x46]; s16 next; u8 pad2[0x6C - 0x48]; } Obj;

extern Player D_800E37C0[];
extern Obj *D_800EE498;
extern s32 *D_800F1458;
extern s32 D_800F145C;
extern s32 D_800F1460;
extern f32 D_800F1468[2];
extern f32 D_800F1470[2];
extern s32 D_800F14B8;
extern s32 D_800F14BC;
void func_80069A84(f32 *a, f32 *b, f32 *c);

void func_80068F38(s32 player, s32 *px, s32 *py)
{
    Player *pl;
    Player *base;
    Obj *o;
    Obj *o2;
    V2 a, b;
    f32 cx, cy, t;
    s32 x, y, w, h;
    f32 fy, nx, ny, dx, dy;
    s32 p, q;

    base = D_800E37C0;
    pl = &base[player];
    cx = (D_800F1468[0] + D_800F1470[0]) * 0.5f;
    cy = (D_800F1468[1] + D_800F1470[1]) * 0.5f;
    o = &D_800EE498[pl->obj];
    o2 = &D_800EE498[o->next];
    func_80069A84((f32 *)o, &a.x, &a.y);
    func_80069A84((f32 *)o2, &b.x, &b.y);
    t = pl->t;
    if (t < 0.0f) {
        t = 0.0f;
    }
    if (1.0f < t) {
        t = 1.0f;
    }
    dx = t * (b.x - a.x);
    dy = t * (b.y - a.y);
    nx = a.x + dx;
    t = (nx - cx) * (f32)D_800F1458[0];
    ny = a.y + dy;
    fy = (ny - cy) * (f32)D_800F1458[1];
    t *= 6.1035156e-05f;
    fy *= 6.1035156e-05f;
    w = D_800F14B8;
    x = (s32)t;
    p = x * w;
    a.x = nx;
    a.y = ny;
    y = (s32)fy;
    y = -y;
    if (p < 0) {
        p += 0x7F;
    }
    x = p >> 7;
    h = D_800F14BC;
    q = y * h;
    if (q < 0) {
        q += 0x7F;
    }
    y = q >> 7;
    x += D_800F145C + w / 2;
    y += D_800F1460 + h / 2;
    *px = x;
    *py = y;
}
