#include "ctx.h"

typedef struct Ent {
    s32 w0;
    struct Ent *next;
    s32 w2;
    s32 w3;
    s32 id;
    s32 w5[7];
    s32 flags;
    s32 w13[4];
    u32 val;
} Ent;

typedef struct {
    u8 pad[0x3C];
    u32 (*fn)(s32);
} Ops;

extern Ent *D_800D33E8;
extern s32 D_800D33F0;
extern Ops *D_800FE860;

void func_8000EE20(void);
void func_8000EE28(void);

static inline Ent *find(s32 id)
{
    Ent *e;

    func_8000EE20();
    for (e = D_800D33E8; e != NULL; e = e->next) {
        if (e->id == id) {
            func_8000EE28();
            return e;
        }
    }
    func_8000EE28();
    return NULL;
}

u32 func_800BA330(s32 p)
{
    Ent *e;

    if (D_800D33F0 == 0) {
        return -1;
    }
    e = find(p);
    if (!(e->flags & 1)) {
        return -1;
    }
    if (e->w2 == -1) {
        return e->val >> 8;
    }
    return D_800FE860->fn(e->w2);
}
