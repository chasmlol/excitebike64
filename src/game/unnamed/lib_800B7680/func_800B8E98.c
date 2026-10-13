#include "ctx.h"

typedef struct Snd {
    s32 unk0;
    struct Snd *next;
    u8 unk8[8];
    s32 id;
    u8 unk14[0x4C - 0x14];
    u32 flags;
} Snd;

extern s32 D_800D33F0;
extern Snd *D_800D33E8;

void func_8000EE20(void);
void func_8000EE28(void);

static inline Snd *findSnd(s32 id) {
    Snd *s;

    func_8000EE20();
    for (s = D_800D33E8; s != NULL; s = s->next) {
        if (s->id == id) {
            func_8000EE28();
            return s;
        }
    }
    func_8000EE28();
    return NULL;
}

void func_800B8E98(s32 id) {
    if (D_800D33F0 != 0) {
        Snd *s = findSnd(id);

        if (s != NULL) {
            s->flags |= 1;
        }
    }
}
