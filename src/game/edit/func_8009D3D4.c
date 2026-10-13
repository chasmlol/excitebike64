#include "ctx.h"

typedef struct {
    s16 f0;
    s16 f2;
    u8 rest[0x3C];
} Ent40;

extern Ent40 D_800CE280[];
extern s16 D_800F55B8[][15];
extern s16 D_800C2C60[];
extern u8 *D_800CEA40;

#define CE_ROW() (*(s32 *)(D_800CEA40 + 0x16FC))
#define CE_LIM() ((D_800CE280[CE_ROW() + 0x12].f2) << 1)
#define GETA(i) (((i) + 1 > 0 && (i) + 1 < CE_LIM()) ? D_800F55B8[CE_ROW()][i] : 0)
#define ANG(p, i) (((i) + 1 < CE_LIM()) ? 0x2000 - (p)[(i) + 1] : 0x2000)
#define SINS(x) D_800C2C60[((x) >> 4) & 0xFFF]
#define COSS(x) D_800C2C60[(((x) >> 4) - 0x400) & 0xFFF]

s32 func_8009D3D4(s32 p1, s16 *p2)
{
    s32 t0;
    s32 q;
    s16 s;

    t0 = GETA(p1) + 500;
    s = SINS(ANG(p2, p1));
    if (s == 0) {
        s = 1;
    }
    q = COSS(ANG(p2, p1)) * 250 / s;
    q += GETA(p1);
    return (t0 < q) ? t0 : q;
}
