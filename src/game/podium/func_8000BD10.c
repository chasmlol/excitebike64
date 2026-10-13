#include "ctx.h"

typedef struct Rec {
    s16 x, y, z, a, b, c;
} Rec;

typedef struct Seg {
    s32 f00;
    s32 len;
    s32 f08;
    s32 f0c;
    Rec *data;
} Seg;

typedef struct Entry {
    s32 w0;
    Seg *seg;
} Entry;

typedef struct Vec {
    s32 x, y, z;
} Vec;

extern Entry D_800C28CC[];
extern s16 D_800C2C60[];
extern s32 D_800E1B7C;
extern s32 D_800E1B9C;
extern Vec D_800E1BF0;
extern volatile Vec D_800E8090;
extern s32 D_800E7978;
extern s32 D_800E78B8;
extern s32 D_800E8000;

void func_8000BD10(void)
{
    s32 t = D_800E1B7C;
    s32 n = 0;
    Rec *p;
    s16 *tp;
    s32 o;
    u16 *ap;
    s32 ang, s, c, A, B, X, Y, as, bs, y2, x2;

    while (t >= D_800C28CC[n].seg->len) {
        t -= D_800C28CC[n].seg->len;
        n++;
        if (D_800C28CC[n].w0 == 0) {
            n--;
            t = D_800C28CC[n].seg->len - 1;
            break;
        }
    }

    o = t * 12;
    p = (Rec *)((u8 *)D_800C28CC[n].seg->data + o);
    A = p->x;
    D_800E8090.x = A;
    B = p->z;
    B = (s16)B;
    D_800E8090.z = B;
    tp = D_800C2C60;
    ang = (s16)D_800E1B9C >> 4;
    s = tp[ang & 0xFFF];
    c = tp[(ang - 0x400) & 0xFFF];
    as = A * s;
    bs = B * s;
    Y = (bs + A * c) >> 14;
    X = (as - B * c) >> 14;
    D_800E8090.z = Y;
    x2 = D_800E1BF0.x;
    y2 = Y + ((volatile Vec *)&D_800E1BF0)->z;
    D_800E8090.x = X;
    D_800E8090.x = X + x2;
    D_800E8090.z = y2;
    D_800E8090.y = p->y + D_800E1BF0.y;
    D_800E7978 = p->a;
    D_800E78B8 = p->b;
    D_800E8000 = p->c + D_800E1B9C;
}
