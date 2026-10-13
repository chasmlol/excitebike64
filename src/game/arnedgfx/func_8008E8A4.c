#include "ctx.h"

typedef struct {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    Vp vp[2];           /* 0x08 */
    Mtx mtx[2];         /* 0x28 */
    s32 fA8;            /* 0xA8 */
    u16 persp;          /* 0xAC */
    u8 fAE;             /* 0xAE */
    u8 pad[0x710 - 0xAF];
} Cam;

extern Cam *D_800F4FF0;
extern volatile s32 D_800F13F0;
extern s16 D_800C2C60[];
extern Mtx D_800C25F8;
extern f32 D_800E1930;

void func_80012D30(Mtx *p);
void func_800AE540(Mtx *m, u16 *pn, f32 fovy, f32 aspect, f32 n, f32 f, f32 s);
void func_800AE898(Mtx *m, f32 a, f32 b, f32 c);
void func_80011490(Mtx *m, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);
void func_800120A4(Mtx *a, Mtx *b, Mtx *c);

void func_8008E8A4(s32 n, s32 id, s32 x, s32 y, s32 w, s32 h, s32 p7, s32 p8,
                   s32 p9, s32 p10, s32 p11, s32 p12)
{
    Mtx persp;
    Mtx rot;
    Mtx m1;
    Mtx m2;
    Mtx res;
    Cam *c; Cam *base;
    f32 t, t2, asp;

    base = D_800F4FF0;
    c = (Cam *)(id * sizeof(Cam) + (u32)base);
    c->fA8 = 0;
    c->x = x;
    c->y = y;
    c->vp[n].vp.vscale[0] = w * 2;
    c->vp[n].vp.vscale[1] = h * 2;
    c->vp[n].vp.vscale[2] = 0x1FF;
    c->vp[n].vp.vtrans[0] = (x + w / 2) * 4;
    c->vp[n].vp.vtrans[1] = (y + h / 2) * 4;
    c->vp[n].vp.vtrans[2] = 0x1FF;
    c->fAE = p12;
    t = (f32)D_800F13F0 * 0.5f * (f32)D_800C2C60[0] / (f32)D_800C2C60[0xC00];
    t2 = t * (f32)D_800F13F0 * 0.00390625f;
    asp = 320.0f / (f32)D_800F13F0;
    *(volatile f32 *)&D_800E1930 = t;
    D_800E1930 = t2;
    func_800AE540(&persp, &D_800F4FF0[id].persp, 33.0f, asp, 50.0f, 16384.0f, 1.0f);
    func_800AE898(&rot, (f32)p10 * 360.0f * 1.5258789e-05f, -(f32)p11 * 360.0f * 1.5258789e-05f, 0.0f);
    func_80012D30(&rot);
    func_80011490(&m1, 0, 0, 0, -p7, -p8, -p9, 0x100);
    func_800120A4(&m1, &rot, &m2);
    func_800120A4(&m2, &D_800C25F8, &m1);
    func_800120A4(&m1, &persp, &res);
    D_800F4FF0[id].mtx[n] = res;
}
