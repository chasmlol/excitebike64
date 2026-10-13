#include "ctx.h"

extern s32 func_800AEC60();
extern s32 func_800BCEB8();
extern void func_8000ED30(char *dst, char *src);
extern void func_8000EFB0();
extern char *strcpy(char *, const char *);

extern s32 D_80100640;
extern s32 D_80100644;
extern u32 D_8010063C;
extern u32 D_80100634;
extern u32 D_800D56C0[8];
extern u32 D_800D56E0[5];

static inline void AddHex(char *s, u32 v)
{
    char dig[12];
    char *dp;
    u32 *tp;
    u32 q;
    char c;

    tp = D_800D56C0;
    dp = dig;
    do {
        q = v / *tp;
        if ((s32)q >= 10) c = q + '7';
        else c = q + '0';
        *dp = c;
        v -= q * *tp;
        tp++;
        dp++;
    } while ((s32)tp < (s32)&D_800D56C0[8]);
    dig[8] = 0;
    func_8000ED30(s, dig);
}

static inline void AddDec(char *s, u16 v)
{
    char dig[12];
    char *dp;
    u32 *tp;
    u32 q;
    char c;
    u32 x = v;
    s32 i;

    tp = D_800D56E0;
    dp = dig;
    do {
        q = x / *tp;
        if ((s32)q >= 10) c = q + '7';
        else c = q + '0';
        *dp = c;
        x -= q * *tp;
        tp++;
        dp++;
    } while ((s32)tp < (s32)&D_800D56E0[5]);
    dig[5] = 0;
    for (i = 0; i < 4 && dig[i] == '0'; i++) dig[i] = ' ';
    func_8000ED30(s, dig);
}

void func_800BC66C(void)
{
    char buf[0x50];
    s32 n;
    s32 s0;

    func_800AEC60(buf, "\n");
    func_8000EFB0(buf);
    s0 = 0;
    n = func_800BCEB8(D_80100640, D_80100644, s0);
    if (n > 0) s0 = n;
    if (s0 < 0) return;

    strcpy(buf, "Total Memory Used == 0x");
    AddHex(buf, D_8010063C << 4);
    func_8000ED30(buf, "\n");
    func_8000EFB0(buf);

    strcpy(buf, "Total Memory Free == 0x");
    AddHex(buf, D_80100634 << 4);
    func_8000ED30(buf, " in ");
    AddDec(buf, s0);
    func_8000ED30(buf, " block(s)\n");
    func_8000EFB0(buf);
}
