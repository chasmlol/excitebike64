#include "ctx.h"

typedef struct { s32 x, y, w, h; } Rect;
typedef struct { s32 v; } S1;
typedef struct { u8 pad[0x2C]; s32 f2c; } Obj;

extern u32 D_800EFD70;
extern s32 D_800C2594;
extern Obj *D_800EFD64;
extern u16 *D_800C29E0;
extern volatile s32 D_800F13EC;
extern u8 D_800E1718[];
extern s32 D_800E8EE0[];
extern s32 D_800C98B4[];

void func_80033E30(void)
{
    s32 i, x, y, m;
    Rect *r;
    Rect *q;
    S1 *p;

    if ((D_800EFD70 & 0x10000) == 0) {
        return;
    }
    m = D_800C2594 - 1;
    for (i = 0; i < D_800C2594; i++) {
        if (D_800EFD64->f2c == 0) {
            D_800E8EE0[i] = ((Rect *)(D_800E1718 + (m * 64 + i * 16)))->h;
        } else {
            D_800E8EE0[i] = 0;
            y = ((Rect *)(D_800E1718 + (m * 64 + i * 16)))->y;
            r = (Rect *)(D_800E1718 + (m * 64 + i * 16));
            if (y < r->y + r->h) {
            yl:
                p = (S1 *)&D_800E8EE0[i];
                for (x = r->x; x < r->x + r->w; x += 8) {
                    if (*(u16 *)((u32)D_800C29E0 + ((y * D_800F13EC + x) << 1)) > 0xFFEF) {
                        p->v = y - r->y;
                    }
                }
                y += 16;
                if (y < r->y + r->h) goto yl;
            }
            D_800E8EE0[i] += 16;
            if (D_800C98B4[i] > 16 && D_800E8EE0[i] < D_800C98B4[i] - 16) {
                D_800E8EE0[i] = D_800C98B4[i] - 16;
            }
            q = (Rect *)(D_800E1718 + (m * 64 + i * 16));
            if (D_800E8EE0[i] > q->h) {
                D_800E8EE0[i] = q->h;
            }
            if (D_800E8EE0[i] < q->h / 2) {
                D_800E8EE0[i] = q->h;
            }
        }
        D_800C98B4[i] = D_800E8EE0[i];
    }
}
