#include "ctx.h"

typedef struct Ctl {
    u8 pad0[0xC];
    u32 fC;
} Ctl;

typedef struct Ent {
    s16 dim[2];
    s16 dir[2];
    s16 off[2];
    union {
        u32 w;
        struct {
            u16 hi;
            u16 lo;
        } h;
    } fC;
    u8 pad10[0x40 - 0x10];
} Ent;

typedef struct Node {
    s16 used;
    s16 pad2;
    struct Node *next;
    struct Node *prev;
    s16 x;
    s16 y;
    u16 st;
    s16 idx;
    Ent *pt;
} Node;

typedef struct Edit {
    u8 pad0[0x170];
    s32 mode;
    u32 flags;
    u32 pad178;
    Node *head;
    Node *tail;
    Node *cur;
    Node *f188;
    Node *f18C;
    Node *f190;
    u8 pad194[0x16AC - 0x194];
    s16 *size;
    u8 pad16B0[6];
    s16 scrollx;
    s16 scrolly;
    u8 pad16BA[0x16E0 - 0x16BA];
    Ctl *f16E0;
    u8 pad16E4[0x16FC - 0x16E4];
    s32 f16FC;
    u8 pad1700[0x17B0 - 0x1700];
    Ent *f17B0;
    u8 pad17B4[0x1800 - 0x17B4];
    s16 f1800;
} Edit;

typedef struct Sel {
    u8 pad0[0xA];
    s16 hA;
    u8 padC[4];
} Sel;

typedef struct Menu {
    u8 pad0[6];
    s16 h6;
    Sel *p8;
    u8 padC[4];
    s16 h10;
} Menu;

extern Edit *D_800CEA40;
extern u32 D_800E30B0;
extern u32 D_800E25B0;
extern Menu D_800CEB9C;
extern Ent D_800CE280[];
extern Ent D_800CE300[];
extern Ent D_800CE400[];
extern Ent D_800CE700[];

void func_8009EBA8();
void func_8001102C(s32 param_1);
void func_8009F260(void);
void func_8008D3B4(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
void func_80094638(void);
void func_8009B820(void);
void func_80094738(Node *n);
s32 func_8008F878(Ctl *c);
s32 func_8009B9A8(Node *n);
void func_80096B3C(Node *n);
void func_8009E584(void);
Node *func_8009AB24(Node *a, Node *b, Ent *pt);

#define PRESSED(m) ((D_800E30B0 & (m)) && !(D_800E25B0 & (m)))
#define IDX (D_800CEB9C.p8[D_800CEB9C.h10].hA)

#define INSERT(T)                                                                          \
    if (D_800CEA40->cur == NULL) {                                                         \
        D_800CEA40->cur = func_8009AB24(NULL, NULL, &T[IDX]);                              \
        D_800CEA40->head = D_800CEA40->cur;                                                \
        D_800CEA40->tail = D_800CEA40->cur;                                                \
        D_800CEA40->f190 = D_800CEA40->cur;                                                \
    } else if (D_800CEA40->cur->prev != NULL ||                                            \
               ((D_800CEA40->flags & 0x200) && D_800CEA40->cur->next == NULL)) {          \
        D_800CEA40->cur = func_8009AB24(D_800CEA40->cur, D_800CEA40->cur->next, &T[IDX]);  \
        D_800CEA40->tail = D_800CEA40->cur;                                                \
    } else {                                                                               \
        D_800CEA40->cur = func_8009AB24(NULL, D_800CEA40->cur, &T[IDX]);                   \
        D_800CEA40->head = D_800CEA40->cur;                                                \
    }

void func_800938BC(void) {
    Node *cur;
    Node *c;
    Node *p;
    Node *n;
    s32 i;
    s32 j;
    s32 ok;

    c = D_800CEA40->cur;
    p = D_800CEA40->f188;
    if (c != NULL && c->prev == NULL && c->next == NULL) {
        if ((PRESSED(0x2000) && !(D_800CEA40->flags & 1)) || (PRESSED(0x4000) && (D_800CEA40->flags & 1))) {
            if (D_800CEA40->scrollx >= -30) {
                D_800CEA40->scrollx--;
            }
        }
        if ((PRESSED(0x4000) && !(D_800CEA40->flags & 1)) || (PRESSED(0x2000) && (D_800CEA40->flags & 1))) {
            if (D_800CEA40->scrollx < 31) {
                D_800CEA40->scrollx++;
            }
        }
        if ((PRESSED(0x1000) && !(D_800CEA40->flags & 1)) || (PRESSED(0x800) && (D_800CEA40->flags & 1))) {
            if (D_800CEA40->scrolly >= -30) {
                D_800CEA40->scrolly--;
            }
        }
        if ((PRESSED(0x800) && !(D_800CEA40->flags & 1)) || (PRESSED(0x1000) && (D_800CEA40->flags & 1))) {
            if (D_800CEA40->scrolly < 31) {
                D_800CEA40->scrolly++;
            }
        }
    }
    if (p != NULL) {
        do {
            p = p->next;
        } while (p->prev != D_800CEA40->f18C);
    }
    if (PRESSED(0x10000)) {
        if ((D_800CEA40->cur->pt->fC.w & 0x300000) == 0x300000) {
            D_800E30B0 &= 0x10000;
            func_8001102C(0);
            for (i = 0; i < D_800CEA40->cur->pt->dim[(D_800CEA40->cur->idx + 1) & 1]; i++) {
                for (j = 0; j < D_800CEA40->cur->pt->dim[D_800CEA40->cur->idx & 1]; j++) {
                    if (D_800CEA40->cur->x + j >= 0 && D_800CEA40->cur->x + j < D_800CEA40->size[0] &&
                        D_800CEA40->cur->y + i >= 0 && D_800CEA40->cur->y + i < D_800CEA40->size[1]) {
                        func_8008D3B4(D_800CEA40->cur->x + j, D_800CEA40->cur->y + i, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
            D_800CEA40->flags ^= 8;
            if (D_800CEA40->flags & 8) {
                D_800CEA40->cur->st |= 2;
            } else {
                D_800CEA40->cur->st &= ~2;
            }
        }
        p = D_800CEA40->head;
        if (p != NULL) {
            do {
                func_80096B3C(p);
                p = p->next;
            } while (p != NULL && p != D_800CEA40->head);
        }
    } else if (PRESSED(0x20000)) {
        if (D_800CEA40->cur != NULL && (D_800CEA40->cur->pt->fC.h.hi & 0x40)) {
            D_800CEA40->f16E0->fC &= ~1;
            func_8009F260();
            func_8009EBA8(0);
            if (D_800CEA40->cur->pt->fC.h.hi & 0x400) {
                D_800CEA40->mode = 10;
            } else if (D_800CEA40->cur->pt->fC.h.hi & 0x200) {
                D_800CEA40->mode = 11;
            } else {
                D_800CEA40->mode = 6;
            }
        }
    }
    n = D_800CEA40->cur;
    if (n->st & 4) {
        if (n->next == NULL) {
            n = n->prev;
        } else {
            n = n->next;
        }
    }
    if (n != NULL) {
        func_8009B9A8(n);
    } else {
        func_8009B820();
    }
    if (D_800CEA40->mode == 6) {
        func_8009E584();
        D_800CEA40->flags |= 0x200;
    } else if (func_8008F878(D_800CEA40->f16E0)) {
        ok = 0;
        switch (D_800CEB9C.h10) {
        case 3:
            func_80094638();
            D_800CEA40->f16FC = IDX;
            INSERT(D_800CE700);
            ok = 1;
            break;
        case 0:
            func_80094638();
            INSERT(D_800CE280);
            ok = 1;
            break;
        case 1:
            func_80094638();
            INSERT(D_800CE300);
            ok = 1;
            break;
        case 2:
            func_80094638();
            INSERT(D_800CE400);
            ok = 1;
            break;
        }
        D_800CEA40->flags &= ~0x200;
        if (ok) {
            D_800CEA40->cur->st &= ~4;
            if (func_8009B9A8(D_800CEA40->cur)) {
                cur = D_800CEA40->cur;
                if (cur->prev == NULL) {
                    if (cur->next == NULL) {
                        D_800CEA40->cur = func_8009AB24(cur, NULL, D_800CEA40->f17B0);
                        D_800CEA40->cur->st |= 4;
                        D_800CEA40->tail = D_800CEA40->cur;
                        D_800CEA40->flags |= 0x200;
                    } else {
                        D_800CEA40->cur = func_8009AB24(NULL, cur, D_800CEA40->f17B0);
                        D_800CEA40->cur->st |= 4;
                        D_800CEA40->head = D_800CEA40->cur;
                    }
                } else {
                    D_800CEA40->cur = func_8009AB24(cur, NULL, D_800CEA40->f17B0);
                    D_800CEA40->cur->st |= 4;
                    D_800CEA40->tail = D_800CEA40->cur;
                }
            } else {
                func_80094738(D_800CEA40->cur);
                D_800CEA40->mode = 0;
                D_800CEA40->flags |= 0x800;
                if (D_800CEA40->head->prev != NULL) {
                    D_800CEA40->f1800 = 2;
                    D_800CEB9C.h10 = 7;
                    D_800CEB9C.h6 = 7;
                } else {
                    D_800CEA40->f1800 = 3;
                }
                D_800CEA40->f16E0->fC &= ~1;
            }
        }
    } else {
        switch (D_800CEB9C.h10) {
        case 3:
            func_80094638();
            D_800CEA40->f16FC = IDX;
            INSERT(D_800CE700);
            D_800CEA40->cur->st |= 4;
            break;
        case 0:
            func_80094638();
            INSERT(D_800CE280);
            D_800CEA40->cur->st |= 4;
            break;
        case 1:
            func_80094638();
            INSERT(D_800CE300);
            D_800CEA40->cur->st |= 4;
            break;
        case 2:
            func_80094638();
            INSERT(D_800CE400);
            D_800CEA40->cur->st |= 4;
            break;
        }
    }
    func_80096B3C(D_800CEA40->cur);
}
