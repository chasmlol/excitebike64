#include "ctx.h"

extern void func_8000EF8C(char *a0, char *a1);
extern s32 func_800AEC10(char *s);
extern void func_8005CD30(char *s, f32 v, s32 n);

void func_8005D004(char *param_1, f32 param_2)
{
    char buf[40];
    s32 len;
    func_8000EF8C(buf, param_1);
    len = func_800AEC10(buf);
    func_8005CD30(buf + len, param_2, 3);
}
