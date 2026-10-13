#include "ctx.h"

typedef struct { u16 id; u8 a; u8 b; u8 flag; u8 pad; } Sel;
typedef struct { u16 id; u8 a; u8 b; } Out;

extern s32 D_800E1E18;
extern Sel D_800E1C00[];
extern u16 D_800E1C40[];
extern Out D_800E1E40[];
extern u8 D_800E2000;
extern u8 D_800E2001;
extern u8 D_800C292C;
extern u8 D_800C292E;
void func_800ADAE8(void *p);

void func_8000CEAC(void)
{
    u16 i;
    u16 cnt;
    u32 started;
    u8 bit;
    u8 nv;
    u8 old;
    Sel *e;

    if (D_800E1E18 != 0) {
        return;
    }
    cnt = 0;
    func_800ADAE8(D_800E1C00);
    started = 0;
    D_800E2000 = D_800E1C00[i].a;
    D_800E2001 = D_800E1C00[i].b;
    i = 0;
    do {
        old = D_800C292C;
        bit = (u8)(1 << i);
        nv = old | bit;
        D_800C292C = nv;
        e = &D_800E1C00[i];
        if (e->flag != 0) {
            D_800C292C = nv & ~bit;
        } else {
            if (started == 0) {
                D_800C292E = (u8)i;
                started = 1;
            }
            D_800E1C40[i] = e->id;
            D_800E1E40[i].id = e->id;
            D_800E1E40[i].a = e->a;
            D_800E1E40[i].b = e->b;
            cnt = cnt + 1;
            if (cnt == 4) {
                return;
            }
        }
        i = i + 1;
    } while (i < 4);
}
