#include "ctx.h"

typedef struct {
    u8 *ptr;
    s32 a;
    s32 b;
    s32 c;
} Slot;

extern u32 D_80100698;
extern u8 *D_801006A4;
extern Slot D_801006B0[4];

s32 func_8000EDA0(u8 *param_1, u8 *param_2);

Slot *func_800BE694(u8 *param_1)
{
    u32 n = 0;
    s32 j;
    Slot *s;
    Slot *base;
    u8 *p = D_801006A4;

    if (D_80100698 != 0) {
        do {
            if (func_8000EDA0(p, param_1) == 0) {
                break;
            }
            p += 0x20;
            n++;
        } while (n < D_80100698);
    }
    if (n != D_80100698) {
        j = 0;
        base = D_801006B0;
        s = base;
        for (; j < 4; j++) {
            if (s->ptr == 0) {
                break;
            }
            s++;
        }
        if (j != 4) {
            base[j].ptr = p;
            base[j].a = 0;
            base[j].b = 0;
            base[j].c = 0;
            return &base[j];
        }
    }
    return 0;
}
