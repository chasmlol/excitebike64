#include "ctx.h"

typedef struct { s16 key; s16 id; u8 pad[12]; } Ent;
typedef struct { s32 count; Ent e[1]; } Tbl;
typedef struct {
    u8 pad[0x628];
    s16 f628;
    u8 pad2[0x660 - 0x62A];
    s16 f660;
    s16 f662;
    u8 pad3[0x7E0 - 0x664];
} Player;

extern s32 D_800EFDBC;
extern Tbl *D_800F0CA8;
extern Player D_800E6700[2];

void func_80062F50(Ent *e);
void func_8006333C(Player *p, Player *q);

void func_80063488(Player *p)
{
    Player *q;

    if (D_800EFDBC == 8) return;
    if (D_800F0CA8 != NULL && p->f662 < 2) {
        Ent *e = D_800F0CA8->e;
        while (e->key < p->f660) e++;
        while (e->key == p->f660) {
            if (e >= &D_800F0CA8->e[D_800F0CA8->count]) break;
            if (e->id == p->f628) func_80062F50(e);
            e++;
        }
    }
    for (q = D_800E6700; q < &D_800E6700[2]; q++) func_8006333C(p, q);
}

