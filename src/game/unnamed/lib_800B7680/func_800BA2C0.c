#include "ctx.h"

typedef struct Node {
    u32 f0;
    struct Node *next;
    u8 pad[0x8];
    s32 key;
} Node;

extern Node *D_800D33E8;

void func_8000EE20(void);
void func_8000EE28(void);

s32 func_800BA2C0(s32 param_1)
{
    Node *p;

    func_8000EE20();
    p = D_800D33E8;
    while (p != NULL) {
        if (p->key == param_1) {
            func_8000EE28();
            return (s32)p;
        }
        p = p->next;
    }
    func_8000EE28();
    return 0;
}
