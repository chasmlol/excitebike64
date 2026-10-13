#include "ctx.h"

typedef struct Node {
    struct Node *next;
    struct Node *prev;
    s32 pad[2];
    s32 cnt;
} Node;

typedef struct Q {
    Node *a;
    Node *b;
    s32 c;
    s32 d;
} Q;

extern Node *D_80100678;
extern Node D_80100680;

void func_800BE770(Q *q)
{
    Node *n;
    Node *h;
    Node *hp;

    if (q->a == 0) {
        return;
    }
    n = q->b;
    if (n != 0 && n != &D_80100680) {
        n->cnt = n->cnt + 1;
        h = D_80100678->next;
        if (n == h) {
            q->a = 0;
            q->b = 0;
            q->c = 0;
            q->d = 0;
            return;
        }
        n->prev->next = n->next;
        n->next->prev = n->prev;
        n->next = h;
        hp = h->prev;
        n->prev = hp;
        hp->next = n;
        h->prev = n;
    }
    q->a = 0;
    q->b = 0;
    q->c = 0;
    q->d = 0;
}
