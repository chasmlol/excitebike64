#include "ctx.h"

extern s32 *D_800CEA40;
char *strcpy(char *, const char *);

#define G ((u8 *)D_800CEA40)

void func_8009EFE4(s32 a0) {
    u8 *p;
    s32 i;

    *(s32 *)(G + 0x17C) = 0;
    *(s32 *)(G + 0x180) = 0;
    *(s32 *)(G + 0x184) = 0;
    *(s32 *)(G + 0x190) = 0;
    *(s16 *)(G + 0x16B6) = 0;
    *(s16 *)(G + 0x16B8) = 0;

    p = G + 0x14F4;
    i = 0x95;
    do {
        *(s16 *)(p + 0x194) = 0;
        i--;
        p -= 0x24;
    } while (i >= 0);

    if (a0 != 0) {
        *(s32 *)(G + 0x16F4) = 8;
    }
    strcpy((char *)D_800CEA40 + 0x16BA, "<NONE>");
}
