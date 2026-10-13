#include "ctx.h"

typedef struct {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
} Rec16;

extern s16 D_800F05CA;
extern u8 D_800E1718[];
extern s32 D_800E1898;
extern s32 D_800C2598;

void func_800C1F2C();
void func_800C0ACC();

void func_800532D4(u8 *p, s32 n)
{
    char buf[0x28];
    s32 m;
    u8 *base;
    s32 off;
    Rec16 *r;

    if (D_800F05CA >= 2) {
        buf[0] = n + 0x31;
        m = n;
        if (m > 3) {
            m = 3;
        }
        buf[1] = m + 0x7B;
        strcpy(buf + 2, " Place");
        func_800C1F2C(p, buf);
    } else {
        func_800C1F2C(p, "Finished");
    }
    base = D_800E1718;
    off = D_800C2598 * 16 + (D_800E1898 - 1) * 64;
    r = (Rec16 *)(base + off);
    func_800C0ACC(p, r->a + r->c / 2, r->b + r->d / 2);
}
