#include "ctx.h"

typedef struct Obj {
    s32 f0;
    s32 f4;
    void (*f8)(void);
    s32 fc;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;
} Obj;

extern void func_800BCB04(void *p);
extern Obj *D_800F0B58;
extern s32 D_800F0B60;
extern u8 D_800F0B40[];

void func_80062D38(void) {
    if (D_800F0B58 != NULL) {
        if (D_800F0B58->f1c != 0) {
            func_800BCB04((void *)D_800F0B60);
        }
        if (D_800F0B58->f18 != 0) {
            func_800BCB04(D_800F0B40);
        }
        D_800F0B58->f8();
    }
}
