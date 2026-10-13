#include "ctx.h"

typedef struct Ent {
    s32 f0;
    struct Ent *next;
    s32 f8;
    s32 fc;
    s32 id;
    s32 f14;
    s32 val;
} Ent;

extern Ent *D_800D33E8;
extern s32 D_800D33F0;
extern s32 *D_800FE860;

void func_8000EE20(void);
void func_8000EE28(void);

void func_800BA3F4(s32 p1, s32 p2) {
    Ent *e;
    Ent *q;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (e = D_800D33E8; e != 0; e = e->next) {
            if (e->id == p1) {
                func_8000EE28();
                q = e;
                goto found;
            }
        }
        func_8000EE28();
        q = 0;
    found:
        if (q != 0) {
            q->val = p2;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))D_800FE860[10])(q->f8, p2);
            }
        }
    }
}
