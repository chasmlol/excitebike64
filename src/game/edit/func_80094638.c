#include "ctx.h"

typedef struct Node {
    s32 f0;
    struct Node *n4;
    struct Node *n8;
    s32 f0c;
    u16 flags;
} Node;

typedef struct Gs {
    u8 pad[0x17C];
    Node *f17c;
    Node *f180;
    Node *f184;
} Gs;

extern Gs *D_800CEA40;
void func_8009AF18(Node *n);

void func_80094638(void) {
    Node *n;
    Node *cur;
    s32 f;

    n = D_800CEA40->f184;
    while (n != 0 && (n->flags & 4) != 0) {
        n = n->n8;
    }
    f = 1;
    if (n == 0) {
        n = D_800CEA40->f184;
        while (n != 0) {
            if ((n->flags & 4) == 0) {
                break;
            }
            n = n->n4;
        }
        f = 0;
    }
    D_800CEA40->f184 = n;
    if (n == 0) {
        n = D_800CEA40->f17c;
        f = 1;
        D_800CEA40->f17c = 0;
        D_800CEA40->f180 = 0;
    } else if (f != 0) {
        D_800CEA40->f180 = n;
    } else {
        D_800CEA40->f17c = n;
    }
    while (n != 0) {
        cur = n;
        if (f != 0) {
            n = cur->n4;
        } else {
            n = cur->n8;
        }
        if ((cur->flags & 4) != 0) {
            func_8009AF18(cur);
        }
    }
}
