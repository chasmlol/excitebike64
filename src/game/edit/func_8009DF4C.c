#include "ctx.h"

extern u32 D_800E30B0;
extern u32 D_800E25B0;
extern s16 D_800F55B8[][15];
typedef struct { u8 pad0[2]; s16 lim; u8 rest[0x3C]; } Tb;
extern Tb D_800CE280[];
typedef struct { u8 pad[0x16F8]; s32 a; s32 b; s16 st; } Ctl;
extern Ctl *D_800CEA40;

#define A (D_800CEA40->a)
#define B (D_800CEA40->b)
#define LIM (D_800CE280[B + 0x12].lim * 2)
#define H(c) ((c) > 0 && (c) < LIM ? D_800F55B8[B][(c) - 1] : 0)
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CELL (({ s16 *_tb = (s16 *)D_800F55B8; _tb; })[({ s32 _c = A - 1; B * 15 + _c; })])

void func_8009DF4C(void)
{
    volatile s32 pad[8];
    s32 t1;
    s32 hi, lo;
    s32 flag;
    s32 v, x;
    u32 F;

    t1 = D_800E30B0 >> 17;
    t1 &= 1;
    hi = MIN(H(A + 1) + 0x8C, H(A - 1) + 0x8C);
    lo = MAX(H(A - 1) - 0x8C, H(A + 1) - 0x8C);
    if (hi > 0x1C2) {
        hi = 0x1C2;
    }
    if (lo < -0x1C2) {
        lo = -0x1C2;
    }
    *(volatile s16 *)&D_800CEA40->st = 2;
    if (D_800E30B0 & 1) {
        goto body;
    }
    if (D_800E30B0 & 2) {
        goto body;
    }
    if ((D_800E30B0 & 0x800) && !(D_800E25B0 & 0x800)) {
        goto body;
    }
    if ((D_800E30B0 & 0x1000) && !(D_800E25B0 & 0x1000)) {
    body:
        if ((D_800E30B0 & 0x800) && !(D_800E25B0 & 0x800)) {
            *(u16 *)&CELL += 0x14;
        } else if ((D_800E30B0 & 0x1000) && !(D_800E25B0 & 0x1000)) {
            *(u16 *)&CELL -= 0x14;
        } else {
            s16 *p = &CELL;
            s32 w = *p;
            if (D_800E30B0 & 3) {
                if (D_800E30B0 & 1) {
                    x = w + (D_800E30B0 >> 28);
                } else {
                    x = w - (D_800E30B0 >> 28);
                }
            } else {
                x = w;
            }
            *p = x;
        }
        if (t1 != 0) {
            v = CELL;
            if (hi < v) {
                if (flag != 0) {
                    D_800CEA40->st = 1;
                }
                CELL = hi;
            } else if (v < lo) {
                CELL = lo;
                D_800CEA40->st = 1;
            }
        } else {
            v = CELL;
            if (hi < v) {
                D_800CEA40->st = 0;
                CELL = hi;
            } else if (v < lo) {
                CELL = lo;
                D_800CEA40->st = 0;
            }
        }
    }
}
