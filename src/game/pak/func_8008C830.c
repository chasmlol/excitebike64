#include "ctx.h"

extern s32 *D_800CE164;
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
void func_800C1F2C(s32 *p, char *s);
void func_800C0ACC(s32 *p, s32 a, s32 b);
void func_800C1394(s32 *p);

void func_8008C830(s32 *p1, char *p2, s32 p3, s32 p4, s32 p5, s32 p6)
{
    char *q;
    if (p3 == 2) {
        q = p2;
        if (*q != 0) {
            do {
                if (*q == 0x40) {
                    *q = 0x7F;
                }
                q++;
            } while (*q != 0);
        }
    }
    func_800C1E60(D_800CE164, 0x80, p3, p4, 0);
    func_800C1F2C(D_800CE164, p2);
    func_800C0ACC(D_800CE164, p5, p6);
    func_800C1394(p1);
}
