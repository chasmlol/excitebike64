#include "ctx.h"

extern s32 D_800E1E18;
extern s32 D_800E1C48;
void func_800ADA60(void *p);

void func_8000D064(void) {
    if (D_800E1E18 == 0) {
        func_800ADA60(&D_800E1C48);
        D_800E1E18 = 1;
    }
}
