#include "ctx.h"

typedef struct Ent {
    s32 w0;
    struct Ent *next;
    s32 w2;
    s32 w3;
    s32 id;
    s32 w5[4];
    s32 f24;
    s32 w10[4];
    s32 f38;
} Ent;

typedef struct {
    s32 w0;
    s32 id;
    u8 pad[0x28];
    u8 b30;
} Handle;

typedef struct {
    u8 pad[0x30];
    void (*fn)(s32, s32);
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

void func_800B9CA0(Handle *h, s32 v)
{
    Ent *e;
    s32 id;

    h->b30 = v;
    func_8000EE20();
    if (h->id != -1) {
        id = h->id;
        if (D_800D33F0 == 0) {
            goto out;
        }
        e = find(id);
        if (e != NULL) {
            e->f24 = v;
            e->f38 = v << 16;
            if (e->w2 != -1) {
                D_800FE860->fn(e->w2, v);
            }
        }
    }
out:
    func_8000EE28();
}
