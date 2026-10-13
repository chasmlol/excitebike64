#include "ctx.h"

typedef struct Obj {
    s32 pad0;
    struct Obj *p4;
    struct Obj *p8;
    s16 hC;
    s16 hE;
    u16 flags;
    s16 h12;
    s16 *p14;
} Obj;

typedef struct {
    u8 pad0[0x170];
    s32 mode;
    s32 flags;
    u8 pad178[0xC];
    Obj *obj;
    u8 pad188[0x16AC - 0x188];
    s16 *p16AC;
} Ctx;

typedef struct {
    s32 x, y;
    u32 z;
} V3;

extern Ctx *D_800CEA40;
extern V3 D_800CE210;
extern V3 D_800CE21C;
extern V3 D_800CE228;
extern V3 D_800CE234;
extern s32 D_800CE240;
extern s32 D_800F56A8;

void func_80090670(void)
{
    Obj *obj;
    Obj *o;
    s32 a;
    V3 *p;
    s32 fl;

    obj = D_800CEA40->obj;
    if (obj != NULL && (obj->flags & 4) && (obj->p4 != NULL || obj->p8 != NULL) && D_800CEA40->mode != 6) {
        o = obj->p4;
        if (o == NULL) {
            o = obj->p8;
        }
    } else {
        o = D_800CEA40->obj;
    }
    if (o != NULL && (D_800CEA40->mode == 6 || !(o->flags & 4))) {
        a = o->p14[7];
        a -= o->h12 << 1;
        if (fl = o->flags & 2, p = &D_800CE210, fl) {
            a += 4;
        }
        if (D_800CEA40->flags & 1) {
            a += 4;
        }
        p->z = a & 7;
    } else {
        D_800CE210.z = (D_800CEA40->flags & 1) << 2;
    }
    D_800CE210.z = (D_800CE210.z - (D_800CE21C.z >> 13)) & 7;
    switch (D_800CE210.z) {
    case 5:
        D_800CE210.z = -3;
        break;
    case 6:
        D_800CE210.z = -2;
        break;
    case 7:
        D_800CE210.z = -1;
        break;
    }
    if (D_800CEA40->flags & 2) {
        D_800CE210.z -= 4;
        D_800CEA40->flags ^= 3;
    }
    if (D_800CEA40->flags & 4) {
        D_800CE210.z += 4;
        D_800CEA40->flags ^= 5;
    }
    if (D_800CEA40->obj != NULL) {
        D_800CE210.x = o->hC * 8000 + *(s16 *)((u8 *)o->p14 + ((o->h12 & 1) << 1)) * 4000 - D_800CEA40->p16AC[0] * 4000;
        D_800CE210.y = o->hE * 8000 + *(s16 *)((u8 *)o->p14 + (((o->h12 + 1) & 1) << 1)) * 4000 - D_800CEA40->p16AC[1] * 4000;
    } else {
        D_800CE210.x = -4000;
        D_800CE210.y = -4000;
    }
    if (D_800CE210.x != D_800CE21C.x || D_800CE210.y != D_800CE21C.y || D_800CE210.z != 0) {
        D_800CE240 = 1;
        D_800F56A8 = 0;
        D_800CE234.x = D_800CE210.x - D_800CE21C.x;
        D_800CE234.y = D_800CE210.y - D_800CE21C.y;
        D_800CE234.z = D_800CE210.z << 13;
        D_800CE228.x = D_800CE21C.x << 10;
        D_800CE228.y = D_800CE21C.y << 10;
        D_800CE228.z = D_800CE21C.z << 10;
    }
}
