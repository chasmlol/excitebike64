#include "ctx.h"

typedef struct Node {
    u32 f0;
    struct Node *next;
    u8 pad[0x44];
    u32 flags;
} Node;

extern Node *D_800D33E8;

void func_8000EE20(void);
void func_8000EE28(void);

void func_800B8F2C(void)
{
    Node *p;

    func_8000EE20();
    p = D_800D33E8;
    if (p != NULL) {
        do {
            p->flags |= 1;
            p = p->next;
        } while (p != NULL);
    }
    func_8000EE28();
}
