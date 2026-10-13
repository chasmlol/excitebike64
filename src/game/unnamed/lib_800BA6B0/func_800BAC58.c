#include "ctx.h"

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
} Hdr;

typedef struct {
    s32 data;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    Hdr *hdr;
    s32 id;
} Slot;

extern s32 *D_800D5604;
extern Slot D_800FE8F0[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
Hdr *func_800BF79C(s32 *a0, s32 a1);

s32 func_800BAC58(s32 param)
{
    s32 i;
    Slot *e;

    if (D_800D5604 == NULL) {
        return -1;
    }
    for (i = 0; i < 0x100; i++) {
        if (D_800FE8F0[i].f4 == 0) {
            break;
        }
    }
    if (i == 0x100) {
        return -1;
    }
    e = &D_800FE8F0[i];
    func_8000ED10((u8 *)e, 0, sizeof(Slot));
    e->id = param;
    e->hdr = func_800BF79C(D_800D5604, param);
    e->f4 = e->hdr->f4;
    e->f8 = e->hdr->f8;
    e->fC = e->hdr->fC;
    e->f10 = e->hdr->f10;
    e->data = (s32)(e->hdr + 1);
    return i;
}
