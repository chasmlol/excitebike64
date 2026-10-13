#include "ctx.h"

typedef struct {
    char pad[0x48];
    s32 f48;
} Ent;

extern s32 D_800CA844;
extern s32 D_800CA848[];
extern s32 D_800E32A0;
extern Ent *D_800EFD24;

void func_8004F8A4(void)
{
    s32 k = 0;

    if (D_800CA844 != 0) {
        s32 *p = D_800CA848;
        s32 a2 = D_800E32A0;
        Ent *e = D_800EFD24;
        do {
            s32 v = *p;
            p++;
            k++;
            e->f48 = (v * a2) >> 8;
            e++;
        } while (k < 4);
    }
}
