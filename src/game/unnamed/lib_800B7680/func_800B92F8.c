#include "ctx.h"

typedef struct Ent {
    s32 w0;
    struct Ent *next;
    s32 f8;
    s32 w0c;
    s32 key;
    s32 w14;
    s32 w18;
    s32 f1c;
    u8 pad[0x28];
    s32 f48;
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

static inline Ent *find(s32 id)
{
    Ent *e;

    func_8000EE20();
    for (e = D_800D33E8; e != NULL; e = e->next) {
        if (e->key == id) {
            func_8000EE28();
            return e;
        }
    }
    func_8000EE28();
    return NULL;
}

void func_800B92F8(s32 key, s32 v)
{
    Ent *e;

    if (D_800D33F0 != 0) {
        e = find(key);
        if (e != NULL) {
            e->f1c = v;
            e->f48 = (e->f1c << 8) / D_800FA844;
            if (e->f8 != -1) {
                D_800FE860->fn(e->f8, e->f1c);
            }
        }
    }
}
