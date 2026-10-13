#include "ctx.h"

typedef struct {
    u8 pad[0x3D2];
    s8 cur;
    u8 pad2[0x3E4 - 0x3D3];
    u8 hist[6];
    u8 n;
    s8 list[16];
} Obj;

s32 func_80070568(Obj *p)
{
    s32 ret = 0;
    s32 i;

    if (p->cur < 16) {
        for (i = 0; i < p->n; i++) {
            if (p->cur == p->list[i]) {
                break;
            }
        }
        if (i == p->n) {
            ret = 1;
            p->list[i] = p->cur;
            p->n = i + 1;
        }
        for (i = 4; i >= 0; i--) { p->hist[i + 1] = p->hist[i]; }
        p->hist[0] = p->cur;
    }
    return ret;
}
