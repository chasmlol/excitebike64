#include "ctx.h"

extern s32 func_800AEC10(char *s);
extern void func_8005CD30(char *s, f32 f, s32 n);

void func_8005D118(s32 arg0, f32 *v)
{
    char bx[24];
    char by[24];
    char bz[24];

    strcpy(bx, "\nx=");
    func_8005CD30(bx + func_800AEC10(bx), v[0], 6);
    strcpy(by, "y=");
    func_8005CD30(by + func_800AEC10(by), v[1], 6);
    strcpy(bz, "z=");
    func_8005CD30(bz + func_800AEC10(bz), v[2], 6);
}
