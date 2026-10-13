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
#define OFS(n) (((n) - 1 > 0 && (n) - 1 < D_800CE280[CUR + 0x12].cnt * 2) ? D_800F55B8[CUR][(n) - 2] : 0)
#define ANG(n, p) ((n) - 1 > 0 ? (p)[(n) - 2] : 0)

int func_8009D5D8(int n, u16 *p)
{
    s32 lo;
    s16 sv;
    s32 res;

    lo = OFS(n) - 500;
    sv = D_800C2C60[((ANG(n, p) - 0x2000) >> 4) & 0xFFF];
    if (sv == 0) {
        sv = 1;
    }
    res = D_800C2C60[(((ANG(n, p) - 0x2000) >> 4) - 0x400) & 0xFFF] * 250 / sv;
    res += OFS(n);
    return res < lo ? lo : res;
}
