#include "ctx.h"

typedef struct {
    s32 key;
    s32 val;
} Pair;

extern Pair *D_800F073C;
extern s32 D_800E7728;
extern u32 func_800BF79C(s32 a, s32 b);

u32 func_8005AED8(s32 key)
{
    Pair *p = D_800F073C;
    s32 idx;
    if (p == NULL) {
        idx = -1;
    } else {
        for (; p->val >= 0; p++) {
            if (p->key == key) {
                idx = ((volatile Pair *)p)->val;
                goto found;
            }
        }
        idx = -1;
    }
found:
    if (idx < 0) {
        return 0;
    }
    return func_800BF79C(D_800E7728, idx);
}
