#include "ctx.h"

typedef struct Obj {
    u8 pad0[0x48];
    f32 f48;
    u8 pad1[0x54 - 0x4C];
    s32 w54;
    u8 pad2[0x5D0 - 0x58];
    s8 b5d0;
    u8 pad3[0x744 - 0x5D1];
    s32 w744;
    u8 pad4[0x7A9 - 0x748];
    u8 u7a9;
    u8 u7aa;
} Obj;

extern s32 D_800F05D4;
extern f32 D_800C8E00;

u8 func_8006BD4C(Obj *p, s32 a1);

void func_8006BC14(Obj *p)
{
    if (D_800F05D4 <= 0) {
        return;
    }
    if ((s8)p->u7aa != 0) {
        if ((s8)p->u7aa > 0) {
            p->u7aa--;
        } else {
            p->u7aa++;
        }
    } else if (p->w744 < 0) {
        if (p->b5d0 < -5) {
            if (p->u7a9 != 0 && func_8006BD4C(p, p->u7a9 - 1)) {
                p->u7a9--;
                p->u7aa = (s8)-D_800C8E00;
            }
        } else if (p->b5d0 >= 6 && p->u7a9 < 3 && func_8006BD4C(p, p->u7a9 + 1)) {
            p->u7a9++;
            p->u7aa = (s8)D_800C8E00;
        }
    }
    p->w54 = 0;
    p->f48 = p->u7a9 * 2.178f - (s8)p->u7aa * (2.178f / D_800C8E00);
}
