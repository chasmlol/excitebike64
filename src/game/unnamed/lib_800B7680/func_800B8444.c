#include "ctx.h"

typedef struct { s32 w[4]; } Info;
typedef struct { s32 f0; Info info; s32 f14; } Slot;
typedef struct {
    u8 pad[0x14];
    s32 (*alloc)(u8 *tb, u32 a, s32 b, s32 c, s32 d);
    void (*free)(s32 id);
    void (*info)(s32 id, Info *out);
} Drv;
typedef struct Voice {
    u8 pad0[4];
    struct Voice *next;
    u8 pad8[0x10 - 8];
    s32 key;
    u8 pad14[0x4C - 0x14];
    u32 flags;
} Voice;
typedef struct {
    u8 pad0[4];
    s32 key;
    s32 id8;
    s32 idC;
    u8 b10;
    u8 b11;
    u8 pad12[0x28 - 0x12];
    s32 w28;
    u32 w2C;
    u8 b30;
    u8 b31;
    u8 b32;
    u8 b33;
} Chan;

extern s32 D_800D33F0;
extern Voice *D_800D33E8;
extern Drv *D_800FE860;
extern Slot D_800FA860[];
extern u8 D_800F8840[];
extern u32 D_800FA850;
extern u32 D_800FA844;

void func_8000EE20(void);
void func_8000EE28(void);
void func_8000ECB8(s32 a);
void func_800B78F0(s32 param_1);

static inline Voice *find(s32 key)
{
    Voice *v;

    func_8000EE20();
    for (v = D_800D33E8; v != 0; v = v->next) {
        if (v->key == key) {
            func_8000EE28();
            return v;
        }
    }
    func_8000EE28();
    return 0;
}

static inline void mark(s32 key)
{
    Voice *v;

    if (D_800D33F0 != 0) {
        v = find(key);
        if (v != 0) {
            v->flags |= 1;
        }
    }
}

static inline s32 alloc(u8 *tb, u32 a, s32 d)
{
    s32 r;
    Slot *s;
    Info buf;

    if (D_800D33F0 == 0) {
        return -1;
    }
    r = D_800FE860->alloc(tb, a, 0, 0, d);
    if (r < 0) {
        return r;
    }
    if (r >= 0x100) {
        D_800FE860->free(r);
        return -1;
    }
    {
        Slot *tb = D_800FA860;
        s = &tb[r];
    }
    s->f0 = 0;
    s->f14 = 1;
    D_800FE860->info(r, &buf);
    s->info.w[0] = buf.w[0];
    s->info.w[1] = buf.w[1];
    s->info.w[2] = buf.w[2];
    s->info.w[3] = buf.w[3];
    return r;
}

void func_800B8444(Chan *p, s32 a1, s32 a2, s32 a3, s32 a4)
{
    Voice *v;
    s32 key;

    if (p->key != -1) {
        mark(p->key);
        p->key = -1;
    }
    if (p->id8 != -1) {
        func_800B78F0(p->id8);
        p->id8 = -1;
    }
    if (p->idC != -1) {
        func_800B78F0(p->idC);
        p->idC = -1;
    }
    p->b33 = 1;
    p->w28 = a1;
    p->b30 = a2;
    p->b31 = a3;
    p->b32 = a4;
    p->w2C = (p->w28 * D_800FA850) / D_800FA844;
    p->id8 = alloc(D_800F8840, p->w2C, p->w28);
    p->idC = alloc(D_800F8840, p->w2C, p->w28);
    if (p->id8 == -1 || p->idC == -1) {
        func_8000ECB8(1);
    } else {
        p->b10 = 0;
    }
    *(volatile u8 *)&p->b11 = 0;
}
