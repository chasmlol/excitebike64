#include "ctx.h"

typedef struct {
    s16 f0;
    s16 cnt;
    u8 pad[60];
} Trk;

extern u8 *D_800CEA40;
extern Trk D_800CE280[];
extern s16 D_800C2C60[];
extern s16 D_800F55B8[][15];

#define CUR (*(s32 *)(D_800CEA40 + 0x16FC))
#define OFS(i) (((i) > 0 && (i) < D_800CE280[CUR + 0x12].cnt * 2) ? D_800F55B8[CUR][(i) - 1] : 0)
#define ANG(i, p) ((i) < D_800CE280[CUR + 0x12].cnt * 2 ? (p)[i] : 0)

int func_8009D788(int n, s16 *p)
{
    s32 lo;
    s16 sv;
    s32 res;

    lo = OFS(n + 1) - 250;
    sv = D_800C2C60[((-0x2000 - ANG(n + 1, p)) >> 4) & 0xFFF];
    if (sv == 0) {
        sv = 1;
    }
    res = D_800C2C60[(((-0x2000 - ANG(n + 1, p)) >> 4) - 0x400) & 0xFFF] * 250 / sv;
    res += OFS(n + 1);
    return res < lo ? lo : res;
}
