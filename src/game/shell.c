#include "game.h"

typedef struct {
    u8 unk0[0xD];
    u8 id;
    u8 unkE[0x28 - 0xE];
} Track_2;

typedef struct {
    u8 pad[0x22];
    u16 f22;
    u8 pad24[0x50 - 0x24];
    u8 f50[6];
} Cfg_3;

typedef struct {
    u8 pad[0x669];
    u8 f669;
    u8 pad66A[0x68C - 0x66A];
    s32 f68C;
    u8 pad690[0x7E0 - 0x690];
} Ent_3;

extern Track_2 D_800CADE4_2[];
extern Cfg_3 D_800F05A8_6;
extern Ent_3 D_800E37C0_3[];

s32 func_800732D0(u8 *src, char *dst, s32 flag)
{
    s32 trk, q, w, ret;
    s32 n, m, v;

    dst[0] = 0;
    if (func_80060078(src, (u32 *)&trk, (u32 *)&q, (u32 *)&w) == 0) {
        w = w - 0x102;
        __builtin_memcpy(dst, D_800D946C, 8);
        func_8000ED30(dst, D_800CB210_13 + trk * 160);
        if (flag == 0) {
            func_8000ED30(dst, D_800D91CC);
            v = w / 3600;
            if (v < 10) {
                func_8000ED30(dst, D_800D91D0);
            }
            func_8000EE30(v, dst + strlen(dst));
            func_8000ED30(dst, D_800D9474);
            n = strlen(dst);
            v = w / 60 % 60;
            dst[n] = v / 10 + '0';
            n++;
            dst[n] = v % 10 + '0';
            n++;
            dst[n] = '.';
            v = w % 60 * 100 / 60;
            n++;
            dst[n] = v / 10 + '0';
            n++;
            dst[n] = v % 10 + '0';
            n++;
            dst[n] = 0;
        }
        ret = D_800F01F8;
        ret = ret ^ 3;
    } else {
        if (src[4] != 0x35) {
            __builtin_memcpy(dst, D_800D9480, 28);
            return 1;
        }
        __builtin_memcpy(dst, D_800D9478, 8);
        m = strlen(dst);
        dst[m + 10] = 0;
        m = strlen(dst);
        __builtin_memcpy(dst + m, src + 6, 10);
        ret = D_800F01F8;
        ret = ret ^ 1;
    }
    return (ret == 0) << 1;
}

typedef struct {
    u8 pad[0x13C];
    u16 *p13C;
} S70_3;

extern S70_3 D_800EFD70_53;

void func_80073664(s32 alpha, u16 *src)
{
    s32 i;
    s32 r, g, b;
    u32 p;

    D_800EFD70_53.p13C[0] = 0;
    for (i = 1; i < 0x100; i++) {
        if (alpha == 0xFF) {
            b = i;
            g = i;
            r = i;
        } else {
            p = src[i];
            b = (p >> 11) * 8;
            g = (p >> 3) & 0xF8;
            r = (p << 2) & 0xF8;
            b = b + ((i - b) * alpha) / 255;
            g = g + ((i - g) * alpha) / 255;
            r = r + ((i - r) * alpha) / 255;
        }
        D_800EFD70_53.p13C[i] = ((r >> 3) << 1) | ((g >> 3) << 6) | ((b >> 3) << 11) | 1;
    }
}

typedef struct {
    s32 w0;
    s32 w4;
    s32 w8;
    s32 wC;
    s32 w10;
    s32 w14;
    s32 w18;
    s32 w1C;
    s32 w20;
    s32 w24;
    s32 w28;
    s32 w2C;
    s32 w30;
    u8 pad34[8];
    s32 w3C;
    s32 w40;
    s32 w44;
    s16 h48;
    s16 h4A;
    s32 w4C;
    u8 b50;
    u8 b51;
    u8 b52;
    u8 b53;
    s32 w54;
    u8 pad58[0x13C - 0x58];
    s32 w13C;
    u8 pad140[0x2A4 - 0x140];
    u8 b2A4;
    u8 b2A5;
    u8 pad2A6[0x3C0 - 0x2A6];
    s32 w3C0;
    u8 pad3C4[0x3FC - 0x3C4];
    void *p3FC;
    s32 w400;
    s32 w404;
    u8 pad408[0x43C - 0x408];
    u8 b43C[6];
    u8 pad442[0x448 - 0x442];
    s16 h448;
    s16 h44A;
    s32 w44C;
    u8 pad450[0x490 - 0x450];
    V7 rec[25];
    u8 pad74C[0x75C - 0x74C];
    s32 w75C;
    u8 b760[4];
    u8 pad764[0x7AA - 0x764];
    u8 b7AA[6];
} Game_27;

typedef struct {
    u8 pad0[0x34];
    V7 r34;
    s32 w50;
    u8 pad54[0xA0 - 0x54];
} Course_2;

typedef struct {
    u8 pad0[4];
    void *p4;
} Link;

typedef struct {
    u8 pad0[0xC];
    Link *pC;
    u8 pad10[0x2C - 0x10];
} Menu;

typedef struct {
    Ent24 *p0;
    u8 pad4[0x60];
    s16 h64;
} Sel_3;

typedef struct { u8 pad0[0x174]; s32 w174; } Ctl_18;

extern Game_27 D_800EFD70_54;
extern Course_2 D_800CB210_14[];
extern Ctl_18 *D_800CEA40_3;
extern Menu D_800CD63C[];
extern Sel_3 *D_800CA930_15;

void func_800737A4(void)
{
    s32 flag;
    s32 mode;
    s32 i;
    V7 *dst;
    s32 n;
    Course_2 *c;
    void *m;
    u8 *p;
    u8 *q;

    flag = 0;
    D_800C2358 = 0;
    D_800C8CE8 = 0;
    if (D_800CA920 == 2) {
        flag = flag < (u32)(D_800EFD70_54.w0 & 8);
    }
    D_800C2388 = 0;
    D_800EFD70_54.w0 &= ~8;
    if (D_800CA920 == 0) {
        func_8000ED10((u8 *)&D_800EFD70_54, 0, 0x7B0);
        n = 0;
        for (i = 0; i < 27; i++) {
            s32 f = D_800CB210_14[i].w50;
            V7 *src = &D_800CB210_14[i].r34;
            if (f & 0x200) {
                dst = &D_800EFD70_54.rec[n];
                *dst = *src;
                n++;
            }
        }
        D_800EFD70_54.w75C = 2;
        D_800EFD70_54.w1C = 4;
        D_800EFD70_54.w4 = 0;
        D_800EFD70_54.w10 = 0;
        D_800EFD70_54.wC = 0;
        D_800EFD70_54.w20 = 0;
        D_800EFD70_54.w24 = 1;
        D_800EFD70_54.w2C = 3;
        D_800EFD70_54.w30 = 5;
        D_800EFD70_54.w28 = 2;
        D_800EFD70_54.w3C = 1;
        D_800EFD70_54.w40 = -1;
        D_800EFD70_54.h448 = 0;
        D_800EFD70_54.h48 = 0;
        D_800EFD70_54.w8 = 0;
        D_800EFD70_54.w18 = 0;
        D_800EFD70_54.b50 = 0;
        D_800EFD70_54.b51 = 0;
        D_800EFD70_54.b52 = 0;
        D_800EFD70_54.b53 = 0;
        D_800EFD70_54.b2A4 = 0;
        D_800EFD70_54.b2A5 = 1;
        D_800EFD70_54.w3C0 = 0;
        D_800EFD70_54.b760[0] = 2;
        D_800EFD70_54.b760[1] = 2;
        D_800EFD70_54.b760[2] = 2;
        D_800EFD70_54.b760[3] = 2;
        i = 5;
        p = (u8 *)&D_800EFD70_54 + i;
        do {
            p[0x7AA] = i;
            p--;
            i--;
        } while (i >= 0);
        i = 5;
        q = (u8 *)&D_800EFD70_54 + i;
        do {
            q[0x43C] = i;
            q--;
            i--;
        } while (i >= 0);
    } else if (D_800EFD70_54.w4C == 8) {
        D_800EFD70_54.w3C = 1;
    }
    if (D_800CA920 == 7) {
        D_800EFD70_54.w0 &= 0xFF0FFFFF;
    }
    D_800EFD70_54.w54 = 3;
    func_800C0258((s32)D_800C29E0_3);
    i = func_800BF530("resource", &D_800E7728);
    func_800C0258(0);
    if (i != 0) {
        func_8000ECB8(1);
    }
    func_800482D0((u32 *)D_800E7728);
    func_800BFDD4((s32 *)D_800E7728);
    D_800EFD70_54.h44A = 0;
    for (i = 0; i < 27; i++) {
        if ((D_800CB210_14[i].w50 & 0x200) && !(D_800CB210_14[i].w50 & 2)) {
            D_800C29E0_3[D_800EFD70_54.h44A++] = i;
        }
    }
    D_800EFD70_54.w44C = func_800BC908(D_800EFD70_54.h44A * 2, 0, "shell.c", 0x75C);
    memcpy(D_800EFD70_54.w44C, D_800C29E0_3, D_800EFD70_54.h44A * 2);
    D_800EFD70_54.w44 = 5;
    D_800CA924_4 = 0;
    D_800EFD70_54.w0 = (D_800EFD70_54.w0 & 0x20F88000) | 1;
    func_800BF530("guires", &D_800F47C0);
    func_800BFC40("ARN", D_800248EC);
    func_80014A80();
    D_800F47C4 = 0;
    D_800F47A4 = 0;
    D_800EFD70_54.w404 = func_800BF79C(D_800F47C0, D_800D1438);
    D_800CDA60[2] = 0x1FF;
    D_800CDA60[6] = 0x1FF;
    D_800CDA60[0] = D_800F13EC * 2;
    D_800CDA60[1] = D_800F13F0 * 2;
    D_800CDA60[4] = D_800F13EC * 2;
    D_800CDA60[5] = D_800F13F0 * 2;
    D_800EFD70_54.w13C = func_800BC908(0x200, 0, "shell.c", 0x777);
    func_80073664(0xFF, 0);
    if (D_800CA920 == 4) {
        D_800EFD64_8->w50 &= ~0x180;
        if (D_800C8CE4 != 0) {
            D_800C8CE4 = 0;
            D_800EFD70_54.w0 &= 0xFFF7FFFF;
        }
    }
    if (D_800CEA40_3 != NULL) {
        if (D_800CEA40_3->w174 & 0x2000) {
            D_800F01F8 = 3;
            func_8007AF38_3(&D_800CD63C[15], 0, 1);
        } else {
            D_800F01F8 = 4;
            func_8007AF38_3(&D_800CD63C[15], 0, 1);
        }
    } else {
        mode = D_800CA920;
        if (mode == 0) {
            func_8007AF38_3(&D_800CD63C[19], 0, 1);
        } else if ((D_800EFD70_54.w0 & 0x8000) || D_800CCD64 != 0 || mode == 3) {
            if (D_800C8CE4 != 0) {
                if (!(D_800EFD70_54.w0 & 0x8000)) {
                    if (!(D_800EFD70_54.w0 & 0x80000)) {
                        D_800C8CE4 = 0;
                    }
                }
            }
            if (D_800CA920 == 3 && D_800EFDBC == 1) {
                func_800882A0();
                if (D_800CA930_15->h64 > 0) {
                    D_800CA930_15->h64--;
                }
            }
            if (D_800EFD70_54.w0 & 0x8000) {
                func_800882A0();
            }
            if (D_800CCD64 != 0 && D_800EFD70_54.p3FC != &D_800CD63C[5]) {
                D_800CCD64 = 0;
                func_8007AF38_3(&D_800CD63C[8], 0, 1);
            } else {
                func_8007AF38_3(D_800F016C, 0, 1);
            }
            D_800EFD70_54.w0 &= 0xFEFF7FFF;
        } else if (flag) {
            func_8007AF38_3(&D_800CD63C[10], 1, 1);
        } else {
            if (D_800F05E8 != 0 && mode != 0 && mode != 6 && D_800EFD70_54.w4C == 1) {
                if (D_800CA930_15 != NULL && D_800CA930_15->p0 - D_800CA9C8 == 4) {
                    m = &D_800CD63C[2];
                } else {
                    m = &D_800CD63C[0];
                }
            } else if ((D_800F05E8 != 0 && D_800CA920 != 0 && D_800CA920 != 6 && D_800EFDBC == 0)
                       || (D_800F05E8 != 0 && D_800CA920 != 0 && D_800CA920 != 6 && D_800EFDBC == 8)) {
                m = &D_800CD63C[1];
            } else {
                if (D_800C8CE4 != 0) {
                    D_800C8CE4 = 0;
                }
                switch (D_800CA920) {
                case 0:
                    m = &D_800CD63C[14];
                    break;
                case 1:
                    m = &D_800CD63C[1];
                    break;
                default:
                    m = &D_800CD63C[8];
                    if (D_800C9570 != 0) {
                        func_800BCB04(D_800C9570);
                        D_800C9570 = 0;
                    }
                    break;
                }
            }
            if (D_800CA920 != 0 && D_800EFD64_8 != NULL && (D_800EFD64_8->w50 & 0x180)) {
                D_800CD63C[18].pC->p4 = m;
                func_8007AF38_3(&D_800CD63C[18], 1, 1);
            } else {
                func_8007AF38_3(m, 1, 1);
            }
        }
    }
    if (D_800CA920 == 0) {
        func_8006F3E4(0);
    }
    func_8006F128();
    func_80019E78();
}

void func_80073F84(s32 param_1)
{
    s32 uVar1;
    s32 uVar2;
    s32 iVar3;
    u8 *base;

    if (param_1 != 0) {
        uVar1 = 0;
        do {
            uVar2 = uVar1 + 1;
            D_800EFD70_2[0x43C + uVar1] = (u8)(((s32)(uVar1 & 3) >> ((*(s32 *)(D_800EFD70_2 + 0x3C) - 1) >> 1)) << 1);
            uVar1 = uVar2;
        } while (uVar2 < 6);
        uVar1 = 0;
        iVar3 = uVar1;
        base = D_800EFD70_2;
        *(s32 *)(base + 0x4C) = 2;
        D_800EFD64 = D_800CBFD0;
        D_800C2388 = 0;
        *(s32 *)(base + 0x1C) = 4;
        *(s32 *)(base + 0x20) = 4;
        *(s32 *)(base + 0x24) = 4;
        *(s32 *)(base + 0x28) = 4;
        *(s32 *)D_800EFD70_2 = *(s32 *)D_800EFD70_2 & 0xffffff7f;
        for (; uVar1 < 4; uVar1++) {
            if (((s32)D_800C292C >> uVar1 & 1) != 0) {
                D_800EFD70_2[0x2C8 + iVar3] = (u8)uVar1;
                iVar3 = iVar3 + 1;
                if (iVar3 == *(s32 *)(D_800EFD70_2 + 0x3C)) {
                    return;
                }
            }
        }
    }
}

typedef struct {
    u32 flags;
    u8 unk4[0xC - 0x4];
    s32 unkC;
    u8 unk10[0x1C - 0x10];
    s32 unk1C;
    u8 unk20[0x3C - 0x20];
    volatile s32 unk3C;
    u8 unk40[0x4C - 0x40];
    s32 mode;
    u8 unk50[0x410 - 0x50];
    s32 unk410;
} GameState;

typedef struct {
    Ent24 *cup;
    u8 unk4[0x64 - 0x4];
    s16 track;
    u8 unk66[0x74 - 0x66];
} Season;

typedef struct {
    u8 unk0[0x2C1];
    u8 trackId;
} Unk1900;

extern GameState D_800EFD70_55;
extern Season D_800F0638;
extern Season *D_800CA930_16;
extern Unk1900 D_800F1900_8;

void func_8007405C(s32 arg0) {
    s32 i;

    if (arg0 != 0) {
        D_800EFD70_55.mode = 1;
        D_800CA930_16 = &D_800F0638;
        D_800EFD70_55.flags |= 0x80;
        func_8000ED10(&D_800F0638, 0, sizeof(Season));
        D_800CA930_16->cup = &D_800CA9C8[D_800EFD70_55.unkC];
        if (D_800EFD70_55.unk410 == 0) {
            D_800EFD64 = D_800CC250;
            D_800EFD70_55.mode = 9;
            D_800EFD70_55.unk3C = 1;
            D_800F0624_2 = 0;
            D_800EFD70_55.unk1C = 4;
            D_800EFD70_55.flags &= ~0x80;
            D_800CA930_16->cup = &D_800CA9C8[6];
            D_800CA930_16->track = 0;
            for (i = 0; i < 17; i++) {
                if (D_800CADE4_2[i].id == D_800F1900_8.trackId) {
                    D_800CA930_16->track = i;
                    return;
                }
            }
        }
    }
}

#define PRESSED(m) ((D_800E30B0 & (m)) != 0 && (D_800E25B0 & (m)) == 0)

typedef struct {
    u8 pad0[0x54];
    s32 f54;
    u8 pad58[0x750 - 0x58];
    s32 timer;
} Glob_4;

struct Item_8 {
    u8 pad0[0x16];
    s16 type;
};

extern Glob_4 D_800EFD70_56;

void func_8007417C(Item_8 *a, Pal *b, u32 *arg2, Pad16 *arg3)
{
    s32 flag;
    s32 ok;
    s16 type;

    flag = 0;
    type = a->type;
    if (type == 0x15 || type == 0x17) {
        ok = D_800EFD70_56.timer < 0x78;
    } else if (type == 0x18) {
        ok = D_800EFD70_56.timer < 0x474;
    } else if (type == 0x19) {
        ok = D_800EFD70_56.timer < 0x21C;
    } else {
        ok = D_800EFD70_56.timer < 0xF0;
    }
    if (ok) {
        if (PRESSED(0x10) | PRESSED(0x40)) {
            flag = 1;
        }
    }
    if (--D_800EFD70_56.timer == 0 || flag) {
        if (flag && D_800EFD70_56.f54 == 3) {
            func_8001102C(1);
        }
        func_8007AF38_2(b->f4, 0, 1);
    }
}

typedef struct {
    u8 pad0[0x50];
    u8 dc[0x39C];
    s32 cnt;
    u8 pad1[0x86];
    u16 pos;
} Blk_18;

extern Blk_18 D_800EFD70_57;

void func_80074298(s32 param_1, s32 param_2, u32 *param_3, s8 *param_4)
{
    s32 iv4;
    s32 uv5;
    s32 uVar6;
    s32 cv;
    u32 flags;
    u16 u2;
    u16 uP;

    uVar6 = (s16)D_800EFD70_57.pos;
    D_800EFD70_57.cnt = D_800EFD70_57.cnt + 1;
    if (D_800EFD70_57.cnt == 0x78) {
        D_800EFD70_57.cnt = 0;
    }
    flags = *param_3;
    if ((flags & 0x20) != 0) {
        func_8007AF38_2(*(s32 *)(param_1 + 0x28), 0, 0);
        iv4 = 2;
        goto CALLPT;
    }
    if ((flags & 4) != 0) {
        do {
            uP = D_800EFD70_57.pos;
            u2 = uP - 1;
            D_800EFD70_57.pos = u2;
            if ((s32)((u32)u2 << 16) < 0) {
                D_800EFD70_57.pos = uP + 3;
            }
        } while (((D_800C292C >> (s16)D_800EFD70_57.pos) & 1) == 0);
        goto LAB_800743d8;
    }
    if ((flags & 8) != 0) {
        do {
            uP = D_800EFD70_57.pos;
            u2 = uP + 1;
            D_800EFD70_57.pos = u2;
            if ((s16)u2 > 3) {
                D_800EFD70_57.pos = uP - 3;
            }
        } while (((D_800C292C >> (s16)D_800EFD70_57.pos) & 1) == 0);
        goto LAB_800743d8;
    }
    if ((flags & 0x10) == 0) {
        goto LAB_800743d8;
    }
    cv = D_800EFD70_57.dc[uVar6];
    D_800EFD70_57.dc[uVar6] = cv + 1;
    if ((s8)(cv + 1) > 3) {
        D_800EFD70_57.dc[(s16)D_800EFD70_57.pos] = D_800EFD70_57.dc[(s16)D_800EFD70_57.pos] - 4;
    }
    iv4 = 1;
CALLPT:
    func_8001102C(iv4);
LAB_800743d8:
    {
        u8 bits;
        bits = D_800C292C;
        if (((bits >> (s16)D_800EFD70_57.pos) & 1) == 0) {
            do {
                uv5 = (D_800EFD70_57.pos + 1) & 3;
                D_800EFD70_57.pos = uv5;
            } while (uv5 != uVar6 && ((bits >> uv5) & 1) == 0);
        }
    }
    if (uVar6 != (s16)D_800EFD70_57.pos) {
        func_8001102C(0);
    }
    return;
}

typedef struct {
    u8 p000[0x17C];
    s32 f17C;
    u8 p180[0x2E8 - 0x180];
    s32 f2E8;
    u8 p2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 p3F0[0x44A - 0x3F0];
    s16 f44A;
    u8 p44C[0x450 - 0x44C];
    s32 f450;
    u8 p454[0x750 - 0x454];
    s32 f750;
} GameState_2;

extern GameState_2 D_800EFD70_58;

void func_80074444(s32 param_1, s32 param_2, u32 *param_3, s32 param_4)
{
    s32 t;
    s32 t1;
    s16 w;
    u32 flags;
    s32 c1;
    s32 c2;
    s32 c3;
    s32 c4;
    s32 c5;

    c1 = D_800EFD70_58.f17C + 1;
    D_800EFD70_58.f17C = c1;
    if (c1 == 0x3c) {
        D_800EFD70_58.f17C = 0;
    }
    c2 = D_800EFD70_58.f3EC + 1;
    D_800EFD70_58.f3EC = c2;
    if (c2 == 0x40) {
        D_800EFD70_58.f3EC = 0;
    }
    if (D_800EFD70_58.f750 != 0) {
        c3 = D_800EFD70_58.f750 - 1;
        D_800EFD70_58.f750 = c3;
        if (c3 == 0) {
            param_4 = 0xb;
            if (D_800EFD70_58.f2E8 != 0xb) {
                if (D_800EFD70_58.f2E8 == 0xd) {
                    D_800EFD70_58.f2E8 = 0xb;
                    D_800EFD70_58.f750 = 0x3b;
                }
            } else {
                D_800EFD70_58.f2E8 = 0xc;
            }
        }
    }
    flags = *param_3;
    if ((flags & 0x20) != 0) {
        func_8007AF38(*(s32 *)(param_1 + 0x28), 0, 0, param_4);
        func_8001102C(2);
        return;
    }
    t1 = D_800EFD70_58.f450;
    if ((flags & 1) != 0) {
        if (t1 % 5 != 0) {
            D_800EFD70_58.f450 = D_800EFD70_58.f450 - 1;
        }
    }
    if ((*param_3 & 2) != 0) {
        c4 = D_800EFD70_58.f450;
        if (c4 % 5 < 4 && c4 + 1 < D_800EFD70_58.f44A) {
            D_800EFD70_58.f450 = c4 + 1;
        }
    }
    if ((*param_3 & 4) != 0) {
        c5 = D_800EFD70_58.f450;
        if (c5 / 5 + (c5 >> 31) != (c5 >> 31)) {
            D_800EFD70_58.f450 = c5 - 5;
        }
    }
    if ((*param_3 & 8) != 0) {
        t = D_800EFD70_58.f450;
        w = D_800EFD70_58.f44A;
        if (t + 5 < w) {
            D_800EFD70_58.f450 = t + 5;
        } else if (t / 5 < (s16)(w / 5)) {
            D_800EFD70_58.f450 = w - 1;
        }
    }
    if (t1 != D_800EFD70_58.f450) {
        D_800EFD70_58.f2E8 = 0xd;
        D_800EFD70_58.f750 = 6;
        func_8001102C(0);
    }
}

#define G_6 D_800EFD70_59
#define BT D_800E30B0_2[pad]
#define PRESSED_2(pad, m) ((D_800E30B0_2[pad] & (m)) && !(D_800E25B0_2[pad] & (m)))

typedef struct {
    u8 c[3];
} Trip;

typedef struct {
    s32 f0;
    u8 pad4[0x2C8 - 0x4];
    s8 f2C8[0x20];
    s32 f2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x454 - 0x3F0];
    s16 f454;
    s16 f456;
    s16 f458;
    u8 pad45A[0x460 - 0x45A];
    s32 f460;
    s16 f464;
    u8 f466[0x12];
    s32 f478;
    u8 pad47C[0x734 - 0x47C];
    Trip f734[12];
    s8 f758[4];
    u8 pad75C[0x7A8 - 0x75C];
    u8 f7A8;
    u8 f7A9;
    u8 pad7AA[0x84C - 0x7AA];
    Trip f84C[4];
} Game_28;

typedef struct {
    s32 f0;
    char *name;
} NameEnt;

struct Obj_61 {
    u8 pad0[0xC];
    Pal *fC;
    u8 pad10[0x16 - 0x10];
    s16 f16;
    u8 pad18[0x24 - 0x18];
    s32 f24;
};

extern Game_28 D_800EFD70_59;
extern NameEnt D_800C2284[];
extern Trip D_800F05BC[];

void func_80074684(Obj_61 *o, s32 unused, s32 *btn, Pad16 *arg3) {
    s32 isNine;
    s32 pad;
    s32 sel;
    s32 found;
    s32 i;
    s32 snd;
    s32 b;
    s32 old;
    s32 lo, hi, lo2, hi2;
    u8 *p;
    u8 *q;
    s32 m;

    isNine = o->f16 == 9;
    if (++G_6.f3EC == 120) {
        G_6.f3EC = 0;
    }
    if (isNine) {
        pad = 0;
    } else {
        sel = G_6.f758[G_6.f460];
        pad = G_6.f2C8[sel];
    }
    if ((u32)(G_6.f2E8 - 0x11) < 2) {
        if (btn[pad] & 0x10) {
            func_8007AF38_2((s32)D_800CD79C, 0, 0);
        }
        return;
    }
    b = btn[pad];
    if (b & 0x10) {
        found = 0;
        if (G_6.f454 == 0x26 && G_6.f464 == 0) {
            snd = 3;
        } else if (isNine && G_6.f454 == 0x27) {
            {
                for (i = 0; i < 14; i++) {
                    p = (u8 *)D_800C2284[i].name;
                    q = G_6.f466;
                loop:
                    m = *p;
                    if (m == *q) {
                        if (m == 0) {
                            found = 1;
                            m = 1 << i;
                            G_6.f7A9 = i;
                            if (m == 4) {
                                G_6.f0 ^= 0x20000000;
                            } else {
                                D_800CD1A8 ^= m;
                            }
                        } else {
                            p++;
                            q++;
                            goto loop;
                        }
                    }
                    if (found) {
                        goto ok;
                    }
                }
                snd = 3;
            }
        } else {
        ok:
            snd = 1;
        }
        func_8001102C(snd);
        if (G_6.f454 == 0x26) {
            if (G_6.f464 != 0) {
                G_6.f466[G_6.f464--] = 0;
            }
        } else if (G_6.f454 == 0x27) {
            if (isNine) {
                if (found) {
                    G_6.f2E8 = 0x11;
                } else {
                    G_6.f2E8 = 0x12;
                }
                return;
            }
            for (i = G_6.f460 + 1; i < 4; i++) {
                if (G_6.f758[i] == G_6.f758[G_6.f460]) {
                    D_800F05BC[i] = *(Trip *)G_6.f466;
                    if (i == 3) {
                        G_6.f7A8 = G_6.f758[i];
                    }
                    G_6.f758[i] = -1;
                }
            }
            { s32 k = G_6.f460; D_800F05BC[k] = *(Trip *)G_6.f466; }
            {
                Trip *t = G_6.f734;
                t[sel] = *(Trip *)G_6.f466;
            }
            for (i = G_6.f460 + 1; i < 4; i++) {
                if (G_6.f758[i] != G_6.f758[G_6.f460] && G_6.f758[i] != -1) {
                    G_6.f460 = i;
                    break;
                }
            }
            if (i < 4) {
                s32 k = G_6.f758[G_6.f460];
                if (G_6.f734[k].c[0] != 0) {
                    Trip *t = G_6.f734;
                    *(Trip *)G_6.f466 = t[k];
                    G_6.f466[3] = 0;
                    G_6.f464 = strlen(G_6.f466);
                    G_6.f456 = 0x31F;
                    G_6.f458 = 0x31F;
                    G_6.f454 = 0x27;
                } else {
                    G_6.f466[0] = 0x41;
                    G_6.f466[1] = 0;
                    G_6.f466[2] = 0;
                    G_6.f466[3] = 0;
                    G_6.f464 = 0;
                    G_6.f456 = 0;
                    G_6.f458 = 0;
                    G_6.f454 = 0;
                }
                {
                    s32 t = o->f24;
                    o->f24 = G_6.f478;
                    G_6.f478 = t;
                }
                return;
            }
            func_8007AF38_2(o->fC->f4, 0, 0);
        } else {
            s32 n = (s16)++G_6.f464;
            if (n >= (isNine ? 10 : 3)) {
                G_6.f456 = 0x31F;
                G_6.f458 = 0x31F;
                G_6.f454 = 0x27;
            }
        }
        return;
    }
    if (b & 0x20) {
        if (G_6.f464 != 0) {
            G_6.f466[G_6.f464 - 1] = G_6.f466[G_6.f464];
            snd = 1;
            G_6.f466[G_6.f464--] = 0;
        } else {
            snd = 3;
        }
        func_8001102C(snd);
        return;
    }
    old = G_6.f454;
    if (G_6.f464 < (isNine ? 10 : 3)) {
        s32 x, v;
        s32 col, row;
        lo = 0;
        lo2 = lo;
        hi = 0x320;
        hi2 = hi;
        G_6.f456 += (BT & 0xC) ? ((BT & 8) ? ((BT >> 24) & 0xF) : -((BT >> 24) & 0xF)) : 0;
        if (PRESSED_2(pad, 0x2000)) {
            G_6.f456 -= 100;
        }
        if (PRESSED_2(pad, 0x4000)) {
            G_6.f456 += 100;
        }
        if (G_6.f456 >= hi) {
            G_6.f456 = lo;
        } else if (G_6.f456 < lo) {
            G_6.f456 = hi - 1;
        }
        G_6.f458 -= (BT & 3) ? ((BT & 1) ? ((s32)((u32)BT >> 28) * 3 >> 1) : (-(s32)((u32)BT >> 28) * 3) / 2) : 0;
        if (PRESSED_2(pad, 0x800)) {
            G_6.f458 -= 0xA0;
        }
        if (PRESSED_2(pad, 0x1000)) {
            G_6.f458 += 0xA0;
        }
        if (G_6.f458 >= hi2) {
            G_6.f458 = lo2;
        } else if (G_6.f458 < lo2) {
            G_6.f458 = hi2 - 1;
        }
        col = G_6.f456;
        row = G_6.f458;
        G_6.f454 = col / 100 + row / 160 * 8;
    } else if (btn[pad] & 0xC) {
        if (G_6.f454 == 0x27) {
            G_6.f456 = 0x258;
            G_6.f458 = 0x31F;
            G_6.f454 = 0x26;
        } else {
            G_6.f456 = 0x31F;
            G_6.f458 = 0x31F;
            G_6.f454 = 0x27;
        }
    }
    if (G_6.f454 < 0x26) {
        if (strlen(G_6.f466) == 0) {
            G_6.f466[1] = 0;
        }
        G_6.f466[G_6.f464] = *D_800CD1AC[G_6.f454];
    } else {
        G_6.f466[G_6.f464] = 0;
    }
    if (old != G_6.f454) {
        func_8001102C(0);
    }
}

typedef struct {
    u32 flags;
    u8 pad04[0x4C - 0x4];
    s32 f4C;
    u8 pad50[0x17C - 0x50];
    s32 f17C;
    u8 pad180[0x2E8 - 0x180];
    s32 state;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x3FC - 0x3F0];
    s32 f3FC;
    s32 sel;
    u8 pad404[0x418 - 0x404];
    s16 f418;
    u8 f41A[6];
    s32 f420[6];
    u8 pad438[0x488 - 0x438];
    s16 f488;
    u8 pad48A[0x764 - 0x48A];
    u8 f764[6];
    u8 f76A[6];
} Menu_2;

typedef struct {
    u8 pad[0x50];
    u32 f50;
} Trk_11;

struct Obj_62 {
    u8 pad[0x16];
    s16 f16;
};

extern Menu_2 D_800EFD70_60;
extern Trk_11 *D_800EFD64_38;

void func_80074EF4(Obj_62 *p1, s32 p2, u32 *p3, Pad16 *arg3)
{
    u32 fl;
    s32 orig, v, i;

    D_800EFD70_60.f17C++;
    if (D_800EFD70_60.f17C == 60) {
        D_800EFD70_60.f17C = 0;
    }
    D_800EFD70_60.f3EC++;
    if (D_800EFD70_60.f3EC == 120) {
        D_800EFD70_60.f3EC = 0;
    }
    fl = *p3;
    if (fl & 0x20) {
        if (D_800EFD70_60.state == 3) {
            D_800EFD70_60.state = 0;
        }
        return;
    }
    if (fl & 0x10) {
        func_8001102C(1);
        if (D_800EFD70_60.state == 3) {
            switch (D_800EFD70_60.sel) {
            case 0:
                if (D_800EFD70_60.f4C == 8) {
                    if (!(D_800EFD70_60.flags & 0x80000)) {
                        D_800C8CE4 = 0;
                    }
                    func_8007AF38_2(0, 0, 0);
                } else if (D_800EFD64_38->f50 & 2) {
                    func_8007AF38_2(0, 0, 0);
                    D_800C8CE4 = 0;
                } else {
                    D_800C8CE4 = 0;
                    func_8007AF38_2((s32)D_800CD7F4, 0, 1);
                }
                break;
            case 1:
                if (D_800EFD70_60.f4C == 8) {
                    D_800EFD70_60.f488 = 2;
                    func_8007AF38_2((s32)D_800CD8D0, 1, 1);
                } else {
                    D_800EFD70_60.f418 = D_800F05A8_6.f22;
                    for (i = 0; i < 6; i++) {
                        D_800EFD70_60.f41A[i] = D_800F05A8_6.f50[i];
                        D_800EFD70_60.f420[i] = D_800E37C0_3[i].f68C;
                        D_800EFD70_60.f76A[i] = D_800E37C0_3[i].f669;
                        if (D_800CA930 != 0) {
                            D_800EFD70_60.f764[i] = (D_800CA930 + i)[0x6C];
                        }
                    }
                    func_8007AF38_2(0, 0, 1);
                    D_800C8CE8 = 1;
                    D_800EFD70_60.f3FC = (s32)p1;
                    D_800EFD70_60.flags |= 0x8000;
                }
                break;
            case 2:
                D_800EFD70_60.flags &= ~0x80000;
                if (D_800EFD64_38->f50 & 2) {
                    func_800BCB04((void *)D_800C9570);
                    D_800C9570 = 0;
                }
                func_8007AF38_2((s32)D_800CD79C, 0, 1);
                D_800C8CE4 = 0;
                break;
            }
        } else if (p1->f16 == 7) {
            func_8007AF38_2((s32)D_800CD694, 1, 1);
        } else {
            D_800EFD70_60.sel = 0;
            D_800EFD70_60.state = 3;
        }
        return;
    }
    if (D_800EFD70_60.state != 3) {
        return;
    }
    orig = D_800EFD70_60.sel;
    if (fl & 0x801) {
        if (D_800F0954 - D_800F0A3C >= D_800F0950 || D_800EFD70_60.f4C == 8) {
            D_800EFD70_60.sel = orig ^ 2;
        } else {
            D_800EFD70_60.sel = orig - 1;
            if (orig - 1 < 0) {
                D_800EFD70_60.sel = orig + 2;
            }
        }
    }
    if (*p3 & 0x1002) {
        if (D_800F0954 - D_800F0A3C >= D_800F0950 || D_800EFD70_60.f4C == 8) {
            D_800EFD70_60.sel ^= 2;
        } else {
            v = D_800EFD70_60.sel;
            D_800EFD70_60.sel = v + 1;
            if (v + 1 >= 3) {
                D_800EFD70_60.sel = v - 2;
            }
        }
    }
    if (D_800EFD70_60.sel != orig) {
        func_8001102C(0);
    }
}

typedef struct {
    s32 f0;
    u8 pad4[0x3C - 0x4];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    s32 f4C;
    u8 pad50[0x17C - 0x50];
    s32 f17C;
    u8 pad180[0x2E8 - 0x180];
    s32 f2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x3FC - 0x3F0];
    s32 f3FC;
    s32 f400;
    u8 pad404[0x418 - 0x404];
    u16 f418;
    u8 f41A[6];
    s32 f420[6];
    u8 pad438[0x47C - 0x438];
    s16 f47C;
    u8 pad47E[0x764 - 0x47E];
    u8 f764[6];
    u8 f76A[6];
} GState_3;

typedef struct {
    u8 n;
    u8 pad[0x23];
} Trk_12;

typedef struct {
    Trk_12 *trk;
    u8 pad4[0x64 - 4];
    s16 f64;
    u8 pad66[0x6C - 0x66];
    u8 f6C[6];
} Race_7;

extern GState_3 D_800EFD70_61;
extern Race_7 *D_800CA930_17;
extern Trk_12 D_800CA9C8_4[];

void func_800752A0(Info *arg0, s32 arg1, u32 *arg2, Pad16 *arg3)
{
    u32 b;
    Trk_12 *t;
    s32 i;
    s32 d;

    if (++D_800EFD70_61.f17C == 0x3C) {
        D_800EFD70_61.f17C = 0;
    }
    if (++D_800EFD70_61.f3EC == 0x78) {
        D_800EFD70_61.f3EC = 0;
    }
    b = *arg2;
    if (b & 0x20) {
        func_8001102C(2);
        if (D_800EFD70_61.f2E8 == 4) {
            D_800EFD70_61.f2E8 = 0;
            return;
        }
        if (D_800EFD70_61.f2E8 == 7) {
            D_800EFD70_61.f2E8 = 4;
            return;
        }
        if (arg0->f28 != 0) {
            func_8007AF38_2(arg0->f28, 0, 0);
        }
    } else if (b & 0x10) {
        func_8001102C(1);
        if (D_800EFD70_61.f2E8 == 4) {
            switch (D_800EFD70_61.f400) {
            case 0:
                D_800EFD70_61.f0 |= 0x80;
                t = D_800CA930_17->trk;
                if (D_800CA930_17->f64 + 1 >= t->n) {
                    if (D_800CA930_17->f6C[0] == 0 && (t - D_800CA9C8_4 != 4 || D_800F05F8 == 0)) {
                        D_800EFD70_61.f0 = (D_800EFD70_61.f0 & ~0x80) | 0x40000;
                        func_8007AF38_2(0, 0, 1);
                        D_800C8CE4 = 0;
                        return;
                    }
                } else if (t - D_800CA9C8_4 != 4 || D_800F05F8 == 0) {
                    func_8007AF38_2(0, 0, 1);
                    D_800CA930_17->f64++;
                    func_80064B9C(D_800EFD70_61.f3C, D_800F05CA - D_800EFD70_61.f3C);
                    func_800650E4();
                    D_800C8CE4 = 0;
                    return;
                }
                func_8007AF38_2((s32)D_800CD6C0, 0, 1);
                D_800C8CE4 = 0;
                return;
            case 1:
                D_800EFD70_61.f418 = D_800F05A8_6.f22;
                for (i = 0; i < 6; i++) {
                    D_800EFD70_61.f41A[i] = D_800F05A8_6.f50[i];
                    D_800EFD70_61.f420[i] = D_800E37C0_3[i].f68C;
                    D_800EFD70_61.f76A[i] = D_800E37C0_3[i].f669;
                    if (D_800CA930_17 != 0) {
                        D_800EFD70_61.f764[i] = D_800CA930_17->f6C[i];
                    }
                }
                func_8007AF38_2(0, 0, 1);
                D_800C8CE8 = 1;
                D_800EFD70_61.f3FC = (s32)arg0;
                D_800EFD70_61.f0 |= 0x8000;
                return;
            case 2:
                D_800EFD70_61.f2E8 = 7;
                D_800EFD70_61.f47C = 0;
                return;
            }
        } else {
            if (D_800EFD70_61.f2E8 == 7) {
                switch (D_800EFD70_61.f47C) {
                case 1:
                    func_8007AF38_2((s32)D_800CD79C, 0, 1);
                    D_800C8CE4 = 0;
                    return;
                case 0:
                    D_800EFD70_61.f2E8 = 4;
                    return;
                }
            } else {
                D_800EFD70_61.f400 = 0;
                D_800EFD70_61.f2E8 = 4;
            }
        }
    } else {
        if (D_800EFD70_61.f2E8 == 7) {
            if (b & 3) {
                D_800EFD70_61.f47C ^= 1;
                func_8001102C(0);
            }
        } else if (D_800EFD70_61.f2E8 == 4) {
            if (b & 0x801) {
                d = D_800F0954 - D_800F0A3C;
                if (D_800EFD70_61.f4C != 8 ? d >= D_800F0950 : d >= 0x2800) {
                    D_800EFD70_61.f400 ^= 2;
                } else {
                    if (--D_800EFD70_61.f400 < 0) {
                        D_800EFD70_61.f400 += 3;
                    }
                }
                func_8001102C(0);
            }
            if (*arg2 & 0x1002) {
                func_8001102C(0);
                d = D_800F0954 - D_800F0A3C;
                if (D_800EFDBC != 8 ? d >= D_800F0950 : d >= 0x2800) {
                    D_800EFD70_61.f400 ^= 2;
                } else {
                    if (++D_800EFD70_61.f400 >= 3) {
                        D_800EFD70_61.f400 -= 3;
                    }
                }
            }
        }
    }
}

typedef struct {
    /* 0x000 */ s32 f0;
    /* 0x004 */ u8 pad4[0x3C - 0x4];
    /* 0x03C */ s32 f3C;
    /* 0x040 */ u8 pad40[0x4C - 0x40];
    /* 0x04C */ s32 f4C;
    /* 0x050 */ u8 pad50[0x54 - 0x50];
    /* 0x054 */ s32 f54;
    /* 0x058 */ u8 pad58[0x17C - 0x58];
    /* 0x17C */ s32 f17C;
    /* 0x180 */ u8 pad180[0x2E8 - 0x180];
    /* 0x2E8 */ s32 f2E8;
    /* 0x2EC */ u8 pad2EC[0x3FC - 0x2EC];
    /* 0x3FC */ void *f3FC;
    /* 0x400 */ u8 pad400[0x448 - 0x400];
    /* 0x448 */ s16 f448;
    /* 0x44A */ u8 pad44A[0x47C - 0x44A];
    /* 0x47C */ u16 f47C;
    /* 0x47E */ u8 pad47E[0x770 - 0x47E];
    /* 0x770 */ s32 f770;
    /* 0x774 */ s32 f774;
} State_7;

extern State_7 D_800EFD70_62;

void func_80075764(Info *arg0, s32 arg1, s32 *arg2, Pad16 *arg3) {
    s32 m;
    s32 old;
    s32 d;

    if (++D_800EFD70_62.f17C == 60) {
        D_800EFD70_62.f17C = 0;
    }
    if (D_800EFD70_62.f448 & 0x100) {
        D_800EFD70_62.f448 &= ~0x100;
        if (D_800EFD70_62.f448 == 0) {
            func_8004C22C(D_800D11D8);
        }
    }
    if ((D_800E30B0 & 0x20) && !(D_800E25B0 & 0x20) && D_800EFD70_62.f54 == 3) {
        if (D_800EFD70_62.f2E8 == 6) {
            D_800EFD70_62.f2E8 = 0;
            func_8001102C(2);
        } else if (D_800EFD70_62.f2E8 != 14 && D_800EFD70_62.f2E8 != 15) {
            func_8007AF38_3((void *)arg0->f28, 0, 0);
            func_800B8F2C();
            func_8004F534();
            if (func_8004F180() != 0) {
                func_8004C3C4();
            }
            func_8001102C(2);
        }
        D_800CCD64 = 0;
        return;
    }
    if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10) && D_800EFD70_62.f54 == 3) {
        switch (D_800EFD70_62.f448) {
        case 3:
                        D_800F1900.f2C4 ^= 1;
            func_8001102C(1);
            return;
        case 8:
                        D_800F1900.f2C4 ^= 0x10;
            func_8001102C(1);
            return;
        case 1:
            func_800B7990(D_800EFD70_62.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
            return;
        case 2:
            if (D_800EFD70_62.f770 == 0) {
                func_8004F4DC(0x9B);
                D_800EFD70_62.f770 = 0x34;
            }
            return;
        case 10:
                        func_8001102C(3);
            return;
        case 9:
                        func_8001102C(3);
            return;
        case 7:
            if (D_800C2940 == 0) {
                return;
            }
                        D_800F1900.f2C4 ^= 2;
            func_8001102C(1);
            return;
        case 4:
            func_8007AF38_3(D_800CD928, 1, 1);
                        func_8001102C(1);
            return;
        case 5:
            func_8007AF38_3(D_800CD8FC, 1, 1);
                        func_8001102C(1);
            return;
        case 6:
            func_8007AF38_3(NULL, 0, 1);
                        D_800CCD64 = 1;
            D_800EFD70_62.f4C = 0;
            D_800EFD64 = D_800CBA30;
            D_800EFD70_62.f3C = 1;
            D_800EFD70_62.f3FC = arg0;
            {
                s32 f = D_800EFD70_62.f0 & ~0x80;
                *(volatile s32 *)&D_800EFD70_62.f0 = f;
                D_800EFD70_62.f0 = f | 0x05000000;
            }
            func_8001102C(1);
            return;
        }
        return;
    }
    m = D_800EFD70_62.f2E8;
    if (m == 6 || m == 15 || m == 14) {
        if (m == 15) {
            func_8006F3E4(1);
            D_800EFD70_62.f2E8 = 0;
        } else if (m != 14) {
            if (*arg2 & 3) {
                D_800EFD70_62.f47C ^= 1;
                func_8001102C(0);
            }
        }
        return;
    }
    old = D_800EFD70_62.f448;
    if (*arg2 & 2) {
        if (old < 3) {
            D_800EFD70_62.f448 += 1;
        } else {
            D_800EFD70_62.f448 += 2;
        }
        if (D_800EFD70_62.f448 >= 11) {
            D_800EFD70_62.f448 = 0;
        }
    }
    if (*arg2 & 1) {
        if (D_800EFD70_62.f448 < 4) {
            D_800EFD70_62.f448 -= 1;
        } else {
            D_800EFD70_62.f448 -= 2;
        }
        if (D_800EFD70_62.f448 < 0) {
            D_800EFD70_62.f448 = 9;
        }
    }
    if (old != D_800EFD70_62.f448) {
        switch (old) {
        case 2:
            func_8004F534();
            break;
        case 0:
            func_8004C3C4();
            break;
        case 1:
            break;
        }
        switch (D_800EFD70_62.f448) {
        case 2:
            func_8004F4DC(0x9B);
            D_800EFD70_62.f770 = 0x34;
            break;
        case 0:
            func_8004C22C(D_800D11D8);
            break;
        case 1:
            func_800B7990(D_800EFD70_62.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
            break;
        }
    }
    if (D_800EFD70_62.f448 >= 3) {
        if (*arg2 & 0xC) {
            if ((D_800EFD70_62.f448 - 3) & 1) {
                D_800EFD70_62.f448--;
            } else {
                D_800EFD70_62.f448++;
            }
        }
    } else {
        if (D_800EFD70_62.f770 != 0) {
            D_800EFD70_62.f770--;
        }
        if ((D_800E30B0 & 0x2000) && !(D_800E25B0 & 0x2000)) {
            d = -8;
        } else if ((D_800E30B0 & 0x4000) && !(D_800E25B0 & 0x4000)) {
            d = 8;
        } else {
            s32 b = D_800E30B0;
            if (b & 0xC) {
                d = ((b & 8) ? ((b >> 24) & 0xF) : -((b >> 24) & 0xF)) / 3;
            } else {
                d = 0;
            }
        }
        if (d != 0 && D_800EFD70_62.f770 == 0) {
            switch (D_800EFD70_62.f448) {
            case 1:
                func_800B7990(D_800EFD70_62.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
                D_800EFD70_62.f770 = 0x28;
                break;
            case 2:
                func_8004F4DC(0x9B);
                D_800EFD70_62.f770 = 0x34;
                break;
            }
        }
        if (d < 0) {
            switch (D_800EFD70_62.f448) {
            case 1:
                if (D_800F1900.f2C6 >= -d) {
                    D_800F1900.f2C6 += d;
                } else {
                    D_800F1900.f2C6 = 0;
                }
                break;
            case 0:
                if (D_800F1900.f2C5 >= -d) {
                    D_800F1900.f2C5 += d;
                } else {
                    D_800F1900.f2C5 = 0;
                }
                break;
            case 2:
                if (D_800F1900.f2C7 >= -d) {
                    D_800F1900.f2C7 += d;
                } else {
                    D_800F1900.f2C7 = 0;
                }
                break;
            }
        } else {
            s32 t;
            switch (D_800EFD70_62.f448) {
            case 1:
                t = D_800F1900.f2C6 + d;
                if (t >= 256) {
                    D_800F1900.f2C6 = 255;
                } else {
                    D_800F1900.f2C6 = t;
                }
                break;
            case 0:
                t = D_800F1900.f2C5 + d;
                if (t >= 256) {
                    D_800F1900.f2C5 = 255;
                } else {
                    D_800F1900.f2C5 = t;
                }
                break;
            case 2:
                t = D_800F1900.f2C7 + d;
                if (t >= 256) {
                    D_800F1900.f2C7 = 255;
                } else {
                    D_800F1900.f2C7 = t;
                }
                break;
            }
        }
        func_80010E5C(1, D_800F1900.f2C5);
        func_80010E5C(2, D_800F1900.f2C7);
        func_80010E5C(0, D_800F1900.f2C6);
    }
    if (D_800EFD70_62.f448 != old) {
        func_8001102C(0);
    }
}

typedef struct {
    u32 flags;          /* 0x000 */
    u8 pad4[0x18];
    s32 sel[4];         /* 0x01C */
    u8 pad2C[0x10];
    s32 count;          /* 0x03C */
    u8 pad40[0xC];
    s32 f4C;            /* 0x04C */
    s32 f50;
    s32 f54;            /* 0x054 */
    u8 pad58[0x2A8 - 0x58];
    Pair f2A8[4];       /* 0x2A8 */
    s8 f2C8[4];         /* 0x2C8 */
    u8 pad2CC[0x14];
    s16 f2E0[4];        /* 0x2E0 */
    u8 pad2E8[0x3C0 - 0x2E8];
    s32 f3C0;           /* 0x3C0 */
    s32 f3C4;           /* 0x3C4 */
    u8 pad3C8[0x408 - 0x3C8];
    s16 f408[2];        /* 0x408 */
    u8 f40C[4];         /* 0x40C */
} State_8;

typedef struct {
    s32 pad0;
    s32 f4;
} Sub_11;

struct Obj_63 {
    u8 pad0[0xC];
    Sub_11 *fC;
    u8 pad10[0x18];
    s32 f28;
};

extern State_8 D_800EFD70_63;

void func_80075F3C(Obj_63 *p1, s32 p2, u32 *p3, Pad16 *arg3)
{
    State_8 *g;
    State_8 *h;
    State_8 *k;
    State_8 *m;
    s32 i;
    s32 ok;
    s32 idx;
    s32 t;
    u32 mask;

    g = &D_800EFD70_63;
    for (i = 0; i < 4; i++) {
        if (D_800EFD70_63.f2E0[i] != 0) {
            D_800EFD70_63.f2E0[i]--;
        }
        D_800EFD70_63.f2A8[i].a = (D_800EFD70_63.f2A8[i].a + 0x23) & 0xFFFF;
    }

    ok = 1;
    h = &D_800EFD70_63;
    for (i = 0; i < h->count; i++) {
        if (!(D_800EFD70_63.flags & (0x400 << i))) {
            ok = 0;
        }
    }
    func_80017338(D_800CD090);

    k = &D_800EFD70_63;
    if (k->count == 1) {
        if (*p3 & 0x780) func_8001102C(0);
        if (*p3 & 0x200) k->f3C0 = (k->f3C0 + 5) % 6;
        if (*p3 & 0x400) k->f3C0 = (k->f3C0 + 1) % 6;
        if (k->f4C != 8) {
            if (*p3 & 0x80) k->f3C4 = (k->f3C4 + 5) % 6;
            if (*p3 & 0x100) k->f3C4 = (k->f3C4 + 1) % 6;
        }
    }

    if ((*p3 & 0x50) && ok) {
        func_8001102C(1);
        if (D_800F0128 < 8) {
            if (D_800F0128 >= 2) {
                func_8007AF38_2(0, 1, 1);
            } else {
                func_8007AF38_2(p1->fC->f4, 1, 1);
            }
        } else {
            func_8007AF38_2(p1->fC->f4, 1, 1);
        }
        return;
    }
    if (*p3 & 0x20) {
        if (!(D_800EFD70_63.flags & 0x400)) {
            if (p1->f28 != 0) {
                if (p1->f28 == (s32)D_800CD8D0 && D_800C9570 != 0) {
                    D_800EFD70_63.flags |= 0x2000000;
                }
                func_8007AF38_2(p1->f28, 0, 0);
                func_8001102C(2);
            }
            return;
        }
    }
    m = &D_800EFD70_63;
    if (m->f54 != 3) return;
    for (i = 0; i < D_800EFD70_63.count; i++) {
        if (!((D_800C292C >> (idx = D_800EFD70_63.f2C8[i])) & 1)) continue;
        if (D_800EFD70_63.f40C[i] == 0 && !(D_800EFD70_63.flags & (0x400 << i))) {
            if (p3[idx] & 4) {
                t = D_800EFD70_63.sel[i];
                D_800EFD70_63.sel[i] = t - 1;
                if (t - 1 < 0) D_800EFD70_63.sel[i] = t + 5;
                if (D_800EFD70_63.count == 1) D_800EFD70_63.f3C0 = (D_800EFD70_63.f3C0 + 5) % 6;
                func_8001102C(0);
            }
            if (p3[D_800EFD70_63.f2C8[i]] & 8) {
                t = D_800EFD70_63.sel[i];
                D_800EFD70_63.sel[i] = t + 1;
                if (t + 1 >= 6) D_800EFD70_63.sel[i] = t - 5;
                if (D_800EFD70_63.count == 1) D_800EFD70_63.f3C0 = (D_800EFD70_63.f3C0 + 1) % 6;
                func_8001102C(0);
            }
        }
        if (D_800EFD70_63.count >= 3 || D_800EFD70_63.f40C[i] == 0) {
            if (!(D_800EFD70_63.flags & (0x400 << i)) && (p3[D_800EFD70_63.f2C8[i]] & 0x50)) {
                D_800EFD70_63.f2E0[i] = 0x28 - D_800EFD70_63.f2E0[i];
                D_800EFD70_63.flags |= 0x400 << i;
                func_8001102C(1);
            } else if ((D_800E30B0 & 0x20000) && D_800EFD70_63.count == 1) {
                D_800EFD70_63.f40C[i] = 1;
                D_800EFD70_63.f408[i] = 0x3D;
            }
        } else {
            if (D_800EFD70_63.f40C[i] == 1) {
                if (--D_800EFD70_63.f408[i] <= 0) {
                    D_800EFD70_63.f40C[i] = 2;
                } else if (!(D_800E30B0 & 0x20000)) {
                    D_800EFD70_63.f40C[i] = 3;
                    D_800EFD70_63.f408[i] = 0x3E - D_800EFD70_63.f408[i];
                }
            } else if (D_800EFD70_63.f40C[i] == 2) {
                if (!(D_800E30B0 & 0x20000)) {
                    D_800EFD70_63.f40C[i] = 3;
                    D_800EFD70_63.f408[i] = 0x3D;
                }
            } else if (D_800EFD70_63.f40C[i] == 3) {
                if (D_800E30B0 & 0x20000) {
                    D_800EFD70_63.f40C[i] = 1;
                    D_800EFD70_63.f408[i] = 0x3E - D_800EFD70_63.f408[i];
                }
                if (--D_800EFD70_63.f408[i] <= 0) {
                    D_800EFD70_63.f40C[i] = 0;
                }
            }
        }
        mask = 0x400 << i;
        if ((D_800EFD70_63.flags & mask) && (p3[D_800EFD70_63.f2C8[i]] & 0x20)) {
            func_8001102C(2);
            D_800EFD70_63.f2E0[i] = 0x28 - D_800EFD70_63.f2E0[i];
            D_800EFD70_63.flags &= ~mask;
        }
    }
}

#define OK (D_800F1900_9.f2C0 > D_800EFD70_64.f410 \
    || (D_800F1900_9.f2C0 == D_800EFD70_64.f410 && (*a0->f20 == 0 || ((D_800F1900_9.f2BF >> (*a0->f20 - 1)) & 1))) \
    || (D_800EFD70_64.f4C == 1 && D_800CA930_18 != NULL && *D_800CA930_18 - D_800CA9C8_5 == 5 \
        && D_800F1900_9.f2C0 == D_800EFD70_64.f410 && (D_800F1900_9.f2BF & 8)))

typedef struct {
    u8 n, f1, f2, f3;
    u8 pad4[4];
    u16 f8[7];
    s16 f16[7];
} Track_16;

typedef struct {
    u8 pad0[0x4C];
    s32 f4C;
    u8 pad50[0x64 - 0x50];
    s32 f64, f68, f6C; u32 f70;
    u8 pad74[0x134 - 0x74];
    s16 f134;
    s16 pad136;
    s16 f138;
    u8 pad13A[0x17C - 0x13A];
    s32 f17C;
    u8 pad180[0x3EC - 0x180];
    s32 f3EC;
    u8 pad3F0[0x410 - 0x3F0];
    s32 f410;
} Menu_3;

typedef struct {
    u8 pad0[0x2A8];
    u8 f2A8[5];
    u8 pad2AD[0x2BF - 0x2AD];
    u8 f2BF;
    u8 f2C0;
} Save_3;

struct Arg0 { u8 pad[0x20]; s32 *f20; u8 pad24[4]; s32 f28; };
struct Arg1 { u8 pad[4]; s32 f4; u16 f8; };

extern Menu_3 D_800EFD70_64;
extern Save_3 D_800F1900_9;
extern Track_16 D_800CA9C8_5[];
extern Track_16 **D_800CA930_18;

void func_8007658C(Arg0 *a0, Arg1 *a1, s32 *pad, Pad16 *arg3) {
    s32 k, j, i;

    if (++D_800EFD70_64.f17C == 60) D_800EFD70_64.f17C = 0;
    if (++D_800EFD70_64.f3EC == 64) D_800EFD70_64.f3EC = 0;
    switch (D_800EFD70_64.f70) {
    case 0:
        if (++D_800EFD70_64.f64 == 60) {
            if (++D_800EFD70_64.f68 >= D_800CA9C8_5[*a0->f20].n) D_800EFD70_64.f68 = 0;
            D_800EFD70_64.f64 = 0;
        }
        if (*a0->f20 == 5 && D_800CAA7C != 0) {
            if (*pad & 1) {
                s32 v = D_800EFD70_64.f6C;
                D_800EFD70_64.f6C = v - 1;
                if (v - 1 < 0) D_800EFD70_64.f6C = v + 1;
            }
            if (*pad & 2) {
                s32 v = D_800EFD70_64.f6C;
                if (++D_800EFD70_64.f6C >= 2) D_800EFD70_64.f6C -= 2;
            }
        }
        if (D_800EFD70_64.f410 != 0 && (*pad & 0xC)) {
            if (*pad & 8) {
                s32 *p = a0->f20;
                s32 v = *p;
                switch (D_800EFD70_64.f410) {
                case 1:
                    if (v <= 0) *p = v + 1;
                    break;
                case 2:
                    if (v < 2) *p = v + 1;
                    break;
                case 3:
                    if (v < 5) *p = v + 1;
                    break;
                }
                if (v != *a0->f20) {
                    func_8001102C(0);
                    D_800EFD70_64.f64 = 0;
                    D_800EFD70_64.f68 = 0;
                }
            } else if (*a0->f20 > 0) {
                func_8001102C(0);
                (*a0->f20)--;
                D_800EFD70_64.f64 = 0;
                D_800EFD70_64.f68 = 0;
            }
        }
        if ((*pad & 0x50) && !OK) {
            func_8001102C(3);
        } else if ((*pad & 0x50) && OK) {
            if (*a0->f20 == 5 && (D_800EFD70_64.f6C == 1 || D_800CAA7C == 0)) {
                D_800EFD70_64.f70 = 1;
                D_800EFD70_64.f6C = 0;
            } else if (!(a1->f8 & 1)) {
                func_8007AF38_2(a1->f4, 1, 1);
                func_8001102C(1);
            } else {
                func_8001102C(3);
            }
        } else if ((*pad & 0x20) && a0->f28 != 0) {
            func_8007AF38_2(a0->f28, 0, 0);
            func_8001102C(2);
        }
        break;
    case 1:
        if ((*pad & 1) && D_800EFD70_64.f6C != 0) D_800EFD70_64.f6C--;
        if (D_800EFD70_64.f6C < D_800CA9C8_5[*a0->f20].n) {
            if (*pad & 4) {
                if (D_800CA9C8_5[*a0->f20].f16[D_800EFD70_64.f6C] >= 2) D_800CA9C8_5[*a0->f20].f16[D_800EFD70_64.f6C]--;
            }
            if (*pad & 8) {
                if (D_800CA9C8_5[*a0->f20].f16[D_800EFD70_64.f6C] < 6) D_800CA9C8_5[*a0->f20].f16[D_800EFD70_64.f6C]++;
            }
        }
        if ((*pad & 2) && D_800EFD70_64.f6C < D_800CA9C8_5[*a0->f20].n) {
            s32 v = D_800EFD70_64.f6C + 1;
            if (v < 5) D_800EFD70_64.f6C = v;
        }
        k = *pad;
        if (k & 0x50) {
            u16 w;
            D_800EFD70_64.f70 = 2;
            if (D_800EFD70_64.f6C == D_800CA9C8_5[*a0->f20].n) {
                D_800EFD70_64.f134 = 0;
                D_800CA9C8_5[*a0->f20].f1++;
                D_800CA9C8_5[*a0->f20].f2++;
                D_800CA9C8_5[*a0->f20].f3++;
                D_800CA9C8_5[*a0->f20].n++;
                D_800EFD70_64.f138 = -1;
            } else {
                w = D_800CA9C8_5[*a0->f20].f8[D_800EFD70_64.f6C];
                D_800EFD70_64.f134 = w;
                D_800EFD70_64.f138 = w;
            }
            D_800CA9C8_5[*a0->f20].f8[D_800EFD70_64.f6C] = D_800EFD70_64.f134;
        } else if (k & 0x20) {
            D_800EFD70_64.f70 = 0;
            D_800EFD70_64.f6C = 0;
            for (i = 0; i < 5; i++) {
                if (i < D_800CA9C8_5[5].n) D_800F1900_9.f2A8[i] = D_800CA9C8_5[5].f8[i] & 0x1F;
                else D_800F1900_9.f2A8[i] = 0x3F;
                D_800F1900_9.f2A8[i] |= (D_800CA9C8_5[5].f16[i] & 7) << 5;
            }
            func_8006F5F8(1, 0);
        } else {
            if (D_800EFD70_64.f6C != D_800CA9C8_5[*a0->f20].n && (k & 0x20000)) {
                for (j = D_800EFD70_64.f6C; j < D_800CA9C8_5[*a0->f20].n - 1; j++) {
                    D_800CA9C8_5[*a0->f20].f8[j] = D_800CA9C8_5[*a0->f20].f8[j + 1];
                }
                { u8 m = --D_800CA9C8_5[*a0->f20].n;
                D_800CA9C8_5[*a0->f20].f8[m] = 0; }
            }
        }
        break;
    case 2:
        if (*pad & 4) {
            u16 v = D_800EFD70_64.f134;
            D_800EFD70_64.f134 = v - 1;
            if ((s16)v % 5 == 0) D_800EFD70_64.f134 = v + 4;
        }
        if (*pad & 8) {
            u16 v = D_800EFD70_64.f134;
            D_800EFD70_64.f134 = v + 1;
            if ((s16)(v + 1) % 5 == 0) D_800EFD70_64.f134 = v - 4;
        }
        if (*pad & 1) {
            D_800EFD70_64.f134 -= 5;
            if (D_800EFD70_64.f134 < 0) D_800EFD70_64.f134 += 20;
        }
        if (*pad & 2) {
            u16 v = D_800EFD70_64.f134;
            D_800EFD70_64.f134 = v + 5;
            if (D_800EFD70_64.f134 >= 20) D_800EFD70_64.f134 = v - 15;
        }
        k = *pad;
        if (k & 0x50) {
            D_800EFDE0 = 1;
        } else if (k & 0x20) {
            D_800EFD70_64.f70 = 1;
            if (D_800EFD70_64.f138 == -1) D_800CA9C8_5[*a0->f20].n--;
            else D_800EFD70_64.f134 = D_800EFD70_64.f138;
        }
        D_800CA9C8_5[*a0->f20].f8[D_800EFD70_64.f6C] = D_800EFD70_64.f134;
        break;
    }
    *D_800CA930_18 = &D_800CA9C8_5[*a0->f20];
}

typedef struct {
    u8 pad[0x2BF];
    u8 mask;
    u8 cup;
} Save_4;

extern Save_4 D_800F1900_10;

s32 func_80077000(s32 n)
{
    if (D_800F0180 == D_800F1900_10.cup) {
        s32 r = 0;
        if (n == 0 || ((D_800F1900_10.mask >> (n - 1)) & 1)) {
            r = 1;
        }
        return r;
    }
    switch (D_800F1900_10.cup) {
    case 1:
        if (n < 2 && ((D_800F1900_10.mask >> (n - 1)) & 1)) return 1;
        break;
    case 2:
        if (n < 2 || (n == 2 && ((D_800F1900_10.mask >> (n - 1)) & 1))) return 1;
        break;
    case 3:
        if (n < 3 || (n == 3 && ((D_800F1900_10.mask >> (n - 1)) & 1))) return 1;
        break;
    }
    return 0;
}

struct Ctl_19 {
    u8 pad0[4];
    s32 p4;
    u16 h8;
};

typedef struct {
    s32 w0;
    u8 pad4[0x3C - 0x4];
    s32 w3C;
    u8 pad40[0x4C - 0x40];
    s32 w4C;
    u8 pad50[4];
    s32 w54;
    u8 pad58[0x17C - 0x58];
    s32 w17C;
    u8 pad180[0x29C - 0x180];
    s32 w29C;
    s32 w2A0;
    u8 pad2A4[0x2E8 - 0x2A4];
    s32 w2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 w3EC;
    u8 pad3F0[0x410 - 0x3F0];
    s32 w410;
    u8 pad414[0x47C - 0x414];
    s16 h47C;
    u8 pad47E[0x488 - 0x47E];
    s16 h488;
    u8 pad48A[0x75C - 0x48A];
    s32 w75C;
} Game_29;

typedef struct {
    u8 pad0[0x2C0];
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
} S1900;

extern Game_29 D_800EFD70_65;
extern S1900 D_800F1900_11;

static inline s32 canGo(Item_9 *n) {
    s32 k;
    if (D_800EFD70_65.w4C == 8 || D_800EFD70_65.w4C == 0) {
        k = D_800F1900_11.b2C0;
    } else {
        k = D_800EFD70_65.w410;
    }
    switch (k) {
    case 1:
        if (n->h14 < 10) return 1;
        break;
    case 2:
        if (n->h14 < 15) return 1;
        break;
    case 3:
        return 1;
    }
    return 0;
}

void func_800770F0(Item_9 *item, Ctl_19 *ctl, s32 *btn, Pad16 *arg3) {
    s32 snd;
    s32 m;
    s32 b;
    s32 s;
    s32 v;
    Item_9 *prev;
    Item_9 *n;
    s32 k;
    s32 ok;

    D_800EFD70_65.w29C = D_800CDB5C;
    D_800EFD70_65.w2A0 = D_800CDB60;
    D_800CDB60 -= (D_800E30B0 & 3) ? ((D_800E30B0 & 1) ? (s32)(D_800E30B0 >> 28) / 6.0f : -(s32)(D_800E30B0 >> 28) / 6.0f) : 0.0f;
    if (++D_800EFD70_65.w17C == 60) D_800EFD70_65.w17C = 0;
    if (++D_800EFD70_65.w3EC == 64) D_800EFD70_65.w3EC = 0;
    b = *btn;
    if (b & 0x50) {
        snd = 0;
        if (!(ctl->h8 & 1)) {
            snd = 1;
        } else {
            func_8001102C(3);
        }
        if (item - D_800CD63C_2 == 8 && (D_800E30B0 & 0x8500) == 0x8500) {
            func_8007AF38_2((s32)&D_800CD63C_2[4], 1, 0);
        } else if ((ctl->h8 & 0x800) && D_800EFD70_65.w2E8 != 8) {
            D_800EFD70_65.w2E8 = 8;
            D_800EFD70_65.h47C = 0;
        } else if ((ctl->h8 & 0x1000) && D_800EFD70_65.w2E8 != 16 && D_800EFD70_65.w2E8 != 9
                   && D_800EFD70_65.w2E8 != 10 && !(D_800EFD70_65.w0 & 0x200)) {
            if (!(ctl->h8 & 1)) {
                D_800EFD70_65.w2E8 = 9;
                D_800EFD70_65.h47C = 0;
            }
        } else if (D_800EFD70_65.w2E8 == 16) {
            D_800EFD70_65.w4C = 0;
            D_800EFD70_65.w2E8 = 0;
            D_800EFD70_65.w0 |= 0x200;
        } else if (!(ctl->h8 & 0x100) || D_800EFD70_65.w2E8 == 5) {
          if (!(ctl->h8 & 1)) {
            if (D_800EFD70_65.w2E8 == 8 && D_800EFD70_65.h47C == 1) {
                D_800EFD70_65.h488 = 1;
                func_8007AF38_2((s32)&D_800CD63C_2[15], 1, 1);
            } else if (D_800EFD70_65.w2E8 == 10 && !(D_800EFD70_65.w0 & 0x200)) {
                D_800EFD70_65.w2E8 = 16;
                if (D_800EFD70_65.w410 == 0) {
                    D_800EFD70_65.w410 = 1;
                }
            } else if (D_800EFD70_65.w2E8 == 9 && D_800EFD70_65.h47C == 0) {
                D_800EFD70_65.w2E8 = 10;
                if (D_800F1900_11.b2C4 & 8) {
                    D_800EFD70_65.h47C = 0;
                } else {
                    D_800EFD70_65.h47C = 1;
                }
            } else {
                if (D_800EFD70_65.w0 & 0x200) {
                    if (D_800EFD70_65.h47C != 0) {
                        D_800EFD70_65.w0 |= 0x2000000;
                        D_800EFD70_65.h488 = 3;
                        D_800EFD70_65.w0 &= ~0x20;
                        func_8007AF38_2((s32)&D_800CD63C_2[15], 1, 1);
                    } else {
                        D_800EFD64 = (s32)D_800CBF30;
                        func_8007AF38_2((s32)&D_800CD63C_2[9], 1, 1);
                        func_800A44EC(D_800F1DF4);
                        D_800EFD70_65.w4C = 0;
                        D_800EFD70_65.w0 &= ~0x80;
                        D_800EFD70_65.w0 &= ~0x20;
                    }
                }
                if (D_800EFD70_65.w2E8 == 5 && D_800EFD70_65.w410 == 0) {
                    func_8007AF38_2(0, 0, 1);
                } else {
                    func_8007AF38_2(ctl->p4, 1, 1);
                }
            }
          }
        } else {
            D_800EFD70_65.w2E8 = 5;
            if ((ctl->h8 & 0x400) && D_800EFD70_65.w410 == 0) {
                D_800EFD70_65.w410 = 1;
            }
        }
        if (snd) {
            func_8001102C(1);
        }
    } else if (b & 0x20) {
        if (D_800EFD70_65.w0 & 0x200) {
            D_800EFD70_65.w2E8 = 16;
            func_8001102C(2);
            D_800EFD70_65.w0 &= ~0x200;
            D_800EFD70_65.w0 &= ~0x20;
            return;
        }
        s = D_800EFD70_65.w2E8;
        if (s == 16) {
            D_800EFD70_65.w2E8 = 10;
        } else if (s == 8 || s == 5 || s == 9 || s == 10) {
            func_8001102C(2);
            D_800EFD70_65.w2E8 = 0;
            D_800EFD70_65.w0 &= ~0x20;
        } else if (item->p28) {
            func_8001102C(2);
            func_8007AF38_2(item->p28, 0, 0);
        } else {
            func_8001102C(3);
        }
    } else if (D_800EFD70_65.w0 & 0x200) {
        m = D_800EFD70_65.w4C;
        if (m == 2) {
            if (D_800C292D < 2) {
                func_8001102C(2);
                D_800EFD70_65.w0 &= ~0x200;
            } else if (D_800EFD70_65.w3C == 0) {
                D_800EFD70_65.w3C = 2;
            } else if (D_800C292D < 4 && D_800EFD70_65.w3C == 4) {
                func_8001102C(0);
                D_800EFD70_65.w3C = 2;
            }
            if (D_800C292D == 4 && (*btn & 3)) {
                func_8001102C(0);
                if (D_800EFD70_65.w3C == 2) {
                    D_800EFD70_65.w3C = 4;
                } else {
                    D_800EFD70_65.w3C = 2;
                }
            }
        } else if (m == 7 || m == 10) {
            if (D_800EFD70_65.w3C == 0 || D_800C292C == 0) {
                D_800EFD70_65.w3C = 1;
            } else {
                s32 o = D_800EFD70_65.w3C;
                if (D_800C292D < o) {
                    D_800EFD70_65.w3C = D_800C292D;
                } else if (b & 3) {
                    D_800EFD70_65.w3C = (o & 1) + 1;
                    if (D_800C292D == 1) {
                        D_800EFD70_65.w3C = 1;
                    }
                    if (D_800EFD70_65.w3C != o) {
                        func_8001102C(0);
                    }
                }
            }
        } else if (D_800C292C == 0) {
            D_800EFD70_65.w3C = 1;
        } else {
            if (D_800EFD70_65.w3C == 0) {
                D_800EFD70_65.w3C = 1;
            }
            v = D_800C292D;
            if (v < D_800EFD70_65.w3C) {
                D_800EFD70_65.w3C = v;
            }
            if (*btn & 1) {
                s32 old = D_800EFD70_65.w3C;
                if ((D_800EFD70_65.w3C = old - 1) <= 0) {
                    D_800EFD70_65.w3C = v;
                }
                if (D_800EFD70_65.w3C != old) {
                    func_8001102C(0);
                }
            }
            if (*btn & 2) {
                s32 old = D_800EFD70_65.w3C;
                D_800EFD70_65.w3C = old + 1;
                if (D_800C292D < old + 1) {
                    D_800EFD70_65.w3C = 1;
                }
                if (D_800EFD70_65.w3C != old) {
                    func_8001102C(0);
                }
            }
            if (D_800EFD70_65.w4C == 0 && D_800EFD70_65.w3C == 2) {
                if (*btn & 4) {
                    s32 old;
                    func_8001102C(0);
                    old = D_800EFD70_65.w75C;
                    if ((D_800EFD70_65.w75C = old - 1) < 0) {
                        D_800EFD70_65.w75C = old + 2;
                    }
                }
                if (*btn & 8) {
                    func_8001102C(0);
                    if (++D_800EFD70_65.w75C >= 3) {
                        D_800EFD70_65.w75C -= 3;
                    }
                }
            }
        }
    } else {
        s = D_800EFD70_65.w2E8;
        if ((u32)(s - 8) < 3) {
            if ((s != 10 || (D_800F1900_11.b2C4 & 8)) && (b & 3)) {
                func_8001102C(0);
                D_800EFD70_65.h47C ^= 1;
            }
            return;
        }
        if (b & 1) {
            if ((s == 16 || s == 5) && D_800F1900_11.b2C0) {
                s32 old = D_800EFD70_65.w410;
                if ((D_800EFD70_65.w410 = old - 1) < 0) {
                    D_800EFD70_65.w410 = D_800F1900_11.b2C0;
                } else if (D_800EFD70_65.w410 == 0 && ((ctl->h8 & 0x400) || s == 16)) {
                    D_800EFD70_65.w410 = D_800F1900_11.b2C0;
                }
                if (D_800EFD70_65.w410 != old) {
                    func_8001102C(0);
                }
            } else {
                s32 old = *item->p20;
                if ((*item->p20 = old - 1) < 0) {
                    *item->p20 = item->h18 - 1;
                }
                if (*item->p20 != old) {
                    func_8001102C(0);
                }
            }
        }
        if (*btn & 2) {
            s32 t = D_800EFD70_65.w2E8;
            if ((t == 16 || t == 5) && D_800F1900_11.b2C0) {
                s32 old = D_800EFD70_65.w410;
                D_800EFD70_65.w410 = old + 1;
                if (D_800F1900_11.b2C0 < old + 1) {
                    if ((ctl->h8 & 0x400) || t == 16) {
                        D_800EFD70_65.w410 = 1;
                    } else {
                        D_800EFD70_65.w410 = 0;
                    }
                }
                if (D_800EFD70_65.w410 != old) {
                    func_8001102C(0);
                }
            } else {
                s32 old = *item->p20;
                *item->p20 = old + 1;
                if (old + 1 >= item->h18) {
                    *item->p20 = 0;
                }
                if (*item->p20 != old) {
                    func_8001102C(0);
                }
            }
        }
        if (D_800EFD70_65.w54 != 1) {
            prev = curA_800F47C4;
            if (*btn & 4) {
                if (item->p4 == NULL) {
                    if (item->p8) {
                        D_800F47C4 = item;
                    }
                } else {
                    item = item->p4;
                    D_800F47C4 = item;
                }
            }
            if (*btn & 8) {
                if (item->p8 == NULL) {
                    if (item->p4) {
                        D_800F47C4 = item;
                    }
                } else {
                    if (canGo(item->p8)) {
                        item = item->p8;
                    }
                    D_800F47C4 = item;
                }
            }
            if (prev != D_800F47C4) {
                func_8001102C(0);
            }
        }
    }
}

void func_80077BA0(u32 *p)
{
    u32 v;
    s8 a2;
    s8 a1;
    s8 a3;
    s32 i;

    for (i = 0; i < 4; i++) {
        v = *p;
        a2 = (v >> 24) & 0xF;
        a1 = v >> 28;
        a3 = a1;
        if (a2 < 7) {
            *p = v & 0xFFFFFFF3;
            a2 = 0;
        }
        if (a1 < 7) {
            *p = *p & 0xFFFFFFFC;
            a3 = 0;
        }
        if (a2 < a3) {
            *p = *p & 0xFFFFFFF3;
        } else if (a3 < a2) {
            *p = *p & 0xFFFFFFFC;
        }
        p++;
    }
}

typedef struct {
    u8 pad0[4];
    s32 p4;
    u16 h8;
    u8 padA[6];
} Ctl_20;

typedef struct Item_10 {
    u8 pad0[4];
    struct Item_10 *p4;
    struct Item_10 *p8;
    Ctl_20 *pC;
    u16 h10;
    u8 pad12[2];
    s16 h14;
    s16 h16;
    u8 pad18[0x20 - 0x18];
    s32 *p20;
    u8 pad24[4];
    s32 p28;
} Item_10;

typedef struct {
    u32 w0;
    u8 pad4[0x10 - 4];
    s32 w10;
    s32 w14;
    u8 pad18[0x4C - 0x18];
    s32 w4C;
    u8 pad50[4];
    u32 w54;
    s16 h58;
    s16 h5A;
    u8 pad5C[0x3E8 - 0x5C];
    u8 *w3E8;
    u8 pad3EC[0x488 - 0x3EC];
    s16 h488;
    u8 pad48A[0x740 - 0x48A];
    s32 w740;
} Game_30;

typedef struct { u8 pad0[8]; s16 h8[1]; } Trk_13;
typedef struct { Trk_13 *p0; u8 pad4[0x64 - 4]; s16 h64; } TrkSel;
typedef struct { u8 pad[0x38]; u16 h38; } S370;
typedef struct { u8 pad[0x10]; s32 w10; } S29CC_2;

extern Game_30 D_800EFD70_66;
extern Item_10 D_800CD63C_3[];
extern Item_10 *D_800F47A4_2;
extern Item_10 *D_800F47C4_2;
extern S370 D_800CD370_2;
extern S29CC_2 *D_800C29CC_3;
extern TrkSel *D_800CA930_19;

void func_80077C3C(Pad16 *pad) {
    u32 buf[4];
    s32 sp20;
    s32 sp24;
    s32 sp28;
    Item_10 *item;
    Ctl_20 *ctl;
    Item_10 *p;
    s32 vol;
    s32 i;
    s32 any;

    func_8000ED10((u8 *)buf, 0, 16);
    if (!(D_800E2038 & 0xF)) {
        func_8008BE9C();
    }
    if (D_800C292E || !D_800C292C) {
        D_800EFD70_66.w0 |= 0x08000000;
    } else {
        D_800EFD70_66.w0 &= ~0x08000000;
    }
    if (D_800C292D < 2) {
        D_800CD370_2.h38 |= 1;
    } else {
        func_8006F128();
    }
    if (D_800EFD70_66.w54 == 0) {
        item = D_800F47A4_2;
        if (item == &D_800CD63C_3[5] ||
            (!(item->h10 & 0x200) && (D_800F47C4_2 == NULL || (D_800F47C4_2->h10 & 0x200)))) {
            if (D_800EFD70_66.w54 == 0) {
                vol = D_800EFD70_66.h58;
            }
            func_80010DF8(vol);
        }
    } else {
        item = D_800F47C4_2;
    }
    if (item != NULL && (item->h10 & 0x2000)) {
        func_80077BA0(pad->w);
    }
    any = 0;
    for (i = 0; i < 4; i++) {
        if (((D_800C292C >> i) & 1) && pad->w[i] != 0) {
            any = 1;
        }
        if ((((s32)pad->w[i] >> 24) & 0xF) < 6) {
            pad->w[i] &= ~0xC;
        }
        if ((s32)(pad->w[i] >> 28) < 6) {
            pad->w[i] &= ~3;
        }
    }
    if (any) {
        D_800EFD70_66.w740 = 0;
    } else {
        D_800EFD70_66.w740++;
    }
    for (i = 0; i < 4; i++) {
        buf[i] = 0;
        if (D_800EFD70_66.w54 == 3) {
            buf[i] = pad->w[i] & ~D_800F47B0.w[i];
            if (buf[i] & 0x800) {
                buf[i] |= 1;
            }
            if (buf[i] & 0x1000) {
                buf[i] |= 2;
            }
            if (buf[i] & 0x2000) {
                buf[i] |= 4;
            }
            if (buf[i] & 0x4000) {
                buf[i] |= 8;
            }
        }
    }
    if (item != NULL) {
        if (D_800EFD70_66.w54 == 0) {
            ctl = &item->pC[D_800F47A0];
        } else {
            ctl = item->pC;
            if (item->p20 != NULL) {
                ctl += *item->p20 + item->h14;
            }
        }
        if (D_800EFD70_66.w54 != 1) {
            switch (item->h16) {
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
                func_8007417C(item, ctl, buf, pad);
                break;
            case 9:
            case 20:
                func_80074684(item, ctl, buf, pad);
                break;
            case 18:
                func_80074444(item, ctl, buf, pad);
                break;
            case 7:
            case 13:
                func_80074EF4(item, ctl, buf, pad);
                break;
            case 14:
                func_800752A0(item, ctl, buf, pad);
                break;
            case 8:
                if (buf[0] & 0x50) {
                    func_8007AF38_2((s32)&D_800CD63C_3[8], 0, 1);
                    func_8001102C(1);
                } else if ((buf[0] & 0x20) && item->p28 != 0) {
                    func_8007AF38_2(item->p28, 0, 0);
                    func_8001102C(2);
                }
                break;
            case 3:
                if (item - D_800CD63C_3 == 8) {
                    item->p28 = (s32)&D_800CD63C_3[14];
                    if (D_800EFD70_66.w740 > 0xE10 && D_800EFD70_66.w54 == 3) {
                        func_8007AF38_2((s32)&D_800CD63C_3[14], 0, 0);
                    }
                }
            case 11:
                func_800770F0(item, ctl, buf, pad);
                if (item->h16 == 11) {
                    D_800EFD70_66.w10 = item->h14 + D_800EFD70_66.w14;
                }
                break;
            case 19:
                func_80074298(item, ctl, buf, pad);
                break;
            case 12:
                func_80089F2C(pad);
                if (D_800F4A04 == 1 && D_800EFD70_66.w54 == 3) {
                    switch (D_800EFD70_66.h488) {
                    case 0:
                        if (item->p28 == 0) {
                            func_8007AF38_2((s32)&D_800CD63C_3[21], 0, 1);
                        } else {
                            func_8007AF38_2(item->p28, 0, 0);
                        }
                        break;
                    case 1:
                        if (D_800F4AF4 == 0) {
                            if (func_80060078(D_800C8CE4 + 0x6400, &sp20, &sp24, &sp28)) {
                                func_8007AF38_2(item->p28, 0, 0);
                                D_800C8CE4 = 0;
                                D_800EFD70_66.w0 &= ~0x80000;
                            } else {
                                D_800EFD70_66.w0 |= 0x80000;
                                D_800EFD64 = &D_800CB210[sp20];
                                D_800EFD70_66.w10 = sp20;
                                func_8007AF38_2((s32)&D_800CD63C_3[9], 1, 1);
                            }
                        } else {
                            D_800EFD70_66.w0 &= ~0x80000;
                            D_800C8CE4 = 0;
                            func_8007AF38_2(item->p28, 0, 0);
                        }
                        break;
                    case 2:
                        func_8007AF38_2(item->p28, 0, 0);
                        break;
                    case 4:
                        D_800EFD70_66.w0 |= 0x20;
                        func_8007AF38_2(0, 0, 1);
                        break;
                    case 3:
                        if (D_800EFD70_66.w0 & 0x02000000) {
                            D_800EFD70_66.w0 &= ~0x20;
                            if (D_800F4AF4 != 0) {
                                func_8007AF38_2(item->p28, 0, 0);
                            } else {
                                D_800EFD64 = &D_800CB210[21];
                                func_8007AF38_2((s32)&D_800CD63C_3[9], 1, 1);
                                func_800A44EC(D_800EFD70_66.w3E8 + 4);
                                D_800EFD70_66.w0 &= ~0x80;
                            }
                            D_800EFD70_66.w4C = 0;
                            func_800BCB04(D_800EFD70_66.w3E8);
                            D_800EFD70_66.w3E8 = 0;
                        } else {
                            if (D_800F4AF4 != 0) {
                                func_800BCB04(D_800EFD70_66.w3E8);
                                D_800EFD70_66.w3E8 = 0;
                            }
                            D_800EFD70_66.w0 |= 0x20;
                            func_8007AF38_2(0, 0, 1);
                        }
                        break;
                    }
                }
                break;
            case 6:
                func_80075764(item, ctl, buf, pad);
                break;
            case 0:
                func_80017338(D_800CD090);
                if (D_800EFD70_66.w740 > 0xE10 && D_800EFD70_66.w54 == 3 &&
                    !(D_800EFD70_66.w0 & 0x08000000)) {
                    func_8007AF38_2((s32)&D_800CD63C_3[8], 1, 1);
                }
                break;
            case 2:
                func_80075F3C(item, ctl, buf, pad);
                break;
            case 4:
                func_8007658C(item, ctl, buf, pad);
                break;
            }
            if (D_800EFD70_66.w54 == 3 && (item->h10 & 0x10)) {
                if (item->h16 == 0 && D_800E3704 != 0) {
                    func_8007AF38_2(item->pC->p4, 1, 1);
                } else if (buf[0] & 0x50) {
                    if ((item->h16 != 0 || !(D_800EFD70_66.w0 & 0x08000000)) && !(ctl->h8 & 1)) {
                        func_8001102C(1);
                        func_8007AF38_2(ctl->p4, 1, 1);
                    }
                } else if (buf[0] & 0x20) {
                    if (item->p28 != 0) {
                        func_8001102C(2);
                        func_8007AF38_2(item->p28, 0, 0);
                    } else {
                        func_8001102C(3);
                    }
                }
            }
        }
    }
    switch (D_800EFD70_66.w54) {
    case 0:
        if (D_800EFD70_66.h58 == 0) {
            D_800EFD70_66.h58 = 30;
            D_800EFD70_66.w54 = 1;
            D_800EFD70_66.h5A = 2;
        } else {
            D_800EFD70_66.h58 -= 16;
            if (D_800EFD70_66.h58 <= 0) {
                D_800EFD70_66.h58 = 0;
            }
        }
        break;
    case 1:
        if (D_800EFD70_66.h5A == 0) {
            func_80010AFC();
            func_80010DF8(0xFF);
            if (D_800F47A4_2 != NULL) {
                if (func_8004F180() && (D_800F47C4_2 == NULL || (D_800F47C4_2->h10 & 0x200))) {
                    func_8004C3C4();
                }
                for (p = D_800F47A4_2; p != NULL; p = p->p8) {
                    func_80078E54(p);
                }
                for (p = D_800F47A4_2->p4; p != NULL; p = p->p4) {
                    func_80078E54(p);
                }
            }
            if (D_800F47C4_2 != NULL) {
                for (p = D_800F47C4_2; p != NULL; p = p->p8) {
                    p->p28 = D_800F47C4_2->p28;
                    func_80079E00(p);
                }
                for (p = D_800F47C4_2->p4; p != NULL; p = p->p4) {
                    p->p28 = D_800F47C4_2->p28;
                    func_80079E00(p);
                }
                if (!func_8004F180() && !(D_800F47C4_2->h10 & 0x200)) {
                    func_8004C22C(D_800D11D4);
                    func_80010DF8(0xFF);
                }
                if (D_800F47C4_2->h16 == 11) {
                    while (D_800F47C4_2->p4 != NULL) {
                        D_800F47C4_2 = D_800F47C4_2->p4;
                    }
                    for (i = D_800EFD70_66.w10 - 5; i >= 0; i -= 5) {
                        D_800F47C4_2 = D_800F47C4_2->p8;
                    }
                    *D_800F47C4_2->p20 = D_800EFD70_66.w10 % 5;
                }
            } else {
                D_800C29CC_3->w10 = 3;
                if (D_800EFD70_66.w4C == 9) {
                    D_800EFD64 = &D_800CB210[26];
                } else if (D_800EFD70_66.w4C == 1) {
                    D_800EFD64 = &D_800CB210[D_800CA930_19->p0->h8[D_800CA930_19->h64]];
                }
                if (!(D_800EFD70_66.w0 & 0x8000)) {
                    if (!(D_800EFD70_66.w0 & 0x20)) {
                        if (D_800EFD70_66.w4C == 8) {
                            if (D_800C8CE4 == 0) {
                                D_800C8CE4 = D_800E203C;
                            }
                            D_800E3748 = 0x6400;
                        } else if (D_800EFD70_66.w4C != 9) {
                            D_800E3748 = 0xC800;
                            D_800C8CE4 = D_800E203C;
                        }
                    }
                }
            }
            D_800EFD70_66.h5A = -1;
        }
        if (--D_800EFD70_66.h58 == 0) {
            if (D_800EFD70_66.h5A != -2) {
                D_800EFD70_66.h58++;
            } else {
                D_800EFD70_66.w54 = 2;
                D_800EFD70_66.h58 = 255;
            }
        }
        break;
    case 2:
        D_800EFD70_66.h58 -= 16;
        if (D_800EFD70_66.h58 <= 0) {
            D_800EFD70_66.w54 = 3;
            D_800EFD70_66.h58 = 0;
        }
        break;
    }
    D_800F47B0 = *pad;
}

typedef struct {
    u32 f0;
    u8 pad4[0x1C - 0x4];
    s32 arr[8];
    s32 f3C;
    s32 f40;
    s32 f44;
    u8 pad48[0x4C - 0x48];
    u32 f4C;
    u8 pad50[0x13C - 0x50];
    void *f13C;
    u8 pad140[0x43C - 0x140];
    u8 f43C[6];
    u8 pad442[0x778 - 0x442];
    s32 f778[6];
    s32 f790[6];
} GState_4;

typedef struct {
    u8 n;
    u8 pad[0x15];
    s16 f16[7];
} Trk_14;

typedef struct {
    Trk_14 *trk;
    u8 pad4[0x64 - 4];
    s16 f64;
    u8 pad66[0x6C - 0x66];
    u8 f6C[6];
} Race_8;

typedef struct {
    u8 pad[0x1E];
    s16 f1E;
} Lvl_2;

extern GState_4 D_800EFD70_67;
extern Lvl_2 *D_800EFD64_39;
extern Race_8 *D_800CA930_20;
extern Trk_14 D_800CA9C8_6[];

void func_8007896C(void)
{
    s32 i;
    s32 j;
    s32 v;
    s32 k;
    GState_4 *g;

    D_800EFD70_67.f0 |= 0x4000000;
    D_800C2280 = D_800CD1A8;
    switch (D_800EFD70_67.f4C) {
    case 8:
    case 9:
        D_800C2280 = D_800CD1A8 & ~0x210;
        break;
    }
    switch (D_800EFD70_67.f4C) {
    case 0:
    case 1:
    case 8:
        if (!(D_800EFD70_67.f0 & 0x1000000)) {
            D_800EFD70_67.f0 &= ~0x4000000;
        }
        break;
    }
    if (D_800EFD64_39 != &D_800CBF30 && D_800C9570 != 0) {
        func_800BCB04(D_800C9570);
        D_800C9570 = 0;
    }
    if (D_800EFD70_67.f4C == 1 && D_800CA930_20 != 0 && D_800CA930_20->trk - D_800CA9C8_6 == 4) {
        D_800EFD70_67.arr[1] = D_800CA930_20->f64;
        if (D_800EFD70_67.arr[1] == D_800EFD70_67.arr[0]) {
            D_800EFD70_67.arr[1] = 5;
        }
    } else if (D_800EFD70_67.f4C != 8) {
        v = 0;
        for (i = D_800EFD70_67.f3C; i < 8; i++) {
            for (j = 0; j < D_800EFD70_67.f3C; j++) {
                if (v == D_800EFD70_67.arr[j]) {
                    v++;
                    v %= 8;
                }
            }
            D_800EFD70_67.arr[i] = v;
            v++;
            v %= 8;
        }
    }
    if (D_800EFD70_67.f4C != 7 && D_800EFD70_67.f4C != 2 && D_800CCD64 == 0 && !(D_800EFD70_67.f0 & 0x40000)) {
        switch (D_800EFD70_67.f3C) {
        default:
            D_800EFDB4 = 0;
            break;
        case 1:
            D_800EFD70_67.f44 = 5;
            break;
        case 2:
            D_800EFD70_67.f44 = D_800EFD70_67.f3C;
            break;
        }
    } else {
        D_800EFDB4 = 0;
    }
    if (!(D_800EFD70_67.f0 & 0x8000)) {
        g = &D_800EFD70_67;
        g->f40 = (g->f4C != 1 || D_800CA930_20 == 0 || D_800CA930_20->trk - D_800CA9C8_6 != 4) ?
            ((g->f4C == 1 && D_800CA930_20 != 0 && D_800CA930_20->trk - D_800CA9C8_6 == 5) ?
             D_800CA930_20->trk->f16[D_800CA930_20->f64] : D_800EFD64_39->f1E) : 1;
    }
    func_800BCB04(D_800EFD70_67.f13C);
    func_80014AD0();
    func_800BFDD4(D_800F47C0);
    D_800F47C0 = 0;
    osViSetSpecialFeatures(0xAA);
    D_800EFD70_67.f0 &= ~1;
    if (D_800EFD70_67.f0 & 0x40000) {
        D_800EFD70_67.f3C = D_800F05CA;
        if (D_800EFD70_67.f3C >= 4) {
            D_800EFD70_67.f3C = 3;
        }
        for (k = 0; k < 6; k++) {
            D_800EFD70_67.f778[k] = D_800EFD70_67.arr[k];
            D_800EFD70_67.f790[k] = D_800EFD70_67.f43C[k];
        }
        for (k = 0; k < D_800EFD70_67.f3C; k++) {
            D_800EFD70_67.arr[k] = D_800EFD70_67.f778[D_800CA930_20->f6C[k]];
            D_800EFD70_67.f43C[k] = D_800EFD70_67.f790[D_800CA930_20->f6C[k]];
        }
    }
    if (func_800BF530(D_800D94A4, &D_800E7728) != 0) {
        func_8000ECB8(1);
    }
    func_800BFDD4(D_800E7728);
    func_800BCB04(D_800F01BC);
    D_800CA920 = 6;
    func_8004834C();
}

#define LOAD(id) func_800BFEA4(&D_800F47C0_2->tab[id])

typedef struct {
    s32 f0;
    u32 *tab;
} AssetMgr;

typedef struct {
    u8 pad0[0x34];
    V7 f34;
    u32 f50;
    u8 pad54[0xA0 - 0x54];
} Bike_41;

typedef struct {
    u32 f0;
    u8 pad4[0x1C - 0x4];
    s32 f1C[8];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    u32 f4C;
    u8 pad50[0x74 - 0x50];
    void *f74[0x30];
    u8 pad134[0x2E8 - 0x134];
    s32 f2E8;
    u8 pad2EC[0x3B8 - 0x2EC];
    u32 f3B8;
    Bike_41 *f3BC;
    s32 f3C0;
    s32 f3C4;
    u8 pad3C8[0x43C - 0x3C8];
    u8 f43C[6];
    u8 pad442[0x44A - 0x442];
    s16 f44A;
    s16 *f44C;
    u8 pad450[0x778 - 0x450];
    s32 f778[6];
    s32 f790[6];
    u8 pad7A8[0x7AA - 0x7A8];
    u8 f7AA[6];
} GState_5;

struct Scr {
    s32 f0;
    s32 f4;
    u8 pad8[0x10 - 0x8];
    u16 f10;
    s16 f12;
    s16 f14;
    s16 f16;
    s16 f18;
    u8 pad1A[0x1C - 0x1A];
    void *f1C;
};

extern GState_5 D_800EFD70_68;
extern AssetMgr *D_800F47C0_2;
extern AssetMgr *D_800E7728_5;
extern Bike_41 *D_800EFD64_40;
extern Bike_41 *D_800F0634;
extern Bike_41 D_800CB210_15[];

void func_80078E54(Scr *s)
{
    s32 i;

    if (s == NULL) {
        return;
    }
    D_800EFD70_68.f0 &= ~0x200;
    LOAD(D_800D1398);
    LOAD(D_800D1410);
    if ((s->f10 & 0x20) && s->f4 == 0) {
        for (i = 0; i < 0x30; i++) {
            func_800C204C(D_800EFD70_68.f74[i]);
            func_800BCB04(D_800EFD70_68.f74[i]);
        }
    }
    if (s->f10 & 0x80) {
        LOAD(D_800D1450);
        LOAD(D_800D14C8);
    }
    if (s->f1C != NULL) {
        if (s->f16 == 4) {
            for (i = 0; i < 7; i++) {
                LOAD(*((s32 **)s->f1C)[i]);
            }
        } else {
            LOAD(*(s32 *)s->f1C);
        }
    }
    switch (s->f16) {
    case 23:
        D_800EFD70_68.f0 &= ~0x10000000;
        break;
    case 22:
        func_8008BC30();
        LOAD(D_800D13B4);
        break;
    case 0:
        i = 0;
        LOAD(D_800D1490);
        LOAD(D_800D14A4);
        func_80015CD8();
        D_800EFD70_68.f3C = 1;
        D_800F0634 = D_800EFD64_40;
        for (; i < 6; i++) {
            D_800EFD70_68.f1C[i] = D_800EFD70_68.f778[i];
            D_800EFD70_68.f43C[i] = D_800EFD70_68.f790[i];
        }
        break;
    case 2:
        func_80015CD8();
        if (!(D_800EFD70_68.f0 & 0x400)) {
            D_800C2388 = 0;
        }
        D_800EFD70_68.f0 &= ~0x3C10;
        switch (D_800EFD70_68.f3B8) {
        case 8:
            if (D_800E3700 == 0) {
                D_800EFD70_68.f1C[1] = D_800EFD70_68.f1C[0];
            }
        case 0:
        case 1:
            D_800EFD70_68.f0 |= 0x80;
            if (D_800EFD70_68.f3BC != NULL && (D_800EFD70_68.f3BC->f50 & 2)) {
                D_800EFD70_68.f0 &= ~0x80;
            }
            break;
        }
        i = 0;
        func_800BFEA4(&D_800E7728_5->tab[D_800D09F8]);
        LOAD(D_800D149C);
        LOAD(D_800D1388);
        LOAD(D_800D144C);
        LOAD(D_800D1494);
        LOAD(D_800D1414);
        LOAD(D_800D13A8);
        LOAD(D_800D1388);
        LOAD(D_800D13EC);
        D_800EFD70_68.f2E8 = 0;
        D_800EFD70_68.f4C = D_800EFD70_68.f3B8;
        D_800EFD64_40 = D_800EFD70_68.f3BC;
        for (; i < 6; i++) {
            if (D_800EFD70_68.f3C >= 2) {
                D_800EFD70_68.f43C[i] = i;
            } else {
                D_800EFD70_68.f43C[i] = (D_800EFD70_68.f3C0 + i) % 6;
            }
            D_800EFD70_68.f7AA[i] = D_800EFD70_68.f3C4;
            D_800EFD70_68.f7AA[i] += D_800EFD70_68.f1C[i];
            D_800EFD70_68.f7AA[i] %= 6;
        }
        break;
    case 19:
        func_80073664(0xFF, NULL);
        LOAD(D_800D1470);
        LOAD(D_800D1498);
        LOAD(D_800D139C);
        LOAD(D_800D13A0);
        func_800BCB04(D_800F01B4);
        break;
    case 6:
        func_8006F5F8(5, 0);
        func_8006F5F8(1, 0);
        func_800B78F0(D_800F04E4);
        LOAD(D_800D1470);
        LOAD(D_800D1424);
        LOAD(D_800D1444);
        LOAD(D_800D1428);
        break;
    case 24:
        LOAD(D_800D14A0);
        LOAD(D_800D13BC);
        break;
    case 25:
        LOAD(D_800D13E0);
        LOAD(D_800D142C);
        break;
    case 9:
    case 20:
        if (s->f16 == 9) {
            LOAD(D_800D13CC);
        } else {
            D_800EFD64_40->f34 = D_800F05AC;
            D_800EFD64_40->f50 &= ~0x180;
            func_8006F5F8(2, D_800EFD64_40 - D_800CB210_15);
            for (i = 0; i < D_800EFD70_68.f44A; i++) {
                if (D_800EFD70_68.f44C[i] == D_800EFD64_40 - D_800CB210_15) {
                    break;
                }
            }
            LOAD(*D_800CD148[D_800CA958[i]]);
        }
        func_800BCB04(D_800F01B4);
        LOAD(D_800D1488);
        LOAD(D_800D1380);
        LOAD(D_800D148C);
        LOAD(D_800D13A0);
        break;
    case 7:
    case 13:
    case 14:
        func_800BCB04(D_800F01B4);
        LOAD(D_800D13A0);
        LOAD(D_800D143C);
        LOAD(D_800D13C0);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        break;
    case 12:
        func_8008B6DC();
        D_800EFD70_68.f0 &= ~0x02000000;
        break;
    case 3:
        LOAD(D_800D141C);
        LOAD(D_800D1430);
    case 11:
        LOAD(D_800D149C);
        D_800EFD70_68.f2E8 = 0;
        LOAD(D_800D14AC);
        LOAD(D_800D140C);
        LOAD(D_800D13F4);
        LOAD(D_800D1394);
        LOAD(D_800D13C8);
        LOAD(D_800D1418);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        for (i = 0; i < s->f18; i++) {
            if (s->f16 == 11) {
                LOAD(*D_800CD148[D_800CA958[i + s->f14]]);
            } else {
                LOAD(*D_800CD0B8[i + s->f12]);
            }
        }
        break;
    case 4:
    case 18:
        i = 0;
        LOAD(D_800D14AC);
        LOAD(D_800D1394);
        LOAD(D_800D13C8);
        LOAD(D_800D1418);
        LOAD(D_800D1448);
        LOAD(D_800D13F4);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        for (; i < 0x18; i++) {
            LOAD(*D_800CD148[i]);
        }
        func_80073664(0xFF, NULL);
        break;
    }
}

typedef struct {
    u8 pad[0x444];
    u16 *buf;
} Ctx_2;

extern Ctx_2 D_800EFD70_69;

void func_80079AA4(void)
{
    s32 i;
    s32 v;

    D_800EFD70_69.buf = (u16 *)func_800BC908(0x800, 0, D_800D94B0, 0x174F);
    D_800EFD70_69.buf[0] = 0;
    D_800EFD70_69.buf[0x100] = 0;
    D_800EFD70_69.buf[0x200] = 0;
    D_800EFD70_69.buf[0x300] = 0;
    i = 1;
    do {
        D_800EFD70_69.buf[i] = ((i >> 3) << 1) | 0x18C1;
        v = i >> 3;
        D_800EFD70_69.buf[i + 0x100] = (v << 11) | 0xC7;
        D_800EFD70_69.buf[i + 0x200] = (v << 11) | (v << 6) | 7;
        D_800EFD70_69.buf[i + 0x300] = (v << 6) | 0x1807;
        i++;
    } while (i < 0x100);
}

typedef struct {
    u8 *p;
    u8 pad0[0x60];
    s16 cnt;
    u8 pad1[6];
    u8 flag;
} Rec_46;

typedef struct {
    u32 flags;
    u8 pad0[8];
    u32 f0c;
    u8 pad1[0x400];
    s32 mode;
    u8 pad2[0x340];
    u32 f754;
} Gs;

extern Gs D_800EFD70_70;
extern Rec_46 *D_800CA930_21;

void func_80079B64(void)
{
    u8 *q;
    s32 old;
    s32 idx;
    u32 fl;
    Gs *g;
    Gs *g2;
    s32 m;
    s32 idx2;
    s32 mode;
    s32 nv;
    u32 v;
    u32 w;
    s32 md;
    s32 lv;
    u32 f;
    u8 *pb;
    u8 *e;
    u8 t8;
    Rec_46 *r;
    u8 *p0;
    u8 *cb;
    u8 *pb2;

    fl = D_800EFD70_70.flags;
    g = &D_800EFD70_70;
    if ((fl & 0x28000) == 0 && D_800CA920 != 3) {
        v = fl | 0x20000;
        D_800EFD70_70.flags = v;
        mode = g->mode;
        switch (mode) {
        case 1: m = 3; break;
        case 2: m = 7; break;
        case 3: m = 0x1f; break;
        }
        func_8006F128();
        r = D_800CA930_21;
        p0 = r->p;
        cb = &D_800CA9C8_7;
        idx = ((p0 - cb) * 0x38E38E39) >> 2;
        if (idx < 5) if (idx >= 0) {
            pb = r->p;
            if ((s32)*pb <= (s32)r->cnt + 1) {
                if (!(idx == 4 && D_800F05F8 != 0) && r->flag == 0) {
                    g2 = &D_800EFD70_70;
                    q = &D_800F1900_12;
                    lv = q[0x2C0];
                    md = g2->mode;
                    if (md == lv && ((old = q[0x2BF]) >> idx & 1) == 0) {
                        v = old | (1 << idx);
                        q[0x2BF] = v;
                        if ((((u8)v) & m) == m) {
                            if (md != 3) {
                                t8 = q[0x2C0];
                                D_800EFD70_70.f0c = 0;
                                do {
                                q[0x2BF] = 0;
                                q[0x2C0] = t8 + 1;
                                } while (0);
                                D_800EFD70_70.flags |= 0x400000;
                            } else {
                                D_800EFD70_70.flags |= 0x800000;
                            }
                        } else {
                            D_800EFD70_70.flags |= 0x100000;
                        }
                    }
                }
            }
            idx2 = ((D_800CA930_21->p - &D_800CA9C8_7) * 0x38E38E39) >> 2;
            if (idx2 != 4 || D_800F05F8 == 0) {
                pb2 = D_800CA930_21->p;
                e = pb2 + (D_800EFD70_70.mode - 1);
                if (D_800CA930_21->cnt == e[1]) {
                    nv = D_800CA930_21->cnt + 1;
                    if (nv < *pb2) {
                        e[1] = nv;
                    } else if (D_800CA930_21->flag == 0) {
                        e[1] = nv;
                    }
                }
            }
            func_8006F5F8(1, 0);
        }
        f = func_8006F128();
        D_800EFD70_70.f754 = f;
        if (f != 0) {
            D_800EFD70_70.flags |= 0x200000;
        }
    }
}

#define LD(x) func_800BF79C(D_800F47C0, x)

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
} Game_31;

typedef struct { Ent24 *p0; } RecPtr;
typedef struct { u8 pad[8]; u16 h8; u8 padA[6]; } Rec10;
typedef struct { u8 pad[0x14]; u8 b14; u8 b15; u8 b16; } Slot_12;

typedef struct {
    s32 w0;
    u8 pad4[0x1C - 0x4];
    s32 w1C;
    u8 pad20[0x24 - 0x20];
    s32 w24;
    void *w28;
} Snd_4;

extern Game_31 D_800EFD70_71;
extern Rec10 D_800CD370_3[];
extern RecPtr *D_800CA930_22;
extern Snd_4 D_800F4A28;

void func_80079E00(Item_11 *p) {
    s32 i;
    s32 j;
    s32 c;

    func_80010AFC();
    if (p == NULL) {
        return;
    }
    D_800EFD70_71.w740 = 0;
    D_800EFD70_71.w480 = LD(D_800D1410);
    D_800EFD70_71.w484 = LD(D_800D1398);
    if ((p->h10 & 0x20) && p->w4 == 0) {
        for (i = 0; i < 48; i++) {
            D_800EFD70_71.w74[i] = (s32)func_800C1E60(0, 0x40, 4, 2, 0x20);
        }
    }
    if (p->h10 & 0x80) {
        D_800EFD70_71.w390 = LD(D_800D1450);
        D_800EFD70_71.w3DC = LD(D_800D14C8);
    }
    if (p->p1C != NULL) {
        if (p->h16 == 4) {
            for (i = 0; i < 7; i++) {
                D_800EFD70_71.w36C[i] = LD(*p->p1C[i]);
            }
        } else {
            p->w24 = LD((s32)*p->p1C);
        }
    } else {
        p->w24 = 0;
    }
    osViSetSpecialFeatures((p->h10 & 4) ? 0x6A : 0xAA);

    switch (p->h16) {
    case 23:
        D_800EFD70_71.w0 |= 0x10000000;
    case 21:
        D_800EFD70_71.w750 = 0xB4;
        break;
    case 24:
        D_800EFD70_71.w390 = LD(D_800D14A0);
        D_800EFD70_71.w3DC = LD(D_800D13BC);
        D_800EFD70_71.w750 = 0x4B0;
        break;
    case 25:
        D_800EFD70_71.w390 = LD(D_800D13E0);
        D_800EFD70_71.w3DC = LD(D_800D142C);
        D_800EFD70_71.w750 = 0x258;
        break;
    case 22:
        D_800EFD70_71.w750 = 0x12C;
        D_800EFD70_71.w3A0 = LD(D_800D13B4);
        break;
    case 0:
        for (i = 0; i < 6; i++) {
            D_800EFD70_71.w778[i] = D_800EFD70_71.w1C[i];
            D_800EFD70_71.w790[i] = D_800EFD70_71.b43C[i];
        }
        D_800EFD70_71.w1C[0] = 4;
        D_800EFD70_71.w1C[2] = 1;
        D_800EFD70_71.w1C[3] = 2;
        D_800EFD70_71.w1C[4] = 3;
        D_800EFD70_71.w1C[1] = 0;
        D_800EFD70_71.w1C[5] = 5;
        for (i = 0; i < 6; i++) {
            D_800EFD70_71.b43C[i] = i;
        }
        D_800EFD70_71.w0 &= ~0x80;
        D_800EFD70_71.w38C = LD(D_800D1490);
        D_800EFD70_71.w390 = LD(D_800D14A4);
        D_800EFD64_2 = D_800CB210;
        D_800EFD70_71.w4C = 0;
        D_800EFD70_71.w3C = 1;
        func_80064D28(0, 6);
        func_800154FC(0);
        break;
    case 6:
        D_800EFD70_71.w770 = 0;
        D_800EFD70_71.w2E8 = 0;
        func_800BBABC(D_800C29E0, 0x25800);
        D_800EFD70_71.w774 = func_800B781C(D_800D1204);
        func_800BBABC(0, 0);
        D_800EFD70_71.w390 = LD(D_800D1444);
        D_800EFD70_71.w3A4 = LD(D_800D1428);
        D_800EFD70_71.w3AC = LD(D_800D1424);
        D_800EFD70_71.w3A0 = LD(D_800D1470);
        if (D_800CCD64 != 0) {
            D_800EFD70_71.h448 = 6;
            D_800CCD64 = 0;
        }
        D_800EFD70_71.h448 |= 0x100;
        break;
    case 19:
        for (i = 1; i < 0x100; i++) {
            c = (i / 3) >> 3;
            D_800EFD70_71.p13C[i] = (c << 1) | (c << 6) | (c << 11) | 1;
        }
        D_800EFD70_71.h476 = 0;
        D_800EFD70_71.w3A0 = LD(D_800D1470);
        D_800EFD70_71.w45C = LD(D_800D139C);
        D_800EFD70_71.w390 = LD(D_800D1498);
        D_800EFD70_71.w39C = LD(D_800D13A0);
        func_80079AA4();
        break;
    case 9:
    case 20:
        D_800EFD70_71.w2E8 = 0;
        i = 0;
        if (p->h16 == 9) {
            D_800EFD70_71.w2F4[0] = LD(D_800D13CC);
            D_800EFD70_71.name[0] = 0x41;
            for (i = 1; i < 16; i++) {
                D_800EFD70_71.name[i] = 0;
            }
            D_800EFD70_71.h464 = 0;
            D_800EFD70_71.h456 = 0;
            D_800EFD70_71.h458 = 0;
            D_800EFD70_71.h454 = 0;
        } else {
            D_800EFD70_71.w460 = -1;
            D_800EFD70_71.b7A8 = 0xFF;
            for (i = 0; i < 4; i++) {
                D_800EFD70_71.b758[i] = -1;
                if (((Slot_12 *)(D_800F05A8_2 + i * 3))->b14 == 0xFF && ((Slot_12 *)(D_800F05A8_2 + i * 3))->b15 == 0xFF) {
                    D_800EFD70_71.b758[i] = ((Slot_12 *)(D_800F05A8_2 + i * 3))->b16;
                    if (D_800EFD70_71.w460 == -1) {
                        D_800EFD70_71.w460 = i;
                    }
                }
            }
            if (D_800EFD70_71.rgb734[D_800EFD70_71.b758[D_800EFD70_71.w460]][0] != 0) {
                __builtin_memcpy(D_800EFD70_71.name, D_800EFD70_71.rgb734[D_800EFD70_71.b758[D_800EFD70_71.w460]], 3);
                D_800EFD70_71.name[3] = 0;
                D_800EFD70_71.h464 = strlen(D_800EFD70_71.name);
                D_800EFD70_71.h456 = 0x31F;
                D_800EFD70_71.h458 = 0x31F;
                D_800EFD70_71.h454 = 0x27;
            } else {
                D_800EFD70_71.name[0] = 0x41;
                D_800EFD70_71.name[1] = 0;
                D_800EFD70_71.name[2] = 0;
                D_800EFD70_71.name[3] = 0;
                D_800EFD70_71.h464 = 0;
                D_800EFD70_71.h456 = 0;
                D_800EFD70_71.h458 = 0;
                D_800EFD70_71.h454 = 0;
            }
            for (i = 0; i < D_800EFD70_71.h44A; i++) {
                if (D_800EFD70_71.p44C[i] == D_800EFD64_2 - D_800CB210) {
                    break;
                }
            }
            D_800EFD70_71.w2F4[0] = LD(*D_800CD148[D_800CA958[i]]);
        }
        D_800EFD70_71.w390 = LD(D_800D148C);
        D_800EFD70_71.w3A0 = LD(D_800D1380);
        D_800EFD70_71.w478 = LD(D_800D1488);
        D_800EFD70_71.w3EC = 0;
        D_800EFD70_71.w39C = LD(D_800D13A0);
        func_80079AA4();
        break;
    case 13:
    case 14:
        if (p->h16 == 14 && D_800EFD70_71.w4C == 1 && D_800CA930_22 != NULL
            && D_800CA930_22->p0 - D_800CA9C8 == 4) {
            func_80079B64();
        }
        if (D_800EFD70_71.w0 & 0x8000) {
            D_800EFD70_71.w2E8 = (p->h16 == 13) ? 3 : 4;
            D_800EFD70_71.w0 = (D_800EFD70_71.w0 | 0x20000) & ~0x8000;
        } else {
            D_800EFD70_71.w2E8 = 0;
        }
        D_800EFD70_71.w3EC = 0;
        D_800EFD70_71.w39C = LD(D_800D13A0);
        func_80079AA4();
        D_800EFD70_71.w390 = LD(D_800D143C);
        D_800EFD70_71.w3DC = LD(D_800D14C8);
        D_800EFD70_71.w414 = LD(D_800D13C0);
        D_800EFD70_71.w3A4 = LD(D_800D1428);
        D_800EFD70_71.w3AC = LD(D_800D1424);
        break;
    case 7:
        func_80079B64();
        D_800EFD70_71.w2E8 = 0;
        D_800EFD70_71.w3EC = 0;
        func_80079AA4();
        D_800EFD70_71.w39C = LD(D_800D13A0);
        D_800EFD70_71.w390 = LD(D_800D143C);
        D_800EFD70_71.w3A4 = LD(D_800D1428);
        D_800EFD70_71.w3AC = LD(D_800D1424);
        D_800EFD70_71.w414 = LD(D_800D13C0);
        D_800EFD70_71.w3DC = LD(D_800D14C8);
        break;
    case 12:
        D_800F4A50 = 0;
        switch (D_800EFD70_71.h488) {
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
            if (D_800EFD70_71.p3E8 != NULL) {
                func_800BCB04(D_800EFD70_71.p3E8);
            }
            D_800EFD70_71.p3E8 = func_800BC908(0x400, 0, D_800D94B0, 0x18E6);
            D_800F4A28.w24 = (s32)D_800EFD70_71.p3E8;
            D_800F4A28.w1C = 4;
            D_800F4A28.w0 = 1;
            D_800F4A28.w28 = D_800881D4;
            break;
        case 4:
            D_800F4A28.w1C = 4;
            D_800F4A28.w0 = 2;
            D_800F4A28.w24 = (s32)D_800EFD70_71.p3E8;
            break;
        }
        D_800F4A54 = D_800732D0;
        func_80088AE0();
        break;
    case 3:
        D_800C2388 = 0;
        if (p - D_800CD63C_4 == 7) {
            D_800EFD70_71.w4C = 0;
            while (D_800CD370_3[*p->p20].h8 & 1) {
                (*p->p20)--;
            }
        }
        D_800EFD70_71.w0 &= 0xFDFFFFFF;
        D_800EFD70_71.w744 = LD(D_800D141C);
        D_800EFD70_71.w748 = LD(D_800D1430);
        D_800EFD70_71.w2E8 = 0;
        if (p - D_800CD63C_4 == 8) {
            D_800EFD70_71.w3C = 1;
        }
        if (p - D_800CD63C_4 == 10) {
            D_800EFD70_71.w10 = 0;
        }
    case 11:
        D_800EFD70_71.w74C = LD(D_800D149C);
        D_800EFD70_71.w17C = 0;
        D_800EFD70_71.w3EC = 0;
        D_800EFD70_71.w438 = LD(D_800D14AC);
        D_800EFD70_71.w388 = LD(D_800D140C);
        D_800EFD70_71.w3A0 = LD(D_800D1418);
        D_800EFD70_71.w398 = LD(D_800D13F4);
        D_800EFD70_71.w39C = LD(D_800D1394);
        D_800EFD70_71.w3A8 = LD(D_800D13C8);
        D_800EFD70_71.w3A4 = LD(D_800D1428);
        D_800EFD70_71.w3AC = LD(D_800D1424);
        for (i = 0; i < p->h18; i++) {
            if (p->h16 == 11) {
                D_800EFD70_71.w2F4[i + p->h14] = LD(*D_800CD148[D_800CA958[i + p->h14]]);
            } else {
                D_800EFD70_71.w2F4[i + p->h14] = LD(*D_800CD0B8[i + p->h12]);
            }
        }
        break;
    case 18:
        D_800EFD70_71.w2E8 = 13;
        D_800EFD70_71.w750 = 6;
    case 4:
        D_800EFD70_71.w70 = 0;
        switch (D_800EFD70_71.w410) {
        case 1:
            if (D_800EFD70_71.wC >= 2) {
                D_800EFD70_71.wC = 0;
            }
            break;
        case 2:
            if (D_800EFD70_71.wC >= 3) {
                D_800EFD70_71.wC = 0;
            }
            break;
        }
        if (D_800EFD70_71.w410 == 0) {
            D_800EFD70_71.wC = 6;
        } else if (D_800EFD70_71.wC == 6) {
            D_800EFD70_71.wC = 0;
        }
        D_800EFD70_71.w6C = 0;
        D_800EFD70_71.w17C = 0;
        D_800EFD70_71.w3EC = 0;
        D_800EFD70_71.w438 = LD(D_800D14AC);
        D_800EFD70_71.w3A4 = LD(D_800D1428);
        D_800EFD70_71.w3AC = LD(D_800D1424);
        D_800EFD70_71.w3A0 = LD(D_800D1418);
        D_800EFD70_71.w398 = LD(D_800D13F4);
        D_800EFD70_71.w39C = LD(D_800D1394);
        D_800EFD70_71.w3A8 = LD(D_800D13C8);
        D_800EFD70_71.w3B0 = LD(D_800D1448);
        for (i = 0; i < 24; i++) {
            D_800EFD70_71.w2F4[i] = LD(*D_800CD148[i]);
        }
        D_800EFD70_71.w64 = 0;
        D_800EFD70_71.w68 = 0;
        break;
    case 2:
        D_800EFD70_71.b40C = 0;
        D_800EFD70_71.b40D = 0;
        for (i = 3; i >= 0; i--) {
            D_800EFD70_71.h2E0[i] = 0;
        }
        for (i = 0; i < 6; i++) {
            D_800EFD70_71.b7AA[i] = i;
        }
        D_800EFD70_71.w2E8 = 0;
        if (D_800EFD70_71.w3C == 1) {
            D_800EFD70_71.w3C8 = LD(D_800D1494);
        }
        j = 0;
        D_800EFD70_71.b2C8[1] = -1;
        D_800EFD70_71.b2C8[2] = -1;
        D_800EFD70_71.b2C8[3] = -1;
        D_800EFD70_71.b2CC[0] = -1;
        D_800EFD70_71.w3B8 = D_800EFD70_71.w4C;
        D_800EFD70_71.w3BC = (s32)D_800EFD64_2;
        D_800EFD70_71.w0 &= ~0x3C90;
        for (i = 0; i < 4; i++) {
            if ((D_800C292C >> i) & 1) {
                D_800EFD70_71.b2C8[j] = i;
                D_800EFD70_71.b2CC[i] = j;
                j++;
            }
        }
        for (i = 0; i < 4; i++) {
            D_800EFD70_71.s2A8[i].a = (((D_800E2038 & 0x1FF) << 7) + ((-(i & 1) + (i & 2)) << 14)) & 0xFFFF;
            D_800EFD70_71.s2A8[i].b = 0;
        }
        D_800EFD64_2 = D_800CB670;
        D_800EFD70_71.w4C = 0;
        D_800EFD70_71.w0 |= 0x10;
        D_800EFD70_71.w390 = LD(D_800D1388);
        D_800EFD70_71.w3DC = LD(D_800D144C);
        D_800EFD70_71.w74C = LD(D_800D149C);
        func_80064D28(0, 6);
        func_800154FC(0);
        D_800EFD70_71.w730 = func_800BF79C(D_800E7728, D_800D09F8);
        break;
    }
}

typedef struct Game_32 {
    u8 pad0[0x54];
    s32 f54;
    u16 f58;
    s16 f5A;
} Game_32;

extern Game_32 D_800EFD70_72;

void func_8007AF38(Item_12 *p1, s32 p2, s32 p3, s32 arg3)
{
    Item_12 *p;

    if (D_800EFD70_72.f54 != 0) {
        if (D_800F47C4_3 == NULL) {
            if (p1 != NULL) {
                p = p1;
                do {
                    if (D_800CA920 != 3 && D_800CCD64 == 0) {
                        p->parent = NULL;
                    }
                    func_80079E00(p);
                    p = p->next;
                } while (p != NULL);
                for (p = p1->child; p != NULL; p = p->child) {
                    if (D_800CA920 != 3 && D_800CCD64 == 0) {
                        p->parent = NULL;
                    }
                    func_80079E00(p);
                }
                if (func_8004F180() == 0 && (p1->flags & 0x200) == 0) {
                    func_8004C22C(D_800D11D4);
                }
            }
            D_800EFD70_72.f54 = 1;
            D_800EFD70_72.f58 = 1;
            D_800EFD70_72.f5A = -1;
        } else {
            if (p3 != 0) {
                MenuItem *items = D_800F47C4_3->items;
                s32 *sel = D_800F47C4_3->sel;
                if ((sel != NULL ? &items[*sel] : items)->func != NULL) {
                    (sel != NULL ? &items[*sel] : items)->func(1);
                }
            }
            D_800EFD70_72.f54 = 0;
            D_800EFD70_72.f58 = 0xFF;
            D_800F47A0 = D_800F47C4_3->sel != NULL ? *D_800F47C4_3->sel : 0;
        }
        if (D_800F47C4_3 != NULL) {
            D_800F47A4 = D_800F47C4_3;
        }
        if (p1 != NULL && p2 != 0) {
            p1->parent = D_800F47C4_3;
        }
        D_800F016C = D_800F47A4;
        D_800F47C4_3 = p1;
    }
}

typedef struct { s32 f0; s32 pad[14]; s32 mode; s32 pad2[13]; s32 *slot[64]; } G_3;
typedef struct { s32 f0; s32 f4; s32 pad[16]; s32 a48[4]; } Tb;

extern G_3 D_800EFD70_73;
extern Tb D_800CDB78[];

void func_8007B134(s32 x, s32 y, s32 p3, s32 p4)
{
    G_3 *g = &D_800EFD70_73;
    s32 mode = g->mode;
    s32 yy, v, i, j, k, sy, t;
    Rect *q;
    u8 *base;

    if (mode == 1) {
        return;
    }
    yy = y;
    if (D_800EFD70_73.f0 & (0x400 << p4)) {
        yy += 600;
    }
    base = D_800E1718;
    q = (Rect *)(base + (p4 * 16 + (mode - 1) * 64));
    v = q->y + q->h + 5;
    y = yy;
    if (v < yy) {
        y = v;
    }
    func_800C1E60(D_800EFD70_73.slot[p4 * 9], 0x40, 1, 6, 0x21);
    for (i = 1; i < 9; i++) {
        func_800C1E60(D_800EFD70_73.slot[i + p4 * 9], 0x40, 1, 2, 0x20);
    }
    func_800C1F2C(D_800EFD70_73.slot[p4 * 9], (char *)D_800CDB78[p3].f4);
    func_800C0ACC(D_800EFD70_73.slot[p4 * 9], x, y - 0x1F);
    i = 0;
    do {
        k = i + 1;
        func_800C1F2C(D_800EFD70_73.slot[p4 * 9 + k], (char *)D_800CDD88[i]);
        t = i * 14 - 11;
        sy = yy + t;
        func_800C0ACC(D_800EFD70_73.slot[p4 * 9 + k], x - 0x46, sy);
        j = i + 5;
        func_800C1F2C(D_800EFD70_73.slot[p4 * 9 + j], (char *)D_800CDD98[D_800CDB78[p3].a48[i]]);
        func_800C0ACC(D_800EFD70_73.slot[p4 * 9 + j], x + 0x12, sy);
        i = k;
    } while (i < 4);
}

typedef struct {
    u8 pad0[0x4C];
    s32 mode;
    u8 pad50[0x74 - 0x50];
    s32 *f74[3];
    u8 pad80[0x17C - 0x80];
    s32 f17C;
    u8 pad180[0x3A4 - 0x180];
    struct { u8 b[8]; } *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x3DC - 0x3B0];
    s32 *f3DC;
    u8 pad3E0[0x404 - 0x3E0];
    u8 *f404;
} Glob_5;

extern Glob_5 D_800EFD70_74;

Gfx *func_8007B39C(Gfx *gdl, s32 p2, s32 sel, char **strs)
{
    s32 flag;
    s32 i;
    u8 *img;
    s32 x, y, a, c;

    flag = 0;
    if (D_800EFD70_74.mode == 8 || D_800F0954 - D_800F0A3C >= D_800F0950) {
        flag = 1;
    }
    func_800C2224();
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70_74.f3DC[1]);
    i = 0;
    x = -25;
    y = -13;
    c = 0;
    for (; i < 3; i++) {
        gdl = func_8000A690(0xC0 - c, i * 25 + 0x52, (u8 *)D_800EFD70_74.f3DC + 8, gdl, p2, 1, 1, 3, 0);
        func_800C1E60(D_800EFD70_74.f74[i], 0x40, 1, (sel == i) ? 4 : 2, (i == 1 && flag) ? 0x20000021 : 0x21);
        func_800C1F2C(D_800EFD70_74.f74[i], strs[i]);
        func_800C0ACC(D_800EFD70_74.f74[i], D_800F13EC / 2 - y, D_800F13F0 / 2 + x);
        x += 25;
        y += 7;
        c += 7;
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    img = D_800EFD70_74.f404 + 0x10;
    gdl = func_8000A690((D_800F13EC - img[0] * 8) / 2, (D_800F13F0 - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70_74.f3AC + 4);
    return func_8000A690(0xA1 - (sel - 1) * 7, sel * 25 + 0x56,
                         &D_800EFD70_74.f3A4[D_800EFD70_74.f17C / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}

typedef struct {
    u8 b[8];
} Ent8_2;

typedef struct {
    u8 pad0[0x3C];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    s32 f4C;
    u8 pad50[0x74 - 0x50];
    s32 *f74[4];
    u8 pad84[0x17C - 0x84];
    s32 f17C;
    u8 pad180[0x390 - 0x180];
    u8 *f390;
    u8 pad394[0x3A4 - 0x394];
    Ent8_2 *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x3DC - 0x3B0];
    s32 *f3DC;
    u8 pad3E0[0x404 - 0x3E0];
    u8 *f404;
    u8 pad408[0x410 - 0x408];
    s32 f410;
    u8 pad414[0x74C - 0x414];
    s32 *f74C;
    u8 pad750[0x75C - 0x750];
    s32 f75C;
} Glob_6;

extern Glob_6 D_800EFD70_75;

Gfx *func_8007B9B4(Gfx *gdl, s32 p2, s32 p3)
{
    s32 dx, dy, dz;
    s32 i;
    u8 *img;
    char buf[32];
    char num[2];

    if (p3 != 1 || D_800C292D < 2 || D_800EFD70_75.f4C != 0) {
        dx = 0;
        if (p3 == -1) {
            dx = 7;
        }
    } else {
        dx = -0x18;
    }
    dy = 0;
    if (p3 == -1) {
        dy = -0xC;
    }
    dz = 0;
    if (p3 == 1 && D_800C292D >= 2 && D_800EFD70_75.f4C == 0) {
        dz = -0x17;
    }
    func_800C2224();
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70_75.f3DC[1]);
    for (i = (p3 == -1); i < 4; i++) {
        if (p3 == 1 && D_800C292D >= 2 && D_800EFD70_75.f4C == 0) {
            if (i == p3) {
                img = (u8 *)D_800EFD70_75.f3DC + 0x10;
            } else {
                img = (u8 *)D_800EFD70_75.f3DC + 0x18;
            }
        } else {
            img = (u8 *)D_800EFD70_75.f3DC + 8;
        }
        gdl = func_8000A690(0xC0 - i * 7 + dx, i * 25 + 0x46 + dy, img, gdl, p2, 1, 1, 3, 0);
        if (p3 == 1) {
            if (i >= D_800C292D) {
                func_800C1E60(D_800EFD70_75.f74[i], 0x40, 1, 2, 0x20000021);
                func_800C1F2C(D_800EFD70_75.f74[i], D_800D9AAC);
                func_800C0ACC(D_800EFD70_75.f74[i], D_800F13EC / 2 + 17 - i * 7 + dx,
                              D_800F13F0 / 2 - 39 + i * 25 + dy);
            } else {
                if (i == p3 && D_800EFD70_75.f4C == 0) {
                    func_800C1E60(D_800EFD70_75.f74[i], 0x40, 1, (D_800EFD70_75.f3C == 2) ? 4 : 2, 0x21);
                    func_800C1F2C(D_800EFD70_75.f74[i], D_800D9AC0);
                    func_800C0ACC(D_800EFD70_75.f74[i], D_800F13EC / 2 + (dx + 10) + 0x50 + dz,
                                  D_800F13F0 / 2 + (dy - 14));
                    func_800C1E60(D_800EFD70_75.f74[i], 0x40, 1, (D_800EFD70_75.f3C == 2) ? 6 : 2, 0x21);
                    func_8000EE30(D_800EFD70_75.f75C, buf);
                    func_800C1F2C(D_800EFD70_75.f74[i], buf);
                    func_800C0ACC(D_800EFD70_75.f74[i], D_800F13EC / 2 + (dx + 10) + 0x7A + dz,
                                  D_800F13F0 / 2 + (dy - 14));
                }
                if (((D_800EFD70_75.f4C == 2 || D_800EFD70_75.f4C == 3) && !(i & 1)) ||
                    (D_800EFD70_75.f4C == 7 && i >= 2) || (D_800EFD70_75.f4C == 10 && i >= 2)) {
                    func_800C1E60(D_800EFD70_75.f74[i], 0x40, 1, (i == D_800EFD70_75.f3C - 1) ? 4 : 2, 0x20000021);
                } else {
                    func_800C1E60(D_800EFD70_75.f74[i], 0x40, 1, (i == D_800EFD70_75.f3C - 1) ? 4 : 2, 0x21);
                }
                num[0] = i + '1';
                num[1] = 0;
                func_800C1F2C(D_800EFD70_75.f74[i], num);
                func_800C1F2C(D_800EFD70_75.f74[i], D_800D9AC4);
                func_800C0ACC(D_800EFD70_75.f74[i], D_800F13EC / 2 + 17 - i * 7 + dx + dz,
                              D_800F13F0 / 2 - 39 + i * 25 + dy);
            }
        } else {
            func_800C1E60(D_800EFD70_75.f74[i], 0x40, 1, (i == D_800EFD70_75.f410) ? 4 : 2,
                          (i >= D_800F1BC0 + 1) ? 0x20000021 : 0x21);
            func_800C1F2C(D_800EFD70_75.f74[i], D_800CDDE8[i]);
            func_800C0ACC(D_800EFD70_75.f74[i], D_800F13EC / 2 + 17 - i * 7 + dx,
                          D_800F13F0 / 2 - 39 + i * 25 + dy);
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (p3 == -1) {
        img = D_800EFD70_75.f404 + 0x10;
    } else if (p3 == 1 && D_800C292D >= 2 && D_800EFD70_75.f4C == 0) {
        img = D_800EFD70_75.f404 + 8;
    } else {
        img = D_800EFD70_75.f390 + 0x18;
    }
    gdl = func_8000A690((D_800F13EC - img[0] * 8) / 2, (D_800F13F0 - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70_75.f3AC + 4);
    img = (u8 *)&D_800EFD70_75.f3A4[D_800EFD70_75.f17C / 2 % 30 + 1];
    if (p3 == 1) {
        gdl = func_8000A690(0xA7 - (D_800EFD70_75.f3C - 1) * 7 + dx, (D_800EFD70_75.f3C - 1) * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    } else {
        gdl = func_8000A690(0xA7 - D_800EFD70_75.f410 * 7 + dx, D_800EFD70_75.f410 * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    }
    if (p3 == 1 && D_800C292D >= 2 && D_800EFD70_75.f4C == 0 && D_800EFD70_75.f3C == 2 && (D_800E2038 & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPLoadTLUT_pal256(gdl++, D_800EFD70_75.f74C[1]);
        gdl = func_8000A690(D_800F13EC / 2 + 10 + dx + 0x60 + dz, D_800F13F0 / 2 - 18 + dy, (u8 *)D_800EFD70_75.f74C + 0x18,
                            gdl, p2, 1, 1, 3, 0);
        gdl = func_8000A690(D_800F13EC / 2 + 10 + dx + 0x84 + dz, D_800F13F0 / 2 - 18 + dy, (u8 *)D_800EFD70_75.f74C + 0x20,
                            gdl, p2, 1, 1, 3, 0);
    }
    return func_800C1394(gdl);
}

typedef struct {
    u8 pad0[0x74];
    s32 *f74;
    u8 pad78[0x17C - 0x78];
    s32 f17C;
    u8 pad180[0x3A4 - 0x180];
    struct { u8 b[8]; } *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x47C - 0x3B0];
    s16 f47C;
    u8 pad47E[2];
    u8 *f480;
    s32 *f484;
} Glob_7;

extern Glob_7 D_800EFD70_76;

Gfx *func_8007C640(Gfx *gdl, s32 p2, char *str)
{
    u8 *img;
    s32 n;
    s32 y;
    s32 t;

    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70_76.f484[1]);
    gdl = func_8000A690(0xA3, 0x58, (u8 *)D_800EFD70_76.f484 + 8, gdl, p2, 1, 1, 3, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    img = D_800EFD70_76.f480 + 8;
    gdl = func_8000A690(0x100 - img[0] * 4, 0x78 - (img[1] >> 1), img, gdl, p2, 1, 1, 6, 0);
    func_800C2224();
    func_800C1E60(D_800EFD70_76.f74, 0x40, 1, 2, 0x21);
    func_800C1F2C(D_800EFD70_76.f74, str);
    func_800C0ACC(D_800EFD70_76.f74, 0x105, 0x64);
    func_800C1E60(D_800EFD70_76.f74, 0x40, 1, D_800EFD70_76.f47C ? 4 : 2, 0x21);
    func_800C1F2C(D_800EFD70_76.f74, D_800D9ACC);
    func_800C0ACC(D_800EFD70_76.f74, 0x108, 0x76);
    func_800C1E60(D_800EFD70_76.f74, 0x40, 1, D_800EFD70_76.f47C ? 2 : 4, 0x21);
    func_800C1F2C(D_800EFD70_76.f74, D_800D9AD0);
    func_800C0ACC(D_800EFD70_76.f74, 0x103, 0x88);
    gdl = func_800C1394(gdl);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70_76.f3AC + 4);
    n = D_800EFD70_76.f47C;
    return func_8000A690(n * 7 + 0x9D, (t = n * 5, y = 0x82, y - t * 4),
                         &D_800EFD70_76.f3A4[D_800EFD70_76.f17C / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}

typedef struct {
    s32 pad0[2];
    char *names[20];
} Entry_5; /* 0x58 */

typedef struct {
    u8 pad0[0x1C];
    s32 sel[8];
    s32 flag;
    u8 pad40[0x34];
    s32 *obj[40];
} State_9;

extern State_9 D_800EFD70_77;
extern Entry_5 D_800CDB78_2[];

void func_8007CB14(s32 p, s32 q)
{
    s32 i, n, off;

    if (D_800EFD70_77.flag != 1) {
        return;
    }
    func_800C1E60(D_800EFD70_77.obj[20], 0x40, 2, 2, 0x20);
    func_800C1F2C(D_800EFD70_77.obj[20], D_800D9AD4);
    func_800C0ACC(D_800EFD70_77.obj[20], 0x140, 0x34);
    func_800C1F2C(D_800EFD70_77.obj[p * 20 + 17], D_800CDB78_2[D_800EFD70_77.sel[p]].names[0]);
    func_800C0ACC(D_800EFD70_77.obj[p * 20 + 17], D_800E1718_12.x + 0x10E, q + D_800E1718_12.y - 0x6F);
    func_800C1F2C(D_800EFD70_77.obj[p * 20 + 18], D_800CDB78_2[D_800EFD70_77.sel[p]].names[1]);
    func_800C0ACC(D_800EFD70_77.obj[p * 20 + 18], D_800E1718_12.x + 0x10B, q + D_800E1718_12.y - 0x62);
    func_800C1F2C(D_800EFD70_77.obj[p * 20 + 19], D_800CDB78_2[D_800EFD70_77.sel[p]].names[2]);
    func_800C0ACC(D_800EFD70_77.obj[p * 20 + 19], D_800E1718_12.x + 0x108, q + D_800E1718_12.y - 0x55);
    for (i = 0; i < 4; i++) {
        func_800C1F2C(D_800EFD70_77.obj[p * 20 + 6 + i], D_800CDB78_2[D_800EFD70_77.sel[p]].names[i + 4]);
        func_800C0ACC(D_800EFD70_77.obj[p * 20 + 6 + i], D_800E1718_12.x + 60 - i * 3, q + D_800E1718_12.y - 0x6F + i * 13);
    }
    n = 4;
    while (D_800CDB78_2[D_800EFD70_77.sel[p]].names[n + 4] != 0) {
        n++;
    }
    off = -100;
    for (; D_800CDB78_2[D_800EFD70_77.sel[p]].names[i + 4] != 0; i++) {
        func_800C1F2C(D_800EFD70_77.obj[p * 20 + 6 + i], D_800CDB78_2[D_800EFD70_77.sel[p]].names[i + 4]);
        func_800C0ACC(D_800EFD70_77.obj[p * 20 + 6 + i], D_800E1718_12.x + 60 - i * 3, q + D_800E1718_12.y + off + i * 13);
    }
}

#define D D_800EFD70_78
#define SAVE D_800F1900_13
#define R D_800F05A8_40
#define VP D_800E1718
#define CUSTOM5 (D.f4C == 1 && D_800CA930_23 != NULL && D_800CA930_23->course - D_800CA9C8_8 == 5)
#define CHAMP4 (D.f4C == 1 && D_800CA930_23 != NULL && D_800CA930_23->course - D_800CA9C8_8 == 4)
#define UNLOCKED(n) (D.f410 < SAVE.b2C0 || (SAVE.b2C0 == D.f410 && ((n) == 0 || ((SAVE.b2BF >> ((n) - 1)) & 1))))
#define LVLDONE (D.f410 < SAVE.b2C0 || (SAVE.b2C0 == D.f410 && (SAVE.b2BF & 8)))
#define AVAILD(n) ((D.f4C == 8) ? avail8d(n) : func_80077000((n) / 5))
#define AVAIL(n) ((D.f4C == 8) ? avail8(n) : func_80077000((n) / 5))

#define PREAMBLE()                                                  \
    gDPPipeSync(gdl++);                                             \
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);                           \
    gDPSetTexturePersp(gdl++, G_TP_NONE);                           \
    gDPSetTextureFilter(gdl++, G_TF_BILERP);                        \
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);    \
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);                           \
    gDPSetRenderMode(gdl++, 0x00404240, 0);                         \
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279)

#define CMB_TEX() gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279)
#define CMB_PRIM() gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF)
#define DRAW(x, y, img, f) gdl = func_8000A690((x), (y), (img), gdl, fb, 1, 1, (f), 0)
#define PSYNCK(k) { Gfx *_g = gdl++; _g->words.w0 = 0xE7000000; _g->words.w1 = (u16)(k) >> 16; }
#define DRAWK(x, y, img, f, k) gdl = func_8000A690((x), (y), (img), gdl, fb, 1, 1, (f), (u16)(k) >> 16)
#define DRAWV(x, y, img, f) { s32 _f = (f); gdl = func_8000A690((x), (y), (img), gdl, fb, 1, 1, _f, 0); }
#define TDRAW(spr, x, y, f) { Spr_2 *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->data); DRAW((x), (y), _t, (f)); }
#define TDRAWP(spr, x, y, f) { Spr_2 *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->data); gDPPipeSync(gdl++); DRAW((x), (y), _t, (f)); }

typedef struct {
    u8 w;
    u8 h;
    u8 pad[2];
    void *data;
} Spr_2;

typedef struct {
    char *name;
    s32 w4;
    u16 flags;
    u8 padA[6];
} Item_13;

typedef struct {
    s16 h0;
    s16 h2;
    u8 pad4[0x14 - 4];
    s16 h14;
} Obj_64;

typedef struct {
    char *title;
    s32 w4;
    Obj_64 *p8;
    Item_13 *items;
    u8 pad10[4];
    s16 h14;
    s16 state;
    s16 count;
    u8 pad1A[6];
    s32 *sel;
} Menu_4;

struct Ctl_21 {
    u8 pad0[8];
    u16 h8;
};

typedef struct {
    u8 n;
    u8 pad1[3];
    u8 won[4];
    s16 trk[7];
    s16 laps[7];
} Course_3; /* 0x24 */

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
} Track_17; /* 0xA0 */

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
    Spr_2 *spr2F4[37];
    Spr_2 *p388;
    Spr_2 *p38C;
    Spr_2 *p390;
    Spr_2 *p394;
    Spr_2 *p398;
    Spr_2 *p39C;
    Spr_2 *p3A0;
    Spr_2 *p3A4;
    Spr_2 *p3A8;
    u8 *p3AC;
    Spr_2 *p3B0;
    u8 pad3B4[0x3C8 - 0x3B4];
    Spr_2 *p3C8;
    u8 pad3CC[0x3DC - 0x3CC];
    Spr_2 *p3DC;
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
    Spr_2 *p414;
    u8 pad418[0x438 - 0x418];
    Spr_2 *p438;
    u8 pad43C[0x444 - 0x43C];
    u8 *p444;
    s16 h448;
    s16 h44A;
    s16 *p44C;
    s32 f450;
    s16 h454;
    u8 pad456[0x45C - 0x456];
    Spr_2 *p45C;
    s32 f460;
    s16 h464;
    char s466[0x10];
    s16 h476;
    u8 pad478[0x47C - 0x478];
    s16 h47C;
    u8 pad47E[0x744 - 0x47E];
    Spr_2 *p744;
    Spr_2 *p748;
    Spr_2 *p74C;
    s32 f750;
    u8 pad754[0x758 - 0x754];
    s8 b758[4];
    u8 pad75C[0x7A8 - 0x75C];
    u8 b7A8;
    u8 b7A9;
} Glob_8;

typedef struct {
    u8 pad0[0x2BF];
    u8 b2BF;
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
    u8 vol[3];
} Save_5;

typedef struct {
    char *name;
    s32 x;
} Cheat;

typedef struct {
    Course_3 *course;
    s16 pts[8];
    u8 pad14[0x6C - 0x14];
    u8 order[8];
} Champ;

typedef struct {
    u8 pad0[0x22];
    s16 h22;
    u8 pad24[0x50 - 0x24];
    u8 b50[8];
} Race_9;

typedef struct {
    char *name;
    char *f4;
    u8 pad8[8];
    char *f10;
    u8 pad14[0x48 - 0x14];
    s32 stats[4];
} Rider_8;

typedef struct {
    u8 pad0[0x14];
    void *f14;
    char *f18;
    u8 pad1C[0x3C - 0x1C];
} BikeDef;

typedef struct {
    u8 pad0[0x668];
    s16 h668;
    u8 pad66A[0x68C - 0x66A];
    s32 f68C;
    u8 pad690[0x7E0 - 0x690];
} Racer_3;

extern Glob_8 D_800EFD70_78;
extern Track_17 *D_800EFD64_41;
extern Cheat D_800C2284_2[];
extern Save_5 D_800F1900_13;
extern Track_17 D_800CB210_16[];
extern Course_3 D_800CA9C8_8[];
extern BikeDef D_800C9A40_17[];
extern Champ *D_800CA930_23;
extern Race_9 D_800F05A8_40;
extern Rider_8 D_800CDB78_3[];
extern Racer_3 D_800E37C0_91[];

static inline s32 avail8(s32 n)
{
    s32 k = n / 5;
    s32 v;

    if (k == 0) {
        goto ret1;
    }
    switch (D_800F1900_13.b2C0) {
    case 1:
        if (k >= 2) {
            goto ret0;
        }
        k--;
        v = D_800F1900_13.b2BF >> k;
        break;
    case 2:
        if (k < 2) {
            return 1;
        }
        if (k != 2) {
            goto ret0;
        }
        v = D_800F1900_13.b2BF >> (k - 1);
        break;
    case 3:
        if (k < 3) {
            return 1;
        }
        if (k != 3) {
            return 0;
        }
        v = D_800F1900_13.b2BF >> (k - 1);
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
    switch (D_800F1900_13.b2C0) {
    case 1:
        if (k >= 2) {
            goto ret0;
        }
        k--;
        v = D_800F1900_13.b2BF >> k;
        break;
    case 2:
        if (k < 2) {
            return 1;
        }
        if (k != 2) {
            goto ret0;
        }
        v = D_800F1900_13.b2BF >> (k - 1);
        break;
    case 3:
        if (k < 3) {
            return 1;
        }
        if (k != 3) {
            return 0;
        }
        v = D_800F1900_13.b2BF >> (k - 1);
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

static inline s32 objOk(Obj_64 *p)
{
    s32 lvl;
    s32 v;

    if (D.f4C == 8 || D.f4C == 0) {
        lvl = D_800F1900_13.b2C0;
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

Gfx *func_8007CEDC(Mtx *m, s32 fb, Gfx *gdl, Ctl_21 *pad, s32 e)
{
    char unk[0xE8];
    char buf[0x40];
    s16 wid[4];
    char buf2[0x10];
    char buf3[0x20];
    Menu_4 *s;
    s32 k;
    s32 rowh;
    s32 i;
    Track_17 *trk;
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
    Spr_2 *img;

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
        if (D_800C2940_2 != 0 && (D_800E2038 & 0x20)) {
            Spr_2 *t = &D.p3A0[1];

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
            Spr_2 *sp;

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
                func_800C1F2C(D.txt[i], D_800CB210_16[s->h14 + i].name);
                func_8000EE30(D_800CB210_16[s->h14 + i].h1E, buf);
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
                          (D.f2E8 == 10 && !(D_800F1900_13.b2C4 & 8)) ? 0x20000021 : 0x21);
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
        if (D_800CA9C8_8[*s->sel / 5].n != 0) {
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
                    t = D_800CA9C8_8[D.fC].trk[D.f6C];
                    break;
                default:
                    t = D_800CA9C8_8[D.fC].trk[D.f68];
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
            if (s->state == 18 || ((D.f70 == 0 || D.f70 == 3) && i < D_800CA9C8_8[*s->sel].n) ||
                (D.f70 != 0 && i <= D_800CA9C8_8[*s->sel].n)) {
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
                    } else if (((D_800CA9C8_8[*s->sel].won[D.f410 - 1] >> i) & 1) && !CUSTOM5) {
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
                if (s->state == 18 || i < D_800CA9C8_8[*s->sel].n) {
                    if ((s->state != 18 && D.f70 != 2) || (s->state == 18 && (*s->sel / 5) * 5 + i < D.h44A)) {
                        if (s->state == 18) {
                            switch ((*s->sel / 5) * 5 + i) {
                            case 0x15:
                            case 0x16:
                            case 0x17:
                                strcpy(buf2, "-");
                                break;
                            default:
                                func_8000EE30(D_800CB210_16[D.p44C[(*s->sel / 5) * 5 + i]].h1E, buf2);
                                break;
                            }
                        } else if (*s->sel == 5) {
                            func_8000EE30(D_800CA9C8_8[5].laps[i], buf2);
                        } else if (*s->sel == 4) {
                            func_8000EE30(1, buf2);
                        } else {
                            func_8000EE30(D_800CB210_16[D_800CA9C8_8[*s->sel].trk[i]].h1E, buf2);
                        }
                        func_800C1F2C(D.txt[i + 5], buf2);
                        func_800C0ACC(D.txt[i + 5], 0x11A - i * 7 + xoff, 0x52 + i * 25 + yoff);
                    }
                    if (s->state == 18) {
                        if ((*s->sel / 5) * 5 + i < D.h44A) {
                            func_800C1F2C(D.txt[i], D_800CB210_16[D.p44C[(*s->sel / 5) * 5 + i]].name);
                        }
                    } else {
                        func_800C1F2C(D.txt[i], D_800CB210_16[D_800CA9C8_8[*s->sel].trk[i]].name);
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
                    if (D_800CA9C8_8[*s->sel].n == 0) {
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
                if (D.f6C == D_800CA9C8_8[*s->sel].n) {
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
                    if (D_800CA9C8_8[5].n != 0) {
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

            trk = &D_800CB210_16[D.p44C[D.f450]];

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
            Spr_2 *_t = &D.p3A0[1];

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
                              (D_800C2940_2 == 0 && j == 7) ? 0x20000020 : 0x20);
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
                    if (D_800C2940_2 != 0) {
                        tmp = 6;
                    }
                }
                func_800C1E60(D.txt[0], 0x40, 1, tmp, (D_800C2940_2 == 0) ? 0x20000022 : 0x22);
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
                    sprintf(buf3, D_800D9F98);
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
                        func_8000EE30(D_800F4AC8[slot], buf3 + strlen_2(buf3));
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
            func_8000EE30(plr + 1, buf + strlen_2(buf));
            func_8000ED30(buf, D_800DA014);
            func_800C1F2C(D.txt[0], buf);
            func_800C0ACC(D.txt[0], 0x55, 0x35);
        }
        if (cheat && D.f2E8 == 17) {
            func_800C1E60(D.txt[0], 0x20, 1, 2, 0x21);
            func_800C1F2C(D.txt[0], D_800C2284_2[D.b7A9].name);
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
                if (D_800EFD64_41->f50 & 0x400) {
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
            if (j < strlen_2(D.s466)) {
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
        Glob_8 *gp = &D_800EFD70_78;

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
        func_8000EE30(gp->h476 + 1, buf + strlen_2(buf));
        func_800C1F2C(gp->txt[0], buf);
        func_800C0ACC(gp->txt[0], 0xBA, 0x29);
        func_800C1E60(gp->txt[0], 0x40, 4, 2, 0x21);
        strcpy(buf, "Configuration ");
        buf[strlen_2(buf) + 1] = '\0';
        buf[strlen_2(buf)] = gp->b50[gp->h476] + 'A';
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
                r = (*D_800CA930_23).order[j];
            } else {
                r = R.b50[j];
            }
            img = (Spr_2 *)((r < D.f3C || D.f4C == 8) ? ((D.f4C == 8) ? ((r == D_800C8CE0) ? 4 : 2) : 4) : 2);
            func_800C1E60(D.txt[j * 4 + 5], 0x20, 1, (s32)img, 0x20);
            pl = j;
            while (pl > 0 && s->state == 13 && D_800E37C0_91[R.b50[pl]].f68C == D_800E37C0_91[R.b50[pl - 1]].f68C) {
                s32 d0;

                pl--;
            }
            func_8000EE30(pl + 1, buf);
            func_800C1F2C(D.txt[j * 4 + 5], buf);
            x = rx + 0x4E - j * 7;
            func_800C0ACC(D.txt[j * 4 + 5], (s->state == 14 && CHAMP4) ? x - 2 : x, YR);
            func_8000EF8C(buf, D_800CDB78_3[D.f1C[r]].name);
            func_800C1E60(D.txt[j * 4 + 6], 0x20, 1, (s32)img, 0x20);
            func_800C1F2C(D.txt[j * 4 + 6], buf);
            func_800C0ACC(D.txt[j * 4 + 6], rx - (j * 7 - 0x6A), YR);
            if (r < 4) {
                wid[r] = func_800C1118(D.txt[j * 4 + 6], 0x6A - j * 7, YR, strlen_2(buf) - 1) >> 16;
            }
            if (s->state == 7 && D_800CA938_3[j] != 0) {
                func_800C1E60(D.txt[j * 4 + 7], 0x20, 1, (s32)img, 0x20);
                func_8000EE30(D_800CA938_3[j], buf + 1);
                buf[0] = '+';
                func_800C1F2C(D.txt[j * 4 + 7], buf);
                func_800C0ACC(D.txt[j * 4 + 7], 0x14C - j * 7, YR);
            }
            func_800C1E60(D.txt[j * 4 + 7], 0x20, 1, (s32)img, 0x20);
            if (s->state == 14 && !CHAMP4) {
                func_8000EE30((*D_800CA930_23).pts[r], buf);
            } else {
                func_80052F30(D_800E37C0_91[r].f68C, buf);
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
                              (img->h >> 1) + 0x37 + D_800E37C0_91[j].h668 * 25 + ((s->state != 14) ? 7 : 0));
            } else {
                gDPLoadTLUT_pal256(gdl++, D.p39C->data);
            }
            if (s->state == 14) {
                for (k = 0; k < R.h22; k++) {
                    if ((*D_800CA930_23).order[k] == j) {
                        break;
                    }
                }
            }
            px = 5 + wid[j + D_800C8CE0] + rx;
            if (s->state != 14 || CHAMP4) {
                y = 0x3E + rowh * D_800E37C0_91[j + D_800C8CE0].h668 + ry;
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
            gdl = func_8007B39C(gdl, fb, D.f400, (D.f4C != 8) ? ((D_800EFD64_41->f50 & 2) ? D_800CDDDC : D_800CDDD0) : D_800CDDC4);
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
                func_800C1F2C(D.txt[i * 20], D_800CDB78_3[D.f1C[i]].f4);
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
                            if (func_8000EDA0(D_800CDB78_3[D.f1C[i]].f10, D_800C9A40_17[j].f18) == 0) {
                                break;
                            }
                        }
                        if (D_800C9A40_17[j].f14 != NULL) {
                            gdl = func_80058294(gdl, fb, D_800C9A40_17[j].f14, 0x8C, yo + VP[0][0].y - 0x48);
                        }
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    img = &D.p390[6];
                    for (j = 0; j < 4; j++) {
                        s32 ox;
                        s32 oy;

                        ox = j * 6 - 0x55;
                        n = (D_800CDB78_3[D.f1C[i]].stats[j] + 1) * 2;
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
                        nn = (D_800CDB78_3[D.f1C[i]].stats[j] + 1) * 2;
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
                func_800C1F2C(D.txt[i * 4], D_800CDB78_3[D.f1C[i]].f4);
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

#define IMG(s) ((s)->type == 4 ? D_800EFD70_79.imgs[*(s)->idx] : (s)->img)

typedef struct {
    u8 pad0[0x54];
    s32 mode;           /* 0x54 */
    u16 fade;           /* 0x58 */
    s16 timer;          /* 0x5A */
    u8 pad5C[0x36C - 0x5C];
    Pic *imgs[1];       /* 0x36C */
} Glob_9;

typedef struct {
    u8 pad0[0xC];
    u8 *tbl;            /* 0x0C */
    u8 pad10[6];
    s16 type;           /* 0x16 */
    u8 pad18[4];
    s32 f1C;            /* 0x1C */
    s32 *idx;           /* 0x20 */
    Pic *img;           /* 0x24 */
} Obj_65;

typedef struct {
    s16 x, y, w, h;
} Rect_3;

extern Glob_9 D_800EFD70_79;
extern Rect_3 D_800CDB64;

void func_80087310(Mtx *m, s32 fb, Gfx **pgdl)
{
    u8 unk28[0x70];
    Mtx persp;
    u16 pn;
    Gfx *gdl;
    Obj_65 *s;
    u8 *tbl;
    u16 alpha;

    gdl = *pgdl;
    if (D_800EFDC4 == 0) {
        s = D_800F47A4;
    } else {
        s = D_800F47C4;
    }
    if (s != NULL) {
        if (D_800EFDC4 == 0) {
            tbl = s->tbl + D_800F47A0 * 16;
        } else {
            tbl = s->tbl;
            if (s->idx != NULL) {
                tbl += *s->idx * 16;
            }
        }
    }
    alpha = 0xFF;
    if (D_800EFD70_79.mode == 1) goto c1;
    if (D_800EFD70_79.mode == 0) goto c0;
    if (D_800EFD70_79.mode == 2) goto c2;
    goto done;
c1:
    alpha = 0;
    goto done;
c0:
    alpha = D_800EFD70_79.fade;
    goto done;
c2:
    alpha = 0xFF - D_800EFD70_79.fade;
done:
    func_80011490(&m[16], 0, 0, 0, 0, 0, 0, 0x100);
    guOrtho(&m[12], -D_800F13EC / 2, D_800F13EC / 2, -D_800F13F0 / 2, D_800F13F0 / 2, 50.0f, 4192.0f, 1.0f);
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gSPViewport(gdl++, &D_800CDA60);
    gDPPipeSync(gdl++);
    guPerspective(&persp, &pn, 33.0f, 1.9512196f, 25.0f, 4192.0f, 1.0f);
    func_800120A4(&D_800C25F8, &persp, m);
    gSPPerspNormalize(gdl++, pn);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29E0);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[fb]);
    if (D_800EFDC4 == 1 || s == NULL || s->f1C == 0) {
        gDPSetFillColor(gdl++, 0);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    }
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, D_800C29E0);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, G_CULL_BACK);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    gSPMatrix(gdl++, &m[16], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &m[12], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    func_800C2224();
    gDPPipeSync(gdl++);
    if (D_800EFD70_79.mode == 1) {
        if (D_800EFD70_79.timer > 0 || D_800EFD70_79.timer == -1) {
            D_800EFD70_79.timer--;
        }
    } else {
        if (s->type == 4 || s->f1C != 0) {
            gDPLoadTLUT_pal256(gdl++, s->type == 4 ? D_800EFD70_79.imgs[*s->idx]->pal : s->img->pal);
            gdl = func_8000A690(0, 0, IMG(s)->img, gdl, fb, 1, 1, 3, 0);
        }
        if (s->type == 0) {
            Rect_3 *r = &D_800CDB64;
            D_800F4790.vp.vscale[2] = 0x1FF;
            D_800F4790.vp.vscale[3] = 0;
            D_800F4790.vp.vtrans[2] = 0x1FF;
            D_800F4790.vp.vtrans[3] = 0;
            D_800F4790.vp.vscale[0] = r->w * 2;
            D_800F4790.vp.vscale[1] = r->h * 2;
            D_800F4790.vp.vtrans[0] = (r->x + r->w / 2) * 4;
            D_800F4790.vp.vtrans[1] = (r->y + r->h / 2) * 4;
            gSPViewport(gdl++, &D_800F4790);
            gdl = func_8007CEDC(m, fb, gdl, tbl, 0);
        } else if (s != NULL) {
            gdl = func_8007CEDC(m, fb, gdl, tbl, 0);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
            D_800CA80F_2[0] = alpha;
        }
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

void func_80087DD8(void) {
    func_800BF530("guires", D_800F47C0_3);
}

void func_80087E00(void)
{
    func_800BFDD4(D_800F47C0);
    D_800F47C0 = 0;
}

typedef struct {
    u32 w00;
    u8 pad[0x4C - 4];
    u32 w4C;
} Blk_19;

extern Blk_19 D_800EFD70_80;

void func_80087E30(s32 p) {
    if (p != 0) {
        D_800EFD64 = "Desert";
        D_800EFD70_80.w4C = 5;
        D_800CA924_5.b = 0;
        D_800CA924_5.a = 1;
        D_800CA924_5.c = 0;
        D_800EFD70_80.w00 &= 0xFFFFFF7F;
    }
}

void func_80087E84(s32 a) {
    if (a != 0) {
        D_800EFD64 = D_800CB210_2 + D_800EFD70_4[4] * 0xA0;
        D_800EFD70_4[0] |= 0x80;
    }
}

void func_80087EC8(s32 a0)
{
    u8 *base;

    if (a0 != 0) {
        base = D_800EFD70_2;
        *(s32 *)(base + 0x4C) = 8;
        *(s32 *)(base + 0x3C) = 1;
        *(s32 *)(base) = *(s32 *)(base) | 0x80;
    }
}

typedef struct {
    u32 flags;
    u8 pad0[0x18];
    s32 f1C;
    u8 pad1[0x1C];
    s32 f3C;
    u8 pad2[0xC];
    s32 f4C;
} Game_33;

typedef struct {
    u8 pad0[0x2C1];
    u8 track;
} Opts_2;

typedef struct {
    void *p0;
    u8 pad0[0x60];
    s16 f64;
} Obj_66;

extern Opts_2 D_800F1900_14;
extern Game_33 D_800EFD70_81;
extern Obj_66 *D_800CA930_24;

void func_80087EF8(s32 arg)
{
    s32 i;

    if (arg != 0) {
        D_800EFD64 = D_800CC250;
        D_800EFD70_81.f3C = 1;
        D_800EFD70_81.f4C = 9;
        D_800F0624 = 0;
        D_800EFD70_81.f1C = 4;
        D_800EFD70_81.flags &= ~0x80;
        D_800CA930_24->p0 = D_800CAAA0;
        D_800CA930_24->f64 = 0;
        for (i = 0; i < 0x11; i++) {
            if (D_800CADE4_2[i].id == D_800F1900_14.track) {
                D_800CA930_24->f64 = i;
                return;
            }
        }
    }
}

void func_80087F9C(s32 x)
{
    if (x != 0) {
        D_800EFDBC = 0;
    }
}

typedef struct {
    u32 w00;
    u8 p04[0x38];
    u32 w3C;
    u8 p40[0x0C];
    u32 w4C;
    u8 p50[0x370];
    u32 w3C0;
    u8 p3C4[0x4C];
    u32 w410;
    u8 p414[0x28];
    u8 arr43C[6];
} Blk_20;

extern Blk_20 D_800EFD70_82;

void func_80087FB0(s32 p) {
    s32 i;

    if (p != 0) {
        i = 0;
        do {
            if (i == 0) {
                D_800EFD70_82.arr43C[0] = (u8)D_800EFD70_82.w3C0;
            } else {
                D_800EFD70_82.arr43C[i] = i;
            }
            i++;
        } while (i < 6);
        D_800EFD70_82.w4C = 6;
        D_800EFD64 = "Excite 3D";
        D_800EFD70_82.w3C = 1;
        D_800EFD70_82.w410 = 2;
        D_800EFD70_82.w00 &= 0xFFFFFF7F;
    }
}

typedef struct {
    u32 flags;
    u8 pad04[0x38];
    s32 x3C;
    s32 x40;
    s32 x44;
    s32 x48;
    s32 x4C;
    u8 pad50[0x3C3 - 0x50];
    u8 x3C3;
    u8 pad3C4[0x43C - 0x3C4];
    u8 arr[6];
} G_4;

extern G_4 D_800EFD70_83;

void func_8008802C(s32 p)
{
    s32 i;
    if (p != 0) {
        for (i = 0; i < 6; i++) {
            if (D_800EFD70_83.x3C < 2) {
                if (i == 0) {
                    D_800EFD70_83.arr[0] = D_800EFD70_83.x3C3;
                } else {
                    D_800EFD70_83.arr[i] = i;
                }
            } else {
                D_800EFD70_83.arr[i] = i;
            }
        }
        D_800EFD70_83.x44 = 0;
        D_800C2388 = 1;
        D_800EFD70_83.x4C = 7;
        D_800EFD64 = D_800CC070;
        D_800EFD70_83.flags &= ~0x80;
    }
}

void func_800880B8(s32 x)
{
    if (x != 0) {
        D_800EFD70 = (D_800EFD70 & ~0x80) | 0x20;
    }
}

void func_800880DC(s32 a0) {
    s32 i;
    s8 *b;
    u32 *w;
    if (a0 == 0) {
        return;
    }
    w = D_800EFD70_5;
    w[15] = 1;
    b = (s8 *)w;
    D_800EFD64 = D_800CC1B0;
    w[19] = 4;
    w[0] = w[0] & 0xFFFFFF7F;
    for (i = 0; i < 4; i++) {
        if (i == 0) {
            b[0x2C8] = 0;
        } else {
            b[i + 0x2C8] = -1;
        }
        if (i == 0) {
            b[0x2CC] = 0;
        } else {
            b[i + 0x2CC] = -1;
        }
    }
    D_800CA924[1] = 0;
    D_800CA924[0] = 1;
    D_800CA924[2] = 0;
}

void func_80088170(void)
{
    D_800EFD70_4[19] = 12;
    D_800EFD70_4[0] = (D_800EFD70_4[0] & ~0x80) | 0x40;
}

void func_80088198(int param_1)
{
    if (param_1 != 0) {
        u32 *b = &D_800EFD70;
        b[0x13] = 10;
        D_800EFD64 = "Stunt Course";
        D_800EFD70 = D_800EFD70 & 0xffffff7f;
    }
}

s32 func_800881D4(u32 *p, s32 n)
{
    s32 r;
    if (n != 0x400) {
        return 1;
    }
    r = func_80060170((u8 *)(p + 1), 0x3FC);
    return r != *p;
}

typedef struct {
    u8 p0[0x22];
    u16 h22;
    u8 p24[0x50 - 0x24];
    u8 b50[6];
} SrcA;

typedef struct {
    u8 p0[0x418];
    u16 h418;
    u8 b41A[6];
    u32 w420[6];
    u8 p438[0x764 - 0x438];
    u8 b764[6];
    u8 b76A[6];
} DstB;

typedef struct {
    u8 p0[0x669];
    u8 f669;
    u8 p66A[0x68C - 0x66A];
    u32 f68C;
    u8 pad[0x7E0 - 0x690];
} EntE;

extern SrcA D_800F05A8_41;
extern DstB D_800EFD70_84;
extern EntE D_800E37C0_92[];

void func_8008821C(void)
{
    s32 i = 0;

    D_800EFD70_84.h418 = D_800F05A8_41.h22;
    do {
        D_800EFD70_84.b41A[i] = D_800F05A8_41.b50[i];
        D_800EFD70_84.w420[i] = D_800E37C0_92[i].f68C;
        D_800EFD70_84.b76A[i] = D_800E37C0_92[i].f669;
        if (D_800CA930 != 0) {
            D_800EFD70_84.b764[i] = (D_800CA930 + i)[0x6C];
        }
        i++;
    } while (i < 6);
}

typedef struct {
    u8 pad0[0x668];
    u16 h668;
    u8 pad1[0x68C - 0x66A];
    u32 w68C;
    u8 pad2[0x7E0 - 0x690];
} Elem_4;

typedef struct {
    u8 pad0[0x418];
    u16 h418;
    u8 b41A[6];
    u32 w420[6];
    u8 pad1[0x764 - 0x438];
    u8 b764[6];
    u8 b76A[6];
} Src_3;

typedef struct {
    u8 pad0[0x22];
    u16 h22;
    u8 pad1[0x50 - 0x24];
    u8 b50[6];
} Info_3;

typedef struct {
    u8 pad0[0x6C];
    u8 b6C[6];
} Out_4;

extern Info_3 D_800F05A8_42;
extern Src_3 D_800EFD70_85;
extern Elem_4 D_800E37C0_93[];
extern Out_4 *D_800CA930_25;

void func_800882A0(void)
{
    s32 i;

    D_800F05A8_42.h22 = D_800EFD70_85.h418;
    for (i = 0; i < 6; i++) {
        D_800F05A8_42.b50[i] = D_800EFD70_85.b41A[i];
        D_800E37C0_93[i].w68C = D_800EFD70_85.w420[i];
        D_800E37C0_93[i].h668 = D_800EFD70_85.b76A[i];
        if (D_800CA930_25 != 0) {
            D_800CA930_25->b6C[i] = D_800EFD70_85.b764[i];
        }
    }
}

typedef struct {
    u8 pad0[0x4C];
    s32 f4C;
    u8 pad1[0x410 - 0x50];
    s32 f410;
} State_10;

struct Obj_67 {
    u8 pad0[0x14];
    s16 h14;
};

extern State_10 D_800EFD70_86;

s32 func_80088324(Obj_67 *o)
{
    s32 mode;
    s32 t;

    if (D_800EFD70_86.f4C == 8 || D_800EFD70_86.f4C == 0) {
        mode = D_800F1BC0;
    } else {
        mode = D_800EFD70_86.f410;
    }
    if (mode == 2) {
        goto two;
    }
    if (mode < 3) {
        if (mode == 1) {
            goto one;
        }
        goto ret0;
    }
    if (mode == 3) {
        goto ret1;
    }
    goto ret0;
one:
    t = o->h14 < 10;
tail:
    if (t == 0) {
        goto ret0;
    }
ret1:
    return 1;
two:
    t = o->h14 < 0xF;
    goto tail;
ret0:
    return 0;
}

void func_800883BC(u8 *p, s32 x, u32 *f, void *s)
{
    s32 n;
    void *q;

    if ((*f & 0x50) != 0) {
        func_8007AF38(D_800CD79C, 0, 1, s);
        n = 1;
    } else {
        if ((*f & 0x20) == 0) {
            return;
        }
        q = *(void **)(p + 0x28);
        if (q == 0) {
            return;
        }
        func_8007AF38(q, 0, 0, s);
        n = 2;
    }
    func_8001102C(n);
}

typedef struct {
    u8 pad[0x2BF];
    u8 bits;
    u8 mode;
} Save_6;

extern Save_6 D_800F1900_15;

s32 func_80088424(s32 p)
{
    s32 v;
    if (p == 0) goto one;
    switch (D_800F1900_15.mode) {
    case 1:
        if (p >= 2) goto zero;
        v = (D_800F1900_15.bits >> (p - 1)) & 1;
        break;
    one:
        return 1;
    case 2:
        if (p < 2) goto one;
        if (p != 2) goto zero;
        v = (D_800F1900_15.bits >> (p - 1)) & 1;
        break;
    case 3:
        if (p < 3) goto one;
        if (p != 3) goto zero;
        v = (D_800F1900_15.bits >> (p - 1)) & 1;
        break;
    default:
        goto zero;
    }
    if (v) goto one;
zero:
    return 0;
}

s32 func_800884E0(void)
{
    return *(u32 *)0xA4001000 == 0x17D7;
}
