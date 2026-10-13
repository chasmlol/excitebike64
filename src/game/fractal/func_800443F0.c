#include "ctx.h"

typedef struct {
    u8 pad[0x10];
    s32 f10;
    u8 pad2[0x3F0];
} Ent;

extern Ent *D_800EE138;
extern void func_80042A5C(Ent *p, s32 v);

void func_800443F0(s32 n)
{
    s32 i, cnt;

    cnt = 0;
    for (i = 0; i < 16; i++) {
        if (D_800EE138[i].f10 != 0) {
            cnt++;
        }
    }
    if (cnt != 0) {
        n = (n + cnt - 1) / cnt;
        for (i = 0; i < 16; i++) {
            if (D_800EE138[i].f10 != 0) {
                func_80042A5C(&D_800EE138[i], n);
            }
        }
    }
}

