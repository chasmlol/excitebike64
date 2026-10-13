#include "ctx.h"

typedef struct Node {
    u8 flag;
    u8 pad0[0x1F];
    struct Node *sib;
    struct Node *first;
} Node;

extern s32 D_800F0B18;

void func_80061D54(Node *p);

void func_80061F1C(Node *p) {
    Node *q;
    D_800F0B18 = 0;
    q = p;
    if (q != NULL && q->flag != 0) {
        q->flag = 0;
        for (q = q->first; q != NULL; q = q->sib) {
            func_80061D54(q);
        }
    }
}
