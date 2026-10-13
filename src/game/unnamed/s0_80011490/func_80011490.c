#include "ctx.h"

typedef struct MtxS {
    s16 i[4][4];
    u16 f[4][4];
} MtxS;

extern s16 D_800C2C60[];

#define SETM(r, c, x) { v = (x); m->i[r][c] = v >> 16; m->f[r][c] = v; }
#define SETS(r, c, x) { v = (x); v = v * scale / 256; m->i[r][c] = v >> 16; m->f[r][c] = v; }

void func_80011490(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale)
{
    s32 cx, cy, sx, sy, sz, cz;
    s32 cycx, cysx;
    s32 v;

    rx >>= 4;
    ry >>= 4;
    rz >>= 4;
    m->i[0][3] = m->i[1][3] = m->i[2][3] = 0;
    m->i[3][3] = 1;
    m->f[0][3] = m->f[1][3] = m->f[2][3] = 0;
    m->f[3][3] = 0;
    m->i[3][0] = tx >> 4;
    m->i[3][1] = ty >> 4;
    m->i[3][2] = tz >> 4;
    m->f[3][0] = tx << 12;
    m->f[3][1] = ty << 12;
    m->f[3][2] = tz << 12;
    cx = D_800C2C60[(rx - 0x400) & 0xFFF];
    cy = D_800C2C60[(ry - 0x400) & 0xFFF];
    sx = D_800C2C60[rx & 0xFFF];
    sy = D_800C2C60[ry & 0xFFF];
    sz = D_800C2C60[rz & 0xFFF];
    cz = D_800C2C60[(rz - 0x400) & 0xFFF];
    cycx = cy * cx / 0x4000;
    cysx = cy * sx / 0x4000;
    if (scale == 0x100) {
        SETM(0, 0, (sy * sz) >> 12);
        SETM(0, 1, cy << 2);
        SETM(0, 2, (sy * cz) >> 12);
        SETM(1, 0, (-sz * cysx + -cx * cz) >> 12);
        SETM(1, 1, (sx * sy) >> 12);
        SETM(1, 2, (-cz * cysx + cx * sz) >> 12);
        SETM(2, 0, (sz * cycx + -sx * cz) >> 12);
        SETM(2, 1, (-cx * sy) >> 12);
        SETM(2, 2, (cz * cycx + sx * sz) >> 12);
    } else {
        SETS(0, 0, (sy * sz) >> 12);
        SETS(0, 1, cy << 2);
        SETS(0, 2, (sy * cz) >> 12);
        SETS(1, 0, (-sz * cysx + -cx * cz) >> 12);
        SETS(1, 1, (sx * sy) >> 12);
        SETS(1, 2, (-cz * cysx + cx * sz) >> 12);
        SETS(2, 0, (sz * cycx + -sx * cz) >> 12);
        SETS(2, 1, (-cx * sy) >> 12);
        SETS(2, 2, (cz * cycx + sx * sz) >> 12);
    }
}
