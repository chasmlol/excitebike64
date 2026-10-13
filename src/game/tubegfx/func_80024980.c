#include "ctx.h"

typedef struct { u8 pad[0x40]; } Obj40;

typedef struct {
    s32 a, b, c, d;
    u8 pad10[8];
    s32 f18;
    s32 pad1C;
} MEnt;

typedef struct {
    u32 n;
    u32 pad4;
    MEnt e[1];
} Mdl;

typedef struct { s32 x, y, z, w; } V4;

typedef struct {
    s32 f0;
    u16 *pal;
} Pal;

extern Obj40 *D_800E7798[];
extern char D_800D6A50[];
extern char D_800D6A5C[];
extern char D_800D6A60[];
extern s32 D_800E7820;
extern s32 D_800C23F0;
extern s32 D_800D1508, D_800D1504, D_800D1510, D_800D1500, D_800D14FC, D_800D14E0;
extern s32 D_800D14DC, D_800D14E4, D_800D1518, D_800D14F4, D_800D1528;
extern u32 D_800C2280;
extern s32 D_800E7728;
extern Pal *D_800E781C;
extern u32 D_800E77C4, D_800E77F8, D_800E7824, D_800E7828, D_800E78A0;
extern s32 *D_800E7834;
extern V4 *D_800E7814;
extern void *D_800E77D4;
extern u32 D_800E7808[];
extern u32 D_800E783C;
extern Mdl *D_800C9640, *D_800C9644, *D_800C9648;
extern u32 D_800C9574;

void *func_800BC908(u32 size, s32 a1, char *file, s32 line);
void func_80011490(Obj40 *p, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);
void func_800BFC40(u8 *p, s32 f);
void func_80031D08(void);
void func_800248EC(void);
u32 func_8005AED8(s32 key);
void func_80028330(Mdl *p);
u32 func_80027574(Mdl *a, u32 *b, s32 c, s32 d);
s32 func_8005B58C(s32 key);
void func_80027B7C(void);
void func_80066494(Mdl *p);
void func_80072AD0(Mdl *a, u32 b);
void func_8002DA60(Mdl *a, Mdl *b, u32 c, u32 d);
void func_800270F8(Mdl *a, Mdl *b);
void func_80065B10(s32 a0, s32 a1, s32 a2, Mdl *a3, Pal *a4);

void func_80024980(void)
{
    s32 i, j;
    u32 *p;

    for (i = 0; i < 2; i++) {
        D_800E7798[i] = func_800BC908(0x4B00, 0, D_800D6A50, 0xCA);
        for (j = 0; j < 300; j++) {
            func_80011490(&D_800E7798[i][j], 0, 0, 0, 0, 0, 0, 0x100);
        }
    }
    D_800E7820 = -1;
    D_800C23F0 = 8000;
    func_800BFC40((u8 *)D_800D6A5C, (s32)func_800248EC);
    func_800BFC40((u8 *)D_800D6A60, (s32)func_80031D08);
    D_800E781C = (Pal *)func_8005AED8(D_800D1508);
    p = &D_800E783C;
    *p = func_8005AED8(D_800D1504);
    D_800E7824 = func_8005AED8(D_800D1510);
    D_800E77F8 = func_8005AED8(D_800D1500);
    D_800E77C4 = func_8005AED8(D_800D14FC);
    D_800C9640 = (Mdl *)func_8005AED8(D_800D14E0);
    D_800E7808[0] = (u32)D_800E781C;
    D_800E7808[1] = D_800E77C4;
    D_800E7808[2] = D_800E77F8;
    D_800E7828 = func_80027574(D_800C9640, D_800E7808, 0x4C, 1);
    D_800C9644 = (Mdl *)func_8005AED8(D_800D14DC);
    D_800E78A0 = func_80027574(D_800C9644, p, 0, 0);
    D_800C9648 = (Mdl *)func_8005AED8(D_800D14E4);
    if (D_800C2280 & 0x80) {
        func_80028330(D_800C9644);
        func_80028330(D_800C9640);
    }
    D_800E7834 = func_800BC908(D_800C9640->n * 4, 0, D_800D6A50, 0xF9);
    for (i = 0; i < D_800C9640->n; i++) {
        D_800E7834[i] = D_800C9640->e[i].f18;
    }
    func_80066494(D_800C9640);
    D_800E77D4 = func_800BC908(D_800C9644->n * 16, 0, D_800D6A50, 0x103);
    func_80027B7C();
    for (i = 0; i < 256; i++) {
        if (D_800E781C->pal[i] == 0xF83F) {
            D_800E781C->pal[i] = 0;
        }
    }
    p = &D_800C9574;
    *p = func_8005AED8(D_800D1518);
    func_80072AD0(D_800C9640, *p);
    func_8002DA60(D_800C9644, D_800C9640, *p, func_8005AED8(D_800D14F4));
    D_800E7814 = func_800BC908(D_800C9644->n * 16, 0, D_800D6A50, 0x11D);
    for (i = 0; i < D_800C9644->n; i++) {
        D_800E7814[i].x = D_800C9644->e[i].a;
        D_800E7814[i].z = D_800C9644->e[i].b;
        D_800E7814[i].y = D_800C9644->e[i].c;
        D_800E7814[i].w = D_800C9644->e[i].d;
    }
    func_800270F8(D_800C9640, D_800C9644);
    if (D_800C9648 != NULL) {
        func_800270F8(D_800C9648, D_800C9644);
    }
    func_80065B10(D_800E7728, func_8005B58C(D_800D1528), 0, D_800C9644, D_800E781C);
}
