#include "ctx.h"

typedef struct Node {
    s32 f0;
    struct Node *a;     /* 0x04 */
    struct Node *b;     /* 0x08 */
    s16 x;              /* 0x0C */
    s16 y;              /* 0x0E */
    u8 pad10[8];
    s16 dirA;           /* 0x18 */
    s16 dirB;           /* 0x1A */
    s16 f1C;            /* 0x1C */
    s16 f1E;            /* 0x1E */
    s16 w;              /* 0x20 */
    s16 h;              /* 0x22 */
} Node;

typedef struct Game {
    u8 pad0[0x17C];
    Node *first;        /* 0x17C */
    Node *last;         /* 0x180 */
} Game;

extern Game *D_800CEA40;

void func_80094738(Node *n)
{
    Node *c;

    for (c = D_800CEA40->first; c != NULL; c = c->a) {
        if (c->b == D_800CEA40->last) {
            return;
        }
        if (c->a != NULL && c->b != NULL) {
            continue;
        }
        if (c == n) {
            continue;
        }
        if (c->b == NULL) {
            if (n->a == NULL && ((n->dirB - c->dirA) & 3) == 2) {
                switch (n->dirB) {
                case 0:
                    if (n->y + n->h == c->y && n->x + n->f1E == c->x + c->f1C) {
                        n->a = c;
                        c->b = n;
                        return;
                    }
                    break;
                case 2:
                    if (c->y + c->h == n->y && n->x + n->f1E == c->x + c->f1C) {
                        n->a = c;
                        c->b = n;
                        return;
                    }
                    break;
                case 1:
                    if (n->x + n->w == c->x && n->y + n->f1E == c->y + c->f1C) {
                        n->a = c;
                        c->b = n;
                        return;
                    }
                    break;
                case 3:
                    if (n->x == c->x + c->w && n->y + n->f1E == c->y + c->f1C) {
                        n->a = c;
                        c->b = n;
                        return;
                    }
                    break;
                }
            }
        } else {
            if (n->b == NULL && ((n->dirA - c->dirB) & 3) == 2) {
                switch (n->dirA) {
                case 0:
                    if (n->y + n->h == c->y && n->x + n->f1C == c->x + c->f1E) {
                        n->b = c;
                        c->a = n;
                        return;
                    }
                    break;
                case 2:
                    if (c->y + c->h == n->y && n->x + n->f1C == c->x + c->f1E) {
                        n->b = c;
                        c->a = n;
                        return;
                    }
                    break;
                case 1:
                    if (n->x + n->w == c->x && n->y + n->f1C == c->y + c->f1E) {
                        n->b = c;
                        c->a = n;
                        return;
                    }
                    break;
                case 3:
                    if (n->x == c->x + c->w && n->y + n->f1C == c->y + c->f1E) {
                        n->b = c;
                        c->a = n;
                        return;
                    }
                    break;
                }
            }
        }
    }
}
