#include "ctx.h"

typedef struct Part {
    s16 life;
    s16 w2;
    u8 typ;
    u8 pad5[7];
    f32 px, py, pz;
    f32 vx, vy, vz;
} Part;

extern s32 D_800EE140;
extern s32 D_800E1898;
extern s32 D_800E3710;
extern Part *D_800E9214;
extern s32 D_800E9228[];
void func_80037290(void);

void func_80036230(void)
{
    Part *p;
    Part *base;
    s32 i;
    s32 t;
    s32 *q;

    if (D_800EE140 == 0 && D_800E1898 < 4 && D_800E3710 == 0) {
        func_80037290();
        i = 0;
        base = D_800E9214;
        do {
            p = (Part *)((s32)base + i * 0x24);
            if (p->life >= 0) {
                p->life = p->life - 1;
                t = p->typ;
                p->px = p->px + p->vx;
                p->py = p->py + p->vy;
                p->pz = p->pz + p->vz;
                if (t == 1) {
                    p->vy = p->vy - 0.008166667f;
                } else if (t == 3) {
                    p->vx = p->vx * 0.99f;
                    p->vz = p->vz * 0.99f;
                } else if (t == 2) {
                    p->vy = p->vy - 0.0020416668f;
                }
            }
            i = i + 1;
        } while (i < 0x60);
        i = 0;
        q = D_800E9228;
        do {
            i = i + 1;
            if (*q != 0) {
                *q = *q - 1;
            }
            q = q + 1;
        } while (i < 0x10);
    }
}
