#include "ctx.h"

typedef struct Node {
    s32 f0;
    struct Node *next;
    s32 val;
    s32 fc;
    s32 key;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
} Node;

extern s32 D_800D33F0;
extern Node *D_800D33E8;

void func_8000EE20(void);
void func_8000EE28(void);

s32 func_800BA614(s32 param_1)
{
    Node *n;
    Node *r;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    for (n = D_800D33E8; n != NULL; n = n->next) {
        if (n->key == param_1) {
            func_8000EE28();
            r = n;
            goto found;
        }
    }
    func_8000EE28();
    r = NULL;
found:
    if (r == NULL) {
        return -1;
    }
    return r->val;
}
