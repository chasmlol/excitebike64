#include "ctx.h"

typedef struct MenuItem {
    s32 f0;
    s32 f4;
    s32 f8;
    void (*func)(s32);
} MenuItem;

typedef struct Item {
    s32 f0;
    struct Item *child;   /* 0x04 */
    struct Item *next;    /* 0x08 */
    MenuItem *items;      /* 0x0C */
    u16 flags;            /* 0x10 */
    u8 pad12[0x20 - 0x12];
    s32 *sel;             /* 0x20 */
    s32 f24;
    struct Item *parent;  /* 0x28 */
} Item;

typedef struct Game {
    u8 pad0[0x54];
    s32 f54;
    u16 f58;
    s16 f5A;
} Game;

extern Game D_800EFD70;
extern s32 D_800CA920;
extern s32 D_800CCD64;
extern Item *D_800F47C4;
extern s32 D_800F47A0;
extern Item *D_800F47A4;
extern Item *D_800F016C;
extern s32 D_800D11D4;

extern s32 func_8004F180(void);
extern void func_80079E00(Item *p);
extern void func_8004C22C(s32 param_1);

#define CUR(m) ((m)->sel != NULL ? &(m)->items[*(m)->sel] : (m)->items)

void func_8007AF38(Item *p1, s32 p2, s32 p3)
{
    Item *p;

    if (D_800EFD70.f54 != 0) {
        if (D_800F47C4 == NULL) {
            if (p1 != NULL) {
                p = p1;
                do {
                    if (D_800CA920 != 3 && D_800CCD64 == 0) {
                        p->parent = NULL;
                    }
                    func_80079E00(p);
                    p = p->next;
                } while (p != NULL);
                for (p = p1->child; p != NULL; p = p->child) {
                    if (D_800CA920 != 3 && D_800CCD64 == 0) {
                        p->parent = NULL;
                    }
                    func_80079E00(p);
                }
                if (func_8004F180() == 0 && (p1->flags & 0x200) == 0) {
                    func_8004C22C(D_800D11D4);
                }
            }
            D_800EFD70.f54 = 1;
            D_800EFD70.f58 = 1;
            D_800EFD70.f5A = -1;
        } else {
            if (p3 != 0) {
                MenuItem *items = D_800F47C4->items;
                s32 *sel = D_800F47C4->sel;
                if ((sel != NULL ? &items[*sel] : items)->func != NULL) {
                    (sel != NULL ? &items[*sel] : items)->func(1);
                }
            }
            D_800EFD70.f54 = 0;
            D_800EFD70.f58 = 0xFF;
            D_800F47A0 = D_800F47C4->sel != NULL ? *D_800F47C4->sel : 0;
        }
        if (D_800F47C4 != NULL) {
            D_800F47A4 = D_800F47C4;
        }
        if (p1 != NULL && p2 != 0) {
            p1->parent = D_800F47C4;
        }
        D_800F016C = D_800F47A4;
        D_800F47C4 = p1;
    }
}
