#include "ctx.h"

extern s32 D_800EFD64;
extern u8 D_800CB210[];
extern s32 D_800E76D0;
extern s32 D_800E1B7C;
extern s32 D_800E1B9C;
extern s32 D_800E1B74;
extern s32 D_800E1B70;
extern s32 D_800E7728;
extern s32 D_800D11E4;
extern s32 D_800D10CC;
extern s32 D_800D0F30;
extern s32 D_800D0E34;
extern u8 D_800D6000[];
typedef struct { u8 pad[0x10]; void *p; } Obj;
typedef struct { s32 *a; s32 *c; Obj *b; Obj *d; } Rec;
typedef struct { s32 *a; Obj *b; } Rec2;
extern Rec *D_800C28C0[];
extern Rec2 D_800C28CC[];

Obj *func_800BF79C(s32 a, s32 b);
Obj *func_800BC908(s32 a, s32 b, u8 *c, s32 d);
void func_8004C22C(s32 a);
void func_80069CA0(Obj *a, s32 b);
extern Obj *D_800E1BC4;
extern Obj *D_800E1B80;
extern Obj *D_800E1BEC;
extern Obj *D_800E1B78;

void func_8000BEA0(void)
{
    s32 i;
    Rec *r;
    Obj *o;

    D_800E76D0 = 0;
    D_800E1B7C = 0;
    D_800E1B9C = 0x8000;
    i = ((D_800EFD64 - (s32)D_800CB210) * -0x33333333) >> 5;
    D_800E1B74 = 0;
    if (i == 9) {
        D_800E1B74 = 0x46;
    } else if (i == 0xe) {
        D_800E1B74 = 100;
        D_800E1B9C = 24000;
    } else if (i == 0x13) {
        D_800E1B74 = 0x14;
        D_800E1B9C = -2000;
    }
    i = 0;
    D_800E1B70 = 0;
    func_8004C22C(D_800D11E4);
    D_800E1BC4 = func_800BF79C(D_800E7728, D_800D10CC);
    D_800E1B80 = func_800BF79C(D_800E7728, D_800D0F30);
    D_800E1BEC = func_800BC908(0x200, i, D_800D6000, 0xFE);
    D_800E1B78 = func_800BF79C(D_800E7728, D_800D0E34);

    for (i = 0; i < 3; i++) {
        for (r = D_800C28C0[i]; r->a != 0; r++) {
            r->b = func_800BF79C(D_800E7728, *r->a);
            r->b->p = (u8 *)r->b + 0x14;
            if (r->c != 0) {
                r->d = func_800BF79C(D_800E7728, *r->c);
                r->d->p = (u8 *)r->d + 0x1C;
            }
        }
    }
    for (i = 0; D_800C28CC[i].a != 0; i++) {
        D_800C28CC[i].b = func_800BF79C(D_800E7728, *D_800C28CC[i].a);
        D_800C28CC[i].b->p = (u8 *)D_800C28CC[i].b + 0x1C;
    }
    func_80069CA0(D_800E1BC4, 0);
}
