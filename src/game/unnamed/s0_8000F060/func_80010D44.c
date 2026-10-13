#include "ctx.h"

extern s32 D_800C2B5C;
extern s32 D_800CEC04;
void func_800110DC(void);
void func_800B98B4(void);

void func_80010D44(void) {
    if (D_800C2B5C != 0 && D_800CEC04 == 0) {
        func_800110DC();
        func_800B98B4();
    }
}
