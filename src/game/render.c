#include "game.h"

#define G(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u16 c[256]; } Pal_3;
typedef struct { s32 pad0; Pal_3 *pal; } Img_3;
typedef struct { void *a[2]; void *b[2]; } Rec16;

typedef struct {
    u8 pad0[0xC];
    u8 r, g, b, a;
} Col16;

typedef struct {
    u32 count;
    Col16 *items;
} ColList;

typedef struct { u32 w0; u32 img; } TexHdr;

extern Rec16 D_800E1568[];
extern Img_3 *D_800E1600[][6];
extern Pal_3 *D_800E1880[];
extern ColList *D_800E19C8;

typedef struct { s32 *f0, *f4, *f8, *fC; u8 pad[0x18]; } Ent28;

typedef struct {
    void *f0;
    void *f4;
    Img_3 *f8;
    Pal_3 *fC;
    Pal_3 *f10;
    Pal_3 *f14;
    Pal_3 *f18;
    u16 pal[256];
    Rgb col;
    u8 pad21F;
    void *f220;
    void *a224[2];
    void *a22C[2];
    void *a234[2];
    void *b23C[2];
    u8 rest[0xA80 - 0x244];
} Plr;

typedef struct {
    u32 flags;
    u8 pad4[0x18];
    s32 w1C[4];
    u8 pad2C[0x43C - 0x2C];
    u8 b43C[8];
} Glob;

extern Ent28 D_800C26B8[];
extern Plr D_800DC150_2[];
extern Glob D_800EFD70_10;

void func_80000450(ResFile *r, s32 n, s32 **ids, s32 arg3, s32 arg4, s32 arg5)
{
    s32 buf[2];
    void *tmp;
    Ent28 *e;
    s32 c;
    s32 i;
    s32 k;
    s32 id;
    u32 *m;
    Rgb *col;
    void *x;
    s32 len;

    e = &D_800C26B8[arg5];
    if (D_800C2358 == 0) {
        if (arg5 == 0) {
            func_800712E4(n);
        }
        D_800DC150_2[n].f8 = func_800BF79C(r, *D_800C23C8[arg3]);
    }
    if (D_800EFD70_10.flags & 0x10) {
        D_800DC150_2[n].f18 = func_800BC908(0xC00, 0, D_800D5EBC, 0x1FF);
    } else {
        D_800DC150_2[n].f14 = func_800BC908(0x200, 0, D_800D5EBC, 0x203);
    }
    if (!(D_800EFD70_10.flags & 0x10)) {
        c = D_800EFD70_10.b43C[n];
    } else {
        c = D_800EFD70_10.w1C[n & 3] & 3;
    }
    col = &D_800DC150_2[n].col;
    func_8000ED10((u8 *)col, 0xFF, 3);
    k = func_800C0174(r, *ids[c]);
    if (D_800EFD70_10.flags & 0x10) {
        for (i = 0; i < 6; i++) {
            D_800E1600[n][i] = func_800BC908(k, 0, D_800D5EBC, 0x220);
            tmp = func_800BF79C(r, *ids[i]);
            func_80014C80((u32 *)&tmp);
            func_800AEC34(D_800E1600[n][i], tmp, k);
            func_800BFEA4(&r->tbl[*ids[i]]);
            func_80014CCC((u32 *)&D_800E1600[n][i]);
        }
        D_800DC150_2[n].f0 = D_800E1600[n][0];
        D_800DC150_2[n].fC = 0;
    } else if (n >= 6) {
        D_800DC150_2[n].f0 = func_800BF79C(r, D_800D0C88);
        D_800E1A80[0] = ((Img_3 *)func_800BF79C(r, D_800D0C88))->pal;
        D_800E1A80[1] = ((Img_3 *)func_800BF79C(r, D_800D06E8))->pal;
        D_800E1A80[2] = ((Img_3 *)func_800BF79C(r, D_800D06FC))->pal;
        D_800DC150_2[n].fC = D_800E1A80[n % 3];
    } else {
        tmp = func_800BF79C(r, *ids[c]);
        D_800DC150_2[n].f0 = tmp;
        D_800DC150_2[n].fC = ((Img_3 *)tmp)->pal;
        *col = D_800E15D8[c];
    }
    if (D_800C2358 != 0) {
        if (n < 6) {
            D_800DC150_2[n].f4 = func_800BF79C(r, *e->f4);
        } else {
            D_800DC150_2[n].f4 = func_800BF79C(r, *D_800C26B8[0].f4);
        }
    }
    if (D_800EFD70_10.flags & 0x10) {
        for (i = 0; i < 6; i++) {
            D_800DC150_2[n].f18[i] = *D_800E1600[n][i]->pal;
        }
    } else {
        *D_800DC150_2[n].f14 = *D_800DC150_2[n].fC;
    }
    if (D_800C2358 == 0) {
        if (arg4 == 0) {
            D_800DC150_2[n].f10 = D_800DC150_2[n].f8->pal;
        } else {
            D_800DC150_2[n].f10 = D_800E1880[arg3 * 3 + (arg4 - 1)];
        }
        __builtin_memcpy(D_800DC150_2[n].pal, D_800DC150_2[n].f10, sizeof(Pal_3));
        D_800DC150_2[n].pal[0] &= 0xFFFE;
        D_800DC150_2[n].f10->c[0] &= 0xFFFE;
        D_800DC150_2[n].f8->pal->c[0] &= 0xFFFE;
    }
    if (D_800E1568[arg5].a[0] == 0) {
        for (k = 0; k < 2; k++) {
            if (D_800C2358 != 0) {
                if (k == 0) {
                    continue;
                }
                id = *e->f0;
            } else if (k != 0) {
                id = *e->f8;
            } else {
                id = *e->fC;
            }
            func_800C0258((s32)(D_800C29E0 + 0x1E000));
            m = (u32 *)((u32)func_800BF79C(r, id) & ~3);
            func_8003C720(D_800E1A78, m);
            buf[0] = (s32)D_800DC150_2[n].f0;
            D_800C8C70 = 0x06000000;
            buf[1] = (s32)D_800DC150_2[n].f4;
            if (D_800C2358 != 0) {
                func_80014AE4(&D_800E1568[arg5].a[k], m, (s32)buf, 0xC);
                func_80069CA0((s32)D_800DC150_2[n].f4, 0);
                func_80069CA0((s32)D_800DC150_2[n].f0, 0);
            } else {
                func_80014AE4(&D_800E1568[arg5].a[k], m, (s32)buf, 0);
            }
            x = func_800BC908(m[0] << 4, 0, D_800D5EBC, 0x294);
            D_800E1568[arg5].b[k] = x;
            func_800AEC34(x, (void *)m[1], m[0] << 4);
            if (k == 0 && n == 0) {
                m[1] = 0x01000000;
                D_800C8C70 = 0x06000000;
                len = (u32)func_80013D70(0x23, m, 0, D_800C29E0) - (u32)D_800C29E0;
                x = func_800BC908(len, 0, D_800D5EBC, 0x2A0);
                D_800DC150_2[0].f220 = x;
                func_800AEC34(x, D_800C29E0, len);
            }
            func_800BFEA4(&r->tbl[id]);
            func_800C0258(0);
        }
    }
    D_800DC150_2[n].a22C[0] = D_800E1568[arg5].b[0];
    D_800DC150_2[n].a22C[1] = D_800E1568[arg5].b[1];
    D_800DC150_2[n].a224[0] = D_800E1568[arg5].a[0];
    D_800DC150_2[n].a224[1] = D_800E1568[arg5].a[1];
    for (k = 0; k < 2; k++) {
        D_800DC150_2[n].a234[k] = func_800BC908(0x3C0, 0, D_800D5EBC, 0x2B3);
        if (D_800C2358 == 0) {
            D_800DC150_2[n].b23C[k] = func_800BC908(0x280, 0, D_800D5EBC, 0x2B6);
        }
    }
}

void func_80000F0C(void)
{
    s32 n;

    if (D_800CCCA8 == 2) {
        D_800C2494 = D_800C2474;
        D_800E1A54 = 300;
    } else {
        D_800C2494 = D_800C2484;
        D_800E1A54 = 0xAA;
    }
    D_800C25E8_2[0] = D_800F13EC * 2;
    D_800C25E8_2[1] = D_800F13F0 * 2;
    D_800C25E8_2[4] = D_800F13EC * 2;
    D_800C25E8_2[5] = D_800F13F0 * 2;
    D_800E1A50 = D_800F13EC * 0x186 / 512;
    D_800DC0D8 = D_800F13EC * 0x28 / 512;
    D_800DC0DC = D_800F13F0 * 0x19 / 240;
    func_8000BA58();
    if ((D_800EFD64_2 - D_800CB210) == 8 && D_800E1898 >= 2) {
        func_8000ED10(D_800DC0E0, 0, 3);
    }
    n = D_800EFD64_2 - D_800CB210;
    if (n == 8 || n == 0xB) {
        n = 4;
    } else {
        n = 3;
    }
    if (D_800E1898 >= n) {
        D_800C98B0 = 0;
        D_800DC144 = 0;
    }
}

typedef struct {
    s32 flags;
    u8 pad4[0x18];
    s32 a1C[8];
    s32 f3C;
    u8 pad40[0xC];
    u32 mode;
    u8 pad50[0x7AA - 0x50];
    u8 b7AA[6];
} Game_4;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x20];
    s32 f50;
    u8 c54, c55, c56;
    u8 pad57[0xA0 - 0x57];
} Track_3;

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
} Ent28_2;

typedef struct {
    void *p[2];
} Pair_5;

typedef struct {
    s32 f0;
    u16 *f4;
    s32 f8;
    u8 *fC;
} Img_4;

typedef struct {
    s32 f0;
    u16 *f4;
} Res;

extern Game_4 D_800EFD70_11;
extern Track_3 *D_800EFD64_9;
extern Track_3 D_800CB210_4[];
extern Ent28_2 D_800C26C8[];
extern Res *D_800DC140;
extern Img_4 *D_800DC144_2;
extern PalEnt D_800DC150_3[8];
extern Pair_5 D_800E1900[6];
extern Ent80 *D_800E1A78_2;

void func_8000115C(void) {
    u8 buf[0x60];
    s32 a, b, c;
    s32 i, j, k, n, size;
    void *p;
    u32 r, g, bl, m;
    s32 idx;
    s32 x, y, z;

    if (D_800EFD70_11.flags & 0x40000) {
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
    if (D_800EFD64_9->f2C != 2) {
        if (D_800C8CE8 != 0 || (D_800EFD70_11.flags & 0x80) || D_800C8CD8 != 0) {
            D_800C2594 = 1;
        } else {
            D_800C2594 = D_800EFD70_11.f3C;
        }
    }
    if (D_800EFD64_9->f2C == 2) {
        D_800C2594 = 1;
    }
    D_800DC148 = 5;
    D_800E1898 = D_800C2594;
    func_8000ED10(D_800E15E0, 0, 0x20);
    func_80014A80();
    if (D_800EFD64_9->f2C == 0) {
        func_80022BD0();
    } else if (D_800EFD64_9->f2C == 1) {
        func_80024980();
    } else if (D_800EFD64_9->f2C == 2) {
        func_8003F6A0();
    }
    D_800DC0A0 = func_800BF79C(D_800E7728, D_800D10B8);
    func_80014948(D_800DC0A0);
    D_800DC0A4 = func_800BC908(0x400, 0, D_800D5EBC, 0x38B);
    func_8000ED10(D_800DC0A4, 0, 0x400);
    switch (D_800EFD70_11.mode) {
    case 2:
        func_8006EE58();
        break;
    case 0:
    case 13:
    case 14:
        if (!(D_800EFD64_9->f50 & 2)) {
            break;
        }
    case 9:
        func_8006EEC4();
        break;
    }
    p = func_8003C2E0(D_800E7728, D_800D0DA4);
    D_800E1A78_2 = p;
    for (i = 0; i < 15; i++) {
        func_80011A58(D_800E1A78_2[i].f10, 0, 0, 0);
        func_80011A58(D_800E1A78_2[i].f40, 0, 0, 0);
    }
    func_8003C4A4(D_800E1A78_2, 15);
    if (D_800EFD64_9->f2C == 0) {
        j = i;
    } else if (D_800EFD64_9->f2C == 1) {
        j = n;
    }
    func_8000ED10(D_800E1568, 0, 0x70);
    func_8000ED10((u8 *)D_800DC150_3, 0, 0x5400);
    n = 8;
    if (D_800EFD70_11.f3C >= 2 && !(D_800EFD70_11.flags & 0x10)) {
        n = 4;
    }
    if ((D_800EFD70_11.flags & 1) && D_800F05CA == 1) {
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
        if (D_800EFD70_11.flags & 0x10) {
            if (j >= 6) {
                break;
            }
            i = j;
        } else {
            i = 4;
            if (j < 6) {
                i = D_800EFD70_11.a1C[j];
            }
        }
        func_80000450(D_800E7728, j, &D_800C26C8[i], D_800EFD70_11.b7AA[j % 6] & 1, D_800EFD70_11.b7AA[j % 6] >> 1, i);
    }
    if (!(D_800EFD70_11.flags & 0x81)) {
        func_80052C90();
    }
    for (i = 0; i < 8; i++) {
        D_800DC150_3[i].pal[0] = 0;
        for (j = 1; j < 8; j++) {
            x = D_800DC150_3[i].rgb[0] * j / 7 >> 3;
            y = D_800DC150_3[i].rgb[1] * j / 7 >> 3;
            z = D_800DC150_3[i].rgb[2] * j / 7 >> 3;
            D_800DC150_3[i].pal[j] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        for (j = 0; j < 8; j++) {
            x = D_800DC150_3[i].rgb[0];
            y = D_800DC150_3[i].rgb[1];
            z = D_800DC150_3[i].rgb[2];
            x = (x + (255 - x) * j / 7) >> 3;
            y = (y + (255 - y) * j / 7) >> 3;
            z = (z + (255 - z) * j / 7) >> 3;
            D_800DC150_3[i].pal[j + 8] = (x << 11) | (y << 6) | (z << 1) | 1;
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
            size = func_80013D70_2(3, D_800E1900[i].p[1], 0, D_800C29E0) - D_800C29E0;
            D_800E16C0[i] = func_800BC908(size, 0, D_800D5EBC, 0x434);
            func_800AEC34(D_800E16C0[i], D_800C29E0, size);
        }
    }
    D_800E19C8 = (ColList *)func_8005AED8(D_800D152C);
    D_800DC144_2 = (Img_4 *)func_8005AED8(D_800D150C);
    if (D_800E19C8 != NULL) {
        if (D_800DC144_2 != NULL) {
            D_800C98B0 = 3;
        } else {
            D_800C98B0 = 2;
        }
    } else if (D_800DC144_2 != NULL) {
        D_800C98B0 = 1;
        func_80014C14(D_800DC144_2->fC, 0x100, 0x100);
    } else {
        D_800C98B0 = 0;
    }
    if (D_800DC144_2 != NULL) {
        func_80069CA0(D_800DC144_2, 0);
    }
    func_80032150();
    func_80035630();
    if (D_800DC144_2 != NULL) {
        D_800DC0E0[0] = (*D_800DC144_2->f4 >> 11) << 3;
        D_800DC0E0[1] = ((*D_800DC144_2->f4 >> 6) & 0x1F) << 3;
        D_800DC0E0[2] = ((*D_800DC144_2->f4 >> 1) & 0x1F) << 3;
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
    } else if (D_800EFD64_9->f2C == 1) {
        D_800DC0E0[0] = D_800EFD64_9->c54;
        D_800DC0E0[1] = D_800EFD64_9->c55;
        D_800DC0E0[2] = D_800EFD64_9->c56;
    } else {
        D_800DC0E0[0] = 0;
        D_800DC0E0[1] = 0;
        D_800DC0E0[2] = 0;
    }
    idx = D_800EFD64_9 - D_800CB210_4;
    if (idx == 3 || idx == 20) {
        D_800DC144_2 = NULL;
        D_800C98B0 = 0;
    }
    if (D_800C2280 & 0x40) {
        D_800DC144_2 = NULL;
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

typedef struct { u8 pad0[0x14]; s32 f14; s32 f18; u8 pad1c[0x234 - 0x1C]; s32 a[2]; s32 b[2]; u8 rest[0xA80 - 0x244]; } Plr_2;
typedef struct { s32 a[2]; s32 b[2]; } Rec16_2;
typedef struct { s32 flags; u8 pad[0x48]; s32 mode; } State;
typedef struct { u8 pad[0x2C]; s32 m; u8 pad30[0x20]; u32 f50; } Cfg_4;

extern Cfg_4 *D_800EFD64_10;
extern State D_800EFD70_12;
extern Plr_2 D_800DC150_4[];

void func_800020CC(void)
{
    s32 i, k, n;

    if (D_800CCD64 != 0) {
        func_800C204C(D_800C27AC);
        func_800C204C(D_800C27B0);
        func_800BCB04(D_800C27AC);
        func_800BCB04(D_800C27B0);
    }
    func_800BCB04(D_800E1558);
    func_800BCB04(D_800DC0A8);
    if (D_800EFD64_10->m == 0) {
        func_80024708();
    } else if (D_800EFD64_10->m == 1) {
        func_80024DA4();
    } else if (D_800EFD64_10->m == 2) {
        func_80042584();
    }
    func_800BCB04(D_800E1878);
    func_800BCB04(D_800E1A78);
    func_800BCB04(D_800DC0A4);
    if ((D_800EFD70_12.flags & 0x81) == 0) {
        func_8005B33C();
    }
    switch (D_800EFD70_12.mode) {
    case 2:
        func_8006EF68();
        break;
    case 0:
    case 13:
    case 14:
        if ((D_800EFD64_10->f50 & 2) == 0) {
            break;
        }
    case 9:
        func_8006EF8C();
        break;
    }
    func_80014AD0();
    for (n = 0; n < 8; n++) {
        if (D_800EFD70_12.flags & 0x10) {
            if (n >= 6) {
                break;
            }
            func_800BCB04(D_800DC150_4[n].f18);
            for (i = 0; i < 6; i++) {
                func_800BCB04(D_800E1600[n][i]);
            }
        } else if (D_800DC150_4[n].f14 != 0) {
            func_800BCB04(D_800DC150_4[n].f14);
        }
        func_800715DC(n);
        for (i = 0; i < 2; i++) {
            func_800BCB04(D_800DC150_4[n].a[i]);
            func_800BCB04(D_800DC150_4[n].b[i]);
        }
    }
    func_800BCB04(D_800DC370);
    for (i = 0; i < 7; i++) {
        for (k = 0; k < 2; k++) {
            if (D_800E1568[i].a[k] != 0) {
                func_800BCB04(D_800E1568[i].a[k]);
            }
            if (D_800E1568[i].b[k] != 0) {
                func_800BCB04(D_800E1568[i].b[k]);
            }
        }
    }
    for (i = 0; i < 6; i++) {
        func_800BCB04(D_800DC110_2[i].y);
        func_800BCB04(D_800DC110_2[i].x);
        func_800BCB04(D_800E16C0[i]);
    }
    func_800BCB04(D_800E1B48);
    func_8002D8A0();
    func_80033D40();
}

Gfx *func_80002418(Gfx *dl, Mtx *m, s32 n, s32 flag)
{
    Rect *r;
    s32 i;
    u8 *tab;

    if (n == D_800E1A9C) {
        return dl;
    }
    tab = (u8 *)D_800E1718;
    r = (Rect *)(tab + (n * 16 + (D_800C2594 - 1) * 64));
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, r->x, r->y, r->x + r->w, r->y + r->h);
    if (flag) {
        gSPMatrix(dl++, &m[n], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    }
    gSPViewport(dl++, &D_800E19C0[n]);
    D_800E1A9C = n;
    return dl;
}

typedef struct { u8 pad0[0x5AA]; s16 sta; u8 pad1[0x7E0 - 0x5AC]; } E37;
typedef struct { u8 pad0[0x2C]; s32 mode; u8 pad1[0xA0 - 0x30]; } Trk_4;
typedef struct { u32 bits; u8 pad[0x1C]; } Cell_2;

extern Cell_2 *D_800C9644_4;
extern E37 D_800E37C0_4[];
extern Trk_4 *D_800EFD64_11;
extern Trk_4 D_800CB210_5[];

s32 func_80002554(s32 x, s32 y, s32 z, s32 k, s32 c, s32 q, s32 n)
{
    s32 sx, sy, sz, d;
    s32 i3, i4, m;
    s32 x0, x1, y0, y1;
    Rect *b;
    u8 *bb;

    if (D_800C2384 != 0 && n == 0) {
        return 3;
    }
    if (!(D_800E37C0_4[n].sta == 9 && D_800C9654[k] == n)) {
        if ((u32)D_800EFD64_11->mode < 2 && c != -1) {
            Cell_2 *cp = &D_800C9644_4[c];
            if (!((cp[1].bits >> (k * 2)) & 1)) {
                return 0;
            }
        }
    }
    if (D_800EFD64_11->mode == 0) {
        if (func_80033AC8(x >> 4, y >> 4, z >> 4, &sx, &sy, &sz, &d, q, k) != 0) {
            return 0;
        }
    } else {
        if (func_80033AC8((x - D_800E8090_3[k][0]) >> 4, (y - D_800E8090_3[k][1]) >> 4,
                          (z - D_800E8090_3[k][2]) >> 4, &sx, &sy, &sz, &d, q, k) != 0) {
            return 0;
        }
    }
    if (d <= 0) {
        return 0;
    }
    if (D_800EFD64_11->mode == 2 && d > 8000) {
        return 0;
    }
    D_800E1A58[n] = d;
    bb = (u8 *)D_800E1718;
    b = (Rect *)(bb + (k * 16 + (D_800C2594 - 1) * 64));
    x0 = b->x;
    x1 = b->w + x0;
    y0 = b->y;
    y1 = b->h + y0;
    i3 = (D_800E1AA0[k] * 90.0f) / d;
    i4 = (D_800E1AB0[k] * 90.0f) / d;
    if (sx + i3 < x0) {
        return 0;
    }
    if (x1 < sx - i3) {
        return 0;
    }
    if (sy + i4 < y0) {
        return 0;
    }
    if (y1 < sy - i4) {
        return 0;
    }
    if (D_800EFD64_11 - D_800CB210_5 == 20 && D_800E3D6A == 4) {
        return 3;
    }
    m = D_800C9550[D_800E1898 - 1];
    if (m * 6 / 10 < d) {
        return 1;
    }
    if (m * 4 / 10 < d) {
        return 2;
    }
    return 3;
}

Gfx *func_800028D8(Gfx *gdl, s32 fb, u8 *mtx) {
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gSPViewport(gdl++, &D_800C25E8);
    gSPPerspNormalize(gdl++, D_800E1B44);
    gDPPipeSync(gdl++);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[fb]);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetFillColor(gdl++, 0x10001);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718_4[0], D_800E1718_4[1],
                  D_800E1718_4[0] + D_800E1718_4[2], D_800E1718_4[1] + D_800E1718_4[3]);
    gSPSetGeometryMode(gdl++, D_800E19CC);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    return gdl;
}

typedef struct {
    s16 cnt;
    s16 pad;
    TexHdr *tex;
    u8 *bar;
    TexHdr *tex2;
    TexHdr *anim;
    TexHdr *tex3;
} EbState;

typedef struct {
    u8 pad0[0x74];
    s32 *font;
    u8 pad78[0x17C - 0x78];
    s32 frame;
} GState;

typedef struct {
    u8 pad0[0x2C4];
    u8 flag;
} Opts;

extern EbState D_800E3730_2;
extern GState D_800EFD70_13;
extern Opts D_800F1900_2;

Gfx *func_80002D20(Gfx *gdl, s32 fb, Dyn *arg2)
{
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    TexHdr *t;
    s32 xi;
    u8 *p;

    if (D_800E3730_2.cnt > 0) {
        D_800E3730_2.cnt--;
        gdl = func_800028D8_2(gdl, fb);
    } else if (D_800E3730_2.cnt == -1) {
        gdl = func_800028D8_2(gdl, fb);
        G(gdl++, 0xE200001C, 0x0C084000);
        G(gdl++, 0xE3001001, 0x00008000);
        G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        G(gdl++, 0xFD100000, D_800E3730_2.tex->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        G(gdl++, 0xE7000000, 0);
        gdl = func_8000A690(0, 0, D_800E3730_2.tex + 1, gdl, fb, 1, 1, 3, 0);
        G(gdl++, 0xFA000000, 0xE6E6E6FF);
        G(gdl++, 0xE3000C00, 0);
        G(gdl++, 0xE200001C, 0x00404240);
        func_800C2224();
        t = D_800E3730_2.tex2 + 1;
        G(gdl++, 0xFD100000, D_800E3730_2.tex2->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        G(gdl++, 0xE7000000, 0);
        gdl = func_8000A690(0x45, 0x3D, t, gdl, fb, 1, 1, 3, 0);
        G(gdl++, 0xE3001001, 0);
        G(gdl++, 0xD7000002, 0x80008000);
        G(gdl++, 0xE200001C, 0x00404240);
        G(gdl++, 0xFC119623, 0xFF2FFFFF);
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 29; i++) {
                xi = i * 6 + 282;
                x = xi - j * 7;
                y = D_800C27B8[j] - 1;
                if (i <= D_800F1BC5[j] * 29 / 255) {
                    if (j == D_800E36FC) {
                        G(gdl++, 0xFA000000, (((i * 255 / 29) & 0xFF) << 16) | 0xFF0000FF);
                    } else {
                        G(gdl++, 0xFA000000, (((i * 140 / 29) & 0xFF) << 16) | 0x8C0000FF);
                    }
                } else {
                    G(gdl++, 0xFA000000, 0x4B4B4BFF);
                }
                gdl = func_8000A690(x, y, D_800E3730_2.bar + 0x28, gdl, fb, 1, 1, 6, 0);
            }
        }
        G(gdl++, 0xFA000000, 0xD2D2D2FF);
        t = (TexHdr *)(D_800E3730_2.bar + 0x30);
        gdl = func_8000A690(0x100 - ((u8 *)t)[0] * 4, 0x78 - (((u8 *)t)[1] >> 1), t, gdl, fb, 1, 1, 6, ((u32)t & 1) >> 1);
        G(gdl++, 0xE3001001, 0x00008000);
        G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
        G(gdl++, 0xFD100000, &D_800E3730_2.tex3->img);
        G(gdl++, 0xE8000000, 0);
        G(gdl++, 0xF5000100, 0x07000000);
        G(gdl++, 0xE6000000, 0);
        G(gdl++, 0xF0000000, 0x073FC000);
        G(gdl++, 0xE7000000, 0);
        t = D_800E3730_2.anim + ((D_800EFD70_13.frame / 2) % 30 + 1);
        for (j = 0; j < 4; j++) {
            if (j < 3) {
                x = 0x48 - j * 7;
                y = D_800C27B8[j];
            } else {
                xi = ((j - 3) & 1) * 194 + 0x48;
                x = xi - ((j - 3) / 2 + 3) * 7;
                y = D_800C27B8[j];
            }
            func_800C1E60(D_800EFD70_13.font, 0x40, 1, (j == D_800E36FC) ? 4 : 2, 0x20);
            func_800C1F2C(D_800EFD70_13.font, D_800CD274[j]);
            func_800C0ACC(D_800EFD70_13.font, x + 0x24, y + 8);
            if (j == D_800E36FC) {
                gdl = func_8000A690(x - 2, y, t, gdl, fb, 1, 1, 3, 0);
            }
            func_800C1E60(D_800EFD70_13.font, 0x40, 1, (j == D_800E36FC) ? 6 : 2, 0x22);
            if (j == 3) {
                func_800C1F2C(D_800EFD70_13.font, D_800CD26C[D_800F1900_2.flag & 1]);
                func_800C0ACC(D_800EFD70_13.font, x + 0xB4, y + 8);
            }
        }
        gdl = func_800C1394(gdl);
    }
    return gdl;
}

typedef struct { s16 cnt; s16 pad; TexHdr *tex; } EbState_2;

typedef struct {
    u8 pad0[0x50];
    s8 cfg[0x24];
    s32 *font;
    u8 pad78[0x2C8 - 0x78];
    s8 ctl[4];
} GState_2;

extern EbState_2 D_800E3730_3;
extern GState_2 D_800EFD70_14;

Gfx *func_800035AC(Gfx *gdl, s32 fb, u8 *mtx)
{
    char buf[32];
    s32 i;
    s32 a3v;
    s32 v;
    s32 sel;
    s32 lo;

    if (D_800E3730_3.cnt > 0) {
        D_800E3730_3.cnt--;
        gdl = func_800028D8(gdl, fb, mtx);
    } else if (D_800E3730_3.cnt == -1) {
    gdl = func_800028D8(gdl, fb, mtx);
    G(gdl++, 0xE200001C, 0x00404240);
    G(gdl++, 0xE3001001, 0x00008000);
    G(gdl++, 0xFCFFFFFF, 0xFFFCF279);
    G(gdl++, 0xFD100000, D_800E3730_3.tex->img);
    G(gdl++, 0xE8000000, 0);
    G(gdl++, 0xF5000100, 0x07000000);
    G(gdl++, 0xE6000000, 0);
    G(gdl++, 0xF0000000, 0x073FC000);
    G(gdl++, 0xE7000000, 0);
    G(gdl++, 0xE7000000, 0);
    gdl = func_8000A690(D_800E1718_6[0], D_800E1718_6[1], D_800E3730_3.tex + 1, gdl, fb, 1, 1, 3, 0);
    func_800C2224();
    func_800C1E60(D_800EFD70_14.font, 0x40, 4, 2, 0x21);
    strcpy(buf, "Controller ");
    i = 0;
    func_8000EE30(D_800EFD70_14.ctl[D_800E37AC] + 1, buf + func_800AEC10(buf));
    func_800C1F2C(D_800EFD70_14.font, buf);
    func_800C0ACC(D_800EFD70_14.font, 0xBE, 0x2A);
    func_800C1E60(D_800EFD70_14.font, 0x40, 4, 2, 0x21);
    strcpy(buf, "Configuration ");
    buf[func_800AEC10(buf) + 1] = 0;
    buf[func_800AEC10(buf)] = D_800EFD70_14.cfg[D_800EFD70_14.ctl[D_800E37AC]] + 'A';
    func_800C1F2C(D_800EFD70_14.font, buf);
    func_800C0ACC(D_800EFD70_14.font, 0xEE, 0xC2);

    for (; i < 9; i++) {
        sel = i & 1;
        a3v = sel ? 4 : 6;
        lo = i < 5;
        v = lo ? 0x22 : 0x20;
        if (!sel) {
            v |= 0x40000000;
        }
        func_800C1E60(D_800EFD70_14.font, 0x40, 5, a3v, v);
        func_800C1F2C(D_800EFD70_14.font, D_800CDAA0[D_800CDACC[D_800EFD70_14.cfg[D_800EFD70_14.ctl[D_800E37AC]]][i]]);
        func_800C0ACC(D_800EFD70_14.font, D_800CDA70[i].x + 4, D_800CDA70[i].y + 5);
        if (D_800CDACC[D_800EFD70_14.cfg[D_800EFD70_14.ctl[D_800E37AC]]][i] == 6) {
            a3v = sel ? 4 : 6;
            v = lo ? 0x22 : 0x20;
            if (!sel) {
                v |= 0x40000000;
            }
            func_800C1E60(D_800EFD70_14.font, 0x40, 5, a3v, v);
            func_800C1F2C(D_800EFD70_14.font, D_800CDAC8);
            func_800C0ACC(D_800EFD70_14.font, D_800CDA70[i].x + 4, D_800CDA70[i].y + 0x10);
        }
    }
    for (i = 0; i < 3; i++) {
        func_800C1E60(D_800EFD70_14.font, 0x40, 5, (i == 2) ? 4 : 6, (i != 2) ? 0x40000020 : 0x20);
        func_800C1F2C(D_800EFD70_14.font, D_800CDABC[i]);
        func_800C0ACC(D_800EFD70_14.font, D_800CDA94[i].x + 4, D_800CDA94[i].y + 5);
    }
    gdl = func_800C1394(gdl);
    }
    return gdl;
}

Gfx *func_80003AC4(Gfx *gdl)
{
    s32 i;
    s32 n;
    s32 m;
    s32 lim;

    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    n = D_800E1898;
    gDPSetFillColor(gdl++, (GPACK_RGBA5551(D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 1) << 16) |
                               GPACK_RGBA5551(D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 1));
    i = 0;
    m = n - 1;
    if (n > 0) {

    do {
        gDPFillRectangle(gdl++, D_800E1718[m][i].x, D_800E1718[m][i].y,
                         D_800E1718[m][i].x + D_800E1718[m][i].w - 1,
                         D_800E1718[m][i].y + D_800E1718[m][i].h - 1);
    i++;
    } while (i < D_800E1898);
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    return gdl;
}

#define GFX_LEFT() (0x1770 - (gdl - dyn->glist))

#define CHECK(n)                 \
    if (GFX_LEFT() < (n)) {      \
        *pgdl = gdl;             \
        return;                  \
    }

struct Dyn {
    Mtx m0[4];  /* 0x000 */
    Mtx m1[4];  /* 0x100 */
    Mtx m2[4];  /* 0x200 */
    Mtx m3[4];  /* 0x300 */
    Mtx m4;     /* 0x400 */
    Gfx *glist; /* 0x440 */
};

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
} Player_2; /* 0x7E0 */

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
} Track_4; /* 0xA0 */

typedef struct {
    s32 flags; /* 0x00 */
    u8 pad4[0x38];
    s32 f3C;   /* 0x3C */
    u8 pad40[0xC];
    s32 mode;  /* 0x4C */
    u8 pad50[4];
    s32 f54;   /* 0x54 */
    s16 h58;   /* 0x58 */
} Game_5;

typedef struct {
    s16 s0;
    u8 c2;
} Fade;

extern Game_5 D_800EFD70_15;
extern Track_4 *D_800EFD64_12;
extern Track_4 D_800CB210_6[];
extern Player_2 D_800E37C0_5[8];
extern Fade D_800E3730_4;
extern Skel *D_800E1878_2;
extern PG D_800DC150_5[8];

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
    if (D_800EFD70_15.f3C == 1 && D_800E3BBE != 0 && !(D_800EFD70_15.flags & 0x81) && D_800C8CE8 == 0 &&
        D_800EFD70_15.mode != 6 && D_800EFD70_15.mode != 7 && D_800EFD70_15.mode != 8 && D_800EFD70_15.mode != 9 &&
        D_800EFD70_15.mode != 13 && D_800EFD70_15.mode != 14 && D_800EFD64_12->type != 2) {
        D_800C2380 = D_800EFD70_15.f3C;
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
        if (D_800EFD64_12->type == 0) {
            func_8002482C(D_800C2598);
        } else if (D_800EFD64_12->type == 1) {
            func_80024F24(D_800C2598);
        } else if (D_800EFD64_12->type == 2) {
            func_8003F8FC(D_800C2598);
        }
    }

    for (i = 0; i < 8; i++) {
        if (D_800E37C0_5[i].b7A5 == 0 && ((i + D_800DC108) & 3) == 0 && D_800E37C0_5[i].h5AA != 9) {
            D_800E8508 = 0;
            func_8002E920(D_800E37C0_5[i].x, D_800E37C0_5[i].y, D_800E37C0_5[i].z);
            if (D_800E8508 != 0) {
                D_800E15E0[i] = D_800E8958;
                D_800E1B50[i] = D_800E84F8;
                if (D_800EFD64_12->type != 2) {
                    func_8002EF1C(D_800E37C0_5[i].x, D_800E37C0_5[i].z, &D_800E1818[i * 3]);
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
            if (D_800EFD64_12->type == 0) {
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
        if (D_800EFD64_12->type == 0) {
            func_80011490(&dyn->m2[D_800C2598], 0, 0, 0, D_800E8090[D_800C2598].x,
                          D_800E8090[D_800C2598].y - 500, D_800E8090[D_800C2598].z, 0x10);
        } else if (D_800EFD64_12->type == 1) {
            func_80011490(&dyn->m2[D_800C2598], 0, 0, 0, 0, 0, 0, 0x10);
        } else {
            func_80011490(&dyn->m2[D_800C2598], 0, 0, 0, 0, -2000, 0, 0x10);
        }
    }

    for (i = 0; i < 8; i++) {
        if (D_800E37C0_5[i].b7A5 == 0) {
            r = D_800E15E0[i];
            D_800E18A0[i] = 0;
            for (k = 0; k < D_800C2594; k++) {
                t = func_80002554((s32)((f32)(s32)D_800E37C0_5[i].f48 * 1000.0f),
                                  (s32)((f32)(s32)D_800E37C0_5[i].f4C * 1000.0f),
                                  (s32)((f32)(s32)D_800E37C0_5[i].f50 * 1000.0f), k, r, dyn, i) &
                    0xF;
                if (D_800E37C0_5[i].b44 != 0) {
                    t |= (func_80002554(D_800E37C0_5[i].x2, D_800E37C0_5[i].y2, D_800E37C0_5[i].z2, k, r, dyn, i) & 0xF)
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
    if (D_800E36F8_2 == 3 || D_800E36F8_2 == 7) {
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
            if (D_800E3730_4.s0 != -1) {
                if (D_800E3730_4.s0 > 0) {
                    D_800E3730_4.s0--;
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
            if (D_800E37C0_5[i].b7A5 == 0 && !((i ^ D_800DC108) & 1) && D_800E18A0[i] != 0) {
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
    if (D_800DC148 != 0 || D_800E3730_4.c2 != 0 || D_800E3710 != 0) {
        gDPSetFillColor(gdl++, 0x10001);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC, D_800F13F0);
        if (D_800E3730_4.c2 != 0) {
            D_800E3730_4.c2--;
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
            gdl = func_80035178(gdl, dyn, D_800DC144_3);
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
            gDPLoadTLUT_pal256(gdl++, D_800DC144_3->f4);
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
                gdl = func_80013D70(0, D_800E19C8, &D_800DC144_3, gdl);
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
    if (D_800E1898 == 1 && (D_800C8CE8 != 0 || (D_800EFD70_15.flags & 0x81))) {
        gdl = func_8003371C(func_80002418(gdl, dyn, 0, 1), dyn);
    }
    func_8000ED10((u8 *)D_800DC0B8, 0, 0x20);

    for (i = 0; i < 8; i++) {
        if (D_800E37C0_5[i].b7A5 == 0) {
            if (D_800E18A0[i] & 0x0F0F0F0F) {
                func_8003EAC4(D_800E1878_2, i);
                *D_800E37C0_5[i].skel = *D_800E1878_2;
                if (D_800C2358 == 0) {
                    for (k = 0; k < 6; k++) {
                        Bone *bn = &D_800E37C0_5[i].skel->b[D_800C25CC[k]];
                        func_800139A8(&bn->m40, bn->v34, D_800DC150_5[i].p23C[fb] + k);
                    }
                }
            }
            if (D_800E18A0[i] & 0xF0F0F0F0) {
                if (D_800E37C0_5[i].b6E8 == 0 || !((D_800DC108 / 2 + i) & 1) || D_800E1898 != 1 ||
                    ((D_800E18A0[i] >> 4) & 0xF) == 3 || D_800E37C0_5[i].b44 != 0 || (D_800EFD70_15.flags & 1)) {
                    func_8003D4BC(D_800E37C0_5[i].p65C, i);
                }
                func_80013ACC(D_800E37C0_5[i].p65C, D_800DC150_5[i].p234[fb], 0xF);
                if (D_800C2358 == 0 && (D_800C2360 != 0 || D_800C235C != 0)) {
                    u16 *m = (u16 *)&D_800DC150_5[i].p234[fb][2];

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
                    if (D_800EFD64_12->type == 0) {
                        func_80011490(&D_800DC150_5[i].a[fb * 4 + D_800C2598], 0, 0, 0, D_800E37C0_5[i].x,
                                      D_800E37C0_5[i].y, D_800E37C0_5[i].z, 0x10);
                        if (D_800E37C0_5[i].b44 != 0) {
                            func_80011490(&D_800DC150_5[i].c[fb * 4 + D_800C2598], 0, 0, 0, D_800E37C0_5[i].x2,
                                          D_800E37C0_5[i].y2, D_800E37C0_5[i].z2, 0x10);
                        }
                    } else {
                        func_80011490(&D_800DC150_5[i].a[fb * 4 + D_800C2598], 0, 0, 0,
                                      D_800E37C0_5[i].x - D_800E8090[D_800C2598].x,
                                      D_800E37C0_5[i].y - D_800E8090[D_800C2598].y,
                                      D_800E37C0_5[i].z - D_800E8090[D_800C2598].z, 0x10);
                        if (D_800E37C0_5[i].b44 != 0) {
                            func_80011490(&D_800DC150_5[i].c[fb * 4 + D_800C2598], 0, 0, 0,
                                          D_800E37C0_5[i].x2 - D_800E8090[D_800C2598].x,
                                          D_800E37C0_5[i].y2 - D_800E8090[D_800C2598].y,
                                          D_800E37C0_5[i].z2 - D_800E8090[D_800C2598].z, 0x10);
                        }
                    }
                    func_800120A4(&D_800DC150_5[i].a[fb * 4 + D_800C2598], &dyn->m0[D_800C2598],
                                  &D_800DC150_5[i].b[fb * 4 + D_800C2598]);
                    if (D_800E37C0_5[i].b44 != 0) {
                        func_800120A4(&D_800DC150_5[i].c[fb * 4 + D_800C2598], &dyn->m0[D_800C2598],
                                      &D_800DC150_5[i].d[fb * 4 + D_800C2598]);
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
            if (D_800EFD64_12->type == 0) {
                gdl = func_80023C0C(gdl, dyn, fb, D_800C2598);
            } else if (D_800EFD64_12->type == 1) {
                gdl = func_800258F4(gdl, dyn, fb, D_800C2598);
            } else if (D_800EFD64_12->type == 2) {
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
            if (D_800EFD64_12->type == 0) {
                gdl = func_80022FD8(gdl, dyn, fb, D_800C2598);
            } else if (D_800EFD64_12->type == 1) {
                gdl = func_8002803C_2(gdl, dyn, fb, D_800C2598);
            } else if (D_800EFD64_12->type == 2) {
                gdl = func_8003FD4C(gdl, dyn, fb, D_800C2598);
            }
            gdl = func_80062DA0(gdl, dyn, fb, D_800C2598);
        }
    }
    if (D_800C2358 != 0) {
        gdl = func_8000C0E0(gdl, dyn, fb);
    }
    gDPPipeSync(gdl++);
    switch (D_800EFD70_15.mode) {
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
            if (D_800EFD64_12->f50 & 2) {
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
            if (D_800E37C0_5[i].b7A5 == 0) {
                for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
                    if ((D_800E18A0[i] >> (D_800C2598 * 8)) & 0xF) {
                        if (!(D_800DC0B8[i] & 0x10)) {
                            func_8003B7E0(i);
                            D_800DC0B8[i] |= 0x10;
                        }
                        gdl = func_80002418(gdl, dyn, D_800C2598, 0);
                        gSPMatrix(gdl++, &dyn->m1[D_800C2598], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                        gSPMatrix(gdl++, &D_800DC150_5[i].a[fb * 4 + D_800C2598],
                                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        gdl = func_8003B0AC(i, dyn, gdl, D_800C2598);
                    }
                }
                if (D_800E37C0_5[i].b44 != 0) {
                    for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
                        if ((D_800E18A0[i] >> (D_800C2598 * 8)) & 0xF0) {
                            if (!(D_800DC0B8[i] & 0x20)) {
                                func_8003B7E0(i + 8);
                                D_800DC0B8[i] |= 0x20;
                            }
                            gdl = func_80002418(gdl, dyn, D_800C2598, 0);
                            gSPMatrix(gdl++, &dyn->m1[D_800C2598], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                            gSPMatrix(gdl++, &D_800DC150_5[i].c[fb * 4 + D_800C2598],
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
        if (D_800EFD64_12->type == 1) {
            gdl = func_80026120(gdl, dyn, fb, D_800C2598);
        } else if (D_800EFD64_12->type == 0) {
            gdl = func_800238F0(gdl, dyn, fb, D_800C2598);
        } else if (D_800EFD64_12->type == 2) {
            gdl = func_800408A4(gdl, dyn, fb, D_800C2598);
        }
    }
    if ((D_800C2350 & 0x80) && D_800EE140 == 0) {
        for (D_800C2598 = 0; D_800C2598 < D_800C2594; D_800C2598++) {
            CHECK(1000);
            gdl = func_80037414(func_80002418(gdl, dyn, D_800C2598, 1), D_800C2598, dyn);
        }
    }
    if (!(D_800EFD70_15.flags & 1) && D_800E76D0 >= 0x259 && D_800C8CE8 == 0 && D_800EFD70_15.mode != 8) {
        s32 tr = D_800EFD64_12 - D_800CB210_6;

        if (tr != 20 && tr != 23 && tr != 24) {
            gdl = func_80008DE0(gdl, dyn);
        }
    }
    CHECK(1000);
    if (D_800E1898 == 1) {
        s32 tr = D_800EFD64_12 - D_800CB210_6;

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
    if (D_800EFD64_12->type == 2) {
        gdl = func_80040B64(gdl, dyn, fb, 0);
    }
    CHECK(1000);
    gdl = func_80002418(gdl, dyn, 0, 1);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    for (i = 0; i < 8; i++) {
        if (D_800E37C0_5[i].b7A5 == 0 && D_800C238C[i] != 0xFF) {
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
        gdl = func_8006EB34_2(gdl, dyn, fb);
    } else if (D_800C2350 & 0x800) {
        if (D_800CCD64 == 0 && !(D_800EFD70_15.flags & 1)) {
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
    if (D_800EFD70_15.flags & 0x80) {
        gdl = func_8006D664(gdl, dyn, fb, D_800C2598);
    }
    if (D_800CCD64 != 0) {
        gdl = func_8000B3C8(func_80002418(gdl, dyn, 0, 0));
    }
    if (!(D_800EFD70_15.flags & 1)) {
        switch (D_800E36F8_2) {
            case 0:
            case 4:
                if (D_800E374C == -1) {
                    D_800E374C = 20;
                }
                D_800EFD70_15.f54 = 2;
                alpha = D_800E374C * 255 / 20;
                D_800EFD70_15.h58 = alpha;
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
                D_800EFD70_15.f54 = 0;
                D_800EFD70_15.h58 = 255 - alpha;
                break;
            default:
                D_800EFD70_15.f54 = 3;
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

typedef struct {
    u32 seg0;
    u32 seg4;
    u32 seg8;
    u8 padC[0x14 - 0xC];
    u32 tlut14;
    u8 *tlut18;
    u16 pal1C[(0x224 - 0x1C) / 2];
    Gfx *dl224;
    Gfx *dl228;
    u32 seg22C;
    u32 seg230;
    u32 seg234[2];
    Mtx *mtx23C[2];
    u8 pad244[0x448 - 0x244];
    Mtx m448[16];
    Mtx m848[8];
    u8 padA48[0xA4C - 0xA48];
    u8 *fA4C;
    u8 padA50[4];
    void *fA54;
    u8 padA58[0xA80 - 0xA58];
} Bike;

typedef struct {
    Gfx *dl0;
    Gfx *dl4;
} DlPair;

typedef struct {
    u8 pad0[4];
    u32 seg;
} SegSrc;

typedef struct {
    SegSrc *p0;
    SegSrc *p4;
} SegPair;

typedef struct {
    u8 pad0[0x44];
    u8 b44;
    u8 pad45[0x7A5 - 0x45];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Rider;

typedef struct {
    u32 w0;
    u8 pad4[0x1C - 0x4];
    s32 f1C[8];
    s32 w3C;
    u8 pad40[0x3C0 - 0x40];
    s32 w3C0;
    s32 w3C4;
} Game_6;

extern Game_6 D_800EFD70_16;
extern Bike D_800DC150_6[];
extern Rider D_800E37C0_6[];
extern DlPair D_800DC110_3[];
extern SegPair D_800E1900_2[];

Gfx *func_80007010(Gfx *gdl, s32 vp, s32 p3, s32 xlu) {
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    u32 v;
    s32 started;
    u32 *segs;

    for (i = 0; i < D_800C2594; i++) {
        started = 0;
        for (j = 0; j < 8; j++) {
            if (D_800E37C0_6[j].b7A5 != 0) {
                continue;
            }
            v = D_800E18A0[j] >> (i * 8);
            if (!(v & 0xFF)) {
                continue;
            }
            k = D_800C238C[j] != 0xFF;
            if (k != xlu) {
                continue;
            }
            if (D_800EFD70_16.w0 & 0x10) {
                m = 2;
                if (j == D_800EFD70_16.f1C[i]) {
                    m = 3;
                }
            } else {
                m = v & 0xF;
                if (D_800E1898 >= 3) {
                    if (m == 3) {
                        m = 2;
                    }
                    if (i == j && m == 1) {
                        m = 2;
                    }
                }
                if (D_800E1898 == 2 && i == j) {
                    m = 3;
                }
                if (D_800F05E4 != 0 && j == 0) {
                    m = 3;
                }
                if (D_800E1898 == 1 && (j == D_800C9650 || (D_800C9650 == -1 && j == 0))) {
                    m = 3;
                }
            }
            if (D_800C2358 != 0) {
                m = 3;
                if (D_800DC150_6[j].dl228 == 0) {
                    continue;
                }
            }
            if (!started) {
                gdl = func_80002418(gdl, p3, i, 1);
                started = 1;
            }
            D_800E1A98++;
            if (!(D_800DC0B8[j] & 4)) {
                func_80007D90(j, i);
                if (!(D_800EFD70_16.w0 & 0x10)) {
                    D_800DC0B8[j] |= 4;
                }
            }
            if (D_800EFD70_16.w0 & 0x10) {
                if (D_800EFD70_16.w3C == 1) {
                    gDPLoadTLUT_pal256(gdl++, D_800DC150_6[j].tlut18 + D_800EFD70_16.w3C0 * 0x200);
                } else {
                    gDPLoadTLUT_pal256(gdl++, D_800DC150_6[j].tlut18 + i * 0x200);
                }
            } else {
                gDPLoadTLUT_pal256(gdl++, D_800DC150_6[j].tlut14);
            }
            if (D_800E37C0_6[j].b44 != 0) {
                gSPMatrix(gdl++, &D_800DC150_6[j].m848[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            } else {
                gSPMatrix(gdl++, &D_800DC150_6[j].m448[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            }
            if (xlu == 0) {
                gDPSetRenderMode(gdl++, 0x442230, 0);
            } else {
                gDPSetRenderMode(gdl++, 0x404A70, 0);
                gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, D_800C238C[j]);
            }
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF67B);
            if (D_800C2350 & 1) {
                if (m == 3) {
                    gSPSegment(gdl++, 1, D_800DC150_6[j].seg230 & 0xFFFFFF);
                    if (D_800EFD70_16.w0 & 0x10) {
                        if (D_800EFD70_16.w3C == 1) {
                            gSPSegment(gdl++, 2, D_800E1600_2[j][D_800EFD70_16.w3C0] & 0xFFFFFF);
                        } else {
                            gSPSegment(gdl++, 2, D_800E1600_2[j][i] & 0xFFFFFF);
                        }
                    } else {
                        gSPSegment(gdl++, 2, D_800DC150_6[j].seg0 & 0xFFFFFF);
                        gSPSegment(gdl++, 3, D_800DC150_6[j].seg4 & 0xFFFFFF);
                    }
                    gSPSegment(gdl++, 6, D_800DC150_6[j].seg234[D_800E30A8]);
                    if (!(D_800C2280 & 0x20)) {
                        gSPDisplayList(gdl++, D_800DC150_6[j].dl228);
                    }
                } else {
                    gSPSegment(gdl++, 1, D_800DC150_6[j].seg22C & 0xFFFFFF);
                    if (D_800EFD70_16.w0 & 0x10) {
                        if (D_800EFD70_16.w3C == 1) {
                            gSPSegment(gdl++, 2, D_800E1600_2[j][D_800EFD70_16.w3C0 % 5] & 0xFFFFFF);
                        } else {
                            gSPSegment(gdl++, 2, D_800E1600_2[j][i] & 0xFFFFFF);
                        }
                    } else {
                        gSPSegment(gdl++, 2, D_800DC150_6[j].seg0 & 0xFFFFFF);
                    }
                    gSPSegment(gdl++, 6, D_800DC150_6[j].seg234[D_800E30A8]);
                    if (!(D_800C2280 & 0x20)) {
                        gSPDisplayList(gdl++, D_800DC150_6[j].dl224);
                    }
                }
                if (D_800DC150_6[j].fA54 != 0) {
                    D_800C8C70 = D_800DC150_6[j].fA4C + vp * 0xC0;
                    if (!(D_800C2280 & 0x20)) {
                        if (D_800EFD70_16.w0 & 0x10) {
                            segs = D_800E1600_2[j];
                            gdl = func_80013D70(0, D_800DC150_6[j].fA54,
                                                (D_800EFD70_16.w3C == 1) ? segs + D_800EFD70_16.w3C0 : segs + i, gdl);
                        } else {
                            gdl = func_80013D70(0, D_800DC150_6[j].fA54, &D_800DC150_6[j], gdl);
                        }
                    }
                }
            }
            if (D_800C2358 == 0) {
                if (D_800E37C0_6[j].b44 != 0) {
                    gSPMatrix(gdl++, &D_800DC150_6[j].m448[vp * 4 + i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
                }
                if (D_800EFD70_16.w0 & 0x10) {
                    gDPLoadTLUT_pal256(gdl++, D_800DC150_6[(j + D_800EFD70_16.w3C4) % 6].pal1C);
                } else {
                    gDPLoadTLUT_pal256(gdl++, D_800DC150_6[j].pal1C);
                }
                for (k = 0; k < 6; k++) {
                    if (xlu != 1) {
                        if (k != 5 && k != 2 && m != 1) {
                            gDPSetRenderMode(gdl++, 0x442230, 0);
                        } else {
                            gDPSetRenderMode(gdl++, 0x553078, 0);
                        }
                        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                    } else {
                        gDPSetRenderMode(gdl++, 0x404A70, 0);
                        gDPSetCombine(gdl++, 0xFF97FF, 0xFF2CFE7F);
                    }
                    if (D_800C2350 & 1) {
                        gSPMatrix(gdl++, D_800DC150_6[j].mtx23C[D_800E30A8] + k, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        if (D_800EFD70_16.w0 & 0x10) {
                            { Gfx *g = gdl++; g->words.w0 = 0xDB060008; if (j == 0) v = 0; gdl[-1].words.w1 = D_800DC150_6[(j + D_800EFD70_16.w3C4) & 1].seg8; }
                        } else {
                            gSPSegment(gdl++, 2, D_800DC150_6[j].seg8);
                        }
                        if (m == 1) {
                            gSPSegment(gdl++, 2, D_800DC150_6[j].seg8);
                            gSPDisplayList(gdl++, D_800E1B48);
                            break;
                        }
                        if (m == 3) {
                            gSPSegment(gdl++, 1, D_800E1900_2[k].p4->seg);
                            gSPDisplayList(gdl++, D_800DC110_3[k].dl4);
                        } else {
                            gSPSegment(gdl++, 1, D_800E1900_2[k].p0->seg);
                            gSPDisplayList(gdl++, D_800DC110_3[k].dl0);
                        }
                    }
                }
            }
            {
                s32 t;
                if (((u32)t >> 28) & 16) {
                    k = 5;
                }
            }
            gDPPipeSync(gdl++);
        }
    }
    D_800E1A9C = -1;
    return gdl;
}

typedef struct {
    u8 pad0[4];
    u16 *f4;
} Tex;

typedef struct {
    u8 pad0[0xC];
    u16 *src;      /* 0x0C */
    u16 *src2;     /* 0x10 */
    u16 *dst;      /* 0x14 */
    u16 *base;     /* 0x18 */
    u16 pal2[256]; /* 0x1C */
    u8 pad21C[0xA80 - 0x21C];
} Pal_4;

typedef struct {
    s32 w0;
    u8 pad4[0x38];
    s32 w3C;
    u8 pad40[0x380];
    s32 w3C0;
} Game_7;

typedef struct {
    u8 pad0[0x654];
    u8 b654;
    u8 pad655[0x6E8 - 0x655];
    u8 b6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player_3;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0xA0 - 0x30];
} Track_5;

extern Game_7 D_800EFD70_17;
extern Pal_4 D_800DC150_7[];
extern Tex *D_800E1600_3[][6];
extern Player_3 D_800E37C0_7[];
extern Track_5 *D_800EFD64_13;
extern Track_5 D_800CB210_7[];

void func_80007D90(s32 pl, s32 k)
{
    s32 r, g, b;
    s32 i, c, v, t;
    s32 fr, fg, fb;
    s32 cr, cg, cb;

    if (D_800C2358 != 0) {
        return;
    }
    if (D_800EFD70_17.w0 & 0x10) {
        if (D_800EFD70_17.w3C == 1) {
            D_800DC150_7[pl].src = D_800E1600_3[pl][(k + D_800EFD70_17.w3C0) % 5]->f4;
            D_800DC150_7[pl].dst = D_800DC150_7[pl].base + ((k + D_800EFD70_17.w3C0) % 5) * 0x100;
        } else {
            D_800DC150_7[pl].src = D_800E1600_3[pl][k]->f4;
            D_800DC150_7[pl].dst = D_800DC150_7[pl].base + k * 0x100;
        }
    }
    i = 0;
    if (D_800CCD64 != 0) {
        do {
            c = D_800DC150_7[pl].src[i];
            r = c;
            g = (c >> 6) & 0x1F;
            b = (c >> 1) & 0x1F;
            r = (((r >> 11) & 0x1F) + g + b) * 8 / 3;
            func_8006A9DC(r, &r, &g, &b);
            r >>= 3;
            g >>= 3;
            b >>= 3;
            D_800DC150_7[pl].dst[i] = (r << 11) | (g << 6) | (b << 1) | 1;
            c = D_800DC150_7[pl].src2[i];
            r = c;
            g = (c >> 6) & 0x1F;
            b = (c >> 1) & 0x1F;
            r = (((r >> 11) & 0x1F) + g + b) * 8 / 3;
            func_8006A9DC(r, &r, &g, &b);
            r >>= 3;
            g >>= 3;
            b >>= 3;
            D_800DC150_7[pl].pal2[i] = (r << 11) | (g << 6) | (b << 1) | 1;
            i++;
        } while (i < 0x100);
        D_800DC150_7[pl].pal2[0] &= 0xFFFE;
        return;
    }
    if (D_800F14E8 != 0) {
        do {
            s32 x, y, z;

            x = D_800DC150_7[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x + y + z) / 3;
            if (x > 31) {
                x = 31;
            }
            D_800DC150_7[pl].dst[i] = (x << 11) | (x << 6) | (x << 1) | 1;
            x = D_800DC150_7[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x + y + z) / 3;
            if (x > 31) {
                x = 31;
            }
            D_800DC150_7[pl].pal2[i] = (x << 11) | (x << 6) | (x << 1) | 1;
            i++;
        } while (i < 0x100);
        D_800DC150_7[pl].pal2[0] &= 0xFFFE;
        D_800E1A90[pl] = 1;
        return;
    }
    if ((D_800E37C0_7[pl].b654 != 0 && D_800E76D0 % 30 < 15 && D_800E1898 >= 2) ||
        (D_800E37C0_7[pl].b6E8 == 0 && (D_800C2280 & 0x400) && D_800E76D0 % 32 < 16)) {
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = D_800DC150_7[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += (31 - x) * 3 / 8;
            y = y * 5 / 8;
            z = z * 5 / 8;
            D_800DC150_7[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
            x = D_800DC150_7[pl].src2[i];
            x += (31 - x) * 3 / 8;
            y = y * 5 / 8;
            z = z * 5 / 8;
            D_800DC150_7[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        D_800DC150_7[pl].pal2[0] &= 0xFFFE;
        D_800E1A90[pl] = 1;
        return;
    }
    if (D_800EFD64_13->f2C == 2 && D_800E1A58[pl] > 0xC48) {
        fr = D_800DC0E0[0] * 31 / 256;
        fg = D_800DC0E0[1] * 31 / 256;
        fb = D_800DC0E0[2] * 31 / 256;
        t = (D_800E1A58[pl] - 0xC48) * 255 / 5241;
        if (t > 255) {
            t = 255;
        }
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = D_800DC150_7[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += ((fr - x) * t) >> 8;
            y += ((fg - y) * t) >> 8;
            z += ((fb - z) * t) >> 8;
            D_800DC150_7[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
            x = D_800DC150_7[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += ((fr - x) * t) >> 8;
            y += ((fg - y) * t) >> 8;
            z += ((fb - z) * t) >> 8;
            D_800DC150_7[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        return;
    }
    D_800DC0B8[pl] |= 4;
    if (D_800E1B50[pl] & 0x200) {
        if (D_800E1818[pl * 3] == D_800DC0E8[pl][0] && D_800E1818[pl * 3 + 1] == D_800DC0E8[pl][1] &&
            D_800E1818[pl * 3 + 2] == D_800DC0E8[pl][2] && D_800E1A90[pl] == 0) {
            return;
        }
        D_800E1A90[pl] = 0;
        cr = D_800E1818[pl * 3];
        cg = D_800E1818[pl * 3 + 1];
        cb = D_800E1818[pl * 3 + 2];
        if (D_800EFD64_13 - D_800CB210_7 == 15) {
            cr += (255 - cr) / 3;
            cg += (255 - cg) / 3;
            cb += (255 - cb) / 3;
        }
        D_800DC0E8[pl][0] = cr;
        D_800DC0E8[pl][1] = cg;
        D_800DC0E8[pl][2] = cb;
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = D_800DC150_7[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x * cr) >> 8;
            y = (y * cg) >> 8;
            z = (z * cb) >> 8;
            D_800DC150_7[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = D_800DC150_7[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x * cr) >> 8;
            y = (y * cg) >> 8;
            z = (z * cb) >> 8;
            D_800DC150_7[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | (i != 0);
        }
    } else {
        __builtin_memcpy(D_800DC150_7[pl].dst, D_800DC150_7[pl].src, 0x200);
        __builtin_memcpy(D_800DC150_7[pl].pal2, D_800DC150_7[pl].src2, 0x200);
        D_800DC0E8[pl][0] = 0xFF;
        D_800DC0E8[pl][1] = 0xFF;
        D_800DC0E8[pl][2] = 0xFF;
    }
}

Gfx *func_80008B1C(Gfx *gdl, s32 a1, s32 a2)
{
    Gfx *g0, *g1, *g2, *g3, *g4, *g5, *g6, *g7, *g8, *g9, *g10, *g11, *g12, *g13, *g14;
    f32 fv5 = (f32)(D_800F13EC - 0x11) * 4.0f;
    f32 fv4 = (f32)(D_800F13F0 - 0xd) * 4.0f;
    s32 iv2 = D_800C2598 * 0x40;

    g0 = gdl++;
    ((u32 *)g0)[0] = 0xED040030;
    ((u32 *)g0)[1] = (((s32)fv5 & 0xfff) << 12) | ((s32)fv4 & 0xfff);
    g1 = gdl++;
    ((u32 *)g1)[0] = 0xDC080008;
    ((u32 *)g1)[1] = (u32)D_800E19C0;
    g2 = gdl++;
    ((u32 *)g2)[0] = 0xE7000000;
    ((u32 *)g2)[1] = 0;
    g3 = gdl++;
    ((u32 *)g3)[0] = 0xDB0E0000;
    ((u32 *)g3)[1] = (u32)D_800E1B44;
    g4 = gdl++;
    ((u32 *)g4)[0] = 0xFE000000;
    ((u32 *)g4)[1] = (u32)D_800C29E0 + 0x80000000;
    g5 = gdl++;
    ((u32 *)g5)[0] = 0xFF100000 | (((*(volatile s32 *)&D_800F13EC) - 1) & 0xfff);
    ((u32 *)g5)[1] = (u32)D_800C29D8[a1] + 0x80000000;
    g6 = gdl++;
    ((u32 *)g6)[0] = 0xE3000A01;
    ((u32 *)g6)[1] = 0;
    g7 = gdl++;
    ((u32 *)g7)[0] = 0xD9FFFFFF;
    ((u32 *)g7)[1] = (u32)D_800E19CC;
    g8 = gdl++;
    ((u32 *)g8)[0] = 0xE3001801;
    ((u32 *)g8)[1] = 0;
    g9 = gdl++;
    ((u32 *)g9)[0] = 0xDA380007;
    ((u32 *)g9)[1] = (u32)(a2 + iv2) + 0x80000000;
    g10 = gdl++;
    ((u32 *)g10)[0] = 0xDA380003;
    ((u32 *)g10)[1] = (u32)a2 + 0x80000400;
    g11 = gdl++;
    ((u32 *)g11)[0] = 0xDB040004;
    ((u32 *)g11)[1] = 1;
    g12 = gdl++;
    ((u32 *)g12)[0] = 0xDB04000C;
    ((u32 *)g12)[1] = 1;
    g13 = gdl++;
    ((u32 *)g13)[0] = 0xDB040014;
    ((u32 *)g13)[1] = 0xFFFF;
    g14 = gdl++;
    ((u32 *)g14)[0] = 0xDB04001C;
    ((u32 *)g14)[1] = 0xFFFF;

    D_800E1A9C = -1;
    return gdl;
}

typedef struct {
    s32 x, y, z;
    u8 pad0C[0x44 - 0xC];
    u8 b44;
    u8 pad45[0x662 - 0x45];
    s16 h662;
    u8 pad664[0x7A5 - 0x664];
    u8 b7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player_4;

typedef struct {
    u8 pad[8];
    u8 w;
    u8 h;
    u8 pad2[2];
    void *tex;
} TexInfo_2;

extern TexInfo_2 *D_800DC140_2;
extern Player_4 D_800E37C0_8[];

Gfx *func_80008DE0(Gfx *gdl, s32 arg1) {
    s32 sx, sy, sz, sw;
    s32 w1, h1, s1, t1, w2, h2, s2, t2;
    s32 tw8, tw, th, i, j, y, off;
    s32 pl;
    Player_4 *p;

    w2 = D_800F13EC / 16;
    h2 = D_800F13F0 / 15;
    s2 = 0x8000 / w2;
    t2 = 0x4000 / h2;
    tw = D_800DC140_2->w;
    w1 = D_800F13EC * 24 / 320;
    s1 = (tw << 13) / w1;
    th = D_800DC140_2->h;
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xE200001C;
        g->words.w1 = 0x404A50;
    }
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureFilter(gdl++, G_TF_POINT);
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    tw8 = tw * 8;
    h1 = D_800F13F0 / 10;
    t1 = (th << 10) / h1;
    for (i = 0; i < D_800E1898; i++) {
        gdl = func_80002418(gdl, arg1, i, 0);
        for (j = 0; j < 8; j++) {
            pl = (j << 11) + (j << 9) + (j << 7);
            p = &D_800E37C0_8[j];
            if (p->b7A5 != 0) continue;
            if (j >= 6) continue;
            if (!((D_800E18A0[j] >> (i * 8)) & 0xF)) continue;
            if (p->b44 != 0) continue;
            if (p->h662 != D_800E37C0_8[i].h662 - 1) continue;
            if (D_800EFD64_4->f2C == 0) {
                if (func_80033AC8(p->x >> 4, (p->y + 2000) >> 4, p->z >> 4, &sx, &sy, &sz, &sw, arg1, i) != 0) continue;
            } else {
                if (func_80033AC8((p->x - D_800E8090[i].x) >> 4, (p->y - D_800E8090[i].y + 2000) >> 4,
                                  (p->z - D_800E8090[i].z) >> 4, &sx, &sy, &sz, &sw, arg1, i) != 0) continue;
            }
            if (sx < -50) continue;
            if (sy < -50) continue;
            if (sx > D_800F13EC + 50) continue;
            if (sy > D_800F13F0 + 50) continue;
            off = -12;
            y = sy + D_800F13F0 * off / 240;
            gDPSetPrimDepth(gdl++, sz, 0);
            gDPPipeSync(gdl++);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetTextureImage(gdl++, G_IM_FMT_CI, G_IM_SIZ_16b, 1, D_800DC140_2->tex);
            gDPSetTile(gdl++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 7, 0, 0, 7, 0);
            gDPLoadSync(gdl++);
            gDPLoadBlock(gdl++, G_TX_LOADTILE, 0, 0, (s32)((u32)(tw8 * th) >> 2) - 1, CALC_DXT_4b(tw8));
            gDPPipeSync(gdl++);
            {
                Gfx *g = gdl++;
                g->words.w0 = ((u32)((tw8 >> 1) + 7) >> 3 << 9) | 0xF5400000;
                g->words.w1 = 0x1C070;
            }
            gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, (tw8 - 1) << G_TEXTURE_IMAGE_FRAC, (th - 1) << G_TEXTURE_IMAGE_FRAC);
            gDPLoadTLUT_pal256(gdl++, &D_800DCBB0[0][pl]);
            gSPTextureRectangle(gdl++, (sx - w1 / 2) * 4, (y - h1 / 2) * 4, (sx + w1 / 2) * 4, (y + h1 / 2) * 4,
                                0, 0, 0, s1, t1);
            gDPPipeSync(gdl++);
            off = -7;
            y = sy + D_800F13F0 * off / 240;
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            off = p->h662;
            gDPLoadTextureBlock(gdl++, D_800E1558 + (off << 9), G_IM_FMT_CI, G_IM_SIZ_8b, 32, 16, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 5, 4, 0, 0);
            gSPTextureRectangle(gdl++, (sx - w2 / 2) * 4, (y - h2 / 2) * 4, (sx + w2 / 2) * 4, (y + h2 / 2) * 4,
                                0, 0, 0, s2, t2);
        }
    }
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    return gdl;
}

typedef struct {
    u8 pad0[0x648];
    f32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 b654;
    u8 pad655[0x7E0 - 0x655];
} Player_5;

typedef struct {
    u8 pad0[0xC];
    u8 *tC;
    u8 pad10[4];
    u8 *t14;
    u8 pad18[4];
    u8 *t1C;
} TexSet;

extern Player_5 D_800E37C0_9[];
extern TexSet *D_800DC0A0_2;

Gfx *func_80009754(Gfx *gdl, s32 a1, s32 a2, s32 pl) {
    s32 p;
    s32 sx, sy;
    s32 lvl, k;
    s32 i;
    u16 *pal;
    s32 x, y;

    if (D_800EFDBC == 8) {
        p = D_800C8CE0;
    } else {
        p = pl;
    }
    sx = 0x80000 / D_800F13EC;
    sy = 0x3C000 / D_800F13F0;
    gDPPipeSync(gdl++);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gdl++, 0x0C087008, 0);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetBlendColor(gdl++, 0, 0, 0, 0xFE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gdl = func_80002418(gdl, a1, 0, 1);
    gSPClipRatio(gdl++, FRUSTRATIO_6);

    lvl = D_800E37C0_9[p].f648 * 256.0f / 12000.0f;
    if (lvl < 0) {
        lvl = 0;
    }
    if (lvl > 255) {
        lvl = 255;
    }
    pal = D_800DC0A4_2 + a2 * 256;
    k = lvl * 87 / 256;
    k += 148;
    for (i = 148; i < 192; i++) {
        if (k < i) {
            pal[i] = 0x201;
        } else {
            pal[i] = 0x7C1;
        }
    }
    for (i = 192; i < 224; i++) {
        if (k < i) {
            pal[i] = 0x4201;
        } else {
            pal[i] = 0xFFC1;
        }
    }
    for (i = 224; i < 235; i++) {
        if (k < i) {
            pal[i] = 0x4001;
        } else {
            pal[i] = 0xF801;
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, pal);
    for (i = 0; i < 50; i += 25) {
        gDPLoadTextureBlock(gdl++, D_800DC0A0_2->t14 + i * 80, G_IM_FMT_CI, G_IM_SIZ_8b, 80, 25, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, D_800E1A50 * 4, (D_800E1A54 + i * D_800F13F0 / 240) * 4,
                            (D_800E1A50 + D_800F13EC * 80 / 512) * 4,
                            (D_800E1A54 + (i + 25) * D_800F13F0 / 240) * 4, 0, 0, 0, sx, sy);
    }
    gDPPipeSync(gdl++);
    gDPSetBlendColor(gdl++, 0, 0, 0, 8);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    if (D_800E37C0_9[p].b654 != 0) {
        gDPSetCombineLERP(gdl++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPLoadTextureBlock(gdl++, D_800DC0A0_2->tC, G_IM_FMT_IA, G_IM_SIZ_8b, 48, 16, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        k = (-D_800E76D0 * 8) & 0xFF;
        gDPSetPrimColor(gdl++, 0, 0, 255, 0, 0, k);
        gSPTextureRectangle(gdl++, (D_800E1A50 + D_800F13EC / 32) * 4, (D_800E1A54 + D_800F13F0 / 24) * 4,
                            (D_800E1A50 + D_800F13EC / 8) * 4, (D_800E1A54 + D_800F13F0 * 23 / 240) * 4,
                            0, 0, 0, sx, sy);
        gDPPipeSync(gdl++);
    }
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    for (i = 0; i < 50; i += 25) {
        gDPLoadTextureBlock(gdl++, D_800DC0A0_2->t1C + i * 80, G_IM_FMT_IA, G_IM_SIZ_8b, 80, 25, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, D_800E1A50 * 4, (D_800E1A54 + i * D_800F13F0 / 240) * 4,
                            (D_800E1A50 + D_800F13EC * 80 / 512) * 4,
                            (D_800E1A54 + (i + 25) * D_800F13F0 / 240) * 4, 0, 0, 0, sx, sy);
    }
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0x80);
    gSPMatrix(gdl++, a1 + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &D_800C2678, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPViewport(gdl++, &D_800C25E8);
    k = 55000 - lvl * 175;
    for (i = 0; i < 4; i++) {
        s32 x0 = D_800C23D0[i * 2];
        s32 y0 = D_800C23D0[i * 2 + 1];
        x = (x0 * D_800C2C60[(k >> 4) & 0xFFF] >> 14) + (y0 * D_800C2C60[((k >> 4) - 0x400) & 0xFFF] >> 14);
        y = (y0 * D_800C2C60[(k >> 4) & 0xFFF] >> 14) - (x0 * D_800C2C60[((k >> 4) - 0x400) & 0xFFF] >> 14);
        x = x * D_800F13EC / 320;
        y = y * D_800F13F0 / 240;
        x += D_800E1A50 + D_800DC0D8;
        y += D_800E1A54 + D_800DC0DC;
        x -= D_800F13EC / 2;
        y = D_800F13F0 / 2 - y;
        D_800DC0A8_2[a2 * 8 + i].v.ob[0] = x;
        D_800DC0A8_2[a2 * 8 + i].v.ob[1] = y;
        D_800DC0A8_2[a2 * 8 + i].v.ob[2] = 0x200;
        D_800DC0A8_2[a2 * 8 + i + 4].v.ob[0] = x + 4;
        D_800DC0A8_2[a2 * 8 + i + 4].v.ob[1] = y - 2;
        D_800DC0A8_2[a2 * 8 + i + 4].v.ob[2] = 0x200;
    }
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gSPVertex(gdl++, &D_800DC0A8_2[a2 * 8], 8, 0);
    gSP2Triangles(gdl++, 7, 4, 5, 0, 4, 5, 6, 0);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0x8C, 0x8C, 0x8C, 0xFF);
    gSP2Triangles(gdl++, 3, 0, 1, 0, 3, 1, 2, 0);
    gSPMatrix(gdl++, a1, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    return gdl;
}

Gfx *func_8000A690(s32 x, s32 y, Img *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw) {
    s32 step, row, n, sc, srow, sn, clip, wdt;
    s32 m;
    u8 *addr;

    m = 2;
    if (flags & 2) m = 1;
    step = ((flags & 1) ? m << 11 : m << 12) / (img->w * 8);
    if (img->h < step) step = img->h;

    switch (sx) {
    case 2:
        x = x * D_800F13EC / 512 + D_800F13EC / 2;
        break;
    case 3:
        x = x * D_800F13EC / 512 + D_800F13EC;
        break;
    case 1:
        x = x * D_800F13EC / 512;
        break;
    }
    switch (sy) {
    case 2:
        y = y * D_800F13F0 / 240 + D_800F13F0 / 2;
        break;
    case 3:
        y = y * D_800F13F0 / 240 + D_800F13F0;
        break;
    case 1:
        y = y * D_800F13F0 / 240;
        break;
    }
    if (x >= 0) {
        clip = 0;
    } else {
        if (-x < img->w * 8) {
            clip = -x;
            x = 0;
        } else {
            return gdl;
        }
    }
    for (row = 0; row < img->h; row += step) {
        n = step;
        if (img->h - row < step) n = img->h - row;
        if (flags & 0x100) {
            sc = 2;
            srow = row / 2;
            sn = n / 2;
        } else {
            sc = 1;
            srow = row;
            sn = n;
        }
        gDPPipeSync(gdl++);
        if (flags & 2) {
            if (flags & 0x20) addr = img->addr + row * img->w * 8;
            else addr = img->addr + (img->h - row - n) * img->w * 8;
        } else {
            addr = img->addr + (img->h - row - n) * img->w * 4;
        }
        if (flags & 4) {
            if (flags & 2) {
                gDPLoadTextureTile(gdl++, addr, G_IM_FMT_IA, G_IM_SIZ_8b, img->w * 8, 0, clip, 0, img->w * 8 - 1, n - 1,
                                   0, 0, 0, 0, 0, 0, 0);
            } else {
                while (1);
            }
        } else if (flags & 2) {
            gDPLoadTextureTile(gdl++, addr, G_IM_FMT_CI, G_IM_SIZ_8b, img->w * 8, 0, clip, 0, img->w * 8 - 1, n - 1,
                               0, 0, 0, 0, 0, 0, 0);
        } else {
            gDPLoadTextureTile_4b(gdl++, addr, G_IM_FMT_CI, img->w * 8 - clip, 0, clip, 0, img->w * 8 - 1, n - 1,
                                  img->pal & 0xF, 0, 0, 0, 0, 0, 0);
        }
        gDPPipeSync(gdl++);
        if (flags & 0x20) {
            gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + img->w * 8 - clip) * 4, (y + srow + sn) * 4,
                                0, clip << 5, 0, sc << 10, sc << 10);
        } else if (flags & 0x40) {
            gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + img->w * 8) * 4, (y + srow + sn) * 4,
                                0, (img->w << 8) - 0x20, (n - 1) << 5, -(sc << 10), -(sc << 10));
        } else {
            if (flags & 0x80) wdt = dw;
            else wdt = img->w * 8;
            if (D_800F13EC == 640) {
                gSPTextureRectangle(gdl++, x * 4, (y + srow * 2) * 4, x * 4 + (wdt - clip) * 5, (y + srow * 2 + sn * 2) * 4,
                                    0, clip << 5, (n - 1) << 5, sc * 0x333, -(sc << 9));
            } else {
                gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + wdt / sc - clip) * 4, (y + srow + sn) * 4,
                                    0, clip << 5, (n - 1) << 5, sc << 10, -(sc << 10));
            }
        }
        gDPPipeSync(gdl++);
    }
    return gdl;
}

typedef struct { s16 v[8]; } Q;

extern Q D_800E19D0[];
extern Q D_800E1AC0[];

void func_8000B028(void)
{
    s32 i;
    s32 j;
    s32 n;
    Q *pb;
    V4 *s;
    V4 *p;
    s32 t;
    s32 u;

    if (D_800C2384 != D_800C2380) {
        if (D_800C2380 != 0) {
            D_800C2594 = 2;
            D_800E19C0 = D_800E19D0;
            D_800E1B40 = 24000;
        } else {
            D_800E19C0 = D_800E1AC0;
            D_800C2594 = D_800E1898;
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                D_800E1718_7[i][j].x = D_800C2494_2[i][j].x * D_800F13EC_2[0] / 512;
                D_800E1718_7[i][j].z = D_800C2494_2[i][j].z * D_800F13EC_2[0] / 512;
                D_800E1718_7[i][j].y = D_800C2494_2[i][j].y * D_800F13F0_3[0] / 240;
                D_800E1718_7[i][j].w = D_800C2494_2[i][j].w * D_800F13F0_3[0] / 240;
            }
        }
        if (D_800C2380 != 0) {
            t = *D_800F13EC_2;
            D_800E1758[0] = D_800E1758[-4];
            p = D_800E1758 - 4;
            p[5].x = t / 16;
            p[5].z = *D_800F13EC_2 * 130 / 512;
            p[5].w = D_800F13F0_3[0] * 70 / 240;
            if (D_800CCCA8 == 2) {
                p[5].y = 100;
            } else {
                p[5].y = 30;
            }
        }
        pb = D_800E19C0;
        n = D_800C2594 - 1;
        for (D_800C2598 = 0; D_800C2598 < 4; D_800C2598++) {
            s = (V4 *)((u8 *)D_800E1718_7 + (D_800C2598 * 16 + n * 64));
            pb[D_800C2598].v[0] = s->z * 2;
            pb[D_800C2598].v[1] = s->w * 2;
            pb[D_800C2598].v[4] = (s->x + s->z / 2) * 4;
            pb[D_800C2598].v[5] = (s->y + s->w / 2) * 4;
            pb[D_800C2598 + 4].v[0] = s->z * 2;
            pb[D_800C2598 + 4].v[1] = s->w * 2;
            pb[D_800C2598 + 4].v[4] = (s->x + s->z / 2) * 4;
            pb[D_800C2598 + 4].v[5] = (s->y + s->w / 2) * 4 + 2;
            pb[D_800C2598].v[2] = 0x1FF;
            pb[D_800C2598].v[6] = 0x1FF;
            pb[D_800C2598 + 4].v[2] = 0x1FF;
            pb[D_800C2598 + 4].v[6] = 0x1FF;
        }
        if (D_800C2380 != 0) {
            D_800E19D0[0].v[2] = 0xFF;
            D_800E19D0[0].v[6] = 0x2FF;
            D_800E19D0[4].v[2] = 0xFF;
            D_800E19D0[4].v[6] = 0x2FF;
            D_800E19D0[1].v[2] = 0xFF;
            D_800E19D0[1].v[6] = 0xFF;
            D_800E19D0[5].v[2] = 0xFF;
            D_800E19D0[5].v[6] = 0xFF;
        }
        D_800C2384 = D_800C2380;
    }
}

typedef struct { s32 x, y, w, h; s32 pad[12]; } View;

extern View D_800E1718_8[];

Gfx *func_8000B3C8(Gfx *gdl)
{
    View *e;
    s32 i, y, done, alt, col;
    s32 c;
    s32 *font;
    s32 n;
    char buf[64];

    D_800E19BC = 0xF0 - (D_800E76D0 / 2) % (D_800DC0B0 << 4);
    func_800C2224();
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718_8[D_800C2594 - 1].x, D_800E1718_8[D_800C2594 - 1].y, D_800E1718_8[D_800C2594 - 1].x + D_800E1718_8[D_800C2594 - 1].w, D_800E1718_8[D_800C2594 - 1].y + D_800E1718_8[D_800C2594 - 1].h);
    if (D_800C2280 & 0x800) {
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPLoadTLUT_pal256(gdl++, D_800E1560_2[1]);
        for (i = 0; i < 0xF0; i += 8) {
            gDPLoadTextureBlock(gdl++, D_800E1560_2[3] + i * 256, G_IM_FMT_CI, G_IM_SIZ_8b, 256, 8, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 8, 3, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gdl++, 0, i << 2, 0x800, (i + 8) << 2, G_TX_RENDERTILE, 0, 0, 0x200, 0x400);
        }
    }
    y = D_800E19BC;
    i = 0;
    done = 0;
    alt = 0;
    if (y < 0x104) {
    n = 0;
    do {
        switch (D_800C27B4[i]) {
        case 0xD:
        case '@':
            buf[n] = 0;
            if (y >= -0x13 && n != 0) {
                if (alt) {
                    font = D_800C27B0;
                } else {
                    font = D_800C27AC;
                }
                func_800C1F04((u8 *)font);
                func_800C1F2C(font, buf);
                func_800C0ACC(font, col * 128 - font[6] / 2, y);
                alt = 0;
            }
            n = 0;
            if (D_800C27B4[i] == '@') {
                i++;
                c = D_800C27B4[i];
                switch (c) {
                case 'E':
                    done = 1;
                    i = -1;
                    break;
                case 'H':
                    alt = 1;
                    break;
                case '1':
                case '2':
                case '3':
                    col = c - '0';
                    break;
                }
            } else {
                y += 16;
            }
            i++;
            break;
        case 10:
            i++;
            break;
        default:
            buf[n++] = D_800C27B4[i++];
            break;
        }
    } while (!done && y < 0x104);
    }
    return func_800C1394(gdl);
}

void func_8000B938(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9)
{
    s32 *t;
    if (D_800EFD64_3[11] == 0) {
        func_80011490(p1, p2, p3, p4, p5, p6, p7, p8);
    } else {
        s32 *tb = D_800E8090;
        t = tb + p9 * 3;
        func_80011490(p1, p2, p3, p4, p5 - t[0], p6 - t[1], p7 - t[2], p8);
    }
}

#define PKT(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (unsigned int)(b); }

#define LOADUCODE(pkt, s, d) { \
    Gfx *_g = (Gfx *)(pkt); \
    _g->words.w0 = 0xE1000000; \
    _g->words.w1 = (unsigned int)(d); \
    _g = (Gfx *)(pkt); \
    _g->words.w0 = 0xDD0007FF; \
    _g->words.w1 = (unsigned int)(s); \
}

Gfx *func_8000B9CC(Gfx *gdl, s32 a1, s32 a2, s32 a3, s32 a4)
{
    PKT(gdl++, 0xE7000000, 0);
    LOADUCODE(gdl++, a3, a4);
    PKT(gdl++, 0xDB060000, 0);
    PKT(gdl++, 0xDE000000, D_800C2A38);
    PKT(gdl++, 0xDE000000, D_800C2A00);
    return gdl;
}

void func_8000BA58(void) {
    D_800C2384 = 1;
    D_800C2380 = 0;
    func_8000B028();
}

void func_8000BA84(void)
{
    Blk *p1;
    Blk *p2;
    Blk *e;

    if (D_800C2388 != 0) {
        p2 = (Blk *)D_800C24D4_2;
        p1 = (Blk *)D_800C23F4_2;
        e = p1 + 4;
        do {
            *p2 = *p1;
            p1++;
            p2++;
        } while (p1 != e);
    } else {
        p2 = (Blk *)D_800C24D4_2;
        p1 = (Blk *)D_800C2434_2;
        e = p1 + 4;
        do {
            *p2 = *p1;
            p1++;
            p2++;
        } while (p1 != e);
    }
    func_8000BA58();
}

void func_8000BB34(u8 *src, s32 x, s32 y, s32 w, s32 h)
{
    s32 i, j, xx;

    i = 0;
    do {
        if (h > 0) {
            do {
                if (y < 0) goto next;
                if (y >= 16) goto next;
                do { xx = x; } while (0);
                for (j = 0; j < w; j++, xx++) {
                    if (xx < 0) continue;
                    if (xx >= 32) continue;
                    D_800E1550[y * 32 + xx] = src[i * 64 + j];
                }
                do { next: i++; y++; } while (0);
            } while (i < h);
        }
    } while (0);
}

void func_8000BBC8(u8 *param_1, u8 *param_2, u8 *param_3, s32 param_4, s32 param_5)
{
    s32 total = param_5 * 3;
    s32 i = 0;
    u8 *pa = param_1;
    u8 *pb = param_2;
    u8 *po = param_3;
    if (total > 0) {
        s32 inv = 0xFF - param_4;
        s32 lim = total;
        do {
            *po = ((s32)*pa * inv + (s32)*pb * param_4) >> 8;
            pa++;
            i++;
            pb++;
            po++;
        } while (i < lim);
    }
}

void func_8000BC30(u16 *p1, u16 *p2, u16 *p3, s32 a, s32 n)
{
    s32 i;
    u32 c1, c2;
    s32 r, g, b;

    for (i = 0; i < n; i++) {
        c1 = *p1++;
        c2 = *p2++;
        r = (s32)((c1 >> 11) * (0xFF - a) + (c2 >> 11) * a) >> 8;
        g = (s32)(((c1 >> 6) & 0x1F) * (0xFF - a) + ((c2 >> 6) & 0x1F) * a) >> 8;
        b = (s32)(((c1 >> 1) & 0x1F) * (0xFF - a) + ((c2 >> 1) & 0x1F) * a) >> 8;
        *p3++ = (r << 11) | (g << 6) | (b << 1);
    }
}
