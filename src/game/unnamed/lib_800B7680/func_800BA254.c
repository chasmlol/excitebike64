#include "ctx.h"

typedef struct Node {
    struct Node *prev;
    struct Node *next;
} Node;

extern Node *D_800D33E8;
void func_8000EE20(void);
void func_8000EE28(void);

void func_800BA254(Node *n)
{
    func_8000EE20();
    if (n->prev != NULL) {
        n->prev->next = n->next;
    }
    if (n->next != NULL) {
        n->next->prev = n->prev;
    }
    if (n == D_800D33E8) {
        D_800D33E8 = n->next;
    }
    func_8000EE28();
}
