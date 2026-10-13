#include "ctx.h"

void func_800B8E98(s32 id);

void func_80011098(s32 *p) {
    if (*p != -1) {
        func_800B8E98(*p);
        *p = -1;
    }
}
