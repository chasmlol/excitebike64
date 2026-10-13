#include "ctx.h"

typedef struct {
    u16 idx;
    s16 x0, y0, z0;
    s16 x1, y1, z1;
    s16 pad[3];
    s16 x2, y2, z2;
} BikeTri;

typedef struct { u16 n; BikeTri t[1]; } Tbl;
extern Tbl *D_800F14F0;
extern s32 D_800F14F4;
extern s32 D_800F14FC;
extern s32 D_800C2594;
extern u8 *D_800C9644;
extern BikeTri **D_800F14F8;
extern u8 D_800C9994;
typedef struct { u8 pad[0x1C]; u32 flags; } Ent;

extern u32 func_8000EFE8(void);
extern void func_80036374(s32 a, s32 b, f32 *c, s32 *d, void *e);

void func_8006ADF0(void)
{
    Tbl *tbl = D_800F14F0;
    s32 i;
    s32 j;
    s32 o;
    Tbl *b;

    if (tbl != 0 && D_800F14F4 != 0 && D_800C2594 == 1) {
        D_800F14F4 = D_800F14F4 - 1;
        D_800F14FC = 0;
        i = 0;
        if (tbl->n != 0) {
            b = tbl;
            do {
                j = b->t[i].idx;
                o = j * 32;
                if (((Ent *)((s32)D_800C9644 + o))->flags & 1) {
                    D_800F14F8[D_800F14FC++] = &b->t[i];
                }
                i++;
            } while (i < b->n);
        }
        if (D_800F14FC != 0) {
            i = 0;
            do {
                BikeTri *s;
                s32 a, b;
                f32 v[4];
                s32 w[16];
                i++;
                j = (s32)func_8000EFE8() % D_800F14FC;
                s = D_800F14F8[j];
                a = (s32)func_8000EFE8() % 255;
                b = (s32)func_8000EFE8() % 255;
                v[0] = s->x0;
                v[1] = s->y0;
                v[2] = s->z0;
                v[0] += ((f32)s->x1 - (f32)s->x0) * (f32)a * 0.00390625f;
                v[1] += ((f32)s->y1 - (f32)s->y0) * (f32)a * 0.00390625f;
                v[2] += ((f32)s->z1 - (f32)s->z0) * (f32)a * 0.00390625f;
                v[0] += ((f32)s->x2 - (f32)s->x0) * (f32)b * 0.00390625f;
                v[1] += ((f32)s->y2 - (f32)s->y0) * (f32)b * 0.00390625f;
                v[2] += ((f32)s->z2 - (f32)s->z0) * (f32)b * 0.00390625f;
                v[0] *= 0.016f;
                v[1] *= 0.016f;
                v[2] *= 0.016f;
                w[0] = 0;
                w[1] = 0;
                w[2] = 0;
                func_80036374(5, 0, v, w, &D_800C9994);
            } while (i < 1);
        }
    }
}
