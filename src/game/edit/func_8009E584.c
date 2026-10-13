#include "ctx.h"

typedef struct {
    u8 pad0[0xC];
    u32 flags;
} Sub;

typedef struct {
    u8 pad0[0x170];
    s32 f170;
    u32 f174;
    u8 pad178[0x16E0 - 0x178];
    Sub *sub;
    u8 pad16E4[0x16F4 - 0x16E4];
    s32 f16F4;
    s32 f16F8;
    s32 f16FC;
    u8 pad1700[2];
    s16 f1702;
    u8 pad1704[0x17FC - 0x1704];
    s16 f17FC;
    s16 f17FE;
} Game;

typedef struct {
    u8 pad0[0x34];
    u32 f34;
    u8 pad38[4];
    u32 f3C;
} Obj;

typedef struct {
    u8 pad0[6];
    s16 f6;
    Obj *obj;
    u8 padC[4];
    s16 f10;
} Menu;

typedef struct {
    u8 pad0[2];
    s16 n;
    u8 pad4[8];
    u16 fC;
    u8 padE[0x30 - 0xE];
    void *a;
    void *b;
    u8 pad38[8];
} Tab;

extern Game *D_800CEA40;
extern u32 D_800E30B0;
extern u32 D_800E25B0[];
extern Menu D_800CEB9C;
extern Tab D_800CE280[];
extern s16 D_800F55B8[][15];

void func_8001102C(s32 param_1);
void func_8009CFC4(void);
void func_8009CAA4(void);
void func_8009CD24(void);
void func_8009C7D4(void);
void func_800A4FF0(void *dst, void *src, s16 *h, s32 n);
void func_8009DF4C(void);

#define PRESSED(b) ((D_800E30B0 & (b)) && !(D_800E25B0[0] & (b)))

void func_8009E584(void)
{
    s32 i;

    if (D_800CEA40->f174 & 0x1000) {
        if (PRESSED(0x20)) {
            D_800E25B0[0] |= 0x20;
            D_800CEA40->sub->flags |= 1;
            D_800CEA40->f174 &= ~0x1000;
        } else if (PRESSED(0x10)) {
            D_800E25B0[0] |= 0x10;
            if (D_800CEA40->f17FE != 0) {
                switch (D_800CEA40->f17FC) {
                case 0:
                    if (D_800CEA40->f16F8 == 1) {
                        func_8009CD24();
                    } else {
                        func_8009CFC4();
                    }
                    break;
                case 1:
                    for (i = 0; i < 15; i++) {
                        D_800F55B8[D_800CEA40->f16FC][i] = 0;
                    }
                    if (D_800CEA40->f16F8 == 1) {
                        func_8009C7D4();
                    } else {
                        func_8009CAA4();
                    }
                    break;
                }
            }
            D_800CEA40->sub->flags |= 1;
            D_800CEA40->f174 &= ~0x1000;
        } else if (PRESSED(0x800) || PRESSED(0x1000)) {
            D_800CEA40->f17FE ^= 1;
            func_8001102C(0);
        }
    } else if (PRESSED(0x10)) {
        D_800CEA40->f170 = 9;
        D_800CEA40->sub->flags |= 1;
        D_800CEB9C.f10 = 3;
        D_800CEB9C.f6 = 3;
        D_800CEB9C.obj->f34 |= 0x10;
        D_800CEB9C.obj->f3C |= 1 << (D_800CEA40->f16F4 - 1);
    } else {
        if (PRESSED(0x10000)) {
            Tab *b = D_800CE280;
            s32 k = D_800CEA40->f16FC + 0x12;
            Tab *t;
            t = &b[k];
            if (t->n >= 5 && !(t->fC & 0x200)) {
                if (D_800CEA40->f16F8 == 1 || D_800CEA40->f16F8 == t->n * 2 - 1) {
                    D_800CEA40->f17FC = 1;
                    D_800CEA40->f174 |= 0x1000;
                    D_800CEA40->sub->flags &= ~1;
                    D_800CEA40->f17FE = 0;
                    goto end;
                }
            }
        }
        if ((PRESSED(0x4000) && (D_800CEA40->f174 & 1)) || (PRESSED(0x2000) && !(D_800CEA40->f174 & 1))) {
            if (D_800CEA40->f16F8 >= 2) {
                D_800CEA40->f16F8--;
                func_8001102C(0);
            } else {
                Tab *b = D_800CE280;
            s32 k = D_800CEA40->f16FC + 0x12;
                Tab *t;
            t = &b[k];
                if (t->n < D_800CEA40->f1702 && !(t->fC & 0x200)) {
                    D_800CEA40->f17FC = 0;
                    D_800CEA40->f174 |= 0x1000;
                    D_800CEA40->sub->flags &= ~1;
                    D_800CEA40->f17FE = 0;
                }
            }
        }
        if ((PRESSED(0x4000) && !(D_800CEA40->f174 & 1)) || (PRESSED(0x2000) && (D_800CEA40->f174 & 1))) {
            Tab *b = D_800CE280;
            s32 k = D_800CEA40->f16FC + 0x12;
            Tab *t;
            t = &b[k];
            if (D_800CEA40->f16F8 < t->n * 2 - 1) {
                D_800CEA40->f16F8++;
                func_8001102C(0);
            } else if (t->n < D_800CEA40->f1702 && !(t->fC & 0x200)) {
                D_800CEA40->f17FC = 0;
                D_800CEA40->f174 |= 0x1000;
                D_800CEA40->sub->flags &= ~1;
                D_800CEA40->f17FE = 0;
            }
        }
        func_8009DF4C();
    }
end:
    {
        Tab *b = D_800CE280;
            s32 k = D_800CEA40->f16FC + 0x12;
        Tab *t;
            t = &b[k];
        func_800A4FF0(t->a, t->b, D_800F55B8[D_800CEA40->f16FC], t->n);
    }
}
