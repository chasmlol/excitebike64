#include "ctx.h"

extern s32 D_800C2388;
extern s32 D_800C24D4[];
extern s32 D_800C23F4[];
extern s32 D_800C2434[];

void func_8000BA58(void);

typedef struct {
    s32 w[4];
} Blk;

void func_8000BA84(void)
{
    Blk *p1;
    Blk *p2;
    Blk *e;

    if (D_800C2388 != 0) {
        p2 = (Blk *)D_800C24D4;
        p1 = (Blk *)D_800C23F4;
        e = p1 + 4;
        do {
            *p2 = *p1;
            p1++;
            p2++;
        } while (p1 != e);
    } else {
        p2 = (Blk *)D_800C24D4;
        p1 = (Blk *)D_800C2434;
        e = p1 + 4;
        do {
            *p2 = *p1;
            p1++;
            p2++;
        } while (p1 != e);
    }
    func_8000BA58();
}
