#include "ctx.h"

typedef struct {
    s32 a;
    s32 b;
    s32 c;
    s32 pad[3];
} E18;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
    u8 pad[0x7E0 - 12];
} R7E0;

extern E18 D_800E8030[];
extern E18 D_800E7998[][16];
extern R7E0 D_800E37C0[];
extern s32 D_800C9684;
extern s32 D_800E8508;
extern s32 D_800E8958;
extern s32 D_800E15E0[];
void func_80028864(void);
s32 func_8002E920(s32 x, s32 y, s32 z);

void func_8002D384(s32 p, s32 q)
{
    s32 i;

    D_800E8030[p].c += q * 16;
    for (i = 0; i < 16; i++) {
        D_800E7998[p][i].c += q;
    }
    D_800C9684 = p;
    func_80028864();
    D_800E8508 = 0;
    func_8002E920(D_800E37C0[p].x, D_800E37C0[p].y, D_800E37C0[p].z);
    if (D_800E8508 != 0) {
        D_800E15E0[p] = D_800E8958;
    }
}
