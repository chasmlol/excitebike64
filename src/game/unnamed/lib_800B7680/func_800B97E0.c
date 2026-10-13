#include "ctx.h"

extern void func_8000EE20(void);
extern void func_8000EE28(void);
extern s32 D_800D33F0;
extern s32 D_800D33E8;

typedef struct Node {
    s32 a;
    struct Node *next;
    s32 b;
    s32 c;
    s32 key;
    s32 d[6];
    s32 val;
} Node;

s32 func_800B97E0(s32 param_1)
{
    Node *p;
    Node *r;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    p = (Node *)D_800D33E8;
    while (p != NULL) {
        if (p->key == param_1) {
            func_8000EE28();
            r = p;
            goto done;
        }
        p = p->next;
    }
    func_8000EE28();
    r = NULL;
done:
    if (r == NULL) {
        return -1;
    }
    return r->val;
}
