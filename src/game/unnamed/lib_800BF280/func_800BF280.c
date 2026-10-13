#include "ctx.h"

typedef struct Q {
    s32 *p0;
    s32 f4;
    s32 f8;
    s32 f0c;
    s32 f10;
    s32 *f14;
} Q;

extern s32 *D_800D1F40;

s32 func_800AC890(void);
void func_800AC900(s32 lk);
void func_800B0DBC(s32 *p);
s32 *func_800B0F0C(Q *q);
void func_800B1DE0(s32 *p);

s32 func_800BF280(Q *q, s32 v, s32 mode)
{
    s32 lk;
    s32 idx;
    s32 m;

    m = mode;
    lk = func_800AC890();
    if (q->f8 >= q->f10) {
        do {
            if (m != 1) {
                func_800AC900(lk);
                return -1;
            }
            *(u16 *)((u8 *)*(s32 **)0x800D1F40 + 0x10) = 8;
            func_800B0DBC(&q->f4);
        } while (q->f8 >= q->f10);
    }
    idx = (q->f0c + q->f10 - 1) % q->f10;
    q->f0c = idx;
    q->f14[idx] = v;
    q->f8 = q->f8 + 1;
    if (*q->p0 != 0) {
        s32 *pp = func_800B0F0C(q);
        func_800B1DE0(pp);
    }
    func_800AC900(lk);
    return 0;
}
