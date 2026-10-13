#include "ctx.h"

typedef struct {
    u8 pad0[0x48];
    Vec3f pos;
    u8 pad54[0x628 - 0x54];
    s16 seg;
    u8 pad62A[0x63C - 0x62A];
    f32 frac;
} Obj;

typedef struct {
    u8 pad0[0x4E];
    s16 start;
    u8 pad50[0x6C - 0x50];
} Seg;

typedef struct {
    u16 flags;
    u8 pad2[6];
} Trk;

extern s32 D_800C9684;
extern Trk D_800E7FC8[];
extern s16 *D_800E80C0;
extern Seg *D_800EE498;
extern f32 D_800EE4A0;
extern s32 D_800EE490;

s32 func_8004B5F0(Vec3f *pos, s32 start, s32 count, u8 mode);
f32 func_8004A944(Vec3f *pos, s32 idx);

f32 func_8002BBD4(Obj *p, s32 a, s32 b)
{
    s32 seg;
    s32 len;
    s32 d;
    f32 frac;

    if (D_800E7FC8[D_800C9684].flags & 8) {
        len = D_800E80C0[b] - D_800E80C0[a];
        seg = func_8004B5F0(&p->pos, D_800EE498[p->seg].start, 15, 1);
        func_8004A944(&p->pos, seg);
        frac = D_800EE4A0;
    } else {
        seg = p->seg;
        seg = (s16)seg;
        frac = p->frac;
        len = D_800E80C0[b] - D_800E80C0[a];
    }
    if (len < 0) {
        len += D_800EE490;
    }
    d = D_800E80C0[seg] - D_800E80C0[a];
    if (d < 0) {
        d += D_800EE490;
    }
    return ((f32)d + frac) / (f32)len;
}
