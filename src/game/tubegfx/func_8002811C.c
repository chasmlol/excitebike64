#include "ctx.h"

typedef struct {
    s32 a, b, c;
} Tab;

extern s32 D_800E77D0;
extern Tab D_800E8090[];
extern u8 *D_800E7798[];
extern s32 D_800E30A8;
void func_80011490(void *p, s32 a, s32 b, s32 c, s32 x, s32 y, s32 z, s32 w);

short *func_8002811C(s32 p1, s32 p2, s32 p3, s32 p4)
{
    short *r;
    s32 n;
    s32 t;

    n = D_800E77D0;
    if (n >= 300) {
        return 0;
    }
    p2 = p2 * 0x10 - D_800E8090[p1].a;
    p3 = p3 * 0x10 - D_800E8090[p1].b;
    p4 = p4 * 0x10 - D_800E8090[p1].c;
    t = n * 0x40;
    r = (short *)(D_800E7798[D_800E30A8] + t);
    func_80011490(r, 0, 0, 0, p2, p3, p4, 0x100);
    D_800E77D0 = D_800E77D0 + 1;
    return r;
}
