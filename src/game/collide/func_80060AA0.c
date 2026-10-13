#include "ctx.h"

typedef struct { u8 pad[0x7E0]; } Obj;
typedef struct { s32 w[3]; } W3;

extern u32 D_800C2280;
extern s32 D_800EFDBC;
extern s32 D_800E76D0;
extern s32 D_800F05D4;
extern f32 D_800F0B20[];
extern Obj D_800E37C0[];

extern void func_8004DFB4(s32 a, s32 b);
extern void func_800445B0(void *o, s32 k);
extern void func_80013310(f32 *a, f32 *b, f32 *o);

#define GU8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define GS8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define GS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define GF32(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define IDX(p)     ((s32)((Obj *)(p) - D_800E37C0))

s32 func_80060AA0(void *p1, void *p2)
{
    f32 vec[3];
    f32 o1[3];
    f32 o2[3];
    s32 a, b;
    f32 *q;
    f32 *pa;
    s32 c;

    if (D_800C2280 & 0x400) {
        if (GU8(p1, 0x6E8) != 0) {
            if (GU8(p2, 0x6E8) != 0) goto skip0;
        } else if (GU8(p2, 0x6E8) != 0) {
            func_800445B0(p2, 0x1c);
            return 1;
        }
        if (GU8(p1, 0x6E8) == 0) goto skip0;
        func_800445B0(p1, 0x1c);
        return 1;
    }
skip0:

    if (GS32(p1, 0x744) >= 0 && GS32(p2, 0x744) >= 0) return 0;
    if (GS32(p1, 0x744) < 0x3c) goto L12c;
    if (GS32(p2, 0x744) >= 0) goto L12c;

    a = GU8(p1, 0x6E8);
    b = GU8(p2, 0x6E8);
    if ((a & b) == 0 || D_800F05D4 < 300) func_8004DFB4(IDX(p1), IDX(p2));
    GS8(p2, 0x45) = -0x19;
    goto L1c8;

L12c:
    if (GS32(p2, 0x744) < 0x3c) goto L1d8;
    if (GS32(p1, 0x744) >= 0) goto L1d8;
    a = GU8(p1, 0x6E8);
    b = GU8(p2, 0x6E8);
    if ((a & b) == 0 || D_800F05D4 < 300) func_8004DFB4(IDX(p2), IDX(p1));
    GS8(p1, 0x45) = -0x1C;

L1c8:
    GU8(p1, 0x7A8) = 1;
    GS32(p1, 0x648) = 0;

L1d8:
    if (D_800EFDBC == 6) {
        if (GF32(p1, 0x50) < GF32(p2, 0x50)) {
            if (GF32(p1, 0x48) < GF32(p2, 0x48)) {
                GS8(p2, 0x45) = -0x1F;
            } else {
                GS8(p2, 0x45) = -0x22;
            }
            *(W3 *)((u8 *)p2 + 0x43C) = *(W3 *)((u8 *)p2 + 0x54);
            return 1;
        }
        if (GF32(p2, 0x48) < GF32(p1, 0x48)) {
            GS8(p1, 0x45) = -0x1F;
        } else {
            GS8(p1, 0x45) = -0x22;
        }
        *(W3 *)((u8 *)p1 + 0x43C) = *(W3 *)((u8 *)p1 + 0x54);
        return 1;
    }

    if (D_800E76D0 - *(s32 *)((s32)p1 + (IDX(p2) << 2) + 0x784) < 0x3c) return 0;

    pa = (f32 *)((u8 *)p1 + 0x48);
    vec[0] = D_800F0B20[0] - pa[0];
    vec[1] = D_800F0B20[1] - pa[1];
    vec[2] = D_800F0B20[2] - pa[2];
    q = (f32 *)((u8 *)p2 + 0x48);
    func_80013310(vec, (f32 *)((u8 *)p1 + 0xA0), o1);
    if (-0.2f < o1[2] && o1[2] < 0.2f) return 0;

    vec[0] = D_800F0B20[0] - q[0];
    vec[1] = D_800F0B20[1] - q[1];
    vec[2] = D_800F0B20[2] - q[2];
    func_80013310(vec, (f32 *)((u8 *)q + 0x58), o2);
    if (-0.2f < o2[2] && o2[2] < 0.2f) return 0;

    if (o2[2] > 0.0f && o1[2] < 0.0f) {
        if (GU8(p2, 0x6E8) == 0) {
            if (GS32(p2, 0x5B0) & 0x20) return 0;
            if ((GS32(p2, 0x5B0) & 0x20010) == 0) return 0;
        }
        GU8(p1, 0x7A8) = 1;
        GS32(p1, 0x648) = 0;
        if (GU8(p1, 0x6E8) == 0 || GU8(p2, 0x6E8) == 0) func_8004DFB4(IDX(p1), IDX(p2));
        if (o1[0] - o2[0] > 0.0f) {
            GS8(p2, 0x45) = -0x1F;
        } else {
            GS8(p2, 0x45) = -0x22;
        }
        {
            f64 *d = (f64 *)((u8 *)p2 + 0x43C);
            f64 *sp = (f64 *)((u8 *)p2 + 0x54);
            __builtin_memcpy(d, sp, 12);
        }
        return 1;
    }
    if (o1[2] > 0.0f && o2[2] < 0.0f) {
        if (GU8(p1, 0x6E8) == 0) {
            if (GS32(p1, 0x5B0) & 0x20) return 0;
            if ((GS32(p1, 0x5B0) & 0x20010) == 0) return 0;
        }
        GU8(p2, 0x7A8) = 1;
        GS32(p2, 0x648) = 0;
        if (GU8(p1, 0x6E8) == 0 || GU8(p2, 0x6E8) == 0) func_8004DFB4(IDX(p2), IDX(p1));
        if (o2[0] - o1[0] > 0.0f) {
            GS8(p1, 0x45) = -0x1F;
        } else {
            GS8(p1, 0x45) = -0x22;
        }
        *(W3 *)((u8 *)p1 + 0x43C) = *(W3 *)((u8 *)p1 + 0x54);
        return 2;
    }
    return 0;
}
