#include "ctx.h"

typedef struct Node {
    struct Node *prev;
    struct Node *next;
    s32 w08[2];
    s32 arg;
    s32 w14[10];
    void (*fn)(s32, s32);
    s32 flags40;
    s32 w44[2];
    s32 flags4C;
} Node;

extern s32 D_800D33F0;
extern Node *D_800D33E8;
void func_8000EE20(void);
void func_8000EE28(void);

void func_800B98B4(void) {
    Node *n;
    Node *nx;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        n = D_800D33E8;
        while (n != 0) {
            nx = n->next;
            if (n->flags4C & 2) {
                if (n->fn != 0 && (n->flags40 & 1)) {
                    n->fn(n->arg, 1);
                }
                func_8000EE20();
                if (n->prev != 0) {
                    n->prev->next = n->next;
                }
                if (n->next != 0) {
                    n->next->prev = n->prev;
                }
                if (n == D_800D33E8) {
                    D_800D33E8 = n->next;
                }
                func_8000EE28();
                n->prev = (Node *)-1;
            }
            n = nx;
        }
        func_8000EE28();
    }
}
