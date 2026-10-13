#include "ctx.h"

typedef struct Node {
    s32 f0;
    struct Node *next;
} Node;

typedef struct Ctx {
    s32 f0;
    Node *head;
    Node *end;
} Ctx;

s32 func_8009EB08(Ctx *c)
{
    Node *n = c->head;
    if (n != NULL && c->end != NULL) {
        do {
            if (n == c->end) {
                return 1;
            }
            n = n->next;
        } while (n != NULL);
        return 0;
    }
    return 1;
}
