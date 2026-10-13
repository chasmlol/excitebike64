#include "ctx.h"

u32 *func_800C055C(u32 *p);
u32 *func_800C0350(u32 *p);

u32 *func_800C04EC(u32 *param_1)
{
    switch (*param_1 & 0xF) {
    case 1:
        return param_1;
    case 2:
        return NULL;
    case 3:
        return func_800C055C(param_1);
    case 4:
    case 5:
        return func_800C0350(param_1);
    default:
        return NULL;
    }
}
