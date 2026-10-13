#include "ctx.h"

typedef struct Node {
    struct Node *prev;
    struct Node *next;
    s32 pad[9];
    s32 key;
    s32 flags;
} Node;

extern Node *D_800D33E8;
extern s32 D_800D33EC;
extern s32 D_800FA840;

void func_8000EE20(void);
void func_8000EE28(void);

void func_800BA16C(Node *n) {
    Node *p;
    Node *q;
    s32 cnt;

    cnt = 0;
    func_8000EE20();
    p = 0;
    q = D_800D33E8;
    if (q != 0) {
        s32 k = n->key;
        do {
            p = q;
            if (k < p->key) {
                if (p->flags & 1) cnt++;
                q = p->next;
            } else {
                break;
            }
        } while (q != 0);
    }
    n->next = q;
    if (q != 0) {
        n->prev = q->prev;
        if (q->prev != 0) q->prev->next = n;
        q->prev = n;
    } else {
        if (p != 0) p->next = n;
        n->prev = p;
    }
    if (D_800D33E8 == 0 || q == D_800D33E8) {
        D_800D33E8 = n;
        n->prev = 0;
    }
    if (cnt < D_800FA840) {
        D_800D33EC++;
    }
    func_8000EE28();
}
