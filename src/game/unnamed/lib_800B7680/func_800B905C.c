#include "ctx.h"

typedef struct Ent {
    s32 w0;
    struct Ent *next;
    s32 id;
    s32 w3[4];
    s32 cur;
    s32 val;
    s32 w9[9];
    s32 res;
} Ent;

typedef struct {
    u8 pad[0x2C];
    void (*fn)(s32, s32);
} Ops;

extern s32 D_800D33F0;
extern Ent *D_800D33E8;
extern s32 D_800FA844;
extern Ops *D_800FE860;

void func_8000EE20(void);
void func_8000EE28(void);

void func_800B905C(void)
{
    Ent *e;

    if (D_800D33F0 != 0) {
        func_8000EE20();
        for (e = D_800D33E8; e != NULL; e = e->next) {
            if (e->cur == 0) {
                e->cur = e->val;
                e->res = (e->cur << 8) / D_800FA844;
                if (e->id != -1) {
                    D_800FE860->fn(e->id, e->cur);
                }
            }
        }
        func_8000EE28();
    }
}
