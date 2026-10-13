#include "ctx.h"

void func_8001254C(s32 a, s32 b, u16 *c, s32 *d);
void func_80011A58(s32 *m, s32 a, s32 b, s32 c);
void func_80011C1C(s32 *a, s32 *m, s32 *out);

typedef struct {
    s32 m[10];
    s32 r58[2];
    s32 pad[8];
    u16 l30[2];
    s32 iS;
    u16 l28;
    u16 l26[3];
} Frame;

void func_80012EBC(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4)
{
    Frame f;

    func_8001254C(param_1[2], param_1[0], f.l30, &f.iS);
    func_80011A58(f.m, 0, 0, -(s16)f.l30[0]);
    func_80011C1C(param_1, f.m, f.r58);
    func_8001254C(f.r58[1], f.r58[0], &f.l28, &f.iS);
    func_80011A58(f.m, 0, -(s16)f.l28, 0);
    func_80011C1C(f.r58, f.m, param_1);
    func_8001254C(param_1[5], param_1[4], f.l26, &f.iS);
    func_80011A58(f.m, -(s16)f.l26[0], 0, 0);
    func_80011C1C(param_1, f.m, f.r58);
    *param_2 = f.l26[0];
    *param_3 = f.l28;
    *param_4 = f.l30[0];
}
