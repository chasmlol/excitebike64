#include "ctx.h"

extern s32 D_800E76D0;
extern s32 D_800EFCF0;
extern s32 D_800C8CD8;
extern u32 D_800E3700;
typedef struct { u32 pad[7]; u32 map[12]; u32 mode; } Ctl;
extern Ctl D_800EFD70;
extern s32 D_800EFD28[];
extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800CA858;
extern s32 D_800CA864[];
extern s32 D_800EFD00[];

u32 func_8000EFE8(void);

static inline s32 check(u32 *p)
{
    if (D_800C8CD8 != 0 && *p == D_800E3700) return 1;
    if (D_800EFD70.mode == 2) return 1;
    if (*p >= 6) return 1;
    D_800EFD28[*p] = D_800E76D0;
    *p = D_800EFD70.map[*p];
    if (*p >= 6) return 1;
    return (D_800EFD70.mode == 9) * 2;
}

static inline void push(s32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004C854(u32 a, u32 b)
{
    if (D_800E76D0 - D_800EFCF0 < 600) return;
    if (check(&a)) return;
    if (check(&b)) return;
    D_800EFCF0 = D_800E76D0;
    func_8000EFE8();
    if (func_8000EFE8() & 1) {
        if (D_800CA858 >= 5) return;
        push(0x75);
        push(D_800CA864[a + 12]);
        push(0xBA);
        push(D_800CA864[b + 18]);
    } else {
        if (D_800CA858 >= 5) return;
        push(D_800CA864[a + 12]);
        push(0xBA);
        push(D_800CA864[b + 18]);
        if (func_8000EFE8() & 1) {
            push(0x76);
        } else {
            push(0x74);
        }
    }
}
