#include "ctx.h"

typedef struct {
    s16 f0;
    s16 count;
    u8 pad4[8];
    u16 flags;
    u8 padE[0x22];
    void **f30;
    void **f34;
    u8 pad38[8];
} Rec;

extern Rec D_800CE280[];

void func_800BCB04(void *p);

static inline void freeOne(Rec *q, s32 k)
{
    void **a;
    void **b;

    a = &q->f30[k * 8 + 1];
    b = &q->f34[k * 8 + 1];
    func_800BCB04(*a);
    func_800BCB04(*b);
}

void func_800A5964(s32 param_1)
{
    s32 i;
    s32 j;
    s32 n;

    for (n = 18; n < 30; n++) {
        if (D_800CE280[n].flags & 0x40) {
            for (i = 0; param_1 == 0 ? i < D_800CE280[n].count : i < 8; i++) {
                for (j = 0; j < 2; j++) {
                    freeOne(&D_800CE280[n], i * 2 + j);
                }
            }
            func_800BCB04(D_800CE280[n].f30);
            func_800BCB04(D_800CE280[n].f34);
        }
    }
}
