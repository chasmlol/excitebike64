#include "ctx.h"

typedef struct {
    u8 unk0[0x1760];
    f32 mtx[4][4];
} Camera;

extern Vp_t D_800F4FD8;
extern Camera *D_800CEA40;
extern s32 D_800C23F0;

s32 func_80097658(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, s32 *sz, s32 *sw) {
    f32 hx;
    f32 hy;
    f32 tx;
    f32 ty;
    f32 px;
    f32 py;
    f32 pz;
    f32 pw;

    hx = D_800F4FD8.vscale[0] * 0.25f;
    hy = D_800F4FD8.vscale[1] * 0.25f;
    x *= 0.0625f;
    y *= 0.0625f;
    z *= 0.0625f;
    px = D_800CEA40->mtx[0][0] * x + D_800CEA40->mtx[1][0] * y + D_800CEA40->mtx[2][0] * z + D_800CEA40->mtx[3][0];
    py = D_800CEA40->mtx[0][1] * x + D_800CEA40->mtx[1][1] * y + D_800CEA40->mtx[2][1] * z + D_800CEA40->mtx[3][1];
    pz = D_800CEA40->mtx[0][2] * x + D_800CEA40->mtx[1][2] * y + D_800CEA40->mtx[2][2] * z + D_800CEA40->mtx[3][2];
    pw = D_800CEA40->mtx[0][3] * x + D_800CEA40->mtx[1][3] * y + D_800CEA40->mtx[2][3] * z + D_800CEA40->mtx[3][3];
    tx = D_800F4FD8.vtrans[0] * 0.25f;
    ty = D_800F4FD8.vtrans[1] * 0.25f;
    if (pw == 0.0f) {
        return 1;
    }
    px /= pw;
    py /= pw;
    pz /= pw;
    if (pz <= -1.0f || pz >= 1.0f) {
        return 1;
    }
    {
        f32 rx, ry, rz;
        rx = px * hx + tx;
        ry = py * -hy + ty;
        rz = pz * (D_800C23F0 - 50.0f) + 50.0f;
        *sx = rx;
        *sy = ry;
        *sz = rz;
    }
    if (sw != NULL) {
        *sw = pw;
    }
    return 0;
}
