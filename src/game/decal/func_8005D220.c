#include "ctx.h"

void func_8000EF8C(char *a0, char *a1);
void func_8000EF0C(u32 param_1, char *param_2);
s32 func_800AEC10(char *s);

void func_8005D220(char *param_1, u32 param_2) {
    char buf[56];
    char *s = buf + 16;
    func_8000EF8C(s, param_1);
    func_8000EF0C(param_2, s + func_800AEC10(s));
}
