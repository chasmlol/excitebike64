#include "ctx.h"

typedef struct {
    s32 flags;
    u8 pad4[0x18];
    s32 a1C[8];
    s32 f3C;
    u8 pad40[0xC];
    u32 mode;
    u8 pad50[0x7AA - 0x50];
    u8 b7AA[6];
} Game;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x20];
    s32 f50;
    u8 c54, c55, c56;
    u8 pad57[0xA0 - 0x57];
} Track;

typedef struct {
    s32 w[16];
} Blk64;

typedef struct {
    u8 pad0[0x21C];
    u8 rgb[3];
    u8 pad21F[0xA60 - 0x21F];
    u16 pal[16];
} PalEnt;

typedef struct {
    u8 pad0[0x10];
    u8 f10[0x30];
    u8 f40[0x40];
} Ent80;

typedef struct {
    u8 pad0[0x28];
} Ent28;

typedef struct {
    void *p[2];
} Pair;

typedef struct {
    u8 pad0[0xC];
    u8 r, g, b, a;
} Col16;

typedef struct {
    u32 count;
    Col16 *items;
} ColList;

typedef struct {
    s32 f0;
    u16 *f4;
    s32 f8;
    u8 *fC;
} Img;

typedef struct {
    s32 f0;
    u16 *f4;
} Res;

extern Game D_800EFD70;
extern Track *D_800EFD64;
extern Track D_800CB210[];
extern s32 D_800C2280;
extern s32 D_800C2350;
extern s32 D_800C2358;
extern s32 D_800C235C;
extern s32 D_800C2360;
extern s32 D_800C2364;
extern s32 D_800C2368[6];
extern s32 D_800C2384;
extern s32 D_800C2388;
extern s32 *D_800C23B0[6];
extern Blk64 D_800C23F4;
extern Blk64 D_800C2434;
extern Blk64 D_800C24D4;
extern s32 D_800C2594;
extern s32 *D_800C259C[12];
extern s32 D_800C25CC[6];
extern Ent28 D_800C26C8[];
extern s32 D_800C27A8;
extern s32 D_800C27AC;
extern s32 D_800C27B0;
extern u8 *D_800C27B4;
extern s32 D_800C29E0;
extern s32 D_800C8CD8;
extern s32 D_800C8CE8;
extern s32 D_800C98B0;
extern s32 D_800CCD64;
extern s32 D_800D0848;
extern s32 D_800D0920;
extern s32 D_800D0950;
extern s32 D_800D098C;
extern s32 D_800D0D04;
extern s32 D_800D0DA4;
extern s32 D_800D10B8;
extern s32 D_800D1130;
extern s32 D_800D150C;
extern s32 D_800D152C;
extern char D_800D5EBC[];
extern void *D_800DC0A0;
extern u8 *D_800DC0A4;
extern void *D_800DC0A8;
extern s32 D_800DC0B0;
extern s32 D_800DC0B4;
extern u8 D_800DC0E0[3];
extern void *D_800DC110[6][2];
extern Res *D_800DC140;
extern Img *D_800DC144;
extern s32 D_800DC148;
extern PalEnt D_800DC150[8];
extern u8 D_800DC158[];
extern u8 *D_800E1550;
extern u8 *D_800E1558;
extern void *D_800E155C;
extern void *D_800E1560;
extern u8 D_800E1568[];
extern void *D_800E15D8;
extern u8 D_800E15E0[];
extern void *D_800E16C0[6];
extern u8 *D_800E1878;
extern s32 D_800E1880[6];
extern s32 D_800E1898;
extern Pair D_800E1900[6];
extern s32 D_800E19BC;
extern ColList *D_800E19C8;
extern s32 D_800E19CC;
extern Ent80 *D_800E1A78;
extern u8 D_800E1A90[];
extern s32 D_800E1B48;
extern s32 D_800E7728;
extern s16 D_800F05CA;
extern s32 D_800F05E4;
extern s32 D_800F14E8;
extern void (*D_80105368)();

void func_8000BB34();
void func_80035630(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8000BA58(void);
void func_80014C14(u8 *p, s32 stride, s32 rows);
void func_80014A80(void);
void func_8003C720(u8 *tbl, u32 *list);
u32 func_8005AED8(s32 key);
void func_8006EE58(void);
void func_80000450(s32, s32, Ent28 *, s32, s32, s32);
void func_80000F0C(void);
void func_80011A58(void *, s32, s32, s32);
s32 func_80013D70(s32, void *, s32, s32);
void func_80014948(void *);
void func_80014AE4(void *, void *, u8 *, s32);
void func_80014B84(s32 *, void *, u8 *, s32);
void func_80022BD0(void);
void func_80024980(void);
void func_80032150(void);
void *func_8003C2E0(s32, s32);
void func_8003C4A4(void *, s32);
void func_8003C7A8(u8 *, s32);
void func_8003F6A0(void);
void func_80052C90(void);
void func_80069CA0(Img *, s32);
void func_8006A9DC(s32, s32 *, s32 *, s32 *);
void func_8006EEC4(void);
void func_800AEC34(void *, s32, s32);
void *func_800BC908(s32, s32, char *, s32);
void *func_800BF79C(s32, s32);
void func_800C0ACC(void *, s32, s32);
s32 func_800C1E60(void *, s32, s32, s32, s32);
void func_800C1F2C(void *, s32);

void func_8000115C(void) {
    u8 buf[0x60];
    s32 a, b, c;
    s32 i, j, k, n, size;
    void *p;
    u32 r, g, bl, m;
    s32 idx;
    s32 x, y, z;

    if (D_800EFD70.flags & 0x40000) {
        D_800C2358 = 1;
    } else {
        D_800C2358 = 0;
    }
    if (D_800C2280 & 1) {
        D_800C2360 = 1;
    } else {
        D_800C2360 = 0;
    }
    if (D_800C2280 & 0x2000) {
        D_800C235C = 1;
    } else {
        D_800C235C = 0;
    }
    func_8000ED10(D_800E1A90, 1, 8);
    if (D_800CCD64 != 0) {
        i = 0;
        D_800C27B4 = func_800BF79C(D_800E7728, D_800D098C);
        D_800E19BC = 0xF0;
        D_800DC0B0 = 0;
        while (D_800C27B4[i] != '@' || D_800C27B4[i + 1] != 'E') {
            if (D_800C27B4[i++] == '\r') {
                D_800DC0B0++;
            }
        }
        D_800DC0B0 += 3;
        D_800C27AC = func_800C1E60(NULL, 0x40, 1, 4, 0);
        D_800C27B0 = func_800C1E60(NULL, 0x40, 1, 2, 0);
        D_800E1560 = func_800BF79C(D_800E7728, D_800D0950);
        func_80014948(D_800E1560);
    }
    func_80000F0C();
    D_800DC140 = func_800BF79C(D_800E7728, D_800D0D04);
    func_80014948(D_800DC140);
    *D_800DC140->f4 = 0;
    D_80105368 = func_8000BB34;
    D_800E1558 = func_800BC908(0xC00, 0, D_800D5EBC, 0x349);
    func_8000ED10(D_800E1558, 0, 0xC00);
    for (i = 0; i < 6; i++) {
        D_800E1550 = D_800E1558 + i * 0x200;
        func_800C1E60(buf, 3, 1, 1, 0x08000000);
        func_800C1F2C(buf, D_800C2368[i]);
        func_800C0ACC(buf, 0, 0);
    }
    D_80105368 = NULL;
    D_800F14E8 = 0;
    if (D_800C2388 != 0) {
        D_800C24D4 = D_800C23F4;
    } else {
        D_800C24D4 = D_800C2434;
    }
    func_8000BA58();
    D_800DC0A8 = func_800BC908(0x100, 0, D_800D5EBC, 0x359);
    D_800C27A8 = 0;
    if (D_800C2364 != 0) {
        D_800E19CC = 0x200;
    } else {
        D_800E19CC = 0x400;
    }
    D_800DC0B4 = 8;
    D_800E15D8 = func_800BF79C(D_800E7728, D_800D0848);
    if (D_800EFD64->f2C != 2) {
        if (D_800C8CE8 != 0 || (D_800EFD70.flags & 0x80) || D_800C8CD8 != 0) {
            D_800C2594 = 1;
        } else {
            D_800C2594 = D_800EFD70.f3C;
        }
    }
    if (D_800EFD64->f2C == 2) {
        D_800C2594 = 1;
    }
    D_800DC148 = 5;
    D_800E1898 = D_800C2594;
    func_8000ED10(D_800E15E0, 0, 0x20);
    func_80014A80();
    if (D_800EFD64->f2C == 0) {
        func_80022BD0();
    } else if (D_800EFD64->f2C == 1) {
        func_80024980();
    } else if (D_800EFD64->f2C == 2) {
        func_8003F6A0();
    }
    D_800DC0A0 = func_800BF79C(D_800E7728, D_800D10B8);
    func_80014948(D_800DC0A0);
    D_800DC0A4 = func_800BC908(0x400, 0, D_800D5EBC, 0x38B);
    func_8000ED10(D_800DC0A4, 0, 0x400);
    switch (D_800EFD70.mode) {
    case 2:
        func_8006EE58();
        break;
    case 0:
    case 13:
    case 14:
        if (!(D_800EFD64->f50 & 2)) {
            break;
        }
    case 9:
        func_8006EEC4();
        break;
    }
    p = func_8003C2E0(D_800E7728, D_800D0DA4);
    D_800E1A78 = p;
    for (i = 0; i < 15; i++) {
        func_80011A58(D_800E1A78[i].f10, 0, 0, 0);
        func_80011A58(D_800E1A78[i].f40, 0, 0, 0);
    }
    func_8003C4A4(D_800E1A78, 15);
    if (D_800EFD64->f2C == 0) {
        j = i;
    } else if (D_800EFD64->f2C == 1) {
        j = n;
    }
    func_8000ED10(D_800E1568, 0, 0x70);
    func_8000ED10((u8 *)D_800DC150, 0, 0x5400);
    n = 8;
    if (D_800EFD70.f3C >= 2 && !(D_800EFD70.flags & 0x10)) {
        n = 4;
    }
    if ((D_800EFD70.flags & 1) && D_800F05CA == 1) {
        n = 2;
    }
    if (D_800C2358 != 0) {
        n = D_800F05CA;
    }
    if (D_800CCD64 != 0) {
        n = 1;
    }
    for (i = 0; i < 6; i++) {
        D_800E1880[i] = (s32)((Res *)func_800BF79C(D_800E7728, *D_800C23B0[i]))->f4;
    }
    for (j = 0; j < n; j++) {
        if (D_800EFD70.flags & 0x10) {
            if (j >= 6) {
                break;
            }
            i = j;
        } else {
            i = 4;
            if (j < 6) {
                i = D_800EFD70.a1C[j];
            }
        }
        func_80000450(D_800E7728, j, &D_800C26C8[i], D_800EFD70.b7AA[j % 6] & 1, D_800EFD70.b7AA[j % 6] >> 1, i);
    }
    if (!(D_800EFD70.flags & 0x81)) {
        func_80052C90();
    }
    for (i = 0; i < 8; i++) {
        D_800DC150[i].pal[0] = 0;
        for (j = 1; j < 8; j++) {
            x = D_800DC150[i].rgb[0] * j / 7 >> 3;
            y = D_800DC150[i].rgb[1] * j / 7 >> 3;
            z = D_800DC150[i].rgb[2] * j / 7 >> 3;
            D_800DC150[i].pal[j] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        for (j = 0; j < 8; j++) {
            x = D_800DC150[i].rgb[0];
            y = D_800DC150[i].rgb[1];
            z = D_800DC150[i].rgb[2];
            x = (x + (255 - x) * j / 7) >> 3;
            y = (y + (255 - y) * j / 7) >> 3;
            z = (z + (255 - z) * j / 7) >> 3;
            D_800DC150[i].pal[j + 8] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
    }
    func_8000ED10((u8 *)D_800E1900, 0, 0x30);
    func_8000ED10((u8 *)D_800DC110, 0, 0x30);
    func_8000ED10((u8 *)D_800E16C0, 0, 0x18);
    D_800E1B48 = 0;
    D_800E1878 = func_8003C2E0(D_800E7728, D_800D0920);
    func_8003C7A8(D_800E1878, 10);
    if (D_800C2358 == 0) {
        for (i = 0; i < 6; i++) {
            D_800E1900[i].p[1] = func_800BF79C(D_800E7728, *D_800C259C[i]);
            D_800E1900[i].p[0] = func_800BF79C(D_800E7728, *D_800C259C[i + 6]);
        }
        D_800E155C = func_800BF79C(D_800E7728, D_800D1130);
        func_80014B84(&D_800E1B48, D_800E155C, D_800DC158, 0x20);
        for (i = 0; i < 6; i++) {
            func_8003C720(D_800E1878 + D_800C25CC[i] * 0x80, D_800E1900[i].p[1]);
            func_8003C720(D_800E1878 + D_800C25CC[i] * 0x80, D_800E1900[i].p[0]);
        }
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 2; j++) {
                func_80014AE4(&D_800DC110[i][j], D_800E1900[i].p[j], D_800DC158, 0);
            }
            size = func_80013D70(3, D_800E1900[i].p[1], 0, D_800C29E0) - D_800C29E0;
            D_800E16C0[i] = func_800BC908(size, 0, D_800D5EBC, 0x434);
            func_800AEC34(D_800E16C0[i], D_800C29E0, size);
        }
    }
    D_800E19C8 = (ColList *)func_8005AED8(D_800D152C);
    D_800DC144 = (Img *)func_8005AED8(D_800D150C);
    if (D_800E19C8 != NULL) {
        if (D_800DC144 != NULL) {
            D_800C98B0 = 3;
        } else {
            D_800C98B0 = 2;
        }
    } else if (D_800DC144 != NULL) {
        D_800C98B0 = 1;
        func_80014C14(D_800DC144->fC, 0x100, 0x100);
    } else {
        D_800C98B0 = 0;
    }
    if (D_800DC144 != NULL) {
        func_80069CA0(D_800DC144, 0);
    }
    func_80032150();
    func_80035630();
    if (D_800DC144 != NULL) {
        D_800DC0E0[0] = (*D_800DC144->f4 >> 11) << 3;
        D_800DC0E0[1] = ((*D_800DC144->f4 >> 6) & 0x1F) << 3;
        D_800DC0E0[2] = ((*D_800DC144->f4 >> 1) & 0x1F) << 3;
        if (D_800CCD64 != 0) {
            a = (D_800DC0E0[0] + D_800DC0E0[1] + D_800DC0E0[2]) / 3;
            func_8006A9DC(a, &a, &b, &c);
            D_800DC0E0[0] = a;
            D_800DC0E0[1] = b;
            D_800DC0E0[2] = c;
        }
    } else if (D_800E19C8 != NULL) {
        r = 0;
        g = 0;
        bl = 0;
        for (i = 0; i < D_800E19C8->count; i++) {
            r += D_800E19C8->items[i].r;
            g += D_800E19C8->items[i].g;
            bl += D_800E19C8->items[i].b;
        }
        r /= D_800E19C8->count;
        g /= D_800E19C8->count;
        bl /= D_800E19C8->count;
        D_800DC0E0[0] = r;
        D_800DC0E0[1] = g;
        D_800DC0E0[2] = bl;
    } else if (D_800EFD64->f2C == 1) {
        D_800DC0E0[0] = D_800EFD64->c54;
        D_800DC0E0[1] = D_800EFD64->c55;
        D_800DC0E0[2] = D_800EFD64->c56;
    } else {
        D_800DC0E0[0] = 0;
        D_800DC0E0[1] = 0;
        D_800DC0E0[2] = 0;
    }
    idx = D_800EFD64 - D_800CB210;
    if (idx == 3 || idx == 20) {
        D_800DC144 = NULL;
        D_800C98B0 = 0;
    }
    if (D_800C2280 & 0x40) {
        D_800DC144 = NULL;
        D_800C98B0 = 0;
        func_8000ED10(D_800DC0E0, 0, 3);
    }
    if (D_800F05E4 != 0) {
        D_800C2350 &= ~0x800;
    } else {
        D_800C2350 |= 0x800;
    }
    D_800C2384 = -1;
    func_8000BA58();
}
