#include "ctx.h"

typedef struct Ctl Ctl;

typedef struct Rec {
    s32 f0;
    struct Rec *next;
    struct Rec *p8;
} Rec;

typedef struct {
    u8 pad0[0xC];
    u16 hC;
} Info;

typedef struct {
    u8 pad0[0x14];
    Info *p14;
} Track;

typedef struct {
    u8 pad0[0x16C];
    s32 f16C;
    s32 f170;
    u32 f174;
    s32 f178;
    Rec *f17C;
    Rec *f180;
    Track *f184;
    u8 pad188[0x190 - 0x188];
    s32 f190;
    u8 pad194[0x16E0 - 0x194];
    Ctl *f16E0;
    Ctl *f16E4;
    s32 f16E8;
    u8 pad16EC[0x16F6 - 0x16EC];
    u16 f16F6;
    u8 pad16F8[0x172C - 0x16F8];
    s32 f172C;
    s32 f1730;
    s32 f1734;
    s32 f1738;
    s32 f173C;
    s32 f1740;
    Track *f1744;
    Track *f1748;
    u8 pad174C[0x1758 - 0x174C];
    s32 f1758;
    s32 f175C;
    u8 pad1760[0x17B0 - 0x1760];
    void *f17B0;
} Edit;

typedef struct {
    s32 f0;
    u32 f4;
    s32 f8;
    s32 fC;
} Btn;

typedef struct {
    u8 pad0[0x54];
    u32 f54;
} Lbl;

typedef struct {
    u8 pad0[0xA];
    s16 hA;
    u8 padC[4];
} Sel;

typedef struct {
    u8 pad0[8];
    Sel *p8;
    u8 padC[4];
    s16 h10;
} Menu;

typedef struct {
    u8 b[64];
} Blk;

typedef struct {
    u8 pad0[0x10];
    s32 f10;
} Obj;

extern Edit *D_800CEA40;
extern u32 D_800E30B0;
extern u32 D_800E25B0;
extern u16 D_800CEB54[];
extern s32 D_800CE240;
extern Btn D_800CEA88[];
extern Lbl D_800CEB1C;
extern Menu D_800CEB9C;
extern Blk D_800CE280[];
extern Blk D_800CE300[];
extern Blk D_800CE400[];
extern Blk D_800CE700[];
extern Obj *D_800C29CC;

s32 func_80013010(s32 x);
s32 func_80090398(Ctl *c);
s32 func_80090560(void);
void func_8009B820(void);
void func_800A4A20(void);
void func_80090670(void);
void func_80096B3C(Rec *r);
void func_80091AFC(void);
void func_80092908(void);

#define STKY(v) (((v) & 0xC) ? (((v) & 8) ? (((s32)(v) >> 24) & 0xF) : -(((s32)(v) >> 24) & 0xF)) : 0)
#define STKX(v) (((v) & 3) ? (((v) & 1) ? (s32)((v) >> 28) : -(s32)((v) >> 28)) : 0)
#define ABS(x) ((x) > 0 ? (x) : -(x))

#define CALC1(F, R, A, B, C, D)                                                          \
    {                                                                                    \
        f32 x = D_800CEA40->F;                                                           \
        s32 t = (s32)((x - A) * B / C + D);                                              \
        D_800CEA40->R = t;                                                               \
        fs = (f32)t * (f32)t + x * x;                                                    \
    }
#define CALC2(R, O)                                                                          {                                                                                            s32 s = (s32)__builtin_sqrtf(fs);                                                        D_800CEA40->O = (u16)func_80013010(((D_800CEA40->R << 14) / s) & 0x3FFF);            }

void func_800949BC(void) {
    Edit *g;
    Rec *p;
    f32 fs;

    g = D_800CEA40;
    if (g->f1744 != g->f184) {
        g->f1748 = g->f1744;
        g->f1744 = g->f184;
    }
    if ((D_800E30B0 & 4) && ABS(STKY(D_800E30B0)) >= 3) {
        D_800E30B0 |= 0x2000;
    }
    if ((D_800E30B0 & 8) && ABS(STKY(D_800E30B0)) >= 3) {
        D_800E30B0 |= 0x4000;
    }
    if ((D_800E30B0 & 1) && ABS(STKX(D_800E30B0)) >= 3) {
        D_800E30B0 |= 0x800;
    }
    if ((D_800E30B0 & 2) && ABS(STKX(D_800E30B0)) >= 3) {
        D_800E30B0 |= 0x1000;
    }
    if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10)) {
        D_800CEA40->f174 |= 0x10000;
    } else if (D_800CEA40->f174 & 0x10000) {
        if (((D_800E30B0 & 0x20) && !(D_800E25B0 & 0x20)) || !(D_800E30B0 & 0x10)) {
            D_800CEA40->f174 &= ~0x10000;
        }
    }
    if (D_800CEA40->f1758 != 0) {
        D_800CEA40->f1738 += D_800CEA40->f175C / 50;
        D_800CEA40->f1758--;
        CALC1(f1738, f1740, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
        CALC2(f1740, f173C);
    } else if (D_800CEA40->f184 != 0) {
        if (D_800E30B0 & 0x80) {
            if (D_800CEA40->f170 == 6) {
                D_800CEA40->f1738 -= 600;
                if (D_800CEA40->f1738 < 20000) {
                    D_800CEA40->f1738 = 20000;
                }
                CALC1(f1738, f1740, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
                CALC2(f1740, f173C);
            } else {
                D_800CEA40->f172C -= 600;
                if (D_800CEA40->f172C < 20000) {
                    D_800CEA40->f172C = 20000;
                }
                CALC1(f172C, f1734, 20000.0f, 38000.0f, 165000.0f, 40000.0f);
                CALC2(f1734, f1730);
            }
        }
        if (D_800E30B0 & 0x100) {
            if (D_800CEA40->f170 == 6) {
                D_800CEA40->f1738 += 600;
                if (D_800CEA40->f1738 > 120000) {
                    D_800CEA40->f1738 = 120000;
                }
                CALC1(f1738, f1740, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
                CALC2(f1740, f173C);
            } else {
                D_800CEA40->f172C += 600;
                if (D_800CEA40->f172C > 185000) {
                    D_800CEA40->f172C = 185000;
                }
                CALC1(f172C, f1734, 20000.0f, 38000.0f, 165000.0f, 40000.0f);
                CALC2(f1734, f1730);
            }
        }
    }
    if (D_800CEA40->f184 != 0) {
        if ((D_800E30B0 & 0x200) && !(D_800E25B0 & 0x200)) {
            if (D_800CEA40->f174 & 2) {
                D_800CEA40->f174 &= ~2;
            } else if (D_800CEA40->f174 & 4) {
                D_800CEA40->f174 &= ~4;
            } else {
                D_800CEA40->f174 |= 2;
            }
        }
        if ((D_800E30B0 & 0x400) && !(D_800E25B0 & 0x400)) {
            if (D_800CEA40->f174 & 4) {
                D_800CEA40->f174 &= ~4;
            } else if (D_800CEA40->f174 & 2) {
                D_800CEA40->f174 &= ~2;
            } else {
                D_800CEA40->f174 |= 4;
            }
        }
    }
    D_800CEB54[0] = D_800CEA40->f16F6;
    if (D_800CEA40->f16E0 != NULL) {
        func_80090398(D_800CEA40->f16E0);
    }
    if (D_800CE240 == 0) {
        func_80090670();
    }
    if (D_800CE240 != 0) {
        func_80090560();
    }
    if (D_800CEA40->f184 == NULL) {
        func_8009B820();
    }
    if (D_800CEA40->f184 == NULL) {
        D_800CEA88[0].f4 &= ~0x10;
        D_800CEA88[6].f4 &= ~0x10;
    } else {
        D_800CEA88[0].f4 |= 0x10;
        D_800CEA88[6].f4 |= 0x10;
    }
    if (D_800CEA40->f184 != NULL && (D_800CEA40->f184->p14->hC & 2) && D_800CEA40->f190 != 0) {
        D_800CEB1C.f54 |= 0x10;
    } else {
        D_800CEB1C.f54 &= ~0x10;
    }
    if (D_800CEA40->f190 != 0) {
        D_800CEA88[4].f4 |= 0x10;
        D_800CEA88[5].f4 |= 0x10;
    } else {
        D_800CEA88[4].f4 &= ~0x10;
        D_800CEA88[5].f4 &= ~0x10;
    }
    if (D_800CEA40->f190 != 0 && D_800CEA40->f17C->p8 == D_800CEA40->f180) {
        D_800CEA88[3].f4 |= 0x10;
    } else {
        D_800CEA88[3].f4 &= ~0x10;
    }
    if (D_800CEA40->f16E0 != NULL) {
        func_80090398(D_800CEA40->f16E0);
    }
    if (D_800CEA40->f16E8 == 3 && D_800CEA40->f16E4 != NULL) {
        D_800CEA40->f16E8 = 1;
        D_800CEA40->f16E0 = D_800CEA40->f16E4;
        D_800CEA40->f16E4 = NULL;
    }
    switch (D_800CEB9C.h10) {
    case 0:
        D_800CEA40->f17B0 = &D_800CE280[D_800CEB9C.p8[0].hA];
        break;
    case 1:
        D_800CEA40->f17B0 = &D_800CE300[D_800CEB9C.p8[1].hA];
        break;
    case 2:
        D_800CEA40->f17B0 = &D_800CE400[D_800CEB9C.p8[2].hA];
        break;
    case 3:
        D_800CEA40->f17B0 = &D_800CE700[D_800CEB9C.p8[3].hA];
        break;
    }
    if (D_800CEA40->f174 & 0x10) {
        if (++D_800CEA40->f178 >= 0) {
            if (D_800CEA40->f174 & 0x400) {
                func_800A4A20();
            }
            D_800C29CC->f10 = 3;
        }
    } else {
        switch (D_800CEA40->f16C) {
        case 0:
            func_80091AFC();
            break;
        case 1:
            func_80092908();
            break;
        }
    }
    p = D_800CEA40->f17C;
    if (p != NULL) {
        do {
            func_80096B3C(p);
            p = p->next;
        } while (p != NULL && p != D_800CEA40->f17C);
    }
}
