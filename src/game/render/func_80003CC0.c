#include "ctx.h"

typedef struct {
    Mtx m0[4];  /* 0x000 */
    Mtx m1[4];  /* 0x100 */
    Mtx m2[4];  /* 0x200 */
    Mtx m3[4];  /* 0x300 */
    Mtx m4;     /* 0x400 */
    Gfx *glist; /* 0x440 */
} Dyn;

typedef struct {
    u8 pad0[0x34];
    s32 v34[3];
    Mtx m40;
} Bone; /* 0x80 */

typedef struct {
    Bone b[10];
} Skel; /* 0x500 */

typedef struct {
    s32 x, y, z;       /* 0x00 */
    s32 x2, y2, z2;    /* 0x0C */
    u8 pad18[0x2C];
    u8 b44;            /* 0x44 */
    u8 pad45[3];
    f32 f48, f4C, f50; /* 0x48 */
    u8 pad54[0x556];
    s16 h5AA;          /* 0x5AA */
    u8 pad5AC[0xAC];
    Skel *skel;        /* 0x658 */
    s32 p65C;          /* 0x65C */
    u8 pad660[0x88];
    u8 b6E8;           /* 0x6E8 */
    u8 pad6E9[0xBC];
    u8 b7A5;           /* 0x7A5 */
    u8 pad7A6[0x3A];
} Player; /* 0x7E0 */

typedef struct {
    u8 pad0[0x234];
    Mtx *p234[2]; /* 0x234 */
    Mtx *p23C[2]; /* 0x23C */
    s32 pad244;
    Mtx a[8];     /* 0x248 */
    Mtx b[8];     /* 0x448 */
    Mtx c[8];     /* 0x648 */
    Mtx d[8];     /* 0x848 */
    u8 padA48[0x38];
} PG; /* 0xA80 */

typedef struct {
    u8 pad0[0x2C];
    s32 type;  /* 0x2C */
    u8 pad30[0x20];
    s32 f50;   /* 0x50 */
    u8 pad54[0x4C];
} Track; /* 0xA0 */

typedef struct {
    s32 flags; /* 0x00 */
    u8 pad4[0x38];
    s32 f3C;   /* 0x3C */
    u8 pad40[0xC];
    s32 mode;  /* 0x4C */
    u8 pad50[4];
    s32 f54;   /* 0x54 */
    s16 h58;   /* 0x58 */
} Game;

typedef struct {
    s32 x, y, w, h;
} Rect;

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    s16 s0;
    u8 c2;
} Fade;

typedef struct {
    s32 f0;
    s32 f4;
} Pal;

extern s32 D_800E1A9C;
extern s32 D_800E1A98;
extern s32 D_800C2280;
extern s32 D_800C238C[8];
extern Game D_800EFD70;
extern s8 D_800E3BBE;
extern s32 D_800C8CE8;
extern Track *D_800EFD64;
extern Track D_800CB210[];
extern s32 D_800C2380;
extern s32 D_800DC0B4;
extern s32 D_800DC0AC;
extern s32 D_800DC108;
extern s32 D_800C2594;
extern s32 D_800C2598;
extern s32 D_800E8508;
extern s32 D_800E8958;
extern s32 D_800E84F8;
extern s32 D_800E15E0[8];
extern s32 D_800E1B50[8];
extern s32 D_800E1818[24];
extern Player D_800E37C0[8];
extern s32 D_800C23AC;
extern s32 D_800C23F0;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern s16 D_800C2C60[];
extern f32 D_800E1930;
extern s32 D_800C2384;
extern s32 D_800C2388;
extern s32 D_800CCCA8;
extern u16 D_800E1B44;
extern Mtx D_800C25F8;
extern Mtx D_800C2638;
extern Mtx D_800E1938[];
extern s32 D_800E30A8;
extern f32 D_800E1AA0[4];
extern f32 D_800E1AB0[4];
extern Rect D_800E1718[4][4];
extern Mtx *D_800E19C4;
extern s32 D_800E7978[4];
extern s32 D_800E8000[4];
extern s32 D_800E78B8[4];
extern Vec3i D_800E8090[4];
extern s32 D_800C2364;
extern u32 D_800E18A0[8];
extern u32 D_800E36F8;
extern s32 D_800E3710;
extern Fade D_800E3730;
extern s32 D_800E374C;
extern Gfx D_800C2A38[];
extern Gfx D_800C2A00[];
extern Vp D_800C25E8;
extern u32 D_800C29E0;
extern u32 D_800C29D8[];
extern s32 D_800E19CC;
extern s32 D_800C2350;
extern s32 D_800C2358;
extern s32 D_800C235C;
extern s32 D_800C2360;
extern s32 D_800E1898;
extern s32 D_800DC148;
extern s32 D_800C98B0;
extern Pal *D_800DC144;
extern u8 D_800CCD63;
extern s32 D_800E19C8;
extern s32 D_800DC0B8[8];
extern Skel *D_800E1878;
extern s32 D_800C25CC[6];
extern PG D_800DC150[8];
extern Mtx D_800C2678;
extern u8 D_800A9D00[];
extern u8 D_800D15C0[];
extern u8 D_800AB0A0[];
extern u8 D_800D19E0[];
extern s32 D_800EE140;
extern s32 D_800E76D0;
extern s32 D_800CCD64;
extern u8 D_800E76CC;
extern s32 D_800E19B8;

void func_8000B028(void);
void func_8006A0AC(void);
void func_8002482C(s32);
void func_80024F24(s32);
void func_8003F8FC(s32);
void func_8002E920(s32, s32, s32);
void func_8002EF1C(s32, s32, s32 *);
void func_800AE240(Mtx *, f32, f32, f32, f32, f32, f32, f32);
void func_800AE540(Mtx *, u16 *, f32, f32, f32, f32, f32);
void func_800120A4(void *, void *, void *);
void func_800AE898(void *, f32, f32, f32);
void func_800B3240(void *, void *, void *);
void func_80012D30(void *);
void func_80011490(void *, s32, s32, s32, s32, s32, s32, s32);
s32 func_80002554(s32, s32, s32, s32, s32, Dyn *, s32);
Gfx *func_80002D20(Gfx *, s32, Dyn *);
Gfx *func_800035AC(Gfx *, s32, Dyn *);
s32 func_8003AAE0(Gfx *, Dyn *, s32);
Gfx *func_8003AC18(s32, s32, Dyn *);
void func_80033FF0(void);
void func_8003240C(Dyn *, s32, s32);
Gfx *func_80003AC4(Gfx *);
Gfx *func_80035178(Gfx *, Dyn *, Pal *);
Gfx *func_80002418(Gfx *, Dyn *, s32, s32);
Gfx *func_80013D70(s32, s32, Pal **, Gfx *);
Gfx *func_8003371C(Gfx *, Dyn *);
void func_8000ED10(u8 *, u8, s32);
void func_8003EAC4(Skel *, s32);
void func_800139A8(void *, void *, void *);
void func_8003D4BC(s32, s32);
void func_80013ACC(s32, Mtx *, s32);
Gfx *func_80007010(Gfx *, s32, Dyn *, s32);
Gfx *func_80023C0C(Gfx *, Dyn *, s32, s32);
Gfx *func_800258F4(Gfx *, Dyn *, s32, s32);
Gfx *func_80040190(Gfx *, Dyn *, s32, s32);
Gfx *func_80062DD4(Gfx *, Dyn *, s32, s32);
Gfx *func_8000B9CC(Gfx *, s32, Dyn *, void *, void *);
Gfx *func_80008B1C(Gfx *, s32, Dyn *);
Gfx *func_80022FD8(Gfx *, Dyn *, s32, s32);
Gfx *func_8002803C(Gfx *, Dyn *, s32, s32);
Gfx *func_8003FD4C(Gfx *, Dyn *, s32, s32);
Gfx *func_80062DA0(Gfx *, Dyn *, s32, s32);
Gfx *func_8000C0E0(Gfx *, Dyn *, s32);
Gfx *func_8006C510(Gfx *, Dyn *, s32);
Gfx *func_8006CD3C(Gfx *, Dyn *, s32);
Gfx *func_8006C794(Gfx *, Dyn *, s32, s32);
void func_8003B7E0(s32);
Gfx *func_8003B0AC(s32, Dyn *, Gfx *, s32);
Gfx *func_8005E090(Gfx *, s32, Dyn *, s32);
Gfx *func_80026120(Gfx *, Dyn *, s32, s32);
Gfx *func_800238F0(Gfx *, Dyn *, s32, s32);
Gfx *func_800408A4(Gfx *, Dyn *, s32, s32);
Gfx *func_80037414(Gfx *, s32, Dyn *);
Gfx *func_80008DE0(Gfx *, Dyn *);
Gfx *func_80032854(Gfx *, Dyn *, s32);
Gfx *func_80039E04(Gfx *, Dyn *, s32);
Gfx *func_80071A7C(Gfx *, Dyn *, s32);
Gfx *func_80040B64(Gfx *, Dyn *, s32, s32);
Gfx *func_8006EB34(Gfx *, Dyn *, s32);
Gfx *func_80068910(Gfx *, Dyn *, s32);
Gfx *func_80059614(Gfx *, Dyn *, s32, s32);
Gfx *func_8006D664(Gfx *, Dyn *, s32, s32);
Gfx *func_8000B3C8(Gfx *);
Gfx *func_80072F70(Gfx *, Dyn *, s32);

#define GFX_LEFT() (0x1770 - (gdl - dyn->glist))
#define CHECK(n)                 \
    if (GFX_LEFT() < (n)) {      \
        *pgdl = gdl;             \
        return;                  \
    }

void func_80003CC0(Dyn *dyn, s32 fb, Gfx **pgdl) {
    Mtx mf;
    Mtx rotB;
    Mtx rotA;
    Mtx out;
    u8 unused[0x70];
    Mtx persp;
    Gfx *gdl;
    s32 i;
    s32 k;
    s32 a;
    s32 b;
    s32 n;
    s32 alpha;
    s32 r;
    s32 ang;
    s32 t;
    Mtx *pp;

    D_800E1A9C = -1;
    D_800E1A98 = 0;
    if (D_800C2280 & 0x100) {
        for (i = 0; i < 8; i++) {
            D_800C238C[i] = 0x80;
        }
    }
    if (D_800EFD70.f3C == 1 && D_800E3BBE != 0 && !(D_800EFD70.flags & 0x81) && D_800C8CE8 == 0 &&
        D_800EFD70.mode != 6 && D_800EFD70.mode != 7 && D_800EFD70.mode != 8 && D_800EFD70.mode != 9 &&
        D_800EFD70.mode != 13 && D_800EFD70.mode != 14 && D_800EFD64->type != 2) {
        D_800C2380 = D_800EFD70.f3C;
    } else {
        D_800C2380 = 0;
    }
    func_8000B028();
    if (D_800DC0B4 <= 0) {
        D_800DC0B4 = 1;
    }
    if (D_800DC0B4 > 8) {
        D_800DC0B4 = 8;
    }
    D_800DC0AC = 0;
    D_800DC108++;
    func_8006A0AC();

    for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
        if (D_800EFD64->type == 0) {
            func_8002482C(D_800C2598);
        } else if (D_800EFD64->type == 1) {
            func_80024F24(D_800C2598);
        } else if (D_800EFD64->type == 2) {
            func_8003F8FC(D_800C2598);
        }
    }

    for (i = 0; i < 8; i++) {
        if (D_800E37C0[i].b7A5 == 0 && ((i + D_800DC108) & 3) == 0 && D_800E37C0[i].h5AA != 9) {
            D_800E8508 = 0;
            func_8002E920(D_800E37C0[i].x, D_800E37C0[i].y, D_800E37C0[i].z);
            if (D_800E8508 != 0) {
                D_800E15E0[i] = D_800E8958;
                D_800E1B50[i] = D_800E84F8;
                if (D_800EFD64->type != 2) {
                    func_8002EF1C(D_800E37C0[i].x, D_800E37C0[i].z, &D_800E1818[i * 3]);
                } else {
                    D_800E1818[i * 3] = 0xFF;
                    D_800E1818[i * 3 + 1] = 0xFF;
                    D_800E1818[i * 3 + 2] = 0xFF;
                }
            }
        }
    }

    for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
        D_800C23AC = 0x21;
        func_800AE240(&dyn->m3[D_800C2598], -D_800F13EC / 2, D_800F13EC / 2, -D_800F13F0 / 2, D_800F13F0 / 2,
                      50.0f, D_800C23F0, 1.0f);
        for (i = 0; i < 2; i++) {
            ang = 0;
            D_800E1930 = D_800F13F0 * 0.5f * D_800C2C60[(ang >> 4) & 0xFFF] /
                         D_800C2C60[((ang >> 4) - 0x400) & 0xFFF];
            D_800E1930 = D_800E1930 * D_800F13F0 * 0.00390625f;
            k = (i != 0) * 2;
            if (D_800C2594 == 2 && D_800C2384 == 0) {
                if (D_800C2388 != 0) {
                    func_800AE540(&persp, &D_800E1B44, D_800C23AC, 256.0f / D_800F13F0, k + 50.0f,
                                  D_800C23F0 + k, 1.0f);
                } else {
                    func_800AE540(&persp, &D_800E1B44, D_800C23AC, 480.0f / D_800F13F0, k + 50.0f,
                                  D_800C23F0 + k, 1.0f);
                }
            } else if (D_800CCCA8 == 2) {
                func_800AE540(&persp, &D_800E1B44, D_800C23AC, 640.0f / D_800F13F0, k + 50.0f,
                              D_800C23F0 + k, 1.0f);
            } else {
                func_800AE540(&persp, &D_800E1B44, D_800C23AC, 320.0f / D_800F13F0, k + 50.0f,
                              D_800C23F0 + k, 1.0f);
            }
            pp = &persp;
            func_800120A4(&D_800C25F8, pp, &D_800E1938[D_800E30A8]);
            t = ((s16 *)pp)[0];
            t <<= 16;
            t |= ((u16 *)pp)[16];
            D_800E1AA0[D_800C2598] = t * (1.0f / 65536);
            D_800E1AA0[D_800C2598] = D_800E1AA0[D_800C2598] * (D_800E1718[D_800C2594 - 1][D_800C2598].w * 0.5f);
            t = ((s16 *)pp)[5];
            t <<= 16;
            t |= ((u16 *)pp)[21];
            D_800E1AB0[D_800C2598] = t * (1.0f / 65536);
            D_800E1AB0[D_800C2598] = D_800E1AB0[D_800C2598] * (D_800E1718[D_800C2594 - 1][D_800C2598].h * 0.5f);
            D_800E19C4 = pp;
            func_800AE898(&rotA, D_800E7978[D_800C2598] * 360.0f * (1.0f / 65536),
                          -(f32)D_800E8000[D_800C2598] * 360.0f * (1.0f / 65536), 0.0f);
            func_800AE898(&rotB, 0.0f, 0.0f, D_800E78B8[D_800C2598] * 360.0f * (1.0f / 65536));
            func_800B3240(&rotB, &rotA, &mf);
            func_80012D30(&mf);
            if (D_800EFD64->type == 0) {
                func_80011490(&rotB, 0, 0, 0, -D_800E8090[D_800C2598].x, -D_800E8090[D_800C2598].y,
                              -D_800E8090[D_800C2598].z, 0x100);
            } else {
                func_80011490(&rotB, 0, 0, 0, 0, 0, 0, 0x100);
            }
            func_800120A4(&rotB, &mf, &rotA);
            if (D_800C2364 != 0) {
                func_800120A4(&rotA, &D_800C2638, &rotB);
            } else {
                func_800120A4(&rotA, &D_800C25F8, &rotB);
            }
            func_800120A4(&rotB, &persp, &out);
            if (i == 0) {
                dyn->m0[D_800C2598] = out;
            } else {
                dyn->m1[D_800C2598] = out;
            }
        }
        if (D_800EFD64->type == 0) {
            func_80011490(&dyn->m2[D_800C2598], 0, 0, 0, D_800E8090[D_800C2598].x,
                          D_800E8090[D_800C2598].y - 500, D_800E8090[D_800C2598].z, 0x10);
        } else if (D_800EFD64->type == 1) {
            func_80011490(&dyn->m2[D_800C2598], 0, 0, 0, 0, 0, 0, 0x10);
        } else {
            func_80011490(&dyn->m2[D_800C2598], 0, 0, 0, 0, -2000, 0, 0x10);
        }
    }

    for (i = 0; i < 8; i++) {
        if (D_800E37C0[i].b7A5 == 0) {
            r = D_800E15E0[i];
            D_800E18A0[i] = 0;
            for (k = 0; k < D_800C2594; k++) {
                t = func_80002554((s32)((f32)(s32)D_800E37C0[i].f48 * 1000.0f),
                                  (s32)((f32)(s32)D_800E37C0[i].f4C * 1000.0f),
                                  (s32)((f32)(s32)D_800E37C0[i].f50 * 1000.0f), k, r, dyn, i) &
                    0xF;
                if (D_800E37C0[i].b44 != 0) {
                    t |= (func_80002554(D_800E37C0[i].x2, D_800E37C0[i].y2, D_800E37C0[i].z2, k, r, dyn, i) & 0xF)
                         << 4;
                } else {
                    t |= t << 4;
                }
                D_800E18A0[i] |= t << (k * 8);
            }
        }
    }

    gdl = *pgdl;
    func_80011490(&dyn->m4, 0, 0, 0, 0, 0, 0, 0x100);
    if (D_800E36F8 == 3 || D_800E36F8 == 7) {
        gSPSegment(gdl++, 0, 0);
        gSPDisplayList(gdl++, D_800C2A38);
        gSPDisplayList(gdl++, D_800C2A00);
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
        gSPViewport(gdl++, &D_800C25E8);
        gSPPerspNormalize(gdl++, D_800E1B44);
        gDPPipeSync(gdl++);
        gDPSetDepthImage(gdl++, D_800C29E0 + 0x80000000);
        gDPPipeSync(gdl++);
        gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29E0 + 0x80000000);
        gDPSetCycleType(gdl++, G_CYC_FILL);
        gDPSetFillColor(gdl++, 0xFFFCFFFC);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC, D_800F13F0);
        gDPPipeSync(gdl++);
        gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[fb]);
        gDPSetFillColor(gdl++, 0x10001);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC, D_800F13F0);
        gDPPipeSync(gdl++);
        gDPSetCycleType(gdl++, G_CYC_1CYCLE);
        gSPSetGeometryMode(gdl++, D_800E19CC);
        gDPSetColorDither(gdl++, G_CD_MAGICSQ);
        if (D_800E374C > 0) {
            D_800E374C--;
        }
        goto end;
    }
    switch (D_800E3710) {
        case 3:
            gdl = func_80002D20(gdl, fb, dyn);
            goto end;
        case 2:
            gdl = func_800035AC(gdl, fb, dyn);
            goto end;
        case 1:
            if (D_800E3730.s0 != -1) {
                if (D_800E3730.s0 > 0) {
                    D_800E3730.s0--;
                }
                goto end;
            }
            break;
    }
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    D_800C2598 = 0;
    CHECK(1000);
    if ((D_800C2350 & 0x40) && D_800C2358 == 0) {
        for (i = 0; i < 8; i++) {
            if (D_800E37C0[i].b7A5 == 0 && !((i ^ D_800DC108) & 1) && D_800E18A0[i] != 0) {
                gdl = func_8003AC18(i, func_8003AAE0(gdl, dyn, i), dyn);
            }
        }
    }
    func_80033FF0();
    if (D_800E1898 == 1) {
        func_8003240C(dyn, 0, fb);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gSPViewport(gdl++, &D_800C25E8);
    gSPPerspNormalize(gdl++, D_800E1B44);
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, D_800C29E0 + 0x80000000);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29E0 + 0x80000000);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[fb]);
    if (D_800DC148 != 0 || D_800E3730.c2 != 0 || D_800E3710 != 0) {
        gDPSetFillColor(gdl++, 0x10001);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC, D_800F13F0);
        if (D_800E3730.c2 != 0) {
            D_800E3730.c2--;
        }
    }
    if (D_800DC148 != 0) {
        D_800DC148--;
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, D_800E19CC);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    if (D_800DC148 != 0) {
        *pgdl = gdl;
        return;
    }
    gSPMatrix(gdl++, &dyn->m4, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    if (D_800C98B0 == 0) {
        gdl = func_80003AC4(gdl);
    } else if (D_800C98B0 == 1) {
        if (D_800E1898 < 4) {
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gSPClearGeometryMode(gdl++, G_ZBUFFER);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, 0x0C084000, 0);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
            gdl = func_80035178(gdl, dyn, D_800DC144);
        } else {
            gdl = func_80003AC4(gdl);
        }
    }
    n = D_800C98B0;
    if (n == 2 || n == 3) {
        gSPClearGeometryMode(gdl++, G_ZBUFFER);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gSPClipRatio(gdl++, FRUSTRATIO_3);
        if (n == 3) {
            gDPLoadTLUT_pal256(gdl++, D_800DC144->f4);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
            gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
        } else {
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gSPTexture(gdl++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
            gDPSetCombine(gdl++, 0x61FEC3, 0x44FE7F3F);
            gDPSetPrimColor(gdl++, 0, 0, D_800CCD63, D_800CCD63, D_800CCD63, 0);
            gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
        }
        for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
            gdl = func_80002418(gdl, dyn, D_800C2598, 1);
            gSPMatrix(gdl++, &dyn->m2[D_800C2598], G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
            if (D_800C2350 & 2) {
                gdl = func_80013D70(0, D_800E19C8, &D_800DC144, gdl);
            }
            gDPPipeSync(gdl++);
            gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
        }
        gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    }
    if (D_800C2384 != 0) {
        gDPPipeSync(gdl++);
        gDPSetCycleType(gdl++, G_CYC_FILL);
        gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29E0 + 0x80000000);
        gDPSetFillColor(gdl++, 0x7FFC7FFC);
        gDPFillRectangle(gdl++, D_800E1718[1][1].x, D_800E1718[1][1].y,
                         D_800E1718[1][1].x + D_800E1718[1][1].w - 4, D_800E1718[1][1].y + D_800E1718[1][1].h - 4);
        gDPPipeSync(gdl++);
        gDPSetCycleType(gdl++, G_CYC_1CYCLE);
        gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[fb]);
    }
    if (D_800E1898 == 1 && (D_800C8CE8 != 0 || (D_800EFD70.flags & 0x81))) {
        gdl = func_8003371C(func_80002418(gdl, dyn, 0, 1), dyn);
    }
    func_8000ED10((u8 *)D_800DC0B8, 0, 0x20);

    for (i = 0; i < 8; i++) {
        if (D_800E37C0[i].b7A5 == 0) {
            if (D_800E18A0[i] & 0x0F0F0F0F) {
                func_8003EAC4(D_800E1878, i);
                *D_800E37C0[i].skel = *D_800E1878;
                if (D_800C2358 == 0) {
                    for (k = 0; k < 6; k++) {
                        Bone *bn = &D_800E37C0[i].skel->b[D_800C25CC[k]];
                        func_800139A8(&bn->m40, bn->v34, D_800DC150[i].p23C[fb] + k);
                    }
                }
            }
            if (D_800E18A0[i] & 0xF0F0F0F0) {
                if (D_800E37C0[i].b6E8 == 0 || !((D_800DC108 / 2 + i) & 1) || D_800E1898 != 1 ||
                    ((D_800E18A0[i] >> 4) & 0xF) == 3 || D_800E37C0[i].b44 != 0 || (D_800EFD70.flags & 1)) {
                    func_8003D4BC(D_800E37C0[i].p65C, i);
                }
                func_80013ACC(D_800E37C0[i].p65C, D_800DC150[i].p234[fb], 0xF);
                if (D_800C2358 == 0 && (D_800C2360 != 0 || D_800C235C != 0)) {
                    u16 *m = (u16 *)&D_800DC150[i].p234[fb][2];

                    for (a = 0; a < 3; a++) {
                        for (b = 0; b < 3; b++) {
                            s32 v = m[a * 4 + b];

                            v <<= 16;
                            v |= m[a * 4 + b + 16];

                            if (D_800C2360 != 0) {
                                v <<= 1;
                            } else {
                                v >>= 1;
                            }
                            m[a * 4 + b] = v >> 16;
                            m[a * 4 + b + 16] = v;
                        }
                    }
                }
            }
            for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
                if (D_800E18A0[i] & (0xFF << (D_800C2598 * 8))) {
                    if (D_800EFD64->type == 0) {
                        func_80011490(&D_800DC150[i].a[fb * 4 + D_800C2598], 0, 0, 0, D_800E37C0[i].x,
                                      D_800E37C0[i].y, D_800E37C0[i].z, 0x10);
                        if (D_800E37C0[i].b44 != 0) {
                            func_80011490(&D_800DC150[i].c[fb * 4 + D_800C2598], 0, 0, 0, D_800E37C0[i].x2,
                                          D_800E37C0[i].y2, D_800E37C0[i].z2, 0x10);
                        }
                    } else {
                        func_80011490(&D_800DC150[i].a[fb * 4 + D_800C2598], 0, 0, 0,
                                      D_800E37C0[i].x - D_800E8090[D_800C2598].x,
                                      D_800E37C0[i].y - D_800E8090[D_800C2598].y,
                                      D_800E37C0[i].z - D_800E8090[D_800C2598].z, 0x10);
                        if (D_800E37C0[i].b44 != 0) {
                            func_80011490(&D_800DC150[i].c[fb * 4 + D_800C2598], 0, 0, 0,
                                          D_800E37C0[i].x2 - D_800E8090[D_800C2598].x,
                                          D_800E37C0[i].y2 - D_800E8090[D_800C2598].y,
                                          D_800E37C0[i].z2 - D_800E8090[D_800C2598].z, 0x10);
                        }
                    }
                    func_800120A4(&D_800DC150[i].a[fb * 4 + D_800C2598], &dyn->m0[D_800C2598],
                                  &D_800DC150[i].b[fb * 4 + D_800C2598]);
                    if (D_800E37C0[i].b44 != 0) {
                        func_800120A4(&D_800DC150[i].c[fb * 4 + D_800C2598], &dyn->m0[D_800C2598],
                                      &D_800DC150[i].d[fb * 4 + D_800C2598]);
                    }
                }
            }
        }
    }

    gSPClipRatio(gdl++, FRUSTRATIO_2);
    CHECK(1500);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gSPMatrix(gdl++, &D_800C2678, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gdl = func_80007010(gdl, fb, dyn, 0);
    gSPMatrix(gdl++, &dyn->m4, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClipRatio(gdl++, FRUSTRATIO_2);
    for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
        CHECK(500);
        gdl = func_80002418(gdl, dyn, D_800C2598, 1);
        if (D_800C2350 & 8) {
            if (D_800EFD64->type == 0) {
                gdl = func_80023C0C(gdl, dyn, fb, D_800C2598);
            } else if (D_800EFD64->type == 1) {
                gdl = func_800258F4(gdl, dyn, fb, D_800C2598);
            } else if (D_800EFD64->type == 2) {
                gdl = func_80040190(gdl, dyn, fb, D_800C2598);
            }
        }
        gdl = func_80062DD4(gdl, dyn, fb, D_800C2598);
    }
    CHECK(500);
    gdl = func_80008B1C(func_8000B9CC(gdl, fb, dyn, D_800A9D00, D_800D15C0), fb, dyn);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    if (D_800C2350 & 4) {
        for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
            CHECK(500);
            gdl = func_80002418(gdl, dyn, D_800C2598, 1);
            if (D_800EFD64->type == 0) {
                gdl = func_80022FD8(gdl, dyn, fb, D_800C2598);
            } else if (D_800EFD64->type == 1) {
                gdl = func_8002803C(gdl, dyn, fb, D_800C2598);
            } else if (D_800EFD64->type == 2) {
                gdl = func_8003FD4C(gdl, dyn, fb, D_800C2598);
            }
            gdl = func_80062DA0(gdl, dyn, fb, D_800C2598);
        }
    }
    if (D_800C2358 != 0) {
        gdl = func_8000C0E0(gdl, dyn, fb);
    }
    gDPPipeSync(gdl++);
    switch (D_800EFD70.mode) {
        case 2:
            CHECK(2000);
            gdl = func_8006C510(gdl, dyn, fb);
            break;
        case 9:
            CHECK(500);
            gdl = func_8006CD3C(gdl, dyn, fb);
            break;
        case 0:
        case 13:
        case 14:
            if (D_800EFD64->f50 & 2) {
                CHECK(1000);
                for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
                    gdl = func_8006C794(gdl, dyn, fb, D_800C2598);
                }
            }
            break;
    }
    CHECK(1000);
    if ((D_800C2350 & 0x40) && D_800C2358 == 0) {
        gDPPipeSync(gdl++);
        gSPClipRatio(gdl++, FRUSTRATIO_1);
        for (i = 0; i < 8; i++) {
            if (D_800E37C0[i].b7A5 == 0) {
                for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
                    if ((D_800E18A0[i] >> (D_800C2598 * 8)) & 0xF) {
                        if (!(D_800DC0B8[i] & 0x10)) {
                            func_8003B7E0(i);
                            D_800DC0B8[i] |= 0x10;
                        }
                        gdl = func_80002418(gdl, dyn, D_800C2598, 0);
                        gSPMatrix(gdl++, &dyn->m1[D_800C2598], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                        gSPMatrix(gdl++, &D_800DC150[i].a[fb * 4 + D_800C2598],
                                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        gdl = func_8003B0AC(i, dyn, gdl, D_800C2598);
                    }
                }
                if (D_800E37C0[i].b44 != 0) {
                    for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
                        if ((D_800E18A0[i] >> (D_800C2598 * 8)) & 0xF0) {
                            if (!(D_800DC0B8[i] & 0x20)) {
                                func_8003B7E0(i + 8);
                                D_800DC0B8[i] |= 0x20;
                            }
                            gdl = func_80002418(gdl, dyn, D_800C2598, 0);
                            gSPMatrix(gdl++, &dyn->m1[D_800C2598], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                            gSPMatrix(gdl++, &D_800DC150[i].c[fb * 4 + D_800C2598],
                                      G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                            gdl = func_8003B0AC(i + 8, dyn, gdl, D_800C2598);
                        }
                    }
                }
            }
        }
        gDPPipeSync(gdl++);
    }
    CHECK(1000);
    gSPMatrix(gdl++, &dyn->m4, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
        gdl = func_80002418(gdl, dyn, D_800C2598, 0);
        gSPMatrix(gdl++, &dyn->m1[D_800C2598], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gdl = func_8005E090(gdl, fb, dyn, D_800C2598);
    }
    D_800E1A9C = -1;
    CHECK(1000);
    for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
        gdl = func_80002418(gdl, dyn, D_800C2598, 1);
        if (D_800EFD64->type == 1) {
            gdl = func_80026120(gdl, dyn, fb, D_800C2598);
        } else if (D_800EFD64->type == 0) {
            gdl = func_800238F0(gdl, dyn, fb, D_800C2598);
        } else if (D_800EFD64->type == 2) {
            gdl = func_800408A4(gdl, dyn, fb, D_800C2598);
        }
    }
    if ((D_800C2350 & 0x80) && D_800EE140 == 0) {
        for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
            CHECK(1000);
            gdl = func_80037414(func_80002418(gdl, dyn, D_800C2598, 1), D_800C2598, dyn);
        }
    }
    if (!(D_800EFD70.flags & 1) && D_800E76D0 >= 0x259 && D_800C8CE8 == 0 && D_800EFD70.mode != 8) {
        s32 tr = D_800EFD64 - D_800CB210;

        if (tr != 20 && tr != 23 && tr != 24) {
            gdl = func_80008DE0(gdl, dyn);
        }
    }
    CHECK(1000);
    if (D_800E1898 == 1) {
        s32 tr = D_800EFD64 - D_800CB210;

        if (tr != 11 && tr != 20) {
            if (D_800C2594 != D_800E1898) {
                gdl = func_80002418(gdl, dyn, 0, 1);
            }
            gdl = func_80032854(gdl, dyn, fb);
        }
    }
    CHECK(1000);
    gdl = func_80071A7C(func_80039E04(gdl, dyn, fb), dyn, fb);
    if (D_800C2384 != 0) {
        s32 x = D_800E1718[1][1].x;
        s32 y = D_800E1718[1][1].y;
        s32 w = D_800E1718[1][1].w;
        s32 h = D_800E1718[1][1].h;
        s32 b = y + h;
        s32 b1 = b - 1;

        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gDPPipeSync(gdl++);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0);
        gDPFillRectangle(gdl++, x - 2, y - 2, x + w + 1, y);
        gDPFillRectangle(gdl++, x - 2, y, x, b1);
        gDPFillRectangle(gdl++, x - 2, b1, x + w + 1, b + 1);
        gDPFillRectangle(gdl++, x + w - 1, y, x + w + 1, b + 1);
    }
    if (D_800EFD64->type == 2) {
        gdl = func_80040B64(gdl, dyn, fb, 0);
    }
    CHECK(1000);
    gdl = func_80002418(gdl, dyn, 0, 1);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    for (i = 0; i < 8; i++) {
        if (D_800E37C0[i].b7A5 == 0 && D_800C238C[i] != 0xFF) {
            gdl = func_80008B1C(func_8000B9CC(gdl, fb, dyn, D_800AB0A0, D_800D19E0), fb, dyn);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gSPClipRatio(gdl++, FRUSTRATIO_2);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gSPSetGeometryMode(gdl++, G_ZBUFFER);
            gdl = func_80007010(gdl, fb, dyn, 1);
            gSPClipRatio(gdl++, FRUSTRATIO_1);
            break;
        }
    }
    CHECK(800);
    if (D_800C2358 != 0) {
        gdl = func_8006EB34(gdl, dyn, fb);
    } else if (D_800C2350 & 0x800) {
        if (D_800CCD64 == 0 && !(D_800EFD70.flags & 1)) {
            if (D_800C2594 == 1 || D_800E3710 == 0) {
                gdl = func_80068910(gdl, dyn, fb);
            }
            gSPClearGeometryMode(gdl++, G_CULL_BOTH);
            D_800E1A9C = -1;
            for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
                gdl = func_80059614(func_80002418(gdl, dyn, D_800C2598, 1), dyn, fb, D_800C2598);
                if (D_800C2384 != 0) {
                    break;
                }
            }
            gSPSetGeometryMode(gdl++, D_800E19CC);
        }
    }
    if (D_800EFD70.flags & 0x80) {
        gdl = func_8006D664(gdl, dyn, fb, D_800C2598);
    }
    if (D_800CCD64 != 0) {
        gdl = func_8000B3C8(func_80002418(gdl, dyn, 0, 0));
    }
    if (!(D_800EFD70.flags & 1)) {
        switch (D_800E36F8) {
            case 0:
            case 4:
                if (D_800E374C == -1) {
                    D_800E374C = 20;
                }
                D_800EFD70.f54 = 2;
                alpha = D_800E374C * 255 / 20;
                D_800EFD70.h58 = alpha;
                break;
            case 2:
            case 6:
                if (D_800E374C == -2) {
                    D_800E374C = -3;
                    alpha = 255;
                } else if (D_800E374C == -1) {
                    D_800E374C = 20;
                    alpha = 0;
                } else {
                    alpha = 255 - D_800E374C * 255 / 20;
                }
                D_800EFD70.f54 = 0;
                D_800EFD70.h58 = 255 - alpha;
                break;
            default:
                D_800EFD70.f54 = 3;
                alpha = -1;
                break;
        }
        if (alpha != -1) {
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, alpha);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
            gdl = func_80072F70(gdl, dyn, fb);
        }
    }
end:
    *pgdl = gdl;
    D_800E76CC = 1;
    D_800E19B8 = D_800E1A98;
}
