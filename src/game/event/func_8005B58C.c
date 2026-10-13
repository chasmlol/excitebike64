#include "ctx.h"

typedef struct Ent {
    s32 key;
    s32 val;
} Ent;

extern Ent *D_800F073C;

s32 func_8005B58C(s32 key)
{
    Ent *p = D_800F073C;
    if (p == NULL) {
        return -1;
    }
    while (p->val >= 0) {
        if (p->key == key) {
            return ((volatile Ent *)p)->val;
        }
        p++;
    }
    return -1;
}
