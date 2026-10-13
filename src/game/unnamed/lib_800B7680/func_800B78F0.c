#include "ctx.h"

typedef struct {
    void *p;
    u8 unk4[0x10];
    s32 cnt;
} Ent;

extern s32 D_800D33F0;
extern Ent D_800FA860[];
extern s32 *D_800FE860;

void func_800BCB04(void *a0);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_800B78F0(s32 param_1)
{
    s32 *w;
    Ent *e;

    if (D_800D33F0 != 0) {
        w = (s32 *)D_800FA860;
        e = (Ent *)&w[param_1 * 6];
        if (e->cnt > 0) {
            e->cnt = e->cnt - 1;
        }
        if (e->cnt == 0) {
            if (e->p != 0) {
                func_800BCB04(e->p);
            }
            func_8000ED10((u8 *)e, 0, 0x18);
            ((void (*)(s32))D_800FE860[6])(param_1);
        }
    }
}
