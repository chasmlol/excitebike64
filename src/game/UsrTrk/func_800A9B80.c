#include "ctx.h"

typedef struct Node { u8 pad[4]; struct Node *next; } Node;
typedef struct { u8 pad[0x17C]; Node *head; u8 pad2[0x16F4 - 0x180]; s32 count; } Game;
extern Game *D_800CEA40;
typedef struct { s16 h0; s16 h1; u8 rest[60]; } Row;
extern Row D_800CE280[];

s16 func_800A9B80(void)
{
    s32 cnt = 0;
    Node *n = D_800CEA40->head;
    s32 total;
    s32 i;

    if (n != NULL) {
        do {
            n = n->next;
            cnt++;
        } while (n != NULL && n != D_800CEA40->head);
    }
    total = (s16)cnt * 3 + 8;
    for (i = 0; i < D_800CEA40->count; i++) { total += D_800CE280[i + 18].h1 * 4; }
    return total + 10;
}
