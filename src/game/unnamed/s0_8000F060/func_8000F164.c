#include "ctx.h"

typedef struct {
    s32 *p;
    s32 b;
    s32 c;
    s32 d;
    s32 pad[2];
} Ent;

extern s32 D_800C2AD0;
extern s32 D_800C29E0;
extern Ent D_800C2B68[];

void func_800BBABC(s32 a, s32 b);
s32 func_800BBB88(s32 a0);
s32 func_800B781C();
void func_800B8E08();

void func_8000F164(void) {
    u32 i;
    s32 local[4];

    if (D_800C2AD0 == 0) {
        func_800BBABC(D_800C29E0, 0x25800);
        for (i = 0; i < 7; i++) {
            if (D_800C2B68[i].p != NULL) {
                D_800C2B68[i].b = func_800B781C(*D_800C2B68[i].p);
            } else {
                D_800C2B68[i].b = D_800C2B68[i - 1].b;
            }
            func_800B8E08(D_800C2B68[i].b, local);
            D_800C2B68[i].c = local[0];
            D_800C2B68[i].d = func_800BBB88(D_800C2B68[i].b);
        }
        func_800BBABC(0, 0);
        D_800C2AD0 = 1;
    }
}
