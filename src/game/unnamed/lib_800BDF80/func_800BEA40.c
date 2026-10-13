#include "ctx.h"

typedef struct Node {
    struct Node *next;
    struct Node *prev;
    s32 f8;
    s32 key;
    s32 cnt;
} Node;

extern Node D_80100680;
extern Node *D_80100678;
Node *func_800BE2D0(s32 *k, s32 id, s32 zero);

Node *func_800BEA40(s32 *k, s32 id, s8 flag)
{
    Node *p;
    Node *nx;

    if (id < 0) {
        p = &D_80100680;
        if (p->key == *k) {
            return p;
        }
    }
    nx = D_80100678->next;
    for (;;) {
        p = nx;
        nx = p->next;
        if (nx == NULL) {
            p = func_800BE2D0(k, id, 0);
            if (p == NULL) {
                return NULL;
            }
        }
        if (p->key != *k) {
            continue;
        }
        if (id < 0 && p->cnt < 0) {
            p->cnt--;
            break;
        }
        if (p->cnt == id) {
            break;
        }
    }
    if (flag != 0 && p != &D_80100680) {
        nx = D_80100678->next;
        if (p != nx) {
            p->prev->next = p->next;
            p->next->prev = p->prev;
            p->next = nx;
            p->prev = nx->prev;
            p->prev->next = p;
            nx->prev = p;
        }
    }
    return p;
}
