#include "ctx.h"

typedef struct RecA {
    u8 pad0[0x14];
    s16 i0;
    s16 i1;
    u32 flags;
} RecA;

typedef struct RecB {
    u8 pad0[0x18];
    u32 flags;
    s32 i0;
    s32 i1;
} RecB;

typedef struct Obj {
    u8 pad0[0x74];
    s16 nA;
    s16 nB;
    RecA *la;
    RecB *lb;
} Obj;

typedef struct Flag {
    u16 f0;
    s16 h2;
    void *p4;
} Flag;

typedef struct G {
    u8 pad0[0x3C];
    s32 f3C;
} G;

typedef struct Big {
    u8 pad0[0x628];
    s16 f628;
    u8 pad1[0x7E0 - 0x62A];
} Big;

typedef struct Node {
    u8 pad0[0x4E];
    s16 next;
    u8 pad1[0x6C - 0x50];
} Node;

extern s32 D_800C9684;
extern s32 D_800C97E4;
extern G D_800F05A8;
extern Flag D_800E7FC8[];
extern s16 *D_800E80C0;
extern Node *D_800EE498;
extern Obj *D_800EFD64;
extern Obj *objB_800EFD64;
extern Big D_800E37C0[];
extern s32 D_800E78B0;

s32 func_8002B870(void)
{
    s32 pass = 0;
    s32 cur;
    s32 k;
    s32 a, b, v;
    s32 d;
    s32 off;
    s16 *pv;
    Obj *P;
    u16 *pf;
    Flag *fl;
    G *g;
    s16 *tab;
    void *rec;
    s32 a2, b2;
    void *rec2;

    if (D_800C9684 == 0) {
        D_800C97E4 = -1;
    }
    g = &D_800F05A8;
    fl = D_800E7FC8;
    P = D_800EFD64;
    tab = D_800E80C0;
    cur = D_800E37C0[D_800E78B0].f628;
    do {
        if (cur < 0) {
            goto done;
        }
        if (P->la != NULL && (k = 0, P->nA > 0)) {
            pv = (s16 *)(cur * 2 + (s32)tab);
            pf = (u16 *)(D_800C9684 * 8 + (s32)fl);
            off = k;
        loopA:
            {
                if (pass == 0 || ((*(RecA *)(off + (s32)P->la)).flags & 0x40)) {
                    b = tab[(*(RecA *)(off + (s32)P->la)).i1];
                    a = tab[(*(RecA *)(off + (s32)P->la)).i0];
                    if ((b < a && (*pv >= a || *pv < b)) ||
                        (a < b && *pv >= a && *pv < b)) {
                        if (g->f3C != 0 || !((*(RecA *)(off + (s32)P->la)).flags & 2)) {
                            ((s16 *)pf)[1] = 1;
                            rec = (RecA *)((u8 *)P->la + off);
                            v = rec != *(void **)(pf + 2);
                            *(void **)(pf + 2) = rec;
                            if (v || !(*pf & 2)) {
                                *pf |= 4;
                            }
                            if (pass > 0) {
                                *pf |= 8;
                            } else {
                                *pf &= ~8;
                            }
                            return 1;
                        }
                    }
                }
                if (pv == (s16 *)tab) v = 1; else v = 2;
                off += sizeof(RecA);
                k++;
            }
            if (k < D_800EFD64->nA) goto loopA;
        }
        if ((void *)g == (void *)P) k = 1; else k = 2;
        if (pass == 0) k = 1; else k = 2;
        if (tab == 0) k = 1; else k = 2;
        if (P->lb != NULL && (k = 0, P->nB > 0)) {
            pv = (s16 *)(cur * 2 + (s32)tab);
            d = D_800C9684;
            pf = (u16 *)(d * 8 + (s32)fl);
            off = k;
        loopB:
            {
                if (pass == 0 || ((*(RecB *)(off + (s32)P->lb)).flags & 0x40)) {
                    b2 = tab[(*(RecB *)(off + (s32)P->lb)).i1];
                    a2 = tab[(*(RecB *)(off + (s32)P->lb)).i0];
                    if ((b2 < a2 && (*pv >= a2 || *pv < b2)) ||
                        (a2 < b2 && *pv >= a2 && *pv < b2)) {
                        if (g->f3C != 0 || !((*(RecB *)(off + (s32)P->lb)).flags & 2)) {
                            ((s16 *)pf)[1] = 0;
                            rec2 = (RecB *)((u8 *)P->lb + off);
                            v = rec2 != *(void **)(pf + 2);
                            *(void **)(pf + 2) = rec2;
                            if (v || !(*pf & 2)) {
                                *pf |= 4;
                            }
                            if (pass > 0) {
                                *pf |= 8;
                            } else {
                                *pf &= ~8;
                            }
                            if (d == 0) {
                                D_800C97E4 = k;
                            }
                            return 1;
                        }
                    }
                }
                off += sizeof(RecB);
                k++;
            }
            if (k < objB_800EFD64->nB) goto loopB;
        }
        pass++;
        cur = D_800EE498[cur].next;
    } while (pass < 2);
done:
    return 0;
}
