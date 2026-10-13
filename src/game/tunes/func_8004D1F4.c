#include "ctx.h"

extern s32 D_800F05D4;
extern s32 D_800CA858;
typedef struct { u8 pad[0x668]; s16 f668; u8 pad2[0x7E0 - 0x66A]; } Plr;
extern Plr D_800E37C0[];
extern s32 D_800C8CD8;
extern u32 D_800E3700;
typedef struct { u32 pad[7]; u32 map[8]; u32 f3C; u32 f40; u32 f44; u32 f48; u32 mode; } Ctl;
extern Ctl D_800EFD70;
extern s32 D_800EFD28[];
extern s32 D_800E76D0;
extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800CA864[];
extern s32 D_800EFD00[];
extern s32 D_800CA8D8[];
extern u32 D_800EECD8;

u32 func_8000EFE8(void);

static inline s32 check(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700) return 1;
    if (D_800EFD70.mode == 2) return 1;
    if (*p >= 6) return 1;
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70.map[*p];
    if (*p >= 6) return 1;
    return (D_800EFD70.mode == 9) * 2;
}

static inline void push(s32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004D1F4(u32 a, u32 b)
{
    if (D_800F05D4 < 0x168) return;
    if (D_800CA858 >= 6) return;
    if (D_800E37C0[a].f668 >= 0) return;
    if (check(&a)) return;
    push(D_800CA864[a + 12]);
    if (b == 1) {
        if (a != D_800EECD8) {
            push(D_800CA8D8[func_8000EFE8() & 3]);
        } else {
            push(0x8A);
        }
        D_800EECD8 = a;
    } else if (b == D_800EFD70.f3C + D_800EFD70.f44) {
        if (func_8000EFE8() & 1) {
            push(0x7F);
        } else {
            push(0xBE);
            push(0x9F);
        }
    } else if (b == 2) {
        if (func_8000EFE8() & 1) {
            push(0x80);
        } else {
            push(0xBE);
            push(0x9E);
        }
    } else {
        push(0xBF);
        if (b == 3) {
            push(0x9D);
        } else if (b == 4) {
            push(0x9C);
        } else if (b == 5) {
            push(0x9A);
        }
    }
}
