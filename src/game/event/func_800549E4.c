#include "ctx.h"

typedef struct { s32 a, b, c, d; } Q4;
typedef struct {
    u8 pad0[0x3FF]; u8 f3FF;
    u8 pad1[0x420 - 0x400]; f32 f420;
    u8 pad2[0x668 - 0x424]; s16 f668;
    u8 pad3[0x7E0 - 0x66A];
} Rec37;
typedef struct { s32 f0; u8 pad[0x22]; s16 f26; } Obj5A8;

extern Rec37 D_800E37C0[];
extern Q4 D_800E1718[][4];
extern s32 D_800E1898;
extern u8 D_800E3BC2;
extern s32 D_800CCCA8;
extern s32 D_800CA934;
extern s32 *D_800F0544;
extern s32 *D_800F0548;
extern s32 D_800F05D4;
extern s32 *D_800F0530[];
extern s32 *B_800F0530[];
extern Obj5A8 D_800F05A8;
extern char D_800D8484[];
extern char D_800D84A8[];
extern char D_800D850C[];

Gfx *func_80009754(Gfx *p, s32 a, s32 b, s32 c);
s32 func_800C2224(void);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
Gfx *func_80055658(Gfx *p, s32 a, s32 *b);
void func_800C1F2C(s32 *h, char *s);
void func_800C0ACC(s32 *h, s32 x, s32 y);
void func_80052F30(s32 v, char *s);
s32 func_800AEC10(char *s);
Gfx *func_800C1394(Gfx *p);
Gfx *func_80054ED0(Gfx *p, s32 a);
void func_800532D4(s32 *h, s32 v, s32 w);

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_800549E4(Gfx *gdl, s32 a1, s32 a2, s32 idx)
{
    char buf[32];
    f32 fv;
    s32 y;
    s32 *h;
    s32 n;
    s32 x;
    s32 yy;
    s32 z;
    char *p;

    PKT(0xE7000000, 0);
    PKT(0xE3000C00, 0);
    PKT(0xE3001001, 0);
    PKT(0xFC119623, 0xFF2FFFFF);
    PKT(0xE200001C, 0x00404240);
    PKT(0xD9FFFFFE, 0);
    PKT(0xD7000002, 0x80008000);
    gdl = func_80009754(gdl, a1, a2, idx);
    func_800C2224();
    gdl = func_80055658(gdl, 0, func_800C1E60(D_800F0544, 0x20, 3, 2, 0x21));

    fv = D_800E37C0[idx].f420;
    func_8000EE30((s32)(((0.0f < fv) ? fv * 3600.0f : -fv * 3600.0f) / 1612.9f), buf);

    func_800C1E60(D_800F0530[0], 0x40, 3, 2, 0x12);
    func_800C1F2C(D_800F0530[0], buf);
    x = 0x1BA;
    if (*(volatile s32 *)&D_800CCCA8 == 2) x = 0x228;
    yy = 0xD6;
    if (*(volatile s32 *)&D_800CCCA8 == 2) yy = 0x180;
    func_800C0ACC(D_800F0530[0], x, yy);

    if (D_800CA934 <= 0) {
        func_800C1E60(D_800F0530[1], 0x40, 1, 2, 0x20);
        func_800C1E60(D_800F0530[2], 0x40, 1, 4, 0x20);
        func_80052F30(D_800F05D4, buf);
        y = D_800E1718[D_800E1898 - 1][idx].b;
        func_800C1F2C(D_800F0530[1], D_800D8484);
        z = y + 0x14;
        func_800C0ACC(D_800F0530[1], 0x23, z);
        func_800C1F2C(D_800F0530[2], buf);
        func_800C0ACC(D_800F0530[2], 100, z);
        func_800C1E60(D_800F0530[3], 0x40, 1, 2, 0x20);
        func_800C1E60(D_800F0530[4], 0x40, 1, 4, 0x20);
        func_8000EE30(D_800E3BC2 + 1, buf);
        n = func_800AEC10(buf);
        func_8000EE30(10, buf + n + 1);
        p = buf;
        n = func_800AEC10(p);
        p += n;
        *p = '/';
        if (D_800CCCA8 == 2) z = y + 0x2C;
        else z = y + 0x20;
        func_800C1F2C(B_800F0530[3], D_800D850C);
        func_800C0ACC(B_800F0530[3], 0x23, z);
        func_800C1F2C(B_800F0530[4], buf);
        func_800C0ACC(B_800F0530[4], 100, z);
    }

    if (D_800E37C0[0].f668 != -1) {
        h = func_800C1E60(D_800F0548, 0x20, 1, 4, 0x21);
        if (D_800E37C0[0].f3FF == 0) {
            func_800C1F2C(h, D_800D84A8);
            func_800C0ACC(h,
                          D_800E1718[D_800E1898 - 1][idx].a + D_800E1718[D_800E1898 - 1][idx].c / 2,
                          D_800E1718[D_800E1898 - 1][idx].b + D_800E1718[D_800E1898 - 1][idx].d / 2);
        } else {
            func_800532D4(h, D_800E37C0[0].f668, idx);
        }
    }

    gdl = func_80055658(gdl, 0, func_800C1E60(D_800F0544, 0x20, 3, 2, 0x21));
    gdl = func_800C1394(gdl);
    if (D_800F05A8.f26 != 0 && D_800F05A8.f0 != 0 &&
        (D_800F05A8.f0 != 1 || D_800F05A8.f26 < 0x5B)) {
        gdl = func_80054ED0(gdl, idx);
    }
    return gdl;
}
