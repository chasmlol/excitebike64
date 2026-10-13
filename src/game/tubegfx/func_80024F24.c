#include "ctx.h"

typedef struct {
    u8 pad[0x1C];
    u32 f1C;
} Item;

typedef struct {
    u32 count;
    Item items[1];
} List;

extern s32 D_800E7770[];
extern s32 D_800E7780[];
extern List *D_800C9640;
extern List *D_800C9644;
extern List *D_800C9648;
extern s32 D_800E30A8;
extern s32 D_800E7820;
extern s32 D_800E77D0;

void func_80027DE0(s32 a, s32 *b, s32 *c);
void func_80026924(List *l, s32 n);

void func_80024F24(s32 n)
{
    u32 i;
    Item *it;

    func_80027DE0(n, &D_800E7770[n], &D_800E7780[n]);
    func_80026924(D_800C9644, n);
    for (i = 0; i < D_800C9640->count; i++) {
        it = (Item *)&((u32 *)D_800C9640)[i * 8 + 1];
        it->f1C = (it->f1C & 0xFFFF0000) | D_800C9644->items[it->f1C >> 16].f1C;
    }
    if (D_800C9648 != NULL) {
        for (i = 0; i < D_800C9648->count; i++) {
            it = (Item *)&((u32 *)D_800C9648)[i * 8 + 1];
            it->f1C = (it->f1C & 0xFFFF0000) | D_800C9644->items[it->f1C >> 16].f1C;
        }
    }
    if (D_800E30A8 != D_800E7820) {
        D_800E77D0 = 0;
    }
    D_800E7820 = D_800E30A8;
}
