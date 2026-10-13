#include "ctx.h"

extern void func_8000EFB0(char *buf);
extern void func_8000ED30(char *dst, char *src);
extern void func_800AEC60(char *dst, char *src);
extern s32 func_800BCEB8(s32 a, s32 b, s32 c);
extern s32 D_80100640;
extern s32 D_80100644;
extern u32 D_8010063C;
extern u32 D_80100634;
extern u32 D_800D56C0[];
extern u32 D_800D56E0[];

static __inline__ void addhex(char *b, u32 r)
{
    char s[9];
    char *p;
    u32 *tab;
    u32 q;

    tab = D_800D56C0;
    p = s;
    do {
        q = r / *tab;
        if ((s32)q < 10) {
            *p = q + '0';
        } else {
            *p = q + '7';
        }
        r -= q * *tab;
        tab++;
        p++;
    } while ((s32)tab < (s32)(D_800D56C0 + 8));
    s[8] = 0;
    func_8000ED30(b, s);
}

static __inline__ void adddec(char *b, u32 r)
{
    char s[6];
    char *p;
    u32 *tab;
    u32 q;
    s32 i;

    r = (u16)r;
    tab = D_800D56E0;
    p = s;
    do {
        q = r / *tab;
        if ((s32)q < 10) {
            *p = q + '0';
        } else {
            *p = q + '7';
        }
        r -= q * *tab;
        tab++;
        p++;
    } while ((s32)tab < (s32)(D_800D56E0 + 5));
    s[5] = 0;
    for (i = 0; i < 4; i++) {
        if (s[i] != '0') {
            break;
        }
        s[i] = ' ';
    }
    func_8000ED30(b, s);
}

void func_800BC3D0(void)
{
    char buf[0x50];
    s32 n;
    s32 v;

    func_800AEC60(buf, "\n");
    func_8000EFB0(buf);
    v = 0;
    n = func_800BCEB8(D_80100640, D_80100644, 1);
    if (n > 0) {
        v = n;
    }
    if (v >= 0) {
        strcpy(buf, "Total Memory Used == 0x");
        addhex(buf, D_8010063C << 4);
        func_8000ED30(buf, "\n");
        func_8000EFB0(buf);

        strcpy(buf, "Total Memory Free == 0x");
        addhex(buf, D_80100634 << 4);
        func_8000ED30(buf, " in ");
        adddec(buf, (u16)v);
        func_8000ED30(buf, " block(s)\n");
        func_8000EFB0(buf);
    }
}
