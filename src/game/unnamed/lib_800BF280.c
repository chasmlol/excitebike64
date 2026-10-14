#include "game.h"

s32 func_800BF280(Q_3 *q, s32 v, s32 mode)
{
    s32 lk;
    s32 idx;
    s32 m;

    m = mode;
    lk = __osDisableInt();
    if (q->f8 >= q->f10) {
        do {
            if (m != 1) {
                __osRestoreInt(lk);
                return -1;
            }
            *(u16 *)((u8 *)*(s32 **)0x800D1F40 + 0x10) = 8;
            __osEnqueueAndYield(&q->f4);
        } while (q->f8 >= q->f10);
    }
    idx = (q->f0c + q->f10 - 1) % q->f10;
    q->f0c = idx;
    q->f14[idx] = v;
    q->f8 = q->f8 + 1;
    if (*q->p0 != 0) {
        s32 *pp = __osPopThread(q);
        osStartThread(pp);
    }
    __osRestoreInt(lk);
    return 0;
}
