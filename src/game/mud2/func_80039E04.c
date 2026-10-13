#include "ctx.h"

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    u8 pad[0xA0];
} Rec160;

extern s32 D_800C9990;
extern u16 D_800E7FC8;
extern s32 D_800E7FF0;
extern Vec3i D_800E8090;
extern Vec3i D_800E9218;
extern s32 D_800E8000[];
extern s32 D_800E9268;
extern s16 D_800C2C60[];
extern s32 D_800E3710;
extern s32 D_800C2358;
extern s32 D_800E1B70;
extern Vtx *D_800E9288[];
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern s32 scr_800F13F0;
extern s32 D_800E8508;
extern Rec160 *D_800EFD64;
extern Rec160 D_800CB210[];
extern s32 D_800E84F8;
extern u8 D_800C9994[];
extern s32 D_800E19CC;

s32 func_8000EFE8(void);
Gfx *func_80002418(Gfx *dl, s32 p2, s32 p3);
void func_80036374(s32 type, u8 b, Vec3f *pos, Vec3i *vel, u8 *col);
s32 func_8002E920(s32 x, s32 y, s32 z);

#define SIN(a) D_800C2C60[((a) >> 4) & 0xFFF]
#define COS(a) D_800C2C60[(((a) >> 4) - 0x400) & 0xFFF]

Gfx *func_80039E04(Gfx *gdl, s32 mtx, s32 buf) {
    s32 dx, dz, a, s, c;
    s32 fwd, side, turn;
    s32 i, j, k, row;
    s32 h, w, y, r, q, t, u;
    s32 rad, off, x, z;
    Vtx *src, *dst, *end;
    Vec3f pos;
    Vec3i vel;
    volatile s32 vv;

    if (D_800C9990 == 0 || (!(D_800E7FC8 & 1) && (D_800E7FF0 & 0x40))) {
        return gdl;
    }
    gdl = func_80002418(gdl, mtx, 0);

    t = D_800E8090.x - D_800E9218.x;
    r = D_800E8090.z - D_800E9218.z;
    fwd = (((t) * SIN(D_800E8000[0])) >> 14) + (((r) * COS(D_800E8000[0])) >> 14);
    fwd = -fwd;
    side = (((r) * SIN(D_800E8000[0])) >> 14) - (((t) * COS(D_800E8000[0])) >> 14);
    side *= 2;
    turn = D_800E8000[0] - D_800E9268;
    if (fwd > 0x200) fwd = 0x200;
    if (fwd < -0x200) fwd = -0x200;
    if (side > 0x200) side = 0x200;
    if (side < -0x200) side = -0x200;
    if (turn > 0x200) turn = 0x200;
    if (turn < -0x200) turn = -0x200;

    fwd = turn;
    if (D_800E3710 == 0 && (D_800C2358 == 0 || D_800E1B70 != 0x100)) {
        for (i = 0; i < 64; i++) {
            x = func_8000EFE8() * 2;
            x -= 0x100;
            x = x * D_800F13EC / 512;
            row = i >> 3;
            r = row * 32;
            h = r * 10 / 256 + 5;
            h = h * D_800F13F0 / 240;
            w = ((r >> 1) + 0x80) * D_800F13F0;
            if (w < 0) w += 255;
            w >>= 8;
            r = func_8000EFE8() * w;
            if (r < 0) r += 255;
            y = r >> 8;
            y = D_800F13F0 / 2 - y;
            q = -(x * 32) / D_800F13EC;
            w = 0xFBFF - (row << 8);
            D_800E9288[buf][i * 4].v.ob[0] = x;
            D_800E9288[buf][i * 4].v.ob[1] = y;
            D_800E9288[buf][i * 4].v.ob[2] = w;
            t = (h * (-fwd * 16 / 500)) >> 4;
            t += (q * side) >> 9;
            D_800E9288[buf][i * 4 + 1].v.ob[0] = x + t;
            D_800E9288[buf][i * 4 + 1].v.ob[1] = y + h;
            D_800E9288[buf][i * 4 + 2].v.ob[0] = x + 2;
            D_800E9288[buf][i * 4 + 2].v.ob[1] = y;
            u = t + 2 + (((u32)h & 1) >> 1);
            D_800E9288[buf][i * 4 + 3].v.ob[0] = x + u;
            D_800E9288[buf][i * 4 + 3].v.ob[1] = y + h;
        }
    } else {
        src = D_800E9288[buf ^ 1];
        dst = D_800E9288[buf];
        end = src + 256;
        do {
            *dst++ = *src++;
        } while (src != end);
    }

    gDPPipeSync(gdl++);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xE200001C;
        g->words.w1 = 0x404A50;
    }
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xDA380007;
        g->words.w1 = mtx + 0x300;
    }
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    for (i = 0; i < 64; i += 8) {
        gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, i * 5 * 4 / 64 + 16);
        gDPSetPrimDepth(gdl++, D_800E9288[buf][i * 4].v.ob[2], 0);
        gSPVertex(gdl++, &D_800E9288[buf][i * 4], 32, 0);
        for (j = 0; j < 8; j++) {
            gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 3, 0, j * 4, j * 4 + 3, j * 4 + 2, 0);
        }
    }
    gDPPipeSync(gdl++);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xDA380007;
        g->words.w1 = mtx;
    }
    D_800E9218 = D_800E8090;
    D_800E9268 = D_800E8000[0];
    if (D_800E3710 == 0) {
        w = (func_8000EFE8() << 7) + 8000;
        x = func_8000EFE8() * w / 256 - w / 2;
        t = ((x * SIN(D_800E8000[0])) >> 14) - ((w * COS(D_800E8000[0])) >> 14);
        r = ((w * SIN(D_800E8000[0])) >> 14) + ((x * COS(D_800E8000[0])) >> 14);
        x = t + D_800E8090.x;
        w = r + D_800E8090.z;
        y = func_8002E920(x, 0x8000, w);
        r = (D_800E84F8 >> 4) & 0xF;
        if (D_800E8508 != 0 && (D_800EFD64 - D_800CB210 != 11 || r != 6)) {
            vel.x = 0;
            vel.y = 0;
            vel.z = 0;
            pos.x = (f32)x / 1000.0f;
            pos.y = (f32)(y + 10) / 1000.0f;
            pos.z = (f32)w / 1000.0f;
            func_80036374(3, 0, &pos, &vel, D_800C9994);
        }
    }
    gSPSetGeometryMode(gdl++, D_800E19CC);
    return gdl;
}
