#include "ctx.h"

typedef struct { u32 flags; s32 pad[4]; s32 x, y, z; s32 res; } Obj;
typedef struct { s32 pad; s32 n; Obj o[1]; } Lst;
typedef struct { s32 x, y, w, h; } Rect;

extern Lst *D_800C9860;
extern Rect D_800E1718;
extern s32 D_800F13EC;
extern u16 *D_800C29E0;
void func_800AD7E0(s32, s32);
s32 func_80033DA0(s32 x);

void func_800325FC(s32 a0)
{
    s32 i, r, c;
    s32 x, y, thr, v, cnt, tot;
    s32 x0, y0, w, h, xe, ye;
    Obj *o;
    u16 *tb;

    if (D_800C9860 == NULL) {
        return;
    }
    func_800AD7E0(0, 0x400000);
    w = D_800E1718.w;
    h = D_800E1718.h;
    x0 = D_800E1718.x;
    y0 = D_800E1718.y;
    i = 0;
    if (i < D_800C9860->n) {
        do {
            o = &D_800C9860->o[i];
            o->res = -1;
            if (!(o->flags & 8)) {
                o->flags &= ~0x10;
            } else {
                tot = 0;
                cnt = 0;
                x = o->x - 4;
                y = o->y - 4;
                thr = o->z - 0x19;
                for (r = 0; r < 8; r++) {
                    if (y < y0) {
                        y = y0;
                    }
                    if (y >= y0 + h) {
                        y = y0 + h - 1;
                    }
                    for (c = 0; c < 8; c++) {
                        if (x < x0) {
                            x = x0;
                        }
                        if (x >= x0 + w) {
                            x = x0 + w - 1;
                        }
                        tb = &D_800C29E0[y * D_800F13EC + x];
                        v = func_80033DA0(*tb);
                        if (v > 0x7FFF) {
                            v = 0x7FFF;
                        }
                        if (v >= thr) {
                            cnt++;
                        }
                        tot++;
                        x++;
                    }
                    x -= 8;
                    y++;
                }
                if (tot != 0) {
                    cnt = cnt * 7 / tot;
                }
                cnt--;
                if (cnt > 5) {
                    cnt = 5;
                }
                o->res = cnt;
                o->flags |= 0x10;
            }
            i++;
        } while (i < D_800C9860->n);
    }
}
