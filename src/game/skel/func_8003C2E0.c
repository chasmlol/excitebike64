#include "ctx.h"

typedef struct {
    s32 f0;
    s32 f4[9];
    s32 f28;
    s32 f2C;
    s32 f30;
    s32 f34;
} Ent;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10[9];
    s32 pad[19];
} Out;

extern s32 D_800EA680;
extern Out D_800E9680[];
extern u8 D_800D6E00[];

Ent *func_800BF79C(u32 *p, s32 i);
void func_8003C834(Ent *base, s32 val, s32 n);
void *func_800BC908(s32 size, u32 flags, void *owner, s32 tag);
void func_800AEC34(void *dst, void *src, s32 n);
void func_800BFEA4(u32 *p);

static inline void walk(Ent *base, s32 val, s32 n)
{
    s32 i;

    while (n != 0) {
        i = D_800EA680;
        base[i].f0 = val;
        if (base[i].f34 != 0) {
            D_800EA680++;
            func_8003C834(base, i, base[D_800EA680 - 1].f34);
        } else {
            D_800EA680++;
        }
        n--;
    }
}

void *func_8003C2E0(u32 *param_1, s32 param_2)
{
    Ent *ents;
    Ent *p;
    s32 sum;
    s32 cnt;
    s32 i;
    s32 j;
    void *r;

    ents = func_800BF79C(param_1, param_2);
    sum = 0;
    cnt = 0;
    do {
        ents[cnt].f0 = -1;
        sum += ents[cnt].f34;
        sum--;
        cnt++;
    } while (sum >= 0);
    D_800EA680 = 0;
    walk(ents, -1, 1);
    for (i = 0; i < cnt; i++) {
        D_800E9680[i].f4 = ents[i].f28;
        D_800E9680[i].f8 = ents[i].f2C;
        D_800E9680[i].fC = ents[i].f30;
        D_800E9680[i].f0 = ents[i].f0;
        for (sum = 0; sum < 9; sum++) {
            D_800E9680[i].f10[sum] = ents[i].f4[sum];
        }
    }
    func_800BFEA4((u32 *)(param_1[1] + param_2 * 4));
    r = func_800BC908(cnt * 0x80, 0, D_800D6E00, 0x74);
    func_800AEC34(r, D_800E9680, cnt * 0x80);
    return r;
}
