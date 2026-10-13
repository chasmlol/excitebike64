#include "ctx.h"

typedef struct Ent {
    s32 f0;
    struct Ent *next;
    s32 f8;
    s32 fc;
    s32 id;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    s32 f24;
    s32 f28;
} Ent;

extern s32 D_800D33F0;
extern Ent *D_800D33E8;
extern s32 *D_800FE860;

void func_8000EE20(void);
void func_8000EE28(void);

void func_800B9244(s32 a, s32 b) {
    Ent *p;
    Ent *q;
    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (p = D_800D33E8; p != 0; p = p->next) {
            if (p->id == a) {
                func_8000EE28();
                q = p;
                goto common;
            }
        }
        func_8000EE28();
        q = 0;
common:
        if (q != 0) {
            q->f28 = b;
            if (q->f8 != -1) {
                ((void (*)(s32, s32))D_800FE860[13])(q->f8, b);
            }
        }
    }
}
