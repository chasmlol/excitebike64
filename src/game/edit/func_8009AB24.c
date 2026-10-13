#include "ctx.h"

typedef struct Img {
    s16 dim[2];
    u8 pad4[8];
    u16 flags;
    u16 padE;
    s16 idx;
} Img;

typedef struct Node {
    s16 used;
    s16 pad2;
    struct Node *next;
    struct Node *prev;
    s16 x;
    s16 y;
    s16 st;
    s16 idx;
    s16 *pt;
    u8 pad18[0xC];
} Node;

typedef struct Game {
    u8 pad0[0x174];
    u32 flags;
    u32 pad178;
    Node *head;
    Node *tail;
    u8 pad184[0x10];
    Node nodes[150];
    s16 *size;
    u8 pad16B0[6];
    s16 scrollx;
    s16 scrolly;
} Game;

extern Game *D_800CEA40;
#define AT(p, i) (*(s16 *)((u8 *)(p) + ((i) << 1)))

void func_8009B624(Node *a, Node *b);
void func_8009B3A4(Node *p, Node *c);

Node *func_8009AB24(Node *a, Node *b, s16 *pt)
{
    Node *n;
    s32 i;
    s32 idx;
    Game *g;

    n = D_800CEA40->nodes;
    for (i = 0; i < 150; i++, n++) {
        if (n->used == 0) break;
    }
    g = D_800CEA40;
    n->used = 1;
    if (g->flags & 8) {
        if (((Img *)pt)->flags & 0x20) {
            n->st = 2;
        } else {
            n->st = 0;
            g->flags &= ~8;
        }
    } else if (((Img *)pt)->flags & 0x10) {
        n->st = 0;
    } else {
        n->st = 2;
        g->flags |= 8;
    }
    n->next = b;
    n->prev = a;
    n->pt = pt;
    if (b == NULL && a == NULL) {
        D_800CEA40->head = n;
        D_800CEA40->tail = n;
        idx = ((Img *)pt)->idx;
        if (D_800CEA40->flags & 8) idx += 2;
        n->idx = idx;
        n->x = D_800CEA40->size[0] / 2 - AT(pt, idx & 1) / 2;
        n->y = D_800CEA40->size[1] / 2 - AT(pt, (n->idx + 1) & 1) / 2;
        n->x += D_800CEA40->scrollx;
        n->y += D_800CEA40->scrolly;
        if (n->x < 0) {
            n->x = 0;
            D_800CEA40->scrollx = AT(pt, n->idx & 1) / 2 - D_800CEA40->size[0] / 2;
        }
        if (n->x + AT(n->pt, n->idx & 1) > D_800CEA40->size[0]) {
            n->x = D_800CEA40->size[0] - AT(n->pt, n->idx & 1);
            D_800CEA40->scrollx = n->x - (D_800CEA40->size[0] / 2 - AT(pt, n->idx & 1) / 2);
        }
        if (n->y < 0) {
            n->y = 0;
            D_800CEA40->scrolly = AT(pt, ~n->idx & 1) / 2 - D_800CEA40->size[1] / 2;
        }
        if (n->y + AT(n->pt, ~n->idx & 1) > D_800CEA40->size[1]) {
            n->y = D_800CEA40->size[1] - AT(n->pt, ~n->idx & 1);
            D_800CEA40->scrolly = n->y - (D_800CEA40->size[1] / 2 - AT(pt, ~n->idx & 1) / 2);
        }
    } else {
        if (a != NULL) {
            D_800CEA40->tail = n;
            a->next = n;
            func_8009B3A4(a, n);
        }
        if (b != NULL) {
            D_800CEA40->head = n;
            b->prev = n;
            func_8009B624(b, n);
        }
    }
    return n;
}
