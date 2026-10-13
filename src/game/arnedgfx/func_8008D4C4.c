#include "ctx.h"

extern s32 D_800C23F0;
extern s16 D_800F5580;
extern s16 D_800F5582;
extern f32 D_800E1930;
extern s32 D_800F558C;
extern s32 D_800F5594;
extern s32 D_800F559C;
extern volatile s32 D_800F13F0;
extern s16 D_800C2C60[];
extern u16 D_800E1B44;
extern u8 D_800F4F08[][0x40];
extern u8 D_800C25F8[];

void func_800AE540(void *a, u16 *b, f32 c, f32 d, f32 e, f32 f, f32 g);
void func_800AE898(void *a, f32 b, f32 c, f32 d);
void func_80012D30(u8 *p);
void func_80011490(void *a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);
void func_800120A4(void *a, void *b, void *c);

typedef struct { u32 w[4]; } Blk16;
typedef struct { Blk16 r[4]; } M;

void func_8008D4C4(s32 idx, s32 x, s32 y, s32 z, s16 rx, s16 ry)
{
    M persp;
    M rot;
    M a;
    M b;
    M c;
    f32 q;
    s32 f1, f2, f3;

    D_800F558C = x;
    D_800C23F0 = 0x7FBC;
    D_800F5594 = y;
    D_800F559C = z;
    D_800F5580 = rx;
    D_800F5582 = ry;
    f1 = D_800F13F0;
    f2 = D_800F13F0;
    f3 = D_800F13F0;
    q = (f32)f1 * 0.5f * D_800C2C60[0] / D_800C2C60[0xC00];
    *(volatile f32 *)&D_800E1930 = q;
    D_800E1930 = q * f2 * 0.00390625f;
    func_800AE540(&persp, &D_800E1B44, 33.0f, 320.0f / f3, 50.0f, D_800C23F0, 1.0f);
    func_800AE898(&rot, D_800F5582 * 360.0f * 1.5258789e-05f, -(f32)D_800F5580 * 360.0f * 1.5258789e-05f, 0.0f);
    func_80012D30((u8 *)&rot);
    func_80011490(&a, 0, 0, 0, -D_800F558C, -D_800F5594, -D_800F559C, 0x100);
    func_800120A4(&a, &rot, &b);
    func_800120A4(&b, D_800C25F8, &a);
    func_800120A4(&a, &persp, &c);
    *(M *)D_800F4F08[idx] = c;
}
