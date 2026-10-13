#include "ctx.h"

typedef struct Node {
    s32 unk0;
    struct Node *next;
    s32 unk8;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    u8 *ref;
} Node;

typedef struct {
    u8 pad[0x17C];
    Node *listA;
    u8 pad180[0x190 - 0x180];
    Node *listB;
    u8 pad194[0x16F4 - 0x194];
    s32 nItems;
    u8 pad16F8[0x17F8 - 0x16F8];
    s32 val;
} Game;

typedef struct {
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    s16 idx;
    u16 pad;
} OutRec;

typedef struct {
    s32 cnt;
    s32 n;
    s32 val;
    u8 len[8];
    u8 names[8][30];
    OutRec recs[1];
} Out;

typedef struct {
    u8 unk0[3];
    u8 len;
    u8 pad[60];
} Tbl;

extern Game *D_800CEA40;
extern Out *D_800C9570;
extern Tbl D_800CE280[];
extern u8 D_800F55B8[][30];
extern char D_800DB040[];

void func_800BCB04(void *p);
Out *func_800BC908(s32 size, s32 z, void *file, s32 line);
void func_800AEC34(void *d, void *s, u32 n);

void func_800A4A20(void)
{
    Node *n;
    Game *g;
    OutRec *r;
    Out *o;
    s16 count;
    s32 cnt;
    s32 f;
    s32 k;
    s16 i;
    s32 t;
    u16 d;

    count = 0;
    n = D_800CEA40->listA;
    do {
        n = n->next;
        count++;
    } while (n != NULL && n != D_800CEA40->listA);
    if (D_800C9570 != NULL) {
        func_800BCB04(D_800C9570);
    }
    cnt = (s16)count;
    o = func_800BC908((cnt - 1) * 12 + 0x110, 0, D_800DB040, 0x1AE);
    f = D_800CEA40->val;
    D_800C9570 = o;
    o->cnt = cnt;
    o->val = f;
    k = D_800CEA40->nItems;
    i = 0;
    o->n = k;
    if (k > 0) {
        do {
            D_800C9570->len[i] = D_800CE280[i + 0x12].len;
            func_800AEC34((u8 *)(i * 30 + (s32)D_800C9570) + 0x14, D_800F55B8[i], D_800C9570->len[i] * 4 - 2);
            i++;
        } while (i < D_800C9570->n);
    }
    n = D_800CEA40->listB;
    k = 0;
    do {
        t = (s32)n->ref;
        D_800C9570->recs[k].a = n->a;
        D_800C9570->recs[k].b = n->b;
        D_800C9570->recs[k].c = n->c;
        d = n->d;
        D_800C9570->recs[k].idx = (t - (s32)D_800CE280) >> 6;
        D_800C9570->recs[k].d = d;
        n = n->next;
        k++;
        if (n == NULL) {
            n = D_800CEA40->listA;
            if (n == NULL) {
                return;
            }
        }
    } while (n != D_800CEA40->listB);
}
