#include "ctx.h"

typedef struct Obj {
    s32 pad0;
    u16 *src;
} Obj;

typedef struct Rec {
    u8 *buf;
    s32 val;
    Obj *obj;
    u16 *src;
    u8 *dst;
} Rec;

extern s32 D_800F14C0;
extern s32 D_800F14D8;
extern Rec *D_800F14D0;
extern s32 D_800E1B70;
extern u32 D_800C2280;
extern char D_800D8E10[];

extern void func_8000ECB8(s32) __attribute__((noreturn));
void func_8006AA9C(u16 *dst, u8 *src);
void *func_800BC908(s32 size, u32 flags, char *owner, s32 tag);

void func_80069CA0(Obj *a, s32 b)
{
    s32 i;
    s32 k;
    s32 save;

    if (a != NULL && a->src != NULL) {
        if (D_800F14C0 >= D_800F14D8) func_8000ECB8(1);
        D_800F14D0[D_800F14C0].val = b;
        D_800F14D0[D_800F14C0].obj = a;
        D_800F14D0[D_800F14C0].buf = func_800BC908(0x500, 0, D_800D8E10, 0x8C);
        D_800F14D0[D_800F14C0].src = a->src;
        D_800F14D0[D_800F14C0].dst = D_800F14D0[D_800F14C0].buf + 0x300;
        __builtin_memcpy(D_800F14D0[D_800F14C0].dst, D_800F14D0[D_800F14C0].src, 0x200);
        k = 0;
        for (i = 0; i < 0x100; i++) {
            if (D_800C2280 & 0x80) {
                D_800F14D0[D_800F14C0].buf[k++] = (a->src[i] >> 6) & 0x1F;
                D_800F14D0[D_800F14C0].buf[k++] = (a->src[i] >> 1) & 0x1F;
                D_800F14D0[D_800F14C0].buf[k++] = a->src[i] >> 11;
            } else {
                D_800F14D0[D_800F14C0].buf[k++] = a->src[i] >> 11;
                D_800F14D0[D_800F14C0].buf[k++] = (a->src[i] >> 6) & 0x1F;
                D_800F14D0[D_800F14C0].buf[k++] = (a->src[i] >> 1) & 0x1F;
            }
        }
        i = D_800E1B70;
        D_800E1B70 = 0;
        func_8006AA9C(a->src, D_800F14D0[D_800F14C0].buf);
        D_800E1B70 = i;
        __builtin_memcpy(D_800F14D0[D_800F14C0].dst, a->src, 0x200);
        D_800F14C0++;
    }
}
