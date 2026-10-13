#include "ctx.h"

typedef struct {
    u8 w;
    u8 h;
    u8 pad[2];
    void *data;
} Spr;

typedef struct {
    char *name;
    s32 w4;
    u16 flags;
    u8 padA[6];
} Item;

typedef struct {
    s16 h0;
    s16 h2;
    u8 pad4[0x14 - 4];
    s16 h14;
} Obj;

typedef struct {
    char *title;
    s32 w4;
    Obj *p8;
    Item *items;
    u8 pad10[4];
    s16 h14;
    s16 state;
    s16 count;
    u8 pad1A[6];
    s32 *sel;
} Menu;

typedef struct {
    u8 pad0[8];
    u16 h8;
} Ctl;

typedef struct {
    u8 n;
    u8 pad1[3];
    u8 won[4];
    s16 trk[7];
    s16 laps[7];
} Course; /* 0x24 */

typedef struct {
    char name[0x1E];
    s16 h1E;
    u8 pad20[0x34 - 0x20];
    s32 f34[3];
    s32 f40;
    char init[3][3];
    char init4D[3];
    u32 f50;
    u8 pad54[0xA0 - 0x54];
} Track; /* 0xA0 */

typedef struct {
    u32 flags;
    u8 pad4[0xC - 0x4];
    s32 fC;
    s32 f10;
    u8 pad14[0x1C - 0x14];
    s32 f1C[8];
    s32 f3C;
    u8 pad40[0x48 - 0x40];
    s16 h48;
    s16 h4A;
    s32 f4C;
    s8 b50[4];
    u32 f54;
    u8 pad58[0x68 - 0x58];
    s32 f68;
    s32 f6C;
    u32 f70;
    s32 *txt[48];
    s16 h134;
    u8 pad136[0x13C - 0x136];
    void *p13C;
    u8 pad140[0x17C - 0x140];
    s32 f17C;
    u8 pad180[0x2E8 - 0x180];
    u32 f2E8;
    u8 pad2EC[0x2F4 - 0x2EC];
    Spr *spr2F4[37];
    Spr *p388;
    Spr *p38C;
    Spr *p390;
    Spr *p394;
    Spr *p398;
    Spr *p39C;
    Spr *p3A0;
    Spr *p3A4;
    Spr *p3A8;
    u8 *p3AC;
    Spr *p3B0;
    u8 pad3B4[0x3C8 - 0x3B4];
    Spr *p3C8;
    u8 pad3CC[0x3DC - 0x3CC];
    Spr *p3DC;
    u8 pad3E0[0x3EC - 0x3E0];
    s32 f3EC;
    u8 pad3F0[0x400 - 0x3F0];
    s32 f400;
    u8 pad404[0x408 - 0x404];
    s16 h408;
    u8 pad40A[2];
    u8 b40C;
    u8 pad40D[3];
    s32 f410;
    Spr *p414;
    u8 pad418[0x438 - 0x418];
    Spr *p438;
    u8 pad43C[0x444 - 0x43C];
    u8 *p444;
    s16 h448;
    s16 h44A;
    s16 *p44C;
    s32 f450;
    s16 h454;
    u8 pad456[0x45C - 0x456];
    Spr *p45C;
    s32 f460;
    s16 h464;
    char s466[0x10];
    s16 h476;
    u8 pad478[0x47C - 0x478];
    s16 h47C;
    u8 pad47E[0x744 - 0x47E];
    Spr *p744;
    Spr *p748;
    Spr *p74C;
    s32 f750;
    u8 pad754[0x758 - 0x754];
    s8 b758[4];
    u8 pad75C[0x7A8 - 0x75C];
    u8 b7A8;
    u8 b7A9;
} Glob;

typedef struct {
    u8 pad0[0x2BF];
    u8 b2BF;
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
    u8 vol[3];
} Save;

typedef struct {
    char *name;
    s32 x;
} Cheat;

typedef struct {
    s16 x;
    s16 y;
} Pos;

extern Glob D_800EFD70;
extern char *D_800CDAA0[];
extern char *D_800CDABC[];
extern char *D3_800CDAC8;
extern s32 D_800CDACC[][9];
extern Pos D_800CDA70[];
extern Pos D_800CDA94[];
extern Track *D_800EFD64;
extern Cheat D_800C2284[];
extern s32 D_800CD1A8;
extern char *D_800CD1AC[];
extern char D_800DA014[];
extern char D_800DA020[];
extern char D_800DA02C[];
extern char D_800DA038[];
extern char D_800DA04C[];
extern char D_800DA060[];
extern char D_800DA06C[];
extern char D_800DA078[];
extern char D_800DA084[];
extern Save D_800F1900;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern Menu *D_800F47A4;
extern Menu *D_800F47C4;
extern s32 D_800E2038;
extern volatile s32 D_800C2940;
extern s32 D_800E30A8;
extern Track D_800CB210[];
extern Course D_800CA9C8[];
typedef struct {
    Course *course;
    s16 pts[8];
    u8 pad14[0x6C - 0x14];
    u8 order[8];
} Champ;

typedef struct {
    u8 pad0[0x22];
    s16 h22;
    u8 pad24[0x50 - 0x24];
    u8 b50[8];
} Race;

typedef struct {
    char *name;
    char *f4;
    u8 pad8[8];
    char *f10;
    u8 pad14[0x48 - 0x14];
    s32 stats[4];
} Rider;

typedef struct {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} View;

typedef struct {
    u8 pad0[0x14];
    void *f14;
    char *f18;
    u8 pad1C[0x3C - 0x1C];
} BikeDef;

extern View D_800E1718[][4];
extern s32 D_800C2594;
extern s32 D_800C2388;
extern BikeDef D_800C9A40[];
extern char *D_800CDD88[];
extern char D_800DA120[];
extern char D_800DA144[];
extern char D_800DA150[];
extern char D_800DA170[];
extern char D_800DA184[];

typedef struct {
    u8 pad0[0x668];
    s16 h668;
    u8 pad66A[0x68C - 0x66A];
    s32 f68C;
    u8 pad690[0x7E0 - 0x690];
} Racer;

extern Champ *D_800CA930;
extern Race D_800F05A8;
extern Rider D_800CDB78[];
extern Racer D_800E37C0[];
extern s32 D_800C8CE0;
extern s32 D_800CA938[];
extern char *D_800CDDB8[];
extern char *D_800CDDC4[];
extern char *D_800CDDD0[];
extern char *D_800CDDDC[];
extern char D_800DA0A4[];
extern char D_800DA0B8[];
extern char D_800DA0BC[];
extern char D_800DA0D0[];
extern char D_800DA0E0[];
extern char D_800DA0E8[];
extern char D_800DA0FC[];
extern char D_800DA110[];
extern s32 D_800CA958[];
extern s32 D_800CA9C4;
extern u8 D_800CA80F;
extern char D_800D9C18[];
extern char D_800D9C20[];
extern char D_800D9A4C[];
extern char D_800D9C34[];
extern char D_800D9C48[];
extern char D_800D9C58[];
extern char D_800D9C70[];
extern char D_800D9C8C[];
extern char D_800D9C98[];
extern char D_800D9CAC[];
extern char D_800D9CDC[];
extern char D_800D9CF0[];
extern char D_800D9D14[];
extern char D_800D9A28[];
extern char D_800D9A30[];
extern char D_800D9D54[];
extern char D_800D9D68[];
extern char D_800D9D7C[];
extern char D_800D9D90[];
extern char D_800D9DE4[];
extern char D_800D9DF4[];
extern char D_800D9E24[];
extern char D_800D9E50[];
extern char D_800D9E6C[];
extern char D_800D9E94[];
extern char D_800D9EC0[];
extern char D_800D9EEC[];
extern char D_800D9F14[];
extern char D_800D9F40[];
extern char D_800D9F4C[];
extern char D_800D9F58[];
extern char D_800D9370[];
extern char D_800D9F64[];
extern s16 D_800CD24C[];
extern char *D_800CD274[];
extern char *D_800CD264[];
extern char *D_800CD26C[];
extern char *D_800CDE50[];
extern char *D_800CDE34[];
extern char *D_800CDE28[];
extern char *D_800CDE44[];
extern s32 D_800F4AC8[];
extern char D_800D9F7C[];
extern char D_800D9F84[];
extern char D_800D9F94[];
extern char D_800D9F98[];
extern char D_800D9FD4[];
extern char D_800D9FE0[];
extern char D_800D9FE8[];
extern char D_800D9FF4[];
extern char D_800D91C8[];

char *strcpy(char *, const char *);
s32 func_800C2224(void);
void func_8000EE30(s32 v, char *out);
void func_80052F30(s32 v, char *out);
s32 func_80077000(s32 param_1);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
u8 *func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_8000A690(s32 x, s32 y, void *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw);
Gfx *func_8007B9B4(Gfx *gdl, s32 p2, s32 p3);
void func_80088CF8(Mtx *m, s32 fb, Gfx **pgdl);
Gfx *func_80072F70(Gfx *gdl, Mtx *m, s32 x);
void func_80073664(s32 a, void *p);
Gfx *func_8007C640(Gfx *gdl, s32 fb, char *msg);
void func_8000EF8C(char *a0, char *a1);
void func_8000ED30(char *dst, char *src);
u32 func_800AEC10(char *s);
void func_800AEC60(char *s, char *fmt);
Gfx *func_8007B39C(Gfx *gdl, s32 fb, s32 a, char **items);
s32 func_800C1118(s32 *p, s32 x, s32 y, s32 n);
void func_80003CC0(Mtx *m, s32 fb, Gfx **pgdl);
void func_8007CB14(s32 i, s32 y);
s32 func_8000EDA0(char *a, char *b);
Gfx *func_80058294(Gfx *gdl, s32 fb, void *p, s32 x, s32 y);

#define D D_800EFD70
#define SAVE D_800F1900

#define R D_800F05A8
#define VP D_800E1718
#define CUSTOM5 (D.f4C == 1 && D_800CA930 != NULL && D_800CA930->course - D_800CA9C8 == 5)
#define CHAMP4 (D.f4C == 1 && D_800CA930 != NULL && D_800CA930->course - D_800CA9C8 == 4)
#define UNLOCKED(n) (D.f410 < SAVE.b2C0 || (SAVE.b2C0 == D.f410 && ((n) == 0 || ((SAVE.b2BF >> ((n) - 1)) & 1))))
#define LVLDONE (D.f410 < SAVE.b2C0 || (SAVE.b2C0 == D.f410 && (SAVE.b2BF & 8)))

static inline s32 avail8(s32 n)
{
    s32 k = n / 5;
    s32 v;

    if (k == 0) {
        goto ret1;
    }
    switch (D_800F1900.b2C0) {
    case 1:
        if (k >= 2) {
            goto ret0;
        }
        k--;
        v = D_800F1900.b2BF >> k;
        break;
    case 2:
        if (k < 2) {
            return 1;
        }
        if (k != 2) {
            goto ret0;
        }
        v = D_800F1900.b2BF >> (k - 1);
        break;
    case 3:
        if (k < 3) {
            return 1;
        }
        if (k != 3) {
            return 0;
        }
        v = D_800F1900.b2BF >> (k - 1);
        break;
    default:
        return 0;
    }
    if (!(v & 1)) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

static inline s32 avail8d(s32 n)
{
    s32 k = n / 5;
    s32 v;

    if (k == 0) {
        goto ret1;
    }
    if (k == 9) {
        v = 1;
        n = 2;
    }
    switch (D_800F1900.b2C0) {
    case 1:
        if (k >= 2) {
            goto ret0;
        }
        k--;
        v = D_800F1900.b2BF >> k;
        break;
    case 2:
        if (k < 2) {
            return 1;
        }
        if (k != 2) {
            goto ret0;
        }
        v = D_800F1900.b2BF >> (k - 1);
        break;
    case 3:
        if (k < 3) {
            return 1;
        }
        if (k != 3) {
            return 0;
        }
        v = D_800F1900.b2BF >> (k - 1);
        break;
    default:
        return 0;
    }
    if (!(v & 1)) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

#define AVAILD(n) ((D.f4C == 8) ? avail8d(n) : func_80077000((n) / 5))
#define AVAIL(n) ((D.f4C == 8) ? avail8(n) : func_80077000((n) / 5))

static inline s32 objOk(Obj *p)
{
    s32 lvl;
    s32 v;

    if (D.f4C == 8 || D.f4C == 0) {
        lvl = D_800F1900.b2C0;
    } else {
        lvl = D.f410;
    }
    switch (lvl) {
    case 1:
        v = p->h14 < 10;
        break;
    case 2:
        v = p->h14 < 15;
        break;
    case 3:
        goto ret1;
    default:
        goto ret0;
    }
    if (!v) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

#define PREAMBLE()                                                  \
    gDPPipeSync(gdl++);                                             \
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);                           \
    gDPSetTexturePersp(gdl++, G_TP_NONE);                           \
    gDPSetTextureFilter(gdl++, G_TF_BILERP);                        \
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);    \
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);                           \
    gDPSetRenderMode(gdl++, 0x00404240, 0);                         \
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279)

static inline s32 add2(s32 a, s32 b) { return a + b; }
static inline s32 sub2(s32 a, s32 b) { return a - b; }
#define CMB_TEX() gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279)
#define CMB_PRIM() gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF)
#define DRAW(x, y, img, f) gdl = func_8000A690((x), (y), (img), gdl, fb, 1, 1, (f), 0)
#define PSYNCK(k) { Gfx *_g = gdl++; _g->words.w0 = 0xE7000000; _g->words.w1 = (u16)(k) >> 16; }
#define DRAWK(x, y, img, f, k) gdl = func_8000A690((x), (y), (img), gdl, fb, 1, 1, (f), (u16)(k) >> 16)
#define DRAWV(x, y, img, f) { s32 _f = (f); gdl = func_8000A690((x), (y), (img), gdl, fb, 1, 1, _f, 0); }
#define TDRAW(spr, x, y, f) { Spr *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->data); DRAW((x), (y), _t, (f)); }
#define TDRAWP(spr, x, y, f) { Spr *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->data); gDPPipeSync(gdl++); DRAW((x), (y), _t, (f)); }

Gfx *func_8007CEDC(Mtx *m, s32 fb, Gfx *gdl, Ctl *pad, s32 e)
{
    char unk[0xE8];
    char buf[0x40];
    s16 wid[4];
    char buf2[0x10];
    char buf3[0x20];
    Menu *s;
    s32 k;
    s32 rowh;
    s32 i;
    Track *trk;
    s32 yoff3;
    s32 ry;
    s32 rx;
    s32 w;
    s32 w2;
    s32 w3;
    s32 u1;
    s32 u2;
    s32 u3;
    s32 r;
    s32 j;
    s32 col;
    s32 x;
    s32 y;
    s32 yo2;
    s32 n;
    s32 c;
    s32 tmp;
    s32 pl;
    s32 gr;
    s32 gc;
    s32 t;
    s32 a;
    void *p;
    Spr *img;

    if (D.f54 == 0) {
        s = D_800F47A4;
    } else {
        s = D_800F47C4;
    }
    switch (D.f54) {
    case 0:
        w = D_800F13EC; w2 = D_800F13EC;
        break;
    case 1:
        w3 = D_800F13EC;
        break;
    case 2:
        w3 = D_800F13EC;
        break;
    }
    gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    switch (s->state) {
    case 22:
        PREAMBLE();
        if (D_800C2940 != 0 && (D_800E2038 & 0x20)) {
            Spr *t = &D.p3A0[1];

            gDPLoadTLUT_pal256(gdl++, D.p3A0->data);
            gdl = func_8000A690(0x77, 0x8D, t, gdl, fb, 1, 1, 3, 0);
        }
        break;
    case 12:
        if (D.f54 != 1) {
            func_80088CF8(m, fb, &gdl);
        }
        break;
    case 3:
    case 11:
        yoff3 = 0;
        if (s->state == 11) {
            yoff3 = 12;
        }
        func_800C2224();
        PREAMBLE();
        if (s->state == 3) {
            s32 w;
            s32 r;
            Spr *sp;

            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x32);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            sp = D.p388 + 1;
            w = sp[D.h48].w << 3;
            r = D_800E2038 % (w + 0x200);
            if (r == 0) {
                D.h48 = (s16)(D.h48 + 1) % 10;
            }
            gdl = func_8000A690(r - w, ((0xF0 - (w + 0x200) / 2) >> 1) + r / 2, &D.p388[D.h48 + 1], gdl, fb, 1, 1,
                                6, 0);
        } else if (D.f3EC < 0x20) {
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D.p438->data);
            if (s->w4 != 0) {
                gdl = func_8000A690(0x1C, yoff3 + 0x17, &D.p438[1], gdl, fb, 1, 1, 3, 0);
            }
            if (s->p8 != NULL && objOk(s->p8)) {
                gdl = func_8000A690(0x128, yoff3 + 0x17, &D.p438[2], gdl, fb, 1, 1, 3, 0);
            }
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        TDRAWP(D.p3A0, 0x45, yoff3 + 0x1F, 3);
        if ((s->state == 11 && !AVAILD(s->h14 + *s->sel)) || (s->items[*s->sel + s->h14].flags & 1)) {
            gDPSetPrimColor(gdl++, 0, 0, 0x64, 0x64, 0x64, 0xFF);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            gDPLoadTLUT_pal256(gdl++, D.p13C);
            func_800C1E60(D.txt[0], 0x40, 4, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800D9C18);
            func_800C0ACC(D.txt[0], 0x16B, 0x82);
        } else {
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D.spr2F4[s->h14 + *s->sel]->data);
        }
        img = &D.spr2F4[s->h14 + *s->sel][1];
        if (s->state != 11) {
            gdl = func_8000A690(0x102, yoff3 + 0x3B, img, gdl, fb, 1, 1, 3, 0);
        } else {
            gdl = func_8000A690(0x108, yoff3 + 0x3B, img, gdl, fb, 1, 1, 3, 0);
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        if (s->state == 3) {
            img = &D.p390[2];
            gdl = func_8000A690((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, gdl, fb, 1, 1, 6, 0);
        } else {
            img = &D.p390[6];
            gdl = func_8000A690((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, gdl, fb, 1, 1, 6, 0);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        func_800C1E60(D.txt[s->count * 2], 0x20, 4, 2, 0x21);
        func_800C1F2C(D.txt[s->count * 2], s->title);
        func_800C0ACC(D.txt[s->count * 2], 0xB9, yoff3 + 0x28);
        for (i = 0; i < s->count; i++) {
            if ((s->state == 11 && !AVAIL(s->h14 + i)) || (s->items[i + s->h14].flags & 1)) {
                func_800C1E60(D.txt[i], 0x20, 1, (i == *s->sel) ? 4 : 2, 0x20000020);
                func_800C1E60(D.txt[i + s->count], 0x20, 1, (i == *s->sel) ? 4 : 2, 0x20000021);
            } else {
                func_800C1E60(D.txt[i], 0x20, 1, (i == *s->sel) ? 4 : 2, 0x20);
                func_800C1E60(D.txt[i + s->count], 0x20, 1, (i == *s->sel) ? 4 : 2, 0x21);
            }
            if (s->state == 11 && !AVAIL(D.f10)) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetRenderMode(gdl++, 0x00404240, 0);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                gDPLoadTLUT_pal256(gdl++, D.p398->data);
                gdl = func_8000A690(0x44 - i * 7, 0x3E + i * 25 + yoff3, &D.p398[(D.f17C / 2 + i * 4) % 30 + 1], gdl,
                                    fb, 1, 1, 3, 0);
            } else if (i == *s->sel) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetRenderMode(gdl++, 0x00404240, 0);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
                gdl = func_8000A690(0x45 - i * 7, 0x3D + i * 25 + yoff3, &D.p3A4[D.f17C / 2 % 30 + 1], gdl, fb, 1,
                                    1, 3, 0);
            }
            if (s->state == 11) {
                func_800C1F2C(D.txt[i], D_800CB210[s->h14 + i].name);
                func_8000EE30(D_800CB210[s->h14 + i].h1E, buf);
                func_800C1F2C(D.txt[i + s->count], buf);
                func_800C0ACC(D.txt[i + s->count], 0x112 - i * 7, 0x46 + i * 25 + yoff3);
            } else {
                func_800C1F2C(D.txt[i], s->items[i].name);
            }
            func_800C0ACC(D.txt[i], 0x6E - i * 7, 0x46 + i * 25 + yoff3);
        }
        gdl = func_800C1394(gdl);
        gDPPipeSync(gdl++);
        if (D.flags & 0x200) {
            gdl = func_8007B9B4(gdl, fb, 1);
        } else if (D.f2E8 >= 8 && D.f2E8 <= 10) {
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
            gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
            gDPPipeSync(gdl++);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D.p748->data);
            img = &D.p748[1];
            gdl = func_8000A690(0x9B, 0x5C, img, gdl, fb, 1, 1, 3, 0);
            gdl = func_8000A690(0x94, 0x75, img, gdl, fb, 1, 1, 3, 0);
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            img = &D.p744[1];
            gdl = func_8000A690(0x104 - img->w * 4, 0x78 - img->h / 2, img, gdl, fb, 1, 1, 6, 0);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
            gdl = func_8000A690(0x81 - D.h47C * 7, D.h47C * 25 + 0x62, &D.p3A4[D.f17C / 2 % 30 + 1], gdl, fb, 1, 1,
                                3, 0);
            func_800C2224();
            func_800C1E60(D.txt[0], 0x20, 1, (D.h47C == 0) ? 4 : 2,
                          (D.f2E8 == 10 && !(D_800F1900.b2C4 & 8)) ? 0x20000021 : 0x21);
            switch (D.f2E8) {
            case 9:
                func_800C1F2C(D.txt[0], D_800D9C20);
                break;
            case 8:
                func_800C1F2C(D.txt[0], D_800D9A4C);
                break;
            case 10:
                func_800C1F2C(D.txt[0], D_800D9C34);
                break;
            }
            func_800C0ACC(D.txt[0], 0x113, 0x6A);
            func_800C1E60(D.txt[0], 0x20, 1, (D.h47C == 1) ? 4 : 2, 0x21);
            switch (D.f2E8) {
            case 9:
                func_800C1F2C(D.txt[0], D_800D9C48);
                break;
            case 8:
                func_800C1F2C(D.txt[0], D_800D9C58);
                break;
            case 10:
                func_800C1F2C(D.txt[0], D_800D9C70);
                break;
            }
            func_800C0ACC(D.txt[0], 0x10C, 0x83);
            gdl = func_800C1394(gdl);
        } else if (D.f2E8 == 5 || D.f2E8 == 16) {
            if ((pad->h8 & 0x400) || D.f2E8 == 16) {
                gdl = func_8007B9B4(gdl, fb, -1);
            } else {
                gdl = func_8007B9B4(gdl, fb, 0);
            }
        }
        break;
    case 4:
    case 18: {
        s32 xoff;
        s32 yoff;
        s32 u4;

        if (s->state == 18) {
            xoff = 0;
            yoff = 0;
        } else {
            xoff = 0;
            yoff = -11;
        }
        PREAMBLE();
        func_800C2224();
        gDPLoadTLUT_pal256(gdl++, D.p438->data);
        {
        if (D.f3EC < 0x20 && (s->state == 18 || D.f70 == 0)) {
            if ((s->state == 18 && *s->sel / 5 < (D.h44A + 4) / 5 - 1) ||
                (s->state == 4 &&
                 ((D.f410 == 1 && *s->sel < 1) || (D.f410 == 2 && *s->sel < 2) || (D.f410 == 3 && *s->sel < 5)))) {
                DRAW(xoff + 0x128, yoff + 0x22, &D.p438[2], 3);
            }
            if (((s->state != 18) ? *s->sel : *s->sel / 5) > 0) {
                DRAW(xoff + 0x1C, yoff + 0x22, &D.p438[1], 3);
            }
        }
        }
        TDRAWP(D.p3A0, xoff + 0x49, yoff + 0x2A, 3);
        {
        if (D_800CA9C8[*s->sel / 5].n != 0) {
            s32 a;
            void *p;

            if (s->state == 18) {
                t = *s->sel;
            } else {
                switch (D.f70) {
                case 2:
                    t = D.h134;
                    break;
                case 1:
                    t = D_800CA9C8[D.fC].trk[D.f6C];
                    break;
                default:
                    t = D_800CA9C8[D.fC].trk[D.f68];
                    break;
                }
            }
            if (s->state == 18) {
                if (D.f2E8 == 13) {
                    func_80073664(0, D.spr2F4[D_800CA958[t]]->data);
                } else {
                    func_80073664((0x3B - D.f750) * 0xFF / 0x3B, D.spr2F4[D_800CA958[t]]->data);
                }
            } else {
                func_80073664(0xFF, NULL);
            }
            img = &D.spr2F4[D_800CA958[t]][1];
            if (s->state != 18 &&
                !((CUSTOM5 || ((D.f70 != 0 || UNLOCKED(*s->sel)) && D.f70 != 3 &&
                              (D.f70 != 2 || (D_800CA9C4 & (1 << D.h134))))) &&
                 (!CUSTOM5 || LVLDONE))) {
                if (D.f70 == 0 || D.f70 == 3 || s->state == 18) {
                    func_800C1E60(D.txt[20], 0x40, 4, 2, 0x21);
                    func_800C1F2C(D.txt[20], D_800D9C18);
                    func_800C0ACC(D.txt[20], xoff + 0x16F, yoff + 0x82);
                } else {
                    func_800C1E60(D.txt[20], 0x40, 1, 2, 0x21);
                    func_800C1F2C(D.txt[20], D_800D9C8C);
                    func_800C0ACC(D.txt[20], xoff + 0x172, yoff + 0x74);
                    func_800C1E60(D.txt[21], 0x40, 1, 2, 0x21);
                    func_800C1F2C(D.txt[21], D_800D9C98);
                    func_800C0ACC(D.txt[21], xoff + 0x16F, yoff + 0x82);
                    func_800C1E60(D.txt[22], 0x40, 1, 2, 0x21);
                    func_800C1F2C(D.txt[22], D_800D9CAC);
                    func_800C0ACC(D.txt[22], xoff + 0x16E, yoff + 0x90);
                }
                gDPSetPrimColor(gdl++, 0, 0, 0x64, 0x64, 0x64, 0xFF);
                gDPLoadTLUT_pal256(gdl++, D.p13C);
                CMB_PRIM();
            } else {
                if (s->state == 18) {
                    if (D.f2E8 == 13) {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    } else {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF - (0x3B - D.f750) * 0x82 / 0x3B, 0xFF - (0x3B - D.f750) * 0x82 / 0x3B,
                                        0xFF - (0x3B - D.f750) * 0x82 / 0x3B, 0xFF);
                    }
                    gDPLoadTLUT_pal256(gdl++, D.p13C);
                    CMB_PRIM();
                } else {
                    CMB_TEX();
                    gDPLoadTLUT_pal256(gdl++, D.spr2F4[D_800CA958[t]]->data);
                }
            }
            DRAW(xoff + 0x10C, yoff + 0x47, img, 3);
        }
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        img = &D.p390[(s->state == 18) ? 6 : 7];
        DRAW(((0x200 - img->w * 8) >> 1) + 4, (0xF0 - img->h) / 2, img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        CMB_TEX();
        for (i = 0; i < 5; i++) {
            if (s->state == 18 || ((D.f70 == 0 || D.f70 == 3) && i < D_800CA9C8[*s->sel].n) ||
                (D.f70 != 0 && i <= D_800CA9C8[*s->sel].n)) {
                if (s->state == 18) {
                    if (i == *s->sel % 5) {
                        gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &D.p3A4[(D.f17C / 2 + i * 4) % 30 + 1], 3);
                        func_800C1E60(D.txt[i], 0x20, 1, 4, 0x20);
                        func_800C1E60(D.txt[i + 5], 0x20, 1, 4, 0x22);
                    } else {
                        func_800C1E60(D.txt[i], 0x20, 1, 2, 0x20);
                        func_800C1E60(D.txt[i + 5], 0x20, 1, 2, 0x22);
                    }
                } else if (D.f70 != 0 && D.f70 != 3) {
                    if (i == D.f6C) {
                        if (D.f70 == 1) {
                            gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
                        } else {
                            gDPLoadTLUT_pal256(gdl++, D.p3AC + 0x204);
                        }
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &D.p3A4[(D.f17C / 2 + i * 4) % 30 + 1], 3);
                        func_800C1E60(D.txt[i], 0x20, 1, 4, 0x20);
                        func_800C1E60(D.txt[i + 5], 0x20, 1, 4, 0x22);
                    } else {
                        func_800C1E60(D.txt[i], 0x20, 1, 2, 0x20);
                        func_800C1E60(D.txt[i + 5], 0x20, 1, 2, 0x22);
                    }
                } else {
                    if (!((UNLOCKED(*s->sel) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                        gDPLoadTLUT_pal256(gdl++, D.p398->data);
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &D.p398[(D.f17C / 2 + i * 4) % 30 + 1], 3);
                    } else if (((D_800CA9C8[*s->sel].won[D.f410 - 1] >> i) & 1) && !CUSTOM5) {
                        gDPLoadTLUT_pal256(gdl++, D.p3A8->data);
                        DRAW(0x49 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &D.p3A8[(D.f17C / 2 + i * 4) % 30 + 1], 3);
                    }
                    if (i == D.f68) {
                        if (!((UNLOCKED(*s->sel) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                            func_800C1E60(D.txt[i], 0x20, 1, 4, 0x20000020);
                            func_800C1E60(D.txt[i + 5], 0x20, 1, 4, 0x20000022);
                        } else {
                            func_800C1E60(D.txt[i], 0x20, 1, 4, 0x20);
                            func_800C1E60(D.txt[i + 5], 0x20, 1, 4, 0x22);
                        }
                    } else {
                        if (!((UNLOCKED(*s->sel) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                            func_800C1E60(D.txt[i], 0x20, 1, 2, 0x20000020);
                            func_800C1E60(D.txt[i + 5], 0x20, 1, 2, 0x20000022);
                        } else {
                            func_800C1E60(D.txt[i], 0x20, 1, 2, 0x20);
                            func_800C1E60(D.txt[i + 5], 0x20, 1, 2, 0x22);
                        }
                    }
                }
                if (s->state == 18 || i < D_800CA9C8[*s->sel].n) {
                    if ((s->state != 18 && D.f70 != 2) || (s->state == 18 && (*s->sel / 5) * 5 + i < D.h44A)) {
                        if (s->state == 18) {
                            switch ((*s->sel / 5) * 5 + i) {
                            case 0x15:
                            case 0x16:
                            case 0x17:
                                strcpy(buf2, "-");
                                break;
                            default:
                                func_8000EE30(D_800CB210[D.p44C[(*s->sel / 5) * 5 + i]].h1E, buf2);
                                break;
                            }
                        } else if (*s->sel == 5) {
                            func_8000EE30(D_800CA9C8[5].laps[i], buf2);
                        } else if (*s->sel == 4) {
                            func_8000EE30(1, buf2);
                        } else {
                            func_8000EE30(D_800CB210[D_800CA9C8[*s->sel].trk[i]].h1E, buf2);
                        }
                        func_800C1F2C(D.txt[i + 5], buf2);
                        func_800C0ACC(D.txt[i + 5], 0x11A - i * 7 + xoff, 0x52 + i * 25 + yoff);
                    }
                    if (s->state == 18) {
                        if ((*s->sel / 5) * 5 + i < D.h44A) {
                            func_800C1F2C(D.txt[i], D_800CB210[D.p44C[(*s->sel / 5) * 5 + i]].name);
                        }
                    } else {
                        func_800C1F2C(D.txt[i], D_800CB210[D_800CA9C8[*s->sel].trk[i]].name);
                    }
                    func_800C0ACC(D.txt[i], 0x6F - i * 7 + xoff, 0x52 + i * 25 + yoff);
                } else {
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    if (*s->sel == 5) {
                        img = &D.p390[5];
                    }
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    DRAW(0x63 - i * 7 + xoff, 0x45 + i * 25 + yoff, img, 6);
                    if (D.f70 != 1) {
                        img = &D.p390[4];
                        DRAW(0x47 - i * 7 + xoff, 0x46 + i * 25 + yoff, img, 6);
                    }
                }
            } else {
                gDPSetTextureLUT(gdl++, G_TT_NONE);
                CMB_PRIM();
                img = &D.p390[4];
                gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xF0);
                DRAW(0x47 - i * 7 + xoff, 0x46 + i * 25 + yoff, img, 6);
                if (*s->sel == 5) {
                    img = &D.p390[5];
                }
                DRAW(0x63 - i * 7 + xoff, 0x45 + i * 25 + yoff, img, 6);
            }
        }
        if (s->state == 18) {
            strcpy(buf, "Records");
        } else if (*s->sel == 4) {
            strcpy(buf, "Challenge");
        } else if (*s->sel == 5) {
            if (D.f70 == 2 && (D_800E2038 & 0x20)) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                CMB_TEX();
                gDPLoadTLUT_pal256(gdl++, D.p3B0->data);
                DRAW(xoff + 0x16E, yoff + 0x34, &D.p3B0[2], 3);
                DRAW(xoff + 0x149, yoff + 0xBE, &D.p3B0[2], 0x23);
                DRAW(xoff + 0xFE, yoff + 0x79, &D.p3B0[1], 0x43);
                DRAW(xoff + 0x1C1, yoff + 0x79, &D.p3B0[1], 3);
            }
            switch (D.f70) {
            case 0:
                strcpy(buf, "Custom");
                if (LVLDONE) {
                    for (i = 0; i < 3; i++) {
                        func_800C1E60(D.txt[i + 12], 0x40, 1, (i == D.f6C) ? 4 : 2, 0x21);
                    }
                    if (D_800CA9C8[*s->sel].n == 0) {
                        func_800C1F2C(D.txt[12], D_800D9CDC);
                        func_800C0ACC(D.txt[12], xoff + 0x16E, yoff + 0x80);
                    } else {
                        func_800C1F2C(D.txt[12], D_800D9A30);
                        func_800C0ACC(D.txt[12], xoff + 0x16E, yoff + 0x79);
                        func_800C1F2C(D.txt[13], D_800D9A28);
                        func_800C0ACC(D.txt[13], xoff + 0x16E, yoff + 0x87);
                    }
                }
                break;
            case 1:
                strcpy(buf, "Select Slot");
                func_800C1E60(D.txt[12], 0x40, 5, 4, 0x20);
                if (D.f6C == D_800CA9C8[*s->sel].n) {
                    func_800C1F2C(D.txt[12], D_800D9CF0);
                } else {
                    func_800C1F2C(D.txt[12], D_800D9D14);
                }
                func_800C0ACC(D.txt[12], 0x2D, yoff + 0xCF);
                break;
            case 2:
                strcpy(buf, "Choose Track");
                func_800C1E60(D.txt[12], 0x40, 1, 4, 0x20);
                switch ((s16)(D.h134 / 5)) {
                case 0:
                    func_800C1F2C(D.txt[12], D_800D9D54);
                    break;
                case 1:
                    func_800C1F2C(D.txt[12], D_800D9D68);
                    break;
                case 2:
                    func_800C1F2C(D.txt[12], D_800D9D7C);
                    break;
                case 3:
                    func_800C1F2C(D.txt[12], D_800D9D90);
                    break;
                }
                func_800C0ACC(D.txt[12], 0x2D, yoff + 0xCF);
                break;
            }
        } else {
            switch (*s->sel) {
            case 0:
                strcpy(buf, "Bronze Round");
                break;
            case 1:
                strcpy(buf, "Silver Round");
                break;
            case 2:
                strcpy(buf, "Gold Round");
                break;
            case 3:
                strcpy(buf, "Platinum Round");
                break;
            }
        }
        func_800C1E60(D.txt[11], 0x40, 4, 2, 0x21);
        func_800C1F2C(D.txt[11], buf);
        func_800C0ACC(D.txt[11], xoff + 0xBD, yoff + 0x34);
        if (s->state != 18 && D.f70 == 0) {
            func_800C1E60(D.txt[10], 0x40, 1, 2, 0x20);
            if (D.f410 < SAVE.b2C0 || (SAVE.b2C0 == D.f410 && ((SAVE.b2BF >> *s->sel) & 1))) {
                func_800C1F2C(D.txt[10], D_800D9DE4);
            } else if ((SAVE.b2C0 > D.f410 || (SAVE.b2C0 == D.f410 && (*s->sel == 0 || ((SAVE.b2BF >> (*s->sel - 1)) & 1)))) || (CUSTOM5 && SAVE.b2C0 == D.f410 && (SAVE.b2BF & 8))) {
                s32 fl;

                fl = 0;
                switch (D.f410) {
                case 1:
                    if (*s->sel == 1) {
                        fl = 1;
                    }
                    break;
                case 2:
                    if (*s->sel == 2) {
                        fl = 1;
                    }
                    break;
                case 3:
                    if (*s->sel == 4) {
                        fl = 1;
                    }
                    break;
                }
                if (fl) {
                    if (D.f410 == 3) {
                        func_800C1F2C(D.txt[10], D_800D9DF4);
                    } else {
                        func_800C1F2C(D.txt[10], D_800D9E24);
                    }
                } else if (*s->sel == 5) {
                    if (D_800CA9C8[5].n != 0) {
                        if (D.f6C == 0) {
                            func_800C1F2C(D.txt[10], D_800D9E50);
                        } else {
                            func_800C1F2C(D.txt[10], D_800D9E6C);
                        }
                    } else {
                        func_800C1F2C(D.txt[10], D_800D9E94);
                    }
                } else {
                    func_800C1F2C(D.txt[10], D_800D9EC0);
                }
            } else if (CUSTOM5) {
                func_800C1F2C(D.txt[10], D_800D9EEC);
            } else {
                func_800C1F2C(D.txt[10], D_800D9F14);
            }
            func_800C0ACC(D.txt[10], xoff + 0x2D, yoff + 0xCF);
        }
        gdl = func_800C1394(gdl);
        PSYNCK(u4);
        if (s->state == 18) {
            s32 dx, dy;

            trk = &D_800CB210[D.p44C[D.f450]];

            if (D.f2E8 == 13) {
                D_800CA80F = 0;
            } else {
                D_800CA80F = (0x3B - D.f750) * 0xFF / 0x3B;
            }
            func_800C2224();
            if (trk->f50 & 0x400) {
                func_800C1E60(D.txt[0], 0x20, 1, 9, 0x21);
                func_800C1F2C(D.txt[0], D_800D9F40);
                func_800C0ACC(D.txt[0], 0x175, 0x6D);
                for (j = 0; j < 3; j++) {
                    func_800C1E60(D.txt[0], 0x20, 1, 9, 0x20);
                    func_8000EE30(trk->f34[j], buf);
                    func_800C1F2C(D.txt[0], buf);
                    func_800C0ACC(D.txt[0], 0x134 - j * 4, 0x7E + j * 13);
                    func_800C1E60(D.txt[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init[j], 3);
                    buf[3] = 0;
                    func_800C1F2C(D.txt[0], buf);
                    func_800C0ACC(D.txt[0], 0x1AC - j * 4, 0x7E + j * 13);
                }
            } else {
                if (D.f450 == 0x15 || D.f450 == 0x17) {
                    dx = -4;
                    dy = 0x14;
                } else {
                    dx = 0;
                    dy = 0;
                }
                func_800C1E60(D.txt[0], 0x20, 1, 9, 0x21);
                func_800C1F2C(D.txt[0], D_800D9F4C);
                func_800C0ACC(D.txt[0], dx + 0x17B, dy + 0x59);
                for (j = 0; j < 3; j++) {
                    func_800C1E60(D.txt[0], 0x20, 1, 9, 0x20);
                    func_80052F30(trk->f34[j], buf);
                    func_800C1F2C(D.txt[0], buf);
                    func_800C0ACC(D.txt[0], dx + (0x175 - j * 4) - 0x3C, (0x6A + j * 13) + dy);
                    func_800C1E60(D.txt[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init[j], 3);
                    buf[3] = 0;
                    func_800C1F2C(D.txt[0], buf);
                    func_800C0ACC(D.txt[0], dx + (0x175 - j * 4) + 0x3C, (0x6A + j * 13) + dy);
                }
                if (D.f450 != 0x15 && D.f450 != 0x17) {
                    func_800C1E60(D.txt[0], 0x20, 1, 9, 0x21);
                    func_800C1F2C(D.txt[0], D_800D9F58);
                    func_800C0ACC(D.txt[0], 0x168, 0x9C);
                    func_800C1E60(D.txt[0], 0x20, 1, 9, 0x20);
                    func_80052F30(trk->f40, buf);
                    func_800C1F2C(D.txt[0], buf);
                    func_800C0ACC(D.txt[0], 0x129, 0xAC);
                    func_800C1E60(D.txt[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init4D, 3);
                    buf[3] = 0;
                    func_800C1F2C(D.txt[0], buf);
                    func_800C0ACC(D.txt[0], 0x1A1, 0xAC);
                }
            }
            gdl = func_800C1394(gdl);
        } else if (D.flags & 0x200) {
            gdl = func_8007B9B4(gdl, fb, 1);
        }
        break;
    }
    case 6: {
        s32 u5;

        func_800C2224();
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        if (D.f2E8 == 14) {
            D.f2E8 = 15;
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        CMB_TEX();
        {
            Spr *_t = &D.p3A0[1];

            gDPLoadTLUT_pal256(gdl++, D.p3A0->data);
            gDPPipeSync(gdl++);
            DRAWK(0x4E, 0x1B, _t, 3, u5);
        }
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        CMB_PRIM();
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 29; k++) {
                x = 0x124 - j * 7 + k * 6;
                y = D_800CD24C[j] - 1;
                if (*(SAVE.vol + j) * 29 / 255 >= k) {
                    if (j == D.h448) {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF, k * 0xFF / 29, 0, 0xFF);
                    } else {
                        gDPSetPrimColor(gdl++, 0, 0, 0x8C, k * 0x8C / 29, 0, 0xFF);
                    }
                } else {
                    gDPSetPrimColor(gdl++, 0, 0, 0x4B, 0x4B, 0x4B, 0xFF);
                }
                DRAW(x, y, &D.p390[1], 6);
            }
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        img = &D.p390[2];
        DRAW(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
        img = &D.p3A4[D.f17C / 2 % 30 + 1];
        for (j = 0; j < 11; j++) {
            if (j < 3) {
                x = 0x52 - j * 7;
            } else {
                x = 0x52 - ((j - 3) / 2 + 3) * 7 + ((j - 3) & 1) * 194;
            }
            y = D_800CD24C[j];
            if (j == 9 || j == 10) {
                func_800C1E60(D.txt[0], 0x40, 1, (j == D.h448) ? 4 : 2, 0x20000020);
            } else {
                func_800C1E60(D.txt[0], 0x40, 1, (j == D.h448) ? 4 : 2,
                              (D_800C2940 == 0 && j == 7) ? 0x20000020 : 0x20);
            }
            func_800C1F2C(D.txt[0], D_800CD274[j]);
            func_800C0ACC(D.txt[0], x + 0x24 + ((u16)y >> 16), y + 8);
            if (j == D.h448) {
                DRAW(x - 2, y, img, 3);
            }
            switch (j) {
            case 3:
                func_800C1E60(D.txt[0], 0x40, 1, (D.h448 == j) ? 6 : 2, 0x22);
                func_800C1F2C(D.txt[0], D_800CD26C[SAVE.b2C4 & 1]);
                func_800C0ACC(D.txt[0], x + 0xB4 + ((u16)x >> 16), y + 8);
                break;
            case 8:
                func_800C1E60(D.txt[0], 0x40, 1, (D.h448 == j) ? 6 : 2, 0x22);
                func_800C1F2C(D.txt[0], D_800CD264[(SAVE.b2C4 & 0x10) == 0]);
                func_800C0ACC(D.txt[0], x + 0xB4, y + 8);
                break;
            case 7:
                tmp = 2;
                if (D.h448 == j) {
                    tmp = 4;
                    if (D_800C2940 != 0) {
                        tmp = 6;
                    }
                }
                func_800C1E60(D.txt[0], 0x40, 1, tmp, (D_800C2940 == 0) ? 0x20000022 : 0x22);
                func_800C1F2C(D.txt[0], D_800CD264[(SAVE.b2C4 & 2) >> 1]);
                func_800C0ACC(D.txt[0], x + 0xB4, y + 8);
                break;
            }
        }
        func_800C1E60(D.txt[0], 0x40, 4, 2, 0x20);
        func_800C1F2C(D.txt[0], D_800D9370);
        func_800C0ACC(D.txt[0], 100, 0x23);
        gdl = func_800C1394(gdl);
        if (D.f2E8 == 6 || D.f2E8 == 15 || D.f2E8 == 14) {
            gdl = func_8007C640(gdl, fb, D_800D9F64);
        }
        break;
    }
    case 24:
    case 25:
        func_800C2224();
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &D.p3DC[1];
        gDPLoadTLUT_pal256(gdl++, D.p3DC->data);
        if (s->state == 25) {
            DRAW(0x106 - img->w * 4, 0x41, img, 3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            CMB_PRIM();
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            img = &D.p390[1];
            DRAWV((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, 6);
            func_800C1E60(D.txt[0], 0x40, 4, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800D9F7C);
            func_800C0ACC(D.txt[0], 0x106, 0x4C);
            for (i = 0; D_800CDE50[i] != NULL; i++) {
                func_800C1E60(D.txt[0], 0x40, 1, 4, 0x21);
                func_8000EF8C(buf, D_800CDE50[i]);
                func_800C1F2C(D.txt[0], buf);
                func_800C0ACC(D.txt[0], 0x100 - i * 4, i * 13 + 0x6D);
            }
        } else {
            s32 slot;
            s32 found = 0;
            s32 line = 0;
            s32 col;
            char **pp;
            s32 k2;

            DRAW(0x110 - img->w * 4, 0x1F, img, 3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            CMB_PRIM();
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            img = &D.p390[1];
            DRAWV((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, 6);
            func_800C1E60(D.txt[0], 0x40, 4, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800D9F84);
            func_800C0ACC(D.txt[0], 0x110, 0x28);
            func_800C1E60(D.txt[0], 0x40, 5, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800D9F94);
            func_800C0ACC(D.txt[0], 400, 0x25);
            for (slot = 0; slot < 5; slot++) {
                if (slot == 4) {
                    if (found) {
                        break;
                    }
                    func_800AEC60(buf3, D_800D9F98);
                    line = 4;
                    D_800F4AC8[4] = 0;
                    col = 8;
                    pp = D_800CDE34;
                } else if (D_800F4AC8[slot] != -2) {
                    strcpy(buf3, "Controller Pak #X ");
                    found = 1;
                    if (D_800F4AC8[slot] <= 0) {
                        func_8000ED30(buf3, D_800D9FD4);
                    } else {
                        func_8000ED30(buf3, D_800D9FE0);
                        func_8000EE30(D_800F4AC8[slot], buf3 + func_800AEC10(buf3));
                        func_8000ED30(buf3, D_800D9FE8);
                        if (D_800F4AC8[slot] != found) {
                            func_8000ED30(buf3, D_800D9FF4);
                        }
                        func_8000ED30(buf3, D_800D91C8);
                    }
                    buf3[16] = slot + '1';
                    if (D_800F4AC8[slot] < 40) {
                        col = 8;
                        pp = D_800CDE28;
                    } else {
                        col = 7;
                        pp = D_800CDE44;
                    }
                } else {
                    continue;
                }
                func_800C1E60(D.txt[0], 0x40, 5, 2, 0x21);
                func_800C1F2C(D.txt[0], buf3);
                func_800C0ACC(D.txt[0], 0x106 - line * 3, line * 11 + 0x46);
                line++;
                for (k2 = 0; pp[k2] != NULL; k2++) {
                    func_800C1E60(D.txt[0], 0x40, 5, col, 0x21);
                    func_800C1F2C(D.txt[0], pp[k2]);
                    func_800C0ACC(D.txt[0], 0x106 - line * 3, line * 11 + 0x46);
                    line++;
                }
            }
        }
        gdl = func_800C1394(gdl);
        break;
    case 9:
    case 20: {
        s32 u6;
        s32 plr;
        s32 cheat;

        func_800C2224();
        cheat = (s->state == 9);
        if (!cheat) {
            plr = D.b758[D.f460];
        }
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        gDPLoadTLUT_pal256(gdl++, D.spr2F4[0]->data);
        DRAW(0x108, 0x47, &D.spr2F4[0][1], 3);
        TDRAWP(D.p3A0, 0x45, 0x2A, 3);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        img = &D.p390[1];
        DRAWV(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &D.p39C[D.f3EC / 2 % 60 + 1];
        if (D.f3C >= 2 && !cheat) {
            gDPLoadTLUT_pal256(gdl++, D.p444 + plr * 0x200);
        } else {
            gDPLoadTLUT_pal256(gdl++, D.p39C->data);
        }
        DRAWK(0x18B, 0x2F, img, 3, u6);
        func_800C1E60(D.txt[0], 0x20, 4, 2, 0x20);
        if (cheat) {
            strcpy(buf, "Enter A Cheat Code");
            func_800C1F2C(D.txt[0], buf);
            func_800C0ACC(D.txt[0], 0x55, 0x35);
        } else {
            strcpy(buf, "Player ");
            func_8000EE30(plr + 1, buf + func_800AEC10(buf));
            func_8000ED30(buf, D_800DA014);
            func_800C1F2C(D.txt[0], buf);
            func_800C0ACC(D.txt[0], 0x55, 0x35);
        }
        if (cheat && D.f2E8 == 17) {
            func_800C1E60(D.txt[0], 0x20, 1, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800C2284[D.b7A9].name);
            if (((1 << D.b7A9) == 4 && (D.flags & 0x20000000)) || ((D_800CD1A8 >> D.b7A9) & 1)) {
                func_800C1F2C(D.txt[0], D_800DA020);
            } else {
                func_800C1F2C(D.txt[0], D_800DA02C);
            }
            func_800C0ACC(D.txt[0], 0xAC, 0x71);
            func_800C1E60(D.txt[0], 0x20, 1, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800DA038);
            func_800C0ACC(D.txt[0], 0xA8, 0x7F);
        } else if (cheat && D.f2E8 == 18) {
            func_800C1E60(D.txt[0], 0x20, 1, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800DA04C);
            func_800C0ACC(D.txt[0], 0xAC, 0x71);
            func_800C1E60(D.txt[0], 0x20, 1, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800DA038);
            func_800C0ACC(D.txt[0], 0xA8, 0x7F);
        } else {
            s32 xx;

            for (y = 0; y < 5; y++) {
                for (x = 0; x < 8; x++) {
                    xx = x * 26;
                    func_800C1E60(D.txt[0], 0x20, (y * 8 + x > 0x25) ? 2 : 1, 2,
                                  (D.h464 == ((cheat) ? 10 : 3) && y * 8 + x < 0x26) ? 0x20000021 : 0x21);
                    func_800C1F2C(D.txt[0], D_800CD1AC[y * 8 + x]);
                    func_800C0ACC(D.txt[0], xx + 0x54 - y * 4, y * 17 + 0x56);
                    if (y * 8 + x == D.h454) {
                        gDPSetRenderMode(gdl++, 0x00404240, 0);
                        gDPSetTextureLUT(gdl++, G_TT_NONE);
                        CMB_PRIM();
                        img = &D.p390[3];
                        DRAW(xx + 0x54 - y * 4 - img->w * 4 + 4, y * 17 - (img->h >> 1) + 0x57, img, 6);
                    }
                }
            }
        }
        func_800C1E60(D.txt[0], 0x20, 2, (j == D.h464) ? 4 : 2, 0x20);
        if (!cheat) {
            switch (D.f460) {
            case 0:
            case 1:
            case 2:
                if (D.f460 != 0) {
                    buf[0] = D.f460 + '1';
                    buf[1] = D.f460 + '{';
                    buf[2] = ' ';
                    buf[3] = '\0';
                    func_800C1F2C(D.txt[0], buf);
                }
                if (D_800EFD64->f50 & 0x400) {
                    func_800C1F2C(D.txt[0], D_800DA060);
                } else {
                    func_800C1F2C(D.txt[0], D_800DA06C);
                }
                if (D.b758[3] == plr || D.b7A8 == plr) {
                    func_800C1F2C(D.txt[0], D_800DA078);
                }
                break;
            case 3:
                func_800C1F2C(D.txt[0], D_800D9F58);
                break;
            }
            func_800C0ACC(D.txt[0], 0x2F, 0xB6);
        }
        for (j = 0; (cheat) ? (j < 10) : (j < 3); j++) {
            func_800C1E60(D.txt[0], 0x20, 1, (j == D.h464) ? 4 : 2, 0x21);
            func_800C1F2C(D.txt[0], D_800DA084);
            func_800C0ACC(D.txt[0], (j - 1) * 20 + ((cheat) ? 0x50 : 0xE2), 0xB9);
            if (j < func_800AEC10(D.s466)) {
                func_800C1E60(D.txt[0], 0x20, 1, (j == D.h464) ? 4 : 2, 0x21);
                buf[0] = D.s466[j];
                buf[1] = '\0';
                func_800C1F2C(D.txt[0], buf);
                func_800C0ACC(D.txt[0], (j - 1) * 20 + ((cheat) ? 0x50 : 0xE3), 0xB3);
            }
        }
        gdl = func_800C1394(gdl);
        break;
    }
    case 19: {
        Glob *gp = &D_800EFD70;

        func_800C2224();
        {
            Gfx *_g = gdl++;

            _g->words.w0 = 0xE7000000 | ((u8)(u32)_g >> 8);
            _g->words.w1 = ((u16)(u32)_g >> 16) | (((u32)_g & 0x7F) >> 7);
        }
        gDPSetCycleType(gdl++, G_CYC_1CYCLE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureFilter(gdl++, G_TF_BILERP);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &gp->p45C[1];
        gDPLoadTLUT_pal256(gdl++, gp->p45C->data);
        DRAW(0xFE - img->w * 4, 0x74 - (img->h >> 1), img, 3);
        TDRAWP(gp->p3A0, 0x4A, 0x20, 3);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        func_800C1E60(gp->txt[0], 0x40, 4, 2, 0x21);
        strcpy(buf, "Controller ");
        func_8000EE30(gp->h476 + 1, buf + func_800AEC10(buf));
        func_800C1F2C(gp->txt[0], buf);
        func_800C0ACC(gp->txt[0], 0xBA, 0x29);
        func_800C1E60(gp->txt[0], 0x40, 4, 2, 0x21);
        strcpy(buf, "Configuration ");
        buf[func_800AEC10(buf) + 1] = '\0';
        buf[func_800AEC10(buf)] = gp->b50[gp->h476] + 'A';
        func_800C1F2C(gp->txt[0], buf);
        func_800C0ACC(gp->txt[0], 0xE9, 0xBF);
        for (j = 0; j < 9; j++) {
            tmp = 6;
            if (j & 1) {
                tmp = 4;
            }
            a = 0x20;
            if (j < 5) {
                a = 0x22;
            }
            if (!(j & 1)) {
                a |= 0x40000000;
            }
            func_800C1E60(gp->txt[0], 0x40, 5, tmp, a);
            func_800C1F2C(gp->txt[0], D_800CDAA0[D_800CDACC[(gp->b50 + gp->h476)[0]][j]]);
            func_800C0ACC(gp->txt[0], D_800CDA70[j].x, D_800CDA70[j].y);
            if (D_800CDACC[(gp->b50 + gp->h476)[0]][j] == 6) {
                tmp = 6;
                if (j & 1) {
                    tmp = 4;
                }
                a = 0x20;
                if (j < 5) {
                    a = 0x22;
                }
                if (!(j & 1)) {
                    a |= 0x40000000;
                }
                func_800C1E60(gp->txt[0], 0x40, 5, tmp, a);
                func_800C1F2C(gp->txt[0], D3_800CDAC8);
                func_800C0ACC(gp->txt[0], D_800CDA70[j].x, D_800CDA70[j].y + 11);
            }
        }
        for (j = 0; j < 3; j++) {
            func_800C1E60(D.txt[0], 0x40, 5, (j == 2) ? 4 : 6, (j != 2) ? 0x40000020 : 0x20);
            func_800C1F2C(D.txt[0], D_800CDABC[j]);
            func_800C0ACC(D.txt[0], D_800CDA94[j].x, D_800CDA94[j].y);
        }
        for (j = 0; j < D.f3C; j++) {
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        img = &D.p390[1];
        DRAWV(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        for (j = 0; j < 4; j++) {
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            CMB_TEX();
            img = &D.p39C[D.f3EC / 2 % 60 + 1];
            if (j == D.h476) {
                gDPLoadTLUT_pal256(gdl++, D.p444 + j * 0x200);
            } else {
                gDPLoadTLUT_pal256(gdl++, D.p13C);
            }
            DRAW(0x132 + j * 0x2C, 0x24, img, 3);
        }
        gdl = func_800C1394(gdl);
        break;
    }
    case 7:
#define YR (0x46 + j * rowh + ry)
    case 13:
    case 14: {
        s32 x;
        s32 y;
        s32 px;

        func_800C2224();
        rx = 0;
        if (s->state == 14) {
            if (CHAMP4) {
                rx = -8;
                ry = 0x1B;
            } else {
                ry = -7;
            }
        } else if (D.f4C == 8) {
            ry = 0x32;
            rx = -0xD;
        } else {
            ry = 0;
        }
        for (j = 0; j < 3; j++) {
            func_800C1E60(D.txt[j], 0x20, 4, 2, 0x20);
        }
        if (s->state == 14 && !CHAMP4) {
            func_800C1F2C(D.txt[0], D_800DA0A4);
            func_800C1F2C(D.txt[1], D_800DA0B8);
            func_800C0ACC(D.txt[1], 0x18F, ry + 0x28);
        } else {
            if (D.f4C == 8) {
                func_800C1F2C(D.txt[0], D_800DA0BC);
            } else {
                func_800C1F2C(D.txt[0], D_800DA0D0);
            }
            func_800C1F2C(D.txt[2], D_800DA0E0);
            if (s->state == 7) {
                func_800C1F2C(D.txt[1], D_800DA0B8);
                func_800C0ACC(D.txt[1], 0x147, ry + 0x28);
                func_800C0ACC(D.txt[2], 0x18D, ry + 0x28);
            } else if (s->state == 14 && CHAMP4) {
                func_800C0ACC(D.txt[2], 0x14D, ry + 0x2A);
            } else if (D.f4C == 8) {
                func_800C0ACC(D.txt[2], 0x178, ry + 0x28);
            } else {
                func_800C0ACC(D.txt[2], 0x184, ry + 0x28);
            }
        }
        if (s->state == 14 && CHAMP4) {
            func_800C0ACC(D.txt[0], 0x50, ry + 0x2A);
            func_800C1E60(D.txt[0], 0x20, 4, 2, 0x21);
            if (R.b50[0] == 0) {
                func_800C1F2C(D.txt[0], D_800DA0E8);
            } else {
                func_800C1F2C(D.txt[0], D_800DA0FC);
            }
            func_800C0ACC(D.txt[0], 0xEE, 0x97);
        } else {
            func_800C0ACC(D.txt[0], 0x55, ry + 0x28);
        }
        rowh = 0x19;
        if (s->state == 14 && CHAMP4) {
            rowh = 0x17;
        }
        for (j = 0; j < R.h22; j++) {
            if (s->state == 14 && !CHAMP4) {
                r = (*D_800CA930).order[j];
            } else {
                r = R.b50[j];
            }
            img = (Spr *)((r < D.f3C || D.f4C == 8) ? ((D.f4C == 8) ? ((r == D_800C8CE0) ? 4 : 2) : 4) : 2);
            func_800C1E60(D.txt[j * 4 + 5], 0x20, 1, (s32)img, 0x20);
            pl = j;
            while (pl > 0 && s->state == 13 && D_800E37C0[R.b50[pl]].f68C == D_800E37C0[R.b50[pl - 1]].f68C) {
                s32 d0;

                pl--;
            }
            func_8000EE30(pl + 1, buf);
            func_800C1F2C(D.txt[j * 4 + 5], buf);
            x = rx + 0x4E - j * 7;
            func_800C0ACC(D.txt[j * 4 + 5], (s->state == 14 && CHAMP4) ? x - 2 : x, YR);
            func_8000EF8C(buf, D_800CDB78[D.f1C[r]].name);
            func_800C1E60(D.txt[j * 4 + 6], 0x20, 1, (s32)img, 0x20);
            func_800C1F2C(D.txt[j * 4 + 6], buf);
            func_800C0ACC(D.txt[j * 4 + 6], rx - (j * 7 - 0x6A), YR);
            if (r < 4) {
                wid[r] = func_800C1118(D.txt[j * 4 + 6], 0x6A - j * 7, YR, func_800AEC10(buf) - 1) >> 16;
            }
            if (s->state == 7 && D_800CA938[j] != 0) {
                func_800C1E60(D.txt[j * 4 + 7], 0x20, 1, (s32)img, 0x20);
                func_8000EE30(D_800CA938[j], buf + 1);
                buf[0] = '+';
                func_800C1F2C(D.txt[j * 4 + 7], buf);
                func_800C0ACC(D.txt[j * 4 + 7], 0x14C - j * 7, YR);
            }
            func_800C1E60(D.txt[j * 4 + 7], 0x20, 1, (s32)img, 0x20);
            if (s->state == 14 && !CHAMP4) {
                func_8000EE30((*D_800CA930).pts[r], buf);
            } else {
                func_80052F30(D_800E37C0[r].f68C, buf);
            }
            func_800C1F2C(D.txt[j * 4 + 7], buf);
            switch (s->state) {
            case 7:
                func_800C0ACC(D.txt[j * 4 + 7], 0x186 - j * 7, YR);
                break;
            case 13:
                if (D.f4C == 8) {
                    func_800C0ACC(D.txt[j * 4 + 7], 0x170 - j * 7, YR);
                } else {
                    func_800C0ACC(D.txt[j * 4 + 7], 0x17C - j * 7, YR);
                }
                break;
            case 14:
                if (CHAMP4) {
                    func_800C0ACC(D.txt[j * 4 + 7], 0x146 - j * 7, YR);
                } else {
                    func_800C0ACC(D.txt[j * 4 + 7], 0x199 - j * 7, YR);
                }
                break;
            }
        }
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        TDRAW(D.p414, rx + 0x44, ry + 0x1F, 3);
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        if (s->state == 14 && CHAMP4) {
            img = &D.p390[1];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2, img, 6);
        } else if (D.f4C == 8) {
            img = &D.p390[4];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2, img, 6);
        } else {
            img = &D.p390[(s->state == 7) ? 2 : 3];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2 + ry, img, 6);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &D.p39C[D.f3EC / 2 % 60 + 1];
        for (j = 0; j < D.f3C; j++) {
            if (D.f3C >= 2 && D.f4C != 8) {
                gDPLoadTLUT_pal256(gdl++, D.p444 + j * 0x200);
                func_800C1E60(D.txt[R.h22 * 4 + 5 + j], 0x20, 5, 6, 0x21);
                buf[0] = j + '1';
                buf[1] = '\0';
                func_800C1F2C(D.txt[R.h22 * 4 + 5 + j], buf);
                func_800C0ACC(D.txt[R.h22 * 4 + 5 + j], wid[j] + 5 + img->w * 4,
                              (img->h >> 1) + 0x37 + D_800E37C0[j].h668 * 25 + ((s->state != 14) ? 7 : 0));
            } else {
                gDPLoadTLUT_pal256(gdl++, D.p39C->data);
            }
            if (s->state == 14) {
                for (k = 0; k < R.h22; k++) {
                    if ((*D_800CA930).order[k] == j) {
                        break;
                    }
                }
            }
            px = 5 + wid[j + D_800C8CE0] + rx;
            if (s->state != 14 || CHAMP4) {
                y = 0x3E + rowh * D_800E37C0[j + D_800C8CE0].h668 + ry;
            } else {
                y = 0x3E + rowh * k + ry;
            }
            DRAW(px, y, img, 3);
        }
        gdl = func_800C1394(gdl);
        switch (D.f2E8) {
        case 7:
            gdl = func_8007C640(gdl, fb, D_800DA110);
            break;
        case 4:
            gdl = func_8007B39C(gdl, fb, D.f400, D_800CDDB8);
            gdl = func_800C1394(gdl);
            break;
        case 3:
            gdl = func_8007B39C(gdl, fb, D.f400, (D.f4C != 8) ? ((D_800EFD64->f50 & 2) ? D_800CDDDC : D_800CDDD0) : D_800CDDC4);
            gdl = func_800C1394(gdl);
            break;
        }
        gDPPipeSync(gdl++);
        break;
    }
    case 0:
        if (D.f54 != 1) {
            func_80003CC0(m, fb, &gdl);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            CMB_PRIM();
            DRAWV(0x43, 0x14, &D.p38C[1], 6);
            gDPSetPrimColor(gdl++, 0, 0, 0xE1, 0xD7, 0x1E, 0xFF);
            DRAWV(0x43, 0x14, &D.p38C[2], 6);
            gDPSetPrimColor(gdl++, 0, 0, 0xF5, 0xF5, 0xF5, 0xFA);
            DRAWV(0xFE - D.p390[1].w * 4, 200, &D.p390[1], 6);
        }
        func_800C2224();
        gDPPipeSync(gdl++);
        if (D.flags & 0x8000000) {
            func_800C1E60(D.txt[0], 0, 1, 4, 0x11);
            func_800C1F2C(D.txt[0], D_800DA120);
            func_800C0ACC(D.txt[0], D_800F13EC / 2, D_800F13F0 * 0xB6 / 0xF0);
        } else if (D_800E2038 & 0x20) {
            func_800C1E60(D.txt[0], 0, 1, 2, 0x11);
            func_800C1F2C(D.txt[0], D_800DA144);
            func_800C0ACC(D.txt[0], D_800F13EC / 2, D_800F13F0 * 0xB6 / 0xF0);
        }
        gdl = func_800C1394(gdl);
        gDPPipeSync(gdl++);
        break;
    case 2: {
        s32 u7;
        s32 u8;
        s32 u9;

        func_80003CC0(m, fb, &gdl);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        CMB_PRIM();
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        for (i = 0; i < D.f3C; i++) {
            s32 yo;
            s32 bx;
            s32 by;
            s32 x;
            s32 y;

            switch (D.b40C) {
            case 1:
                yo = (0x3E - D.h408) * 2;
                break;
            case 2:
                yo = 0x7C;
                break;
            case 3:
                yo = D.h408 * 2;
                break;
            default:
                yo = 0;
                break;
            }
            yo2 = yo * 190 / 100;
            func_800C2224();
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[D_800C2594 - 1][i].x, VP[D_800C2594 - 1][i].y,
                          VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w - 1,
                          VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h - 1);
            if (D.f3C < 3) {
                if (!(D.flags & (0x400 << i))) {
                    gDPSetTexturePersp(gdl++, G_TP_NONE);
                    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                    CMB_TEX();
                    if (D.f3C == 1) {
                        gDPLoadTLUT_pal256(gdl++, D.p3DC->data);
                        DRAW(VP[D_800C2594 - 1][i].x + 0x10, VP[D_800C2594 - 1][i].y + 5 + yo, &D.p3DC[1], 3);
                        if ((D_800E2038 & 0x20) && D.b40C == 0) {
                            gDPLoadTLUT_pal256(gdl++, D.p74C->data);
                            DRAW(0x28, yo + 0x17, &D.p74C[1], 3);
                            DRAW(0xF5, yo + 0x17, &D.p74C[2], 3);
                        }
                    } else if (D_800E2038 & 0x20) {
                        if (D_800C2388 != 0 && i == 1) {
                            x = 0x120;
                            y = 0x3C;
                        } else {
                            x = 0x41;
                            y = i * 120 + 0x3C;
                        }
                        gDPLoadTLUT_pal256(gdl++, D.p74C->data);
                        DRAW(x, y, &D.p74C[1], 3);
                        DRAW(x | 0x8C, y, &D.p74C[2], 3);
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    gDPSetRenderMode(gdl++, 0x00404240, 0);
                    CMB_PRIM();
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    img = &D.p390[8];
                    if (D.f3C == 2 && i == 0 && D_800C2388 == 0) {
                        s32 dx = yo2 + VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w - img->w * 8;
                        s32 dy = VP[D_800C2594 - 1][i].y;

                        if (D.flags & (0x400 << i)) {
                            dy += 600;
                        }
                        DRAWV(dx, dy, img, 6);
                    } else {
                        s32 dy = VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h - img->h;
                        s32 dx = yo2 + VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w - img->w * 8;

                        if (D.flags & (0x400 << i)) {
                            dy += 600;
                        }
                        DRAWV(dx, dy, img, 6);
                    }
                    img = &D.p390[8];
                    bx = yo2 + VP[D.f3C - 1][i].x + VP[D.f3C - 1][i].w - img->w * 8 + 0x1F;
                    if (D.f3C == 2 && i == 0 && D_800C2388 == 0) {
                        by = VP[D.f3C - 1][i].y + 11;
                    } else {
                        by = VP[D.f3C - 1][i].y + VP[D.f3C - 1][i].h - img->h + 11;
                    }
                    for (j = 1; j < 17; j++) {
                        func_800C1E60(D.txt[j + i * 20], 0x40, 1, 2, 0x20);
                    }
                    for (; j < 20; j++) {
                        func_800C1E60(D.txt[j + i * 20], 0x40, 1, 2, 0x22);
                    }
                }
                func_800C1E60(D.txt[i * 20], 0x40, 5, 4, 0x21);
                func_800C1F2C(D.txt[i * 20], D_800CDB78[D.f1C[i]].f4);
                if (D.f3C == 1) {
                    if (D.flags & (0x400 << i)) {
                        func_800C0ACC(D.txt[i * 20], VP[0][0].x + VP[0][0].w / 2, VP[0][0].y + VP[0][0].h / 2 + 0x46);
                        func_800C1E60(D.txt[i * 20 + 1], 0x40, 5, 2, 0x21);
                        func_800C1F2C(D.txt[i * 20 + 1], D_800DA150);
                        func_800C0ACC(D.txt[i * 20 + 1], VP[0][0].x + VP[0][0].w / 2, VP[0][0].y + VP[0][0].h / 2 + 0x53);
                    } else {
                        func_800C0ACC(D.txt[i * 20], VP[0][0].x + 0x7E, VP[0][0].y + 0xF + yo);
                    }
                } else if (D_800C2388 != 0) {
                    if (D.flags & (0x400 << i)) {
                        func_800C0ACC(D.txt[i * 20], VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w / 2,
                                      VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h - 0x24);
                    } else {
                        func_800C0ACC(D.txt[i * 20], VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w / 2,
                                      VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h / 2);
                    }
                } else {
                    s32 ty = i * 10 + 0x1C;

                    if (D.flags & (0x400 << i)) {
                        func_800C0ACC(D.txt[i * 20], VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w / 2,
                                      VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h / 2 + ty);
                    } else {
                        func_800C0ACC(D.txt[i * 20], VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w / 4 - 10,
                                      VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h / 2 + ty);
                    }
                }
                if (!(D.flags & (0x400 << i))) {
                    for (j = 1; j < 5; j++) {
                        func_800C1F2C(D.txt[i * 20 + j], D_800CDD88[j - 1]);
                        func_800C0ACC(D.txt[i * 20 + j], bx - (j - 1) * 6, by + (j - 1) * 20);
                    }
                    if (D.f3C == 1) {
                        func_8007CB14(i, yo);
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    if (D.f3C == 1) {
                        j = 0;
                        DRAWV(VP[0][0].x, VP[0][0].y + yo, &D.p390[1], 6);
                        img = &D.p390[2];
                        DRAWV(VP[0][0].x, VP[0][0].y - img->h + yo, img, 6);
                        for (j = 0; j < 16; j++) {
                            if (func_8000EDA0(D_800CDB78[D.f1C[i]].f10, D_800C9A40[j].f18) == 0) {
                                break;
                            }
                        }
                        if (D_800C9A40[j].f14 != NULL) {
                            gdl = func_80058294(gdl, fb, D_800C9A40[j].f14, 0x8C, yo + VP[0][0].y - 0x48);
                        }
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    img = &D.p390[6];
                    for (j = 0; j < 4; j++) {
                        s32 ox;
                        s32 oy;

                        ox = j * 6 - 0x55;
                        n = (D_800CDB78[D.f1C[i]].stats[j] + 1) * 2;
                        switch (n >> 1) {
                        case 5:
                            gDPSetPrimColor(gdl++, 0, 0, 0x14, 0xC8, 0x14, 0xFF);
                            break;
                        case 4:
                            gDPSetPrimColor(gdl++, 0, 0, 0x96, 0xB4, 0x14, 0xFF);
                            break;
                        case 3:
                            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0x14, 0xFF);
                            break;
                        case 2:
                            gDPSetPrimColor(gdl++, 0, 0, 0xBE, 0x91, 0x14, 0xFF);
                            break;
                        case 1:
                            gDPSetPrimColor(gdl++, 0, 0, 0xC8, 0x14, 0x14, 0xFF);
                            break;
                        }
                        c = 0;
                        oy = j * 20 - 7;
                        for (; c < 10; c++) {
                            rowh = n;
                            ry = n;
                            rx = n;
                            t = n;
                            if (c == n) {
                                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0x28, 0x28, 0xFF);
                            }
                            DRAW(bx - ox + c * 9, by + oy, img, 6);
                        }
                    }
                }
            } else {
                s32 u10;
                s32 cx;
                s32 cy;
                s32 u11;
                s32 msk;
                s32 nn;

                gDPSetTextureLUT(gdl++, G_TT_NONE);
                gDPSetRenderMode(gdl++, 0x00404240, 0);
                CMB_PRIM();
                if (!(D.flags & (0x400 << i))) {
                    img = &D.p390[7];
                    for (j = 0; j < 4; j++) {
                        nn = (D_800CDB78[D.f1C[i]].stats[j] + 1) * 2;
                        nn &= ~1;
                        switch (nn >> 1) {
                        case 5:
                            gDPSetPrimColor(gdl++, 0, 0, 0x14, 0xC8, 0x14, 0xFF);
                            break;
                        case 4:
                            gDPSetPrimColor(gdl++, 0, 0, 0x96, 0xB4, 0x14, 0xFF);
                            break;
                        case 3:
                            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0x14, 0xFF);
                            break;
                        case 2:
                            gDPSetPrimColor(gdl++, 0, 0, 0xBE, 0x91, 0x14, 0xFF);
                            break;
                        case 1:
                            gDPSetPrimColor(gdl++, 0, 0, 0xC8, 0x14, 0x14, 0xFF);
                            break;
                        }
                        switch (j) {
                        case 0:
                            cx = VP[D_800C2594 - 1][i].x + 0x12;
                            cy = VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h - 0x1A;
                            break;
                        case 1:
                            cx = VP[D_800C2594 - 1][i].x + 0x74;
                            cy = VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h - 0x1A;
                            break;
                        case 2:
                            cx = VP[D_800C2594 - 1][i].x + 0xF;
                            cy = VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h - 0xD;
                            break;
                        case 3:
                            cx = VP[D_800C2594 - 1][i].x + 0x71;
                            cy = VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h - 0xD;
                            break;
                        }
                        for (c = 0; c < 10; c++) {
                            if (c == nn) {
                                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0x28, 0x28, 0xFF);
                            }
                            DRAWV(cx + c * 9, cy, img, 6);
                        }
                    }
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    img = &D.p390[10];
                    DRAWV(VP[D_800C2594 - 1][i].x, VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h - img->h, img, 6);
                }
                func_800C1E60(D.txt[i * 4], 0x40, 5, 4, 0x21);
                func_800C1F2C(D.txt[i * 4], D_800CDB78[D.f1C[i]].f4);
                {
                    s32 yy = VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h / 2;
                    s32 xx = VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w / 2;
                    s32 ty2;

                    msk = 0x400 << i;
                    ty2 = yy + 0xA;
                    if (D.flags & msk) {
                        ty2 = yy + 0x23;
                    }
                    func_800C0ACC(D.txt[i * 4], xx, ty2);
                }
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                CMB_TEX();
                if (!(D.flags & msk) && (D_800E2038 & 0x20)) {
                    gDPLoadTLUT_pal256(gdl++, D.p74C->data);
                    DRAWK(VP[D_800C2594 - 1][i].x + 5, VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h / 2 + 3, &D.p74C[1], 3, u10);
                    DRAWK(VP[D_800C2594 - 1][i].x + VP[D_800C2594 - 1][i].w - 0x14,
                         VP[D_800C2594 - 1][i].y + VP[D_800C2594 - 1][i].h / 2 + 3, &D.p74C[2], 3, u11);
                }
            }
            if (D.f3C < 3) {
                gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[D_800C2594 - 1][0].x, VP[D_800C2594 - 1][0].y,
                              VP[D_800C2594 - 1][D_800C2594 - 1].x + VP[D_800C2594 - 1][D_800C2594 - 1].w - 1,
                              VP[D_800C2594 - 1][D_800C2594 - 1].y + VP[D_800C2594 - 1][D_800C2594 - 1].h - 1);
            }
            gdl = func_800C1394(gdl);
            gDPPipeSync(gdl++);
        }
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[0][0].x, VP[0][0].y, VP[0][0].x + VP[0][0].w - 1,
                      VP[0][0].y + VP[0][0].h - 1);
        gDPPipeSync(gdl++);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        CMB_PRIM();
        if (D.f3C == 1) {
            if (D.f4C != 8) {
                func_800C1E60(D.txt[0], 0x40, 2, 2, 0x20);
                func_800C1F2C(D.txt[0], D_800DA170);
                func_800C0ACC(D.txt[0], 0x16C, 0x25);
            }
            func_800C1E60(D.txt[1], 0x40, 2, 2, 0x20);
            func_800C1F2C(D.txt[1], D_800DA184);
            func_800C0ACC(D.txt[1], 0x16C, 0x19);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            CMB_TEX();
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gDPLoadTLUT_pal256(gdl++, D.p3C8->data);
            if (D.f4C != 8) {
                DRAWK(0x140, 0x21, &D.p3C8[1], 3, u9);
                DRAW(0x154, 0x21, &D.p3C8[3], 3);
            }
            DRAWK(0x140, 0x13, &D.p3C8[4], 3, u7);
            DRAWK(0x154, 0x13, &D.p3C8[2], 3, u8);
        }
        gdl = func_800C1394(gdl);
        break;
    }
    }
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    if (D.f54 != 3) {
        gdl = func_80072F70(gdl, m, D_800E30A8);
    }
    {
        Gfx *_g = gdl++;

        _g->words.w0 = 0xE7000000;
        _g->words.w1 = ((u16)u1 >> 16) + ((u16)u2 >> 16) + ((u16)u3 >> 16);
    }
    return gdl;
}
