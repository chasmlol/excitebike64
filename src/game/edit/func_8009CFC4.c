#include "ctx.h"

typedef struct {
    u8 pad0[0x8];
    s32 f8;
    s16 x;
    s16 y;
    s16 pad10;
    s16 f12;
    u8 pad14[4];
    s16 sel[2];
} Pl;

typedef struct {
    u8 pad0[0x184];
    Pl *pl;
    u8 pad188[0x16F8 - 0x188];
    s32 f16F8;
    s32 idx;
    u8 pad1700[0x1738 - 0x1700];
    s32 f1738;
    u8 pad173C[0x1758 - 0x173C];
    s32 f1758;
    s32 f175C;
} Game;

typedef struct {
    s16 d[2];
    u8 pad4[0x2C];
    s32 f30;
    s32 f34;
    u8 pad38[8];
} Rec;

extern Game *D_800CEA40;
extern Rec D_800CE700[];
extern Rec D_800CE280[];
extern s16 D_800F55B8[][15];

void func_8008D3B4(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i);
void func_800A4FF0(s32 a, s32 b, s16 *c, s32 d);

void func_8009CFC4(void)
{
    s32 x, y, i;
    Rec *tb;
    Pl *pl;
    s16 s;

    tb = &D_800CE700[D_800CEA40->idx];
    for (y = 0; y < tb->d[(D_800CEA40->pl->f12 + 1) & 1]; y++) {
        for (x = 0; x < tb->d[D_800CEA40->pl->f12 & 1]; x++) {
            func_8008D3B4(D_800CEA40->pl->x + x, D_800CEA40->pl->y + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    for (i = 0; i < 2; i++) {
        D_800F55B8[D_800CEA40->idx][D_800CE280[D_800CEA40->idx + 18].d[1] * 2 + i] = 0;
    }
    D_800CE280[D_800CEA40->idx + 18].d[1]++;
    func_800A4FF0(D_800CE280[D_800CEA40->idx + 18].f30, D_800CE280[D_800CEA40->idx + 18].f34,
                  D_800F55B8[D_800CEA40->idx], D_800CE280[D_800CEA40->idx + 18].d[1]);
    D_800CEA40->f16F8 += 1;
    pl = D_800CEA40->pl;
    s = pl->sel[pl->f8 == 0];
    if (s != 0) {
        if (s == 1) {
            pl->x--;
        }
    } else {
        pl->y--;
    }
    D_800CEA40->f175C = 10000;
    D_800CEA40->f1758 = 50;
}
