#include "ctx.h"

s32 func_80013010(s32 a);

s32 func_80012FE4(s32 a) {
    s32 r = func_80013010(a);
    return (s16)(0x4000 - r);
}
