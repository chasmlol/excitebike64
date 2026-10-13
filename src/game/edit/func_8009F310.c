#include "ctx.h"

typedef struct {
    s32 unk0;
    s32 p;
    u8 pad[0x18];
} Cell;

typedef struct {
    s16 unk0;
    s16 rows;
    u8 pad4[0x2C];
    Cell *a;
    Cell *b;
    u8 pad38[8];
} Rec;

typedef struct {
    Rec rec[28];
    u8 pad[0x30];
    u32 *w;
} Big;

extern Big D_800CE280;
void func_800BCB04();

void func_8009F310(s32 n)
{
    u32 *wp;
    s32 i, j;
    u32 k;
    u32 o;
    Cell *a;
    Cell *b;

    wp = D_800CE280.w;
    for (i = 0; i < n; i++) {
        a = D_800CE280.rec[i + 18].a;
        b = D_800CE280.rec[i + 18].b;
        for (j = 0; j < D_800CE280.rec[i + 18].rows; j++) {
            for (k = 0; k < *wp; k++) {
                o = (j * *wp + k) * 32;
                func_800BCB04(*(s32 *)((u8 *)a + o + 4));
                o = (j * *wp + k) * 32;
                func_800BCB04(*(s32 *)((u8 *)b + o + 4));
            }
        }
        func_800BCB04(a);
        func_800BCB04(b);
    }
}
