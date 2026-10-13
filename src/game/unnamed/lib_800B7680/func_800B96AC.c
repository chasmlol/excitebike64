#include "ctx.h"

typedef struct Node {
    u8 pad0[4];
    struct Node *next;
    u8 pad1[8];
    s32 id;
    u8 pad2[0x14];
    s32 val;
} Node;

extern s32 D_800D33F0;
extern Node *D_800D33E8;

void func_8000EE20(void);
void func_8000EE28(void);

s32 func_800B96AC(s32 p1)
{
    Node *n;
    Node *q;

    if (D_800D33F0 == 0) {
        return -1;
    }
    func_8000EE20();
    for (n = D_800D33E8; n != 0; n = n->next) {
        if (n->id == p1) {
            func_8000EE28();
            q = n;
            goto found;
        }
    }
    func_8000EE28();
    q = 0;
found:
    if (q == 0) {
        return -1;
    }
    return q->val;
}
