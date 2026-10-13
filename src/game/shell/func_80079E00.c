#include "ctx.h"

typedef struct { s32 a, b; } Pair;

typedef struct {
    u32 w0;
    u8 pad4[0xC - 0x4];
    s32 wC;
    s32 w10;
    u8 pad14[0x1C - 0x14];
    s32 w1C[6];
    u8 pad34[0x3C - 0x34];
    s32 w3C;
    u8 pad40[0x4C - 0x40];
    s32 w4C;
    u8 pad50[0x64 - 0x50];
    s32 w64;
    s32 w68;
    s32 w6C;
    s32 w70;
    s32 w74[48];
    u8 pad134[0x13C - 0x134];
    u16 *p13C;
    u8 pad140[0x17C - 0x140];
    s32 w17C;
    u8 pad180[0x2A8 - 0x180];
    Pair s2A8[4];
    s8 b2C8[4];
    s8 b2CC[4];
    u8 pad2D0[0x2E0 - 0x2D0];
    s16 h2E0[4];
    s32 w2E8;
    u8 pad2EC[0x2F4 - 0x2EC];
    s32 w2F4[24];
    u8 pad354[0x36C - 0x354];
    s32 w36C[7];
    s32 w388;
    s32 w38C;
    s32 w390;
    u8 pad394[0x398 - 0x394];
    s32 w398;
    s32 w39C;
    s32 w3A0;
    s32 w3A4;
    s32 w3A8;
    s32 w3AC;
    s32 w3B0;
    u8 pad3B4[0x3B8 - 0x3B4];
    s32 w3B8;
    s32 w3BC;
    u8 pad3C0[0x3C8 - 0x3C0];
    s32 w3C8;
    u8 pad3CC[0x3DC - 0x3CC];
    s32 w3DC;
    u8 pad3E0[0x3E8 - 0x3E0];
    void *p3E8;
    s32 w3EC;
    u8 pad3F0[0x40C - 0x3F0];
    u8 b40C;
    u8 b40D;
    u8 pad40E[0x410 - 0x40E];
    s32 w410;
    s32 w414;
    u8 pad418[0x438 - 0x418];
    s32 w438;
    u8 b43C[6];
    u8 pad442[0x448 - 0x442];
    u16 h448;
    s16 h44A;
    s16 *p44C;
    u8 pad450[0x454 - 0x450];
    s16 h454;
    s16 h456;
    s16 h458;
    u8 pad45A[0x45C - 0x45A];
    s32 w45C;
    s32 w460;
    s16 h464;
    s8 name[16];
    s16 h476;
    s32 w478;
    u8 pad47C[0x480 - 0x47C];
    s32 w480;
    s32 w484;
    s16 h488;
    u8 pad48A[0x730 - 0x48A];
    s32 w730;
    u8 rgb734[4][3];
    s32 w740;
    s32 w744;
    s32 w748;
    s32 w74C;
    s32 w750;
    u8 pad754[0x758 - 0x754];
    s8 b758[4];
    u8 pad75C[0x770 - 0x75C];
    s32 w770;
    s32 w774;
    s32 w778[6];
    s32 w790[6];
    u8 b7A8;
    u8 pad7A9;
    s8 b7AA[6];
} Game;

typedef struct Item {
    u8 pad0[4];
    s32 w4;
    u8 pad8[0x10 - 0x8];
    u16 h10;
    s16 h12;
    s16 h14;
    s16 h16;
    s16 h18;
    u8 pad1A[2];
    s32 **p1C;
    s32 *p20;
    s32 w24;
    u8 pad28[4];
} Item;

typedef struct { u8 pad[0xA0]; } Rec;
typedef struct { u8 pad[0x24]; } Rec24;
typedef struct { Rec24 *p0; } RecPtr;
typedef struct { u8 pad[8]; u16 h8; u8 padA[6]; } Rec10;
typedef struct { u8 pad[0x14]; u8 b14; u8 b15; u8 b16; } Slot;

typedef struct {
    s32 w0;
    u8 pad4[0x1C - 0x4];
    s32 w1C;
    u8 pad20[0x24 - 0x20];
    s32 w24;
    void *w28;
} Snd;

extern Game D_800EFD70;
extern Rec *D_800EFD64;
extern Rec D_800CB210[];
extern Rec D_800CB670[];
extern Item D_800CD63C[];
extern Rec10 D_800CD370[];
extern RecPtr *D_800CA930;
extern Rec24 D_800CA9C8[];
extern s32 D_800F47C0;
extern Snd D_800F4A28;
extern s32 D_800F4A50;
extern void *D_800F4A54;
extern u8 D_800F05A8[];
extern s32 *D_800CD148[];
extern s32 *D_800CD0B8[];
extern s32 D_800CA958[];
extern s32 D_800C2388;
extern u8 D_800C292C;
extern s32 D_800C29E0;
extern s32 D_800C8CE4;
extern s32 D_800CCD64;
extern s32 D_800D09F8;
extern s32 D_800D1204;
extern s32 D_800D1380, D_800D1388, D_800D1394, D_800D1398, D_800D139C, D_800D13A0;
extern s32 D_800D13B4, D_800D13BC, D_800D13C0, D_800D13C8, D_800D13CC, D_800D13E0;
extern s32 D_800D13F4, D_800D140C, D_800D1410, D_800D1418, D_800D141C, D_800D1424;
extern s32 D_800D1428, D_800D142C, D_800D1430, D_800D143C, D_800D1444, D_800D1448;
extern s32 D_800D144C, D_800D1450, D_800D1470, D_800D1488, D_800D148C, D_800D1490;
extern s32 D_800D1494, D_800D1498, D_800D149C, D_800D14A0, D_800D14A4, D_800D14AC;
extern s32 D_800D14C8;
extern u8 D_800D94B0[];
extern s32 D_800E2038;
extern s32 D_800E203C;
extern s32 D_800E7728;
extern u8 D_800600D4[];
extern u8 D_800732D0[];
extern u8 D_800881D4[];

void func_80010AFC(void);
void func_800154FC(s32);
void func_80064D28(s32 a, s32 b);
void func_80079AA4(void);
void func_80079B64(void);
void func_80088AE0(void);
s32 func_800AEC10(s8 *s);
void func_800B2B00(s32);
s32 func_800B781C();
void func_800BBABC(s32 a, s32 b);
void *func_800BC908(s32 size, u32 flags, void *owner, s32 tag);
void func_800BCB04(void *ptr);
s32 func_800BF79C(s32 a, s32 b);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);

#define LD(x) func_800BF79C(D_800F47C0, x)

void func_80079E00(Item *p) {
    s32 i;
    s32 j;
    s32 c;

    func_80010AFC();
    if (p == NULL) {
        return;
    }
    D_800EFD70.w740 = 0;
    D_800EFD70.w480 = LD(D_800D1410);
    D_800EFD70.w484 = LD(D_800D1398);
    if ((p->h10 & 0x20) && p->w4 == 0) {
        for (i = 0; i < 48; i++) {
            D_800EFD70.w74[i] = (s32)func_800C1E60(0, 0x40, 4, 2, 0x20);
        }
    }
    if (p->h10 & 0x80) {
        D_800EFD70.w390 = LD(D_800D1450);
        D_800EFD70.w3DC = LD(D_800D14C8);
    }
    if (p->p1C != NULL) {
        if (p->h16 == 4) {
            for (i = 0; i < 7; i++) {
                D_800EFD70.w36C[i] = LD(*p->p1C[i]);
            }
        } else {
            p->w24 = LD((s32)*p->p1C);
        }
    } else {
        p->w24 = 0;
    }
    func_800B2B00((p->h10 & 4) ? 0x6A : 0xAA);

    switch (p->h16) {
    case 23:
        D_800EFD70.w0 |= 0x10000000;
    case 21:
        D_800EFD70.w750 = 0xB4;
        break;
    case 24:
        D_800EFD70.w390 = LD(D_800D14A0);
        D_800EFD70.w3DC = LD(D_800D13BC);
        D_800EFD70.w750 = 0x4B0;
        break;
    case 25:
        D_800EFD70.w390 = LD(D_800D13E0);
        D_800EFD70.w3DC = LD(D_800D142C);
        D_800EFD70.w750 = 0x258;
        break;
    case 22:
        D_800EFD70.w750 = 0x12C;
        D_800EFD70.w3A0 = LD(D_800D13B4);
        break;
    case 0:
        for (i = 0; i < 6; i++) {
            D_800EFD70.w778[i] = D_800EFD70.w1C[i];
            D_800EFD70.w790[i] = D_800EFD70.b43C[i];
        }
        D_800EFD70.w1C[0] = 4;
        D_800EFD70.w1C[2] = 1;
        D_800EFD70.w1C[3] = 2;
        D_800EFD70.w1C[4] = 3;
        D_800EFD70.w1C[1] = 0;
        D_800EFD70.w1C[5] = 5;
        for (i = 0; i < 6; i++) {
            D_800EFD70.b43C[i] = i;
        }
        D_800EFD70.w0 &= ~0x80;
        D_800EFD70.w38C = LD(D_800D1490);
        D_800EFD70.w390 = LD(D_800D14A4);
        D_800EFD64 = D_800CB210;
        D_800EFD70.w4C = 0;
        D_800EFD70.w3C = 1;
        func_80064D28(0, 6);
        func_800154FC(0);
        break;
    case 6:
        D_800EFD70.w770 = 0;
        D_800EFD70.w2E8 = 0;
        func_800BBABC(D_800C29E0, 0x25800);
        D_800EFD70.w774 = func_800B781C(D_800D1204);
        func_800BBABC(0, 0);
        D_800EFD70.w390 = LD(D_800D1444);
        D_800EFD70.w3A4 = LD(D_800D1428);
        D_800EFD70.w3AC = LD(D_800D1424);
        D_800EFD70.w3A0 = LD(D_800D1470);
        if (D_800CCD64 != 0) {
            D_800EFD70.h448 = 6;
            D_800CCD64 = 0;
        }
        D_800EFD70.h448 |= 0x100;
        break;
    case 19:
        for (i = 1; i < 0x100; i++) {
            c = (i / 3) >> 3;
            D_800EFD70.p13C[i] = (c << 1) | (c << 6) | (c << 11) | 1;
        }
        D_800EFD70.h476 = 0;
        D_800EFD70.w3A0 = LD(D_800D1470);
        D_800EFD70.w45C = LD(D_800D139C);
        D_800EFD70.w390 = LD(D_800D1498);
        D_800EFD70.w39C = LD(D_800D13A0);
        func_80079AA4();
        break;
    case 9:
    case 20:
        D_800EFD70.w2E8 = 0;
        i = 0;
        if (p->h16 == 9) {
            D_800EFD70.w2F4[0] = LD(D_800D13CC);
            D_800EFD70.name[0] = 0x41;
            for (i = 1; i < 16; i++) {
                D_800EFD70.name[i] = 0;
            }
            D_800EFD70.h464 = 0;
            D_800EFD70.h456 = 0;
            D_800EFD70.h458 = 0;
            D_800EFD70.h454 = 0;
        } else {
            D_800EFD70.w460 = -1;
            D_800EFD70.b7A8 = 0xFF;
            for (i = 0; i < 4; i++) {
                D_800EFD70.b758[i] = -1;
                if (((Slot *)(D_800F05A8 + i * 3))->b14 == 0xFF && ((Slot *)(D_800F05A8 + i * 3))->b15 == 0xFF) {
                    D_800EFD70.b758[i] = ((Slot *)(D_800F05A8 + i * 3))->b16;
                    if (D_800EFD70.w460 == -1) {
                        D_800EFD70.w460 = i;
                    }
                }
            }
            if (D_800EFD70.rgb734[D_800EFD70.b758[D_800EFD70.w460]][0] != 0) {
                __builtin_memcpy(D_800EFD70.name, D_800EFD70.rgb734[D_800EFD70.b758[D_800EFD70.w460]], 3);
                D_800EFD70.name[3] = 0;
                D_800EFD70.h464 = func_800AEC10(D_800EFD70.name);
                D_800EFD70.h456 = 0x31F;
                D_800EFD70.h458 = 0x31F;
                D_800EFD70.h454 = 0x27;
            } else {
                D_800EFD70.name[0] = 0x41;
                D_800EFD70.name[1] = 0;
                D_800EFD70.name[2] = 0;
                D_800EFD70.name[3] = 0;
                D_800EFD70.h464 = 0;
                D_800EFD70.h456 = 0;
                D_800EFD70.h458 = 0;
                D_800EFD70.h454 = 0;
            }
            for (i = 0; i < D_800EFD70.h44A; i++) {
                if (D_800EFD70.p44C[i] == D_800EFD64 - D_800CB210) {
                    break;
                }
            }
            D_800EFD70.w2F4[0] = LD(*D_800CD148[D_800CA958[i]]);
        }
        D_800EFD70.w390 = LD(D_800D148C);
        D_800EFD70.w3A0 = LD(D_800D1380);
        D_800EFD70.w478 = LD(D_800D1488);
        D_800EFD70.w3EC = 0;
        D_800EFD70.w39C = LD(D_800D13A0);
        func_80079AA4();
        break;
    case 13:
    case 14:
        if (p->h16 == 14 && D_800EFD70.w4C == 1 && D_800CA930 != NULL
            && D_800CA930->p0 - D_800CA9C8 == 4) {
            func_80079B64();
        }
        if (D_800EFD70.w0 & 0x8000) {
            D_800EFD70.w2E8 = (p->h16 == 13) ? 3 : 4;
            D_800EFD70.w0 = (D_800EFD70.w0 | 0x20000) & ~0x8000;
        } else {
            D_800EFD70.w2E8 = 0;
        }
        D_800EFD70.w3EC = 0;
        D_800EFD70.w39C = LD(D_800D13A0);
        func_80079AA4();
        D_800EFD70.w390 = LD(D_800D143C);
        D_800EFD70.w3DC = LD(D_800D14C8);
        D_800EFD70.w414 = LD(D_800D13C0);
        D_800EFD70.w3A4 = LD(D_800D1428);
        D_800EFD70.w3AC = LD(D_800D1424);
        break;
    case 7:
        func_80079B64();
        D_800EFD70.w2E8 = 0;
        D_800EFD70.w3EC = 0;
        func_80079AA4();
        D_800EFD70.w39C = LD(D_800D13A0);
        D_800EFD70.w390 = LD(D_800D143C);
        D_800EFD70.w3A4 = LD(D_800D1428);
        D_800EFD70.w3AC = LD(D_800D1424);
        D_800EFD70.w414 = LD(D_800D13C0);
        D_800EFD70.w3DC = LD(D_800D14C8);
        break;
    case 12:
        D_800F4A50 = 0;
        switch (D_800EFD70.h488) {
        case 0:
            D_800F4A28.w0 = 0;
            D_800F4A28.w24 = 0;
            D_800F4A28.w1C = 0;
            break;
        case 1:
            if (D_800C8CE4 == 0) {
                D_800C8CE4 = D_800E203C;
            }
            D_800F4A28.w1C = 100;
            D_800F4A28.w0 = 1;
            D_800F4A28.w28 = D_800600D4;
            D_800F4A28.w24 = D_800C8CE4 + 0x6400;
            break;
        case 2:
            D_800F4A28.w1C = 40;
            D_800F4A28.w0 = 2;
            D_800F4A28.w24 = D_800C8CE4;
            break;
        case 3:
            if (D_800EFD70.p3E8 != NULL) {
                func_800BCB04(D_800EFD70.p3E8);
            }
            D_800EFD70.p3E8 = func_800BC908(0x400, 0, D_800D94B0, 0x18E6);
            D_800F4A28.w24 = (s32)D_800EFD70.p3E8;
            D_800F4A28.w1C = 4;
            D_800F4A28.w0 = 1;
            D_800F4A28.w28 = D_800881D4;
            break;
        case 4:
            D_800F4A28.w1C = 4;
            D_800F4A28.w0 = 2;
            D_800F4A28.w24 = (s32)D_800EFD70.p3E8;
            break;
        }
        D_800F4A54 = D_800732D0;
        func_80088AE0();
        break;
    case 3:
        D_800C2388 = 0;
        if (p - D_800CD63C == 7) {
            D_800EFD70.w4C = 0;
            while (D_800CD370[*p->p20].h8 & 1) {
                (*p->p20)--;
            }
        }
        D_800EFD70.w0 &= 0xFDFFFFFF;
        D_800EFD70.w744 = LD(D_800D141C);
        D_800EFD70.w748 = LD(D_800D1430);
        D_800EFD70.w2E8 = 0;
        if (p - D_800CD63C == 8) {
            D_800EFD70.w3C = 1;
        }
        if (p - D_800CD63C == 10) {
            D_800EFD70.w10 = 0;
        }
    case 11:
        D_800EFD70.w74C = LD(D_800D149C);
        D_800EFD70.w17C = 0;
        D_800EFD70.w3EC = 0;
        D_800EFD70.w438 = LD(D_800D14AC);
        D_800EFD70.w388 = LD(D_800D140C);
        D_800EFD70.w3A0 = LD(D_800D1418);
        D_800EFD70.w398 = LD(D_800D13F4);
        D_800EFD70.w39C = LD(D_800D1394);
        D_800EFD70.w3A8 = LD(D_800D13C8);
        D_800EFD70.w3A4 = LD(D_800D1428);
        D_800EFD70.w3AC = LD(D_800D1424);
        for (i = 0; i < p->h18; i++) {
            if (p->h16 == 11) {
                D_800EFD70.w2F4[i + p->h14] = LD(*D_800CD148[D_800CA958[i + p->h14]]);
            } else {
                D_800EFD70.w2F4[i + p->h14] = LD(*D_800CD0B8[i + p->h12]);
            }
        }
        break;
    case 18:
        D_800EFD70.w2E8 = 13;
        D_800EFD70.w750 = 6;
    case 4:
        D_800EFD70.w70 = 0;
        switch (D_800EFD70.w410) {
        case 1:
            if (D_800EFD70.wC >= 2) {
                D_800EFD70.wC = 0;
            }
            break;
        case 2:
            if (D_800EFD70.wC >= 3) {
                D_800EFD70.wC = 0;
            }
            break;
        }
        if (D_800EFD70.w410 == 0) {
            D_800EFD70.wC = 6;
        } else if (D_800EFD70.wC == 6) {
            D_800EFD70.wC = 0;
        }
        D_800EFD70.w6C = 0;
        D_800EFD70.w17C = 0;
        D_800EFD70.w3EC = 0;
        D_800EFD70.w438 = LD(D_800D14AC);
        D_800EFD70.w3A4 = LD(D_800D1428);
        D_800EFD70.w3AC = LD(D_800D1424);
        D_800EFD70.w3A0 = LD(D_800D1418);
        D_800EFD70.w398 = LD(D_800D13F4);
        D_800EFD70.w39C = LD(D_800D1394);
        D_800EFD70.w3A8 = LD(D_800D13C8);
        D_800EFD70.w3B0 = LD(D_800D1448);
        for (i = 0; i < 24; i++) {
            D_800EFD70.w2F4[i] = LD(*D_800CD148[i]);
        }
        D_800EFD70.w64 = 0;
        D_800EFD70.w68 = 0;
        break;
    case 2:
        D_800EFD70.b40C = 0;
        D_800EFD70.b40D = 0;
        for (i = 3; i >= 0; i--) {
            D_800EFD70.h2E0[i] = 0;
        }
        for (i = 0; i < 6; i++) {
            D_800EFD70.b7AA[i] = i;
        }
        D_800EFD70.w2E8 = 0;
        if (D_800EFD70.w3C == 1) {
            D_800EFD70.w3C8 = LD(D_800D1494);
        }
        j = 0;
        D_800EFD70.b2C8[1] = -1;
        D_800EFD70.b2C8[2] = -1;
        D_800EFD70.b2C8[3] = -1;
        D_800EFD70.b2CC[0] = -1;
        D_800EFD70.w3B8 = D_800EFD70.w4C;
        D_800EFD70.w3BC = (s32)D_800EFD64;
        D_800EFD70.w0 &= ~0x3C90;
        for (i = 0; i < 4; i++) {
            if ((D_800C292C >> i) & 1) {
                D_800EFD70.b2C8[j] = i;
                D_800EFD70.b2CC[i] = j;
                j++;
            }
        }
        for (i = 0; i < 4; i++) {
            D_800EFD70.s2A8[i].a = (((D_800E2038 & 0x1FF) << 7) + ((-(i & 1) + (i & 2)) << 14)) & 0xFFFF;
            D_800EFD70.s2A8[i].b = 0;
        }
        D_800EFD64 = D_800CB670;
        D_800EFD70.w4C = 0;
        D_800EFD70.w0 |= 0x10;
        D_800EFD70.w390 = LD(D_800D1388);
        D_800EFD70.w3DC = LD(D_800D144C);
        D_800EFD70.w74C = LD(D_800D149C);
        func_80064D28(0, 6);
        func_800154FC(0);
        D_800EFD70.w730 = func_800BF79C(D_800E7728, D_800D09F8);
        break;
    }
}
