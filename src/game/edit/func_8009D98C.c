#include "ctx.h"

typedef struct {
    u8 pad[0x16FC];
    s32 cur;
} Ctx9D;

typedef struct { s16 n0; s16 lim; s16 rest[30]; } Row9D;

extern Ctx9D *D_800CEA40;
extern Row9D D_800CE280[];
extern s16 D_800F55B8[][15];
void func_8001254C(s32 a0, s32 a1, u16 *a2, s32 *a3);

#define LIM (D_800CE280[D_800CEA40->cur + 0x12].lim * 2)
#define T1 ((s16 *)D_800F55B8)

static inline s16 *At(s16 *b, s32 c, s32 i)
{
    return &b[c * 15 + i];
}

s32 func_8009D98C(s32 n)
{
    u16 r1;
    s32 dummy;
    u16 r2;
    s32 a, cnt, v;

    if (n > 0 && n < LIM)
        a = *At(T1, D_800CEA40->cur, n - 1);
    else
        a = 0;
    cnt = a;
    for (;;) {
        D_800F55B8[D_800CEA40->cur][n - 1]++;
        if (n > 0 && n < LIM)
            v = D_800F55B8[D_800CEA40->cur][n - 1];
        else
            v = 0;
        if (n - 1 > 0 && n - 1 < LIM)
            v -= D_800F55B8[D_800CEA40->cur][n - 2];
        func_8001254C(4000, v << 4, &r1, &dummy);
        r1 = 0x4000 - r1;
        if (n + 1 > 0 && n + 1 < LIM)
            v = T1[D_800CEA40->cur * 15 + n];
        else
            v = 0;
        if (n > 0 && n < LIM)
            v -= D_800F55B8[D_800CEA40->cur][n - 1];
        func_8001254C(4000, v << 4, &r2, &dummy);
        r2 = 0x4000 - r2;
        if ((s16)(r2 - r1) < -0x2000)
            break;
        cnt++;
    }
    {
        s16 *b = T1;
        *At(b, D_800CEA40->cur, n - 1) = a;
    }
    return cnt;
}
