#include "ctx.h"

typedef struct Rec {
    u8 pad[0xA0];
} Rec;

typedef struct Ent {
    s32 key;
    void (*fn)(void);
    s32 pad[4];
    s32 n6;
    s32 n7;
    s32 **names;
} Ent;

extern Ent *D_800F0B58;
extern void *D_800F0B40[2];
extern s32 *D_800F0B60;
extern Rec *D_800EFD64;
extern Rec D_800CB210[];
extern Ent *D_800CCB84[];
extern s32 D_800E7728;
extern char D_800D8C10[];

extern void *func_800BC908(s32, s32, char *, s32);
extern s32 func_800BF79C(s32, s32);

void func_800620A0(void)
{
    s32 key;
    Ent **pp;
    s32 i;

    key = D_800EFD64 - D_800CB210;
    D_800F0B58 = NULL;
    for (i = 0; D_800CCB84[i] != NULL; i++) {
        if (D_800CCB84[i]->key == key) {
            D_800F0B58 = D_800CCB84[i];
            break;
        }
    }
    if (D_800F0B58 != NULL) {
        D_800F0B40[0] = NULL;
        D_800F0B40[1] = NULL;
        D_800F0B60 = NULL;
        if (D_800F0B58->n6 != 0) {
            D_800F0B40[0] = func_800BC908(D_800F0B58->n6 << 7, 0, D_800D8C10, 0x9f);
            D_800F0B40[1] = (u8 *)D_800F0B40[0] + (D_800F0B58->n6 << 6);
        }
        if (D_800F0B58->n7 != 0) {
            D_800F0B60 = func_800BC908(D_800F0B58->n7 << 2, 0, D_800D8C10, 0xa4);
            for (i = 0; i < D_800F0B58->n7; i++) {
                D_800F0B60[i] = func_800BF79C(D_800E7728, *D_800F0B58->names[i]);
            }
        }
        D_800F0B58->fn();
    }
}
