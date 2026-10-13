#include "ctx.h"

s32 func_8000ECE0(s32 a0);
void func_8000EF8C(u8 *a0, u8 *a1);
s32 func_800AEC10(u8 *a0);
u8 *func_800BC908(s32 a0, s32 a1, void *a2, s32 a3);
extern u8 D_800DB160[];

typedef struct {
    u8 *name;
    s32 w[4];
    s32 cnt;
} Ent;

typedef struct {
    void *f0[3];
    s32 (*open)(u8 *);
    void *f10[2];
    void (*close)(s32);
    void (*info)(s32, s32 *);
} Drv;

extern s32 D_800D33F0;
extern Ent D_800FA860[256];
extern Drv *D_800FE860;

static inline s32 cmp(u8 *a, u8 *b)
{
    for (;;) {
        if (func_8000ECE0(*a) != func_8000ECE0(*b)) {
            return 1;
        }
        if (*a++ == 0) {
            return 0;
        }
        b++;
    }
}

s32 func_800B7680(u8 *name)
{
    s32 i, iv, loc[4];
    Ent *tab;
    Ent *e;
    u8 *pc;

    if (D_800D33F0 != 0) {
        for (i = 0; i < 0x100; i++) {
            if (D_800FA860[i].name != NULL && cmp(D_800FA860[i].name, name) == 0) {
                if (D_800FA860[i].cnt >= 0) {
                    D_800FA860[i].cnt++;
                }
                return i;
            }
        }
        iv = D_800FE860->open(name);
        if (iv < 0) {
            return iv;
        }
        if (iv < 0x100) {
            pc = func_800BC908(func_800AEC10(name) + 1, 0, D_800DB160, 0x201);
            tab = D_800FA860;
            e = &tab[iv];
            e->name = pc;
            func_8000EF8C(pc, name);
            D_800FE860->info(iv, loc);
            e->w[0] = loc[0];
            e->w[1] = loc[1];
            e->w[2] = loc[2];
            e->w[3] = loc[3];
            return iv;
        }
        D_800FE860->close(iv);
    }
    return -1;
}
