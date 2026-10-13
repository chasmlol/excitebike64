#include "ctx.h"

typedef struct Ctl {
    u8 pad0[0xC];
    u32 fC;
    s16 h10;
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
    u8 pad18[8];
    s16 w;
    s16 h;
} Node;

typedef struct Edit {
    u8 pad0[0x16C];
    s32 f16C;
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
    s16 f16B0;
    s16 f16B2;
    u8 pad16B4[0x16E0 - 0x16B4];
    Ctl *f16E0;
    Ctl *f16E4;
    u32 f16E8;
    u8 pad16EC[0x17B0 - 0x16EC];
    Ent *f17B0;
} Edit;

typedef struct Item {
    s32 f0;
    u32 flags;
    u8 pad8[8];
} Item;

extern Edit *D_800CEA40;
extern u32 D_800E30B0;
extern u32 D_800E25B0[];
extern Ctl D_800CEB08;
extern Item D_800CEB1C[];
extern Ctl D_800CEB9C;

void func_8001102C(s32 param_1);
void func_80090398(Ctl *c);
void func_8008D3B4(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
void func_80094638();
void func_800938BC(void);
void func_8009B820(void);
void func_8009AF18(Node *p);
s32 func_8008F878(Ctl *c);
s32 func_8009B9A8(Node *n);
void func_80096B3C(Node *n);
Node *func_8009AB24(Node *a, Node *b, Ent *pt);

#define PRESSED(m) ((D_800E30B0 & (m)) && !(D_800E25B0[0] & (m)))

static inline s32 chk(Edit *e, Node *c) {
    Node *n;
    s32 r;

    if (c == NULL || c != e->f190 || (c->prev == NULL && c->next == NULL)) {
        return 1;
    }
    r = 0;
    n = e->head;
    while (n != NULL) {
        if (n != c && (n->pt->fC.h.hi & 2)) {
            r = 1;
            break;
        }
        n = n->next;
        if (n == e->head) {
            break;
        }
    }
    return r;
}

static inline s32 chkg(void) {
    Node *n;
    s32 r;

    if (D_800CEA40->cur == NULL || D_800CEA40->cur != D_800CEA40->f190) {
        return 1;
    }
    if (D_800CEA40->cur->prev != NULL || D_800CEA40->cur->next != NULL) {
        r = 0;
        n = D_800CEA40->head;
        while (n != NULL) {
            if (n != D_800CEA40->cur && (n->pt->fC.h.hi & 2)) {
                r = 1;
                break;
            }
            n = n->next;
            if (n == D_800CEA40->head) {
                break;
            }
        }
        return r;
    }
    return 1;
}

static inline void sel(void) {
    Node *n;

    if (D_800CEA40->f190 == NULL) {
        n = D_800CEA40->head;
        while (n != NULL) {
            if (n->pt->fC.h.hi & 2) {
                D_800CEA40->f190 = n;
                break;
            }
            n = n->next;
            if (n == D_800CEA40->head) {
                break;
            }
        }
    }
}

#define DEL()                                                                          if (D_800CEA40->cur != NULL) {                                                         if (D_800CEA40->cur->prev == NULL) {                                                   n = D_800CEA40->cur;                                                               D_800CEA40->cur = n->next;                                                         func_8009AF18(n);                                                                  sel();                                                                         } else if (D_800CEA40->cur->next == NULL || D_800CEA40->head->prev == D_800CEA40->tail) {             n = D_800CEA40->cur;                                                               D_800CEA40->cur = n->prev;                                                         func_8009AF18(n);                                                                  sel();                                                                         }                                                                              }

void func_80092908(void) {
    s16 minx, miny, maxx, maxy;
    s16 d[2];
    Node *n;
    s32 ok;
    s32 i, j;
    Edit *e;
    Node *c;
    Node *s;

    minx = 1000;
    maxx = -1000;
    miny = 1000;
    maxy = -1000;
    D_800CEA40->flags &= ~0x1E0;
    if (D_800CEA40->cur != NULL) {
        n = D_800CEA40->head;
        do {
            if (n->x < minx) {
                minx = n->x;
            }
            if (n->y < miny) {
                miny = n->y;
            }
            if (n->x + n->w > maxx) {
                maxx = n->x + n->w;
            }
            if (n->y + n->h > maxy) {
                maxy = n->y + n->h;
            }
            n = n->next;
        } while (n != NULL && n != D_800CEA40->head);
        if (minx > 0) {
            D_800CEA40->flags |= 0x20;
        }
        if (miny > 0) {
            D_800CEA40->flags |= 0x100;
        }
        if (maxx < D_800CEA40->size[0]) {
            D_800CEA40->flags |= 0x40;
        }
        if (maxy < D_800CEA40->size[1]) {
            D_800CEA40->flags |= 0x80;
        }
    }
    if (D_800CEA40->cur != NULL && (D_800CEA40->cur->st & 4)) {
        D_800CEB1C[6].flags &= ~0x10;
        D_800CEB1C[4].flags &= ~0x10;
        D_800CEB1C[7].flags &= ~0x10;
        D_800CEB1C[5].flags &= ~0x10;
        D_800CEA40->flags &= ~0x8000;
    } else {
        D_800CEB1C[7].flags |= 0x10;
        if (D_800CEA40->cur == NULL ||
            (D_800CEA40->cur->next != NULL && D_800CEA40->cur->prev != NULL && D_800CEA40->head->prev == NULL) ||
            (e = D_800CEA40, c = e->cur, !chk(e, c))) {
            D_800CEB1C[6].flags &= ~0x10;
            D_800CEA40->flags &= ~0x8000;
        } else {
            D_800CEA40->flags |= 0x8000;
            D_800CEB1C[6].flags |= 0x10;
        }
        if (D_800CEA40->flags & 0x1E0) {
            D_800CEB1C[4].flags |= 0x10;
        } else {
            D_800CEB1C[4].flags &= ~0x10;
        }
    }
    if (D_800CEA40->f16E0 != NULL) {
        func_80090398(D_800CEA40->f16E0);
    }
    if (D_800CEA40->flags & 0x800) {
        if (PRESSED(0x10)) {
            D_800E25B0[0] |= 0x10;
            D_800CEA40->flags &= ~0x800;
            D_800CEA40->f16E0->fC |= 1;
        }
    } else {
        if (D_800CEA40->mode == 10 || D_800CEA40->mode == 11) {
            if (PRESSED(0x10)) {
                D_800CEA40->mode = 6;
            } else if (PRESSED(0x20)) {
                D_800CEA40->f16E0->fC |= 1;
                D_800CEA40->mode = 9;
                D_800CEA40->flags &= ~0x1000;
            }
        } else if (D_800CEA40->mode == 9 || D_800CEA40->mode == 6) {
            if (PRESSED(0x20)) {
                if (D_800CEA40->mode == 9) {
                    func_80094638(D_800CEA40->cur);
                    D_800CEA40->mode = 0;
                    D_800CEA40->flags &= ~0x200;
                }
            } else {
                func_800938BC();
            }
        } else if (D_800CEA40->mode == 4) {
            ok = 0;
            if (PRESSED(0x20)) {
                d[0] = D_800CEA40->head->x - D_800CEA40->f16B0;
                d[1] = D_800CEA40->head->y - D_800CEA40->f16B2;
                n = D_800CEA40->head;
                do {
                    n->x -= d[0];
                    n->y -= d[1];
                    n = n->next;
                } while (n != NULL && n != D_800CEA40->head);
                D_800CEA40->mode = 0;
                D_800CEA40->f16E0->fC |= 1;
                ok = 1;
                func_8001102C(2);
            } else if (PRESSED(0x10)) {
                D_800CEA40->mode = 0;
                D_800CEA40->f16E0->fC |= 1;
                func_8001102C(1);
            } else if ((D_800CEA40->flags & 0x20) && PRESSED(0x2000)) {
                n = D_800CEA40->head;
                do {
                    n->x--;
                    n = n->next;
                } while (n != NULL && n != D_800CEA40->head);
                func_8001102C(0);
                ok = 1;
            } else if ((D_800CEA40->flags & 0x40) && PRESSED(0x4000)) {
                n = D_800CEA40->head;
                do {
                    n->x++;
                    n = n->next;
                } while (n != NULL && n != D_800CEA40->head);
                func_8001102C(0);
                ok = 1;
            } else if ((D_800CEA40->flags & 0x80) && PRESSED(0x800)) {
                n = D_800CEA40->head;
                do {
                    n->y++;
                    n = n->next;
                } while (n != NULL && n != D_800CEA40->head);
                func_8001102C(0);
                ok = 1;
            } else if ((D_800CEA40->flags & 0x100) && PRESSED(0x1000)) {
                n = D_800CEA40->head;
                do {
                    n->y--;
                    n = n->next;
                } while (n != NULL && n != D_800CEA40->head);
                func_8001102C(0);
                ok = 1;
            }
            if (ok) {
                n = D_800CEA40->head;
                do {
                    func_80096B3C(D_800CEA40->head);
                    n = n->next;
                } while (n != NULL && n != D_800CEA40->head);
                for (i = 0; i < D_800CEA40->size[1]; i++) {
                    for (j = 0; j < D_800CEA40->size[0]; j++) {
                        func_8008D3B4(j, i, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        } else if (func_8008F878(D_800CEA40->f16E0)) {
            switch (D_800CEB9C.h10) {
            case 0:
            case 1:
            case 2:
            case 3:
                if (D_800CEA40->cur == NULL) {
                    D_800CEA40->cur = func_8009AB24(NULL, NULL, D_800CEA40->f17B0);
                    D_800CEA40->cur->st |= 4;
                    D_800CEA40->mode = 9;
                    D_800CEA40->head = D_800CEA40->cur;
                    D_800CEA40->tail = D_800CEA40->cur;
                } else if (D_800CEA40->cur->next == NULL) {
                    if (D_800CEA40->cur->prev == NULL) {
                        D_800CEA40->cur = func_8009AB24(D_800CEA40->cur, NULL, D_800CEA40->f17B0);
                        D_800CEA40->cur->st |= 4;
                        D_800CEA40->mode = 0;
                        D_800CEA40->tail = D_800CEA40->cur;
                        D_800CEA40->f16E0->fC |= 1;
                        D_800CEA40->mode = 9;
                        D_800CEA40->flags |= 0x200;
                    } else {
                        D_800CEA40->cur = func_8009AB24(D_800CEA40->cur, NULL, D_800CEA40->f17B0);
                        D_800CEA40->cur->st |= 4;
                        D_800CEA40->mode = 9;
                        D_800CEA40->tail = D_800CEA40->cur;
                    }
                } else if (D_800CEA40->cur->prev == NULL) {
                    D_800CEA40->cur = func_8009AB24(NULL, D_800CEA40->cur, D_800CEA40->f17B0);
                    D_800CEA40->cur->st |= 4;
                    D_800CEA40->mode = 9;
                    D_800CEA40->head = D_800CEA40->cur;
                }
                break;
            case 5:
                if (D_800CEA40->cur != NULL) {
                    D_800CEA40->f190 = D_800CEA40->cur;
                }
                break;
            case 4:
                if (D_800CEA40->cur != NULL) {
                    D_800CEA40->f16B0 = D_800CEA40->head->x;
                    D_800CEA40->f16B2 = D_800CEA40->head->y;
                    D_800CEA40->mode = 4;
                    D_800CEA40->f16E0->fC &= ~1;
                }
                break;
            case 7:
                func_8001102C(2);
                if (D_800CEA40->f16E0 != &D_800CEB08) {
                    switch (D_800CEA40->f16E8) {
                    case 0:
                        D_800CEA40->f16E8 = 1;
                        D_800CEA40->f16E4 = &D_800CEB08;
                        break;
                    case 1:
                        D_800CEA40->f16E0 = &D_800CEB08;
                        break;
                    case 2:
                        D_800CEA40->f16E8 = 3;
                        D_800CEA40->f16E4 = &D_800CEB08;
                        break;
                    case 3:
                        D_800CEA40->f16E8 = 1;
                        D_800CEA40->f16E0 = &D_800CEB08;
                        break;
                    }
                    D_800CEA40->flags &= ~0x10000;
                }
                D_800CEA40->f16C = 0;
                break;
            case 6:
                if (D_800CEA40->cur != NULL && chkg()) {
                    DEL();
                }
                break;
            }
        } else if (PRESSED(0x20)) {
            e = D_800CEA40;
            if (e->flags & 0x8000) {
                c = e->cur;
                if (chk(e, c)) {
                    DEL();
                }
            }
        }
        if (D_800CEA40->cur != NULL) {
            s = D_800CEA40->cur;
            if (s->st & 4) {
                if (s->next == NULL) {
                    s = s->prev;
                } else {
                    s = s->next;
                }
            }
            if (s != NULL) {
                func_8009B9A8(s);
            } else {
                func_8009B820();
            }
        }
        if (D_800CEA40->mode == 0 && D_800CEA40->cur != NULL) {
            if ((PRESSED(0x2000) && !(D_800CEA40->flags & 1)) || (PRESSED(0x4000) && (D_800CEA40->flags & 1))) {
                if (D_800CEA40->cur->prev != NULL) {
                    D_800CEA40->cur = D_800CEA40->cur->prev;
                }
            } else if ((PRESSED(0x4000) && !(D_800CEA40->flags & 1)) ||
                       (PRESSED(0x2000) && (D_800CEA40->flags & 1))) {
                if (D_800CEA40->cur->next != NULL) {
                    D_800CEA40->cur = D_800CEA40->cur->next;
                }
            }
        }
    }
    if (D_800CEA40->cur != NULL) {
        if (D_800CEA40->cur->st & 2) {
            D_800CEA40->flags |= 8;
        } else {
            D_800CEA40->flags &= ~8;
        }
    }
}
