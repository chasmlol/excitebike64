#include "ctx.h"

extern s32 D_800D33F0;
extern struct Ent *D_800D33E8;
extern s32 *D_800FE860;

void func_8000EE20(void);
void func_8000EE28(void);

typedef struct Ent {
    s32 f0;
    struct Ent *next;
    s32 f8;
    s32 fc;
    s32 id;
    s32 p14[4];
    s32 f24;
    s32 p28[4];
    s32 f38;
} Ent;

void func_800B9188(s32 key, s32 val)
{
    Ent *n;
    Ent *q;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (n = D_800D33E8; n != 0; n = n->next) {
            if (n->id == key) {
                func_8000EE28();
                q = n;
                goto found;
            }
        }
        func_8000EE28();
        q = 0;
    found:
        if (q != 0) {
            q->f24 = val;
            q->f38 = val << 16;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))D_800FE860[12])(q->f8, val);
            }
        }
    }
}
