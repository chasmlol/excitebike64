#include "ctx.h"

typedef struct { s16 f0; u8 pad[0x22]; } Ent24;
typedef struct { u8 b[7]; } Blk7;
typedef struct { s16 a, b; s16 pad4[2]; s16 c, d; s16 padC[2]; } Vw;
typedef struct State {
    u8 pad0[0x16C];
    s32 f16C;
    s32 f170;
    s32 f174;
    s32 f178;
    s32 f17C;
    s32 f180;
    s32 f184;
    s32 f188;
    s32 f18C;
    s32 f190;
    Ent24 e[150];
    s16 * f16AC;
    u8 pad16B0[0x6];
    s16 f16B6;
    s16 f16B8;
    char name[7];
    u8 pad16C1[0x1F];
    void * f16E0;
    u8 pad16E4[0x4];
    s32 f16E8;
    s32 f16EC;
    u8 pad16F0[0x4];
    s32 f16F4;
    u8 pad16F8[0x4];
    s32 f16FC;
    u8 pad1700[0xC];
    s32 f170C;
    s32 f1710;
    s32 f1714;
    s32 f1718;
    s32 f171C;
    s32 f1720;
    s32 f1724;
    s32 f1728;
    s32 f172C;
    u8 pad1730[0x8];
    s32 f1738;
    u8 pad173C[0x10];
    void * f174C;
    void * f1750;
    u8 pad1754[0x4C];
    s16 f17A0;
    s16 f17A2;
    s16 f17A4;
    u8 pad17A6[0x2];
    s16 f17A8;
    s16 f17AA;
    s16 f17AC;
    u8 pad17AE[0x2];
    void * f17B0;
    u8 pad17B4[0x4];
    Vw vw[4];
    s32 f17F8;
    u8 pad17FC[0x8];
    s32 f1804;
    s32 f1808;
    u8 pad180C[0x4];
} State;

typedef struct { s32 x, y, w, h; } Rect;
typedef struct { s16 c[8]; } Row;
typedef struct { u32 n; Row *v; } VtxList;
typedef struct { VtxList *vl; u8 pad[0x1C]; } MEnt;
typedef struct { u32 n; MEnt e[1]; } Model;
typedef struct {
    u8 pad0[0xC];
    u16 flags;
    u8 padE[0x12];
    s32 *f20;
    s32 *f24;
    u8 pad28[8];
    Model *f30;
    Model *f34;
    u8 pad38[8];
} Obj;
typedef struct { s32 f0; u8 pad4[0x3E4]; void *f3E8; } Glob70;
typedef struct { s32 f0, f4, f8; } Tri3;
typedef struct { u8 pad0[6]; s16 f6; u8 pad8[8]; s16 f10; } Cfg;

extern State *D_800CEA40;
extern void *D_800EFD64;
extern u8 D_800CBF30[];
extern Glob70 D_800EFD70;
extern char D_800DAB1C[];
extern char D_800DAB24[];
extern char D_800DAB30[];
extern char D_800DAB34[];
extern char D_800DAB38[];
extern s32 D_800E7728;
extern u8 D_800248EC[];
extern u8 D_80014D20[];
extern s32 D_800D125C;
extern s32 D_800D1368;
extern s32 D_800D1370;
extern s32 *D_800CE270[];
extern s32 D_800D1274;
extern void *D_800C9570;
extern Rect D_800CE9C0[];
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern Cfg D_800CEB08;
extern s16 D_800CE26C[];
extern Obj D_800CE280[];
extern s32 D_800F47C0;
extern s32 D_800D13E0;
extern s32 D_800D142C;
extern Tri3 D_800CE210;
extern Tri3 D_800CE21C;
extern s32 D_800CE240;

void *func_800BC908(s32 size, s32 a1, char *file, s32 line);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80087DD8(void);
void func_80014A80(void);
s32 func_800BF530(char *name, s32 *out);
void func_8000ECB8(s32);
void func_800482D0(s32);
void func_800BFC40(char *name, void *fn);
s32 func_800BF79C(s32 a0, s32 a1);
void func_800BCB04(void *p);
s32 func_800C1E60(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
void func_80096B3C(s32);
void func_8008CF94(s32, s32);
void func_8008D3B4(s32, s32, s32, s32, s32, s32, s32, s32, s32);
void func_8008EF84(s32 a0, s32 a1, s32 a2, s32 a3);
void func_8009EBF4(void);
void func_8009ECE0(void);
void func_8009C618(void);
void func_8009EBA8(s32);
void func_8008F0B0(void);
void func_800A4700(void *);
void func_80094738(s32);
void func_80090670(void);
void func_8008E76C(s32, s32, s32);
void func_800B2B00(s32);

static inline s32 rnd50(s32 v)
{
    s32 f = v % 50 >= 15;
    return (v / 50 + f) * 50;
}

void func_80090A04(void)
{
    s32 i, j;
    u32 k, n;
    Obj *o;
    VtxList *vl;
    s32 v;
    s32 c, q, x, y, w, f, d;

    D_800EFD64 = D_800CBF30;
    if (D_800CEA40 == NULL || !(D_800CEA40->f174 & 0x6000)) {
        D_800CEA40 = func_800BC908(0x1810, 0, D_800DAB1C, 0x50A);
        func_8000ED10((u8 *)D_800CEA40, 0, 0x1810);
    } else {
        D_800EFD70.f0 &= ~2;
        D_800CEA40->f174 &= ~0x10;
    }
    func_80087DD8();
    func_80014A80();
    if (func_800BF530(D_800DAB24, &D_800E7728) != 0) {
        func_8000ECB8(1);
    }
    func_800482D0(D_800E7728);
    D_800CEA40->f17C = 0;
    D_800CEA40->f180 = 0;
    D_800CEA40->f184 = 0;
    D_800CEA40->f190 = 0;
    D_800CEA40->f16B6 = 0;
    D_800CEA40->f16B8 = 0;
    for (c = 149; c >= 0; c--) {
        D_800CEA40->e[c].f0 = 0;
    }
    strcpy(D_800CEA40->name, "<NONE>");
    func_800BFC40(D_800DAB30, D_800248EC);
    func_800BFC40(D_800DAB34, D_80014D20);
    func_800BF530(D_800DAB38, &D_800CEA40->f1720);
    D_800CEA40->f1710 = func_800BF79C(D_800CEA40->f1720, D_800D125C);
    D_800CEA40->f1714 = func_800BF79C(D_800CEA40->f1720, D_800D1368);
    D_800CEA40->f1724 = func_800BF79C(D_800CEA40->f1720, D_800D1370);
    D_800CEA40->f1728 = func_800BF79C(D_800CEA40->f1720, *D_800CE270[0]);
    D_800CEA40->f170C = func_800BF79C(D_800CEA40->f1720, D_800D1274);
    if (!(D_800CEA40->f174 & 0x6000)) {
        if (D_800C9570 != NULL) {
            func_800BCB04(D_800C9570);
            D_800C9570 = NULL;
        }
    }
    for (q = 0; q < 4; q++) {
        D_800CEA40->vw[q].a = D_800CE9C0[q].w * 2;
        D_800CEA40->vw[q].b = D_800CE9C0[q].h * 2;
        D_800CEA40->vw[q].c = (D_800CE9C0[q].x + D_800CE9C0[q].w / 2) * 4;
        D_800CEA40->vw[q].d = (D_800CE9C0[q].y + D_800CE9C0[q].h / 2) * 4;
    }
    D_800CEA40->f17A0 = 1000;
    D_800CEA40->f17A2 = 320;
    D_800CEA40->f17A4 = 0x1FF;
    D_800CEA40->f17A8 = (D_800F13EC / 2) * 4;
    D_800CEA40->f17AA = (D_800F13F0 / 2) * 4;
    D_800CEA40->f17AC = 0x1FF;
    if (!(D_800CEA40->f174 & 0x6000)) {
        D_800CEA40->f16E8 = 1;
        D_800CEA40->f16EC = 0;
        D_800CEA40->f16E0 = &D_800CEB08;
        D_800CEA40->f170 = 0;
    }
    D_800CEA40->f16AC = D_800CE26C;
    D_800CEA40->f184 = 0;
    D_800CEA40->f17B0 = D_800CE280;
    D_800CEA40->f188 = 0;
    D_800CEA40->f18C = 0;
    D_800CEA40->f190 = 0;
    D_800CEA40->f17C = D_800CEA40->f184;
    D_800CEA40->f180 = D_800CEA40->f184;
    D_800CEA40->f1718 = func_800C1E60(0, 0x40, 2, 2, 0x21);
    D_800CEA40->f171C = func_800C1E60(0, 0x20, 2, 2, 0x21);
    func_80096B3C(D_800CEA40->f184);
    D_800CEA40->f16C = 0;
    for (i = 0; i < 29; i++) {
        D_800CE280[i].flags |= 0x10;
        if (D_800CE280[i].flags & 0x100) {
            D_800CE280[i].flags |= 0x20;
        } else {
            D_800CE280[i].flags &= ~0x20;
        }
        if (D_800CE280[i].f20 == NULL) continue;
        D_800CE280[i].f30 = (Model *)func_800BF79C(D_800CEA40->f1720, *D_800CE280[i].f20);
        if (D_800CE280[i].f24 != NULL) {
            D_800CE280[i].f34 = (Model *)func_800BF79C(D_800CEA40->f1720, *D_800CE280[i].f24);
        } else {
            D_800CE280[i].f34 = NULL;
        }
        for (k = 0; k < D_800CE280[i].f30->n; k++) {
            vl = D_800CE280[i].f30->e[k].vl;
            for (n = 0; n < vl->n; n++) {
                for (j = 0; j < 3; j += 2) {
                    v = (s16)vl->v[n].c[j];
                    if (v < 0) { s32 w2 = -v; s32 d2 = w2 / 50; s32 f2 = w2 - d2 * 50 >= 15; v = d2 + f2; v = -(v * 50); }
                    else { d = v / 50; f = v - d * 50 >= 15; v = d + f; v = v * 50; }
                    vl->v[n].c[j] = v;
                }
            }
        }
    }
    func_8008CF94(D_800CEA40->f16AC[0], D_800CEA40->f16AC[1]);
    for (y = 0; y < D_800CEA40->f16AC[1]; y++) {
        for (x = 0; x < D_800CEA40->f16AC[0]; x++) {
            func_8008D3B4(x, y, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    func_8008EF84(D_800CE9C0[1].x, D_800CE9C0[1].y, D_800CE9C0[1].w - 1, D_800CE9C0[1].h - 1);
    D_800CEA40->f172C = 90000;
    D_800CEA40->f1738 = 45000;
    D_800CE210.f8 = 0;
    D_800CE21C.f8 = 0;
    {
        s32 fl = D_800CEA40->f174;
        s32 m6 = fl & 0x6000;
        if (m6 != 0) {
            D_800CEA40->f174 = m6;
            if (fl & 0x2000) {
                D_800CEB08.f10 = 2;
                D_800CEB08.f6 = 2;
            } else {
                D_800CEB08.f10 = 3;
                D_800CEB08.f6 = 3;
            }
        } else {
            D_800CEB08.f10 = 1;
            D_800CEB08.f6 = 1;
            D_800CEA40->f174 = 0;
        }
    }
    func_8009EBF4();
    func_8009ECE0();
    D_800CEA40->f16F4 = 8;
    D_800CEA40->f174C = func_800BC908(0xF00, 0, D_800DAB1C, 0x5E1);
    D_800CEA40->f1750 = func_800BC908(0xF00, 0, D_800DAB1C, 0x5E2);
    D_800CEA40->f16FC = 0;
    func_8009C618();
    func_8009EBA8(0);
    D_800CEA40->f16F4 = 8;
    func_8008F0B0();
    D_800CEA40->f1804 = func_800BF79C(D_800F47C0, D_800D13E0);
    D_800CEA40->f1808 = func_800BF79C(D_800F47C0, D_800D142C);
    if (D_800EFD70.f3E8 != NULL) {
        D_800CEA40->f174 &= ~0x6000;
        func_800A4700((u8 *)D_800EFD70.f3E8 + 4);
        func_800BCB04(D_800EFD70.f3E8);
        D_800EFD70.f3E8 = NULL;
        func_80094738(D_800CEA40->f180);
    }
    func_80090670();
    D_800CE21C.f0 = D_800CE210.f0;
    D_800CE21C.f4 = D_800CE210.f4;
    D_800CE21C.f8 = ((u32)(D_800CE21C.f8 + 0x800) >> 13) << 13;
    D_800CE240 = 0;
    func_8008E76C(D_800CEA40->f17F8, D_800CEA40->f1724, D_800CEA40->f1728);
    func_800B2B00(0xAA);
}
