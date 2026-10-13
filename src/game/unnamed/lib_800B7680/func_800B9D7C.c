#include "ctx.h"

void func_8000EE20(void);
void func_8000EE28(void);

typedef struct Node {
    s32 a;
    struct Node *next;
    s32 b;
    s32 c;
    s32 id;
    s32 d[5];
    s32 val;
} Node;

extern s32 D_800D33F0;
extern Node *D_800D33E8;
extern s32 *D_800FE860;

void func_800B9D7C(s32 *o, s32 v)
{
    Node *n;
    Node *r;
    s32 t;
    s32 id;

    *(u8 *)((u8 *)o + 0x31) = (u8)v;
    func_8000EE20();
    t = o[1];
    if (t == -1) {
        goto done;
    }
    id = t;
    if (D_800D33F0 == 0) {
        goto done;
    }
    func_8000EE20();
    for (n = D_800D33E8; n != 0; n = n->next) {
        if (n->id == id) {
            func_8000EE28();
            r = n;
            goto common;
        }
    }
    func_8000EE28();
    r = 0;
common:
    if (r != 0) {
        r->val = v;
        if (r->b != -1) {
            ((void (*)(s32, s32))D_800FE860[13])(r->b, v);
        }
    }
done:
    func_8000EE28();
}
