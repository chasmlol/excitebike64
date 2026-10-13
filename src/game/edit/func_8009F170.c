#include "ctx.h"

typedef struct Sub {
    u8 pad[0xC];
    u16 flags;
} Sub;

typedef struct Node {
    s32 w0;
    struct Node *next;
    s32 w8;
    s32 w12;
    s32 w16;
    Sub *sub;
} Node;

typedef struct Obj {
    u8 p0[0x17C];
    Node *list;
    u8 p1[4];
    Node *cur;
    u8 p2[8];
    Node *other;
} Obj;

extern Obj *D_800CEA40;

s32 func_8009F170(void) {
    Obj *o = D_800CEA40;
    Node *a = o->cur;
    Node *n;
    s32 r;
    if (a == NULL || a != o->other || (a->w8 == 0 && a->next == NULL)) {
        goto ret1;
    }
    n = o->list;
    r = 0;
    while (n != NULL) {
        if (n != a && (n->sub->flags & 2) != 0) {
            r = 1;
            break;
        }
        n = n->next;
        if (n == o->list) {
            break;
        }
    }
    return r;
ret1:
    return 1;
}
