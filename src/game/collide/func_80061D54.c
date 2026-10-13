#include "ctx.h"

typedef struct Node {
    u8 flag;
    u8 pad1[0x1F];
    struct Node *next;
    struct Node *child;
} Node;

void func_80061D54(Node *p) {
    if (p != NULL && p->flag != 0) {
        p->flag = 0;
        p = p->child;
        while (p != NULL) {
            func_80061D54(p);
            p = p->next;
        }
    }
}
