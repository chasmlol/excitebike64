#include "ctx.h"

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
} Ent;

void func_800B78F0(s32 a);

extern s32 D_800C2AD0;
extern Ent D_800C2B68[];

void func_800112A4(void)
{
    Ent *p;
    u32 i;

    if (D_800C2AD0 != 0) {
        i = 0;
        p = D_800C2B68;
        do {
            s32 a = p->f4;
            p++;
            i++;
            func_800B78F0(a);
        } while (i < 7);
        D_800C2AD0 = 0;
    }
}
