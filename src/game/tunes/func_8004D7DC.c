#include "ctx.h"

typedef struct {
    u8 pad[0x1C];
    s32 map[12];
    s32 mode;
} S70;

extern s32 D_800CA858;
extern s32 D_800C8CD8;
extern s32 D_800E3700;
extern S70 D_800EFD70;
extern s32 D_800EFD28[];
extern s32 D_800E76D0;
extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800EFD00[];
extern s32 D_800CA864[];

u32 func_8000EFE8(void);

static inline s32 check(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700) return 1;
    if (D_800EFD70.mode == 2) return 1;
    if (*p >= 6) return 1;
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70.map[*p];
    if (*p >= 6) return 1;
    return (D_800EFD70.mode == 9) << 1;
}

static inline void queue(s32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004D7DC(u32 param_1)
{
    s32 r;

    r = (s32)func_8000EFE8() % 3;
    if (check(&param_1)) {
        return;
    }
    if (r == 0) {
        if (D_800CA858 >= 6) {
            return;
        }
        if (func_8000EFE8() & 1) {
            queue(0xB8);
        } else {
            queue(0xB9);
        }
        queue(D_800CA864[param_1]);
        queue(0x78);
    } else if (r == 1) {
        if (D_800CA858 >= 7) {
            return;
        }
        queue(D_800CA864[param_1]);
        queue(0x99);
    } else {
        if (D_800CA858 >= 7) {
            return;
        }
        queue(D_800CA864[param_1]);
        queue(0x77);
    }
}
