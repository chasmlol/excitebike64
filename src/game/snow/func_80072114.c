#include "ctx.h"

typedef struct {
    s32 x, y, z;
    s32 pad[2];
    s32 f14;
    s32 f18;
} Part;

typedef struct {
    s32 x, y, z;
} Pos;

extern Part *D_800CCFFC;
extern s32 D_800E3710;
extern s32 D_800C2358;
extern s32 D_800E1B70;
extern Pos D_800E8090;
extern Pos D_800F2190;
extern s32 D_800E8000;
extern s32 D_800F21A0;
extern s16 D_800C2C60[];
extern s32 D_800CCFF0;
extern s32 D_800CCFF4;
extern s32 D_800CCFF8;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern s32 D_800E7FF0;
extern Vtx D_800CD018[];

u32 func_8000EFE8(void);

static inline void respawn(Part *p, s32 k) {
    if (D_800E7FF0 & 0x40) {
        p->f18 = 0;
    } else {
        p->f18 = 1;
    }
    p->z = func_8000EFE8() * 192 + 0x4000;
    p->x = ((s32)func_8000EFE8() - 128) * p->z / 128;
    if (func_8000EFE8() & 1) {
        p->y = ((s32)func_8000EFE8() - 128) * p->z / 256;
    } else {
        p->y = p->z;
    }
    p->f14 = func_8000EFE8() + 0x100;
}

void func_80072114(void) {
    s32 dx, dz, ang, s, c;
    s32 mx, mz, rot, dy, ny;
    s32 i;
    s32 x, y, z, sx, sy, hw, hh;
    s32 a, t3, t2;
    s32 w, h;

    if (D_800CCFFC == NULL || D_800E3710 != 0 || (D_800C2358 != 0 && D_800E1B70 == 0x100)) {
        return;
    }
    dz = D_800E8090.z - D_800F2190.z;
    ang = -D_800E8000 >> 4;
    s = D_800C2C60[ang & 0xFFF];
    c = D_800C2C60[(ang - 0x400) & 0xFFF];
    dx = D_800E8090.x - D_800F2190.x;
    D_800CCFF0 = -1;
    dy = D_800E8090.y - D_800F2190.y;
    mz = (s * dz + c * dx) >> 14;
    mx = (c * dz - s * dx) >> 14;
    rot = (D_800E8000 - D_800F21A0) / 64;
    i = 0;
    ny = -dy * 3;
    D_800F2190.x = D_800E8090.x;
    D_800F2190.y = D_800E8090.y;
    D_800F2190.z = D_800E8090.z;
    D_800F21A0 = D_800E8000;
    h = D_800F13F0;
    for (; i < 128; i++) {
        { s32 t = D_800CCFFC[i].z; D_800CCFFC[i].z = t - mz * 3; }
        D_800CCFFC[i].y += ny - D_800CCFFC[i].f14;
        { s32 t = D_800CCFFC[i].x; D_800CCFFC[i].x = t + mx * 3; }
        D_800CCFFC[i].x += rot * D_800CCFFC[i].z / 256;
        z = D_800CCFFC[i].z;
        if (z < 0x1000) {
            if (D_800CCFFC[i].x < z && -z < D_800CCFFC[i].x) {
                if (D_800CCFFC[i].y < z && -z < D_800CCFFC[i].y && z > 0 && D_800CCFFC[i].f18 != 0) {
                    hw = D_800F13EC / 2;
                    sx = hw + D_800CCFFC[i].x * hw / z;
                    hh = D_800F13F0 / 2;
                    sy = hh - D_800CCFFC[i].y * hh / z;
                    if (sx >= 20 && sx < D_800F13EC - 20 && sy >= 20 && sy < D_800F13F0 - 20) {
                        D_800CCFF0 = sx;
                        D_800CCFF4 = sy;
                    }
                }
            }
            respawn(&D_800CCFFC[i], i);
        } else if (D_800CCFFC[i].y < -z) {
            respawn(&D_800CCFFC[i], i);
        } else if (D_800CCFFC[i].x < -z) {
            respawn(&D_800CCFFC[i], i);
        } else if (z < D_800CCFFC[i].x) {
            respawn(&D_800CCFFC[i], i);
        } else if (z > 0x10000) {
            respawn(&D_800CCFFC[i], i);
        }
    }
    if (D_800CCFF8 != 0) {
        D_800CCFF8--;
    }
    func_8000EFE8();
    func_8000EFE8();
    if (D_800CCFF8 == 0 && D_800CCFF0 != -1) {
        i = func_8000EFE8() << 5;
        t2 = D_800C2C60[(i >> 4) & 0xFFF] >> 10;
        t3 = D_800C2C60[((i >> 4) - 0x400) & 0xFFF] >> 10;
        x = D_800CCFF0;
        y = D_800CCFF4;
        D_800CCFF8 = 60;
        D_800CD018[0].v.ob[0] = (x + t3 - 160) * D_800F13EC / 320;
        D_800CD018[0].v.ob[1] = (y + t2 - 120) * D_800F13F0 / 240;
        D_800CD018[1].v.ob[0] = (x + t2 - 160) * D_800F13EC / 320;
        D_800CD018[1].v.ob[1] = (y - t3 - 120) * D_800F13F0 / 240;
        D_800CD018[2].v.ob[0] = (x - t3 - 160) * D_800F13EC / 320;
        D_800CD018[2].v.ob[1] = (y - t2 - 120) * D_800F13F0 / 240;
        D_800CD018[3].v.ob[0] = (x - t2 - 160) * D_800F13EC / 320;
        D_800CD018[3].v.ob[1] = (y + t3 - 120) * D_800F13F0 / 240;
    }
}
