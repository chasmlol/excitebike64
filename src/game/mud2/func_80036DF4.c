#include "ctx.h"

typedef struct {
    s32 a, b, c;
} S12;

extern u8 D_800E37C0[];
typedef struct {
    u8 pad[0x28];
    u32 flags;
    u8 rest[0x14];
} Ent64;
extern Ent64 D_800C9080[];
extern s32 D_800E18A0[];
extern s32 D_800E76D0;
extern s32 D_800E92A8[];
extern s16 D_800C2C60[];
extern u8 D_800C9994[];

s32 func_8002733C(s32 a, s32 b, s32 *c);
void func_8001327C(f32 *v, f32 *m, f32 *o);
void func_8002DA48(s32 i, s32 v);
void func_80036374(s32 a, s32 b, f32 *c, f32 *d, u8 *e);
void func_8002845C(s32 a, s32 b, s32 c, s32 d);
void func_80010994(s32 a, s32 b, s32 c);

void func_80036DF4(u8 *p)
{
    volatile s32 pad[4];
    f32 pt[3];
    f32 A[3];
    f32 B[3];
    f32 C[3];
    s32 loc[2];
    s32 idx;
    s32 mv;
    f32 lim;
    f32 t;

    if (p[0x7a5] != 0) goto done;
    idx = ((s32)p - (s32)D_800E37C0) * -0x41041041 >> 5;
    mv = *(s32 *)(p + 0x780);
    if (!(1.0f < *(f32 *)(p + 0x420))) goto done;
    if (!(D_800C9080[mv].flags & 8)) goto done;

    *(S12 *)pt = *(S12 *)(p + 0x48);
    if (!func_8002733C((s32)(pt[0] * 1000.0f), (s32)(pt[2] * 1000.0f), loc)) goto done;

    lim = (f32)loc[0] / 1000.0f;
    A[0] = 0.0f;
    A[1] = 0.8f;
    A[2] = 1.0f;
    func_8001327C(A, (f32 *)(p + 0xA0), B);
    B[0] = B[0] + *(f32 *)(p + 0x48);
    B[1] = B[1] + *(f32 *)(p + 0x4C);
    B[2] = B[2] + *(f32 *)(p + 0x50);
    if (B[1] < lim) goto done;

    A[0] = 0.0f;
    A[1] = -0.3f;
    A[2] = 1.0f;
    func_8001327C(A, (f32 *)(p + 0xA0), C);
    C[0] = C[0] + *(f32 *)(p + 0x48);
    C[1] = C[1] + *(f32 *)(p + 0x4C);
    C[2] = C[2] + *(f32 *)(p + 0x50);
    if (lim < C[1]) {
        A[0] = 0.0f;
        A[1] = -0.3f;
        A[2] = -1.0f;
        func_8001327C(A, (f32 *)(p + 0xA0), B);
        B[0] = B[0] + *(f32 *)(p + 0x48);
        B[1] = B[1] + *(f32 *)(p + 0x4C);
        B[2] = B[2] + *(f32 *)(p + 0x50);
        if (lim < B[1]) goto done;
    }

    t = (lim - C[1]) / (B[1] - C[1]);
    pt[0] = C[0] + t * (B[0] - C[0]);
    pt[1] = lim;
    pt[2] = C[2] + t * (B[2] - C[2]);

    if (D_800E18A0[idx] != 0 && (D_800E76D0 & 3) == 0) {
        A[0] = (f32)*(s16 *)((u8 *)D_800C2C60 + ((*(s32 *)(p + 0x20) >> 3) & 0x1FFE)) / 655360.0f;
        A[1] = 0.0f;
        A[2] = (f32)*(s16 *)((u8 *)D_800C2C60 + ((((*(s32 *)(p + 0x20) >> 4) - 0x400) & 0xFFF) << 1)) / 655360.0f;
        func_80036374(3, mv, pt, A, D_800C9994);
        A[0] = -(f32)*(s16 *)((u8 *)D_800C2C60 + ((*(s32 *)(p + 0x20) >> 3) & 0x1FFE)) / 655360.0f;
        A[1] = 0.0f;
        A[2] = -(f32)*(s16 *)((u8 *)D_800C2C60 + ((((*(s32 *)(p + 0x20) >> 4) - 0x400) & 0xFFF) << 1)) / 655360.0f;
        func_80036374(3, mv, pt, A, D_800C9994);
    }

    if ((D_800C9080[mv].flags & 0x180) != 0) {
        if (D_800E18A0[idx] != 0) {
            func_8002845C(idx, (s32)(pt[0] * 1000.0f), loc[0], (s32)(pt[2] * 1000.0f));
        }
        if (*(s8 *)(p + 0x45) == 0) {
            *(s8 *)(p + 0x45) = 0x1c;
            func_8002DA48(idx, 1);
        }
    } else {
        if ((s32)(D_800E76D0 - D_800E92A8[idx]) > 0x78) {
            func_80010994(3, idx, 0xff);
            D_800E92A8[idx] = D_800E76D0;
        }
    }

done:
    return;
}
