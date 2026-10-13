#include "ctx.h"

typedef struct {
    u8 pad0[0x62C];
    f32 f62C;
    f32 f630;
    u8 pad1[0x662 - 0x634];
    s16 h662;
    u8 pad2[0x668 - 0x664];
    s16 h668;
    u8 pad3[0x7A5 - 0x66A];
    u8 b7A5;
} Bike;

typedef struct {
    u8 pad[0x22];
    s16 count;
} Race;

extern Bike *D_800F0BE0[];
extern Bike *D_800F0C10[];
extern Race D_800F05A8;
extern s32 D_800F05D4;

void func_800635A4(void);

void func_80063768(void)
{
    u8 swapped;
    s32 i;
    Bike *t;

    do {
        swapped = 0;
        for (i = 0; i < 7; i++) {
            if (D_800F0BE0[i]->f62C > D_800F0BE0[i + 1]->f62C) {
                swapped = 1;
                t = D_800F0BE0[i];
                D_800F0BE0[i] = D_800F0BE0[i + 1];
                D_800F0BE0[i + 1] = t;
            }
        }
    } while (swapped);

    do {
        swapped = 0;
        for (i = 0; i < D_800F05A8.count - 1; i++) {
            if (D_800F0C10[i]->f630 < D_800F0C10[i + 1]->f630
                && (D_800F0C10[i]->b7A5 | D_800F0C10[i + 1]->b7A5) == 0
                && D_800F0C10[i]->h668 < 0) {
                swapped = 1;
                t = D_800F0C10[i];
                D_800F0C10[i] = D_800F0C10[i + 1];
                D_800F0C10[i + 1] = t;
            }
        }
    } while (swapped);

    for (i = 0; i < D_800F05A8.count; i++) {
        D_800F0C10[i]->h662 = i;
    }
    if (D_800F05D4 > 120) {
        func_800635A4();
    }
}
