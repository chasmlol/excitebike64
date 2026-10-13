#include "ctx.h"

typedef struct {
    u8 pad0[0x662];
    s16 f662;
    s16 f664;
    s16 f666;
    s16 f668;
    u8 pad66A[0x6E8 - 0x66A];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Ent;

typedef struct { u8 pad[0x2C]; s32 f2C; } G1;
typedef struct { u8 pad[0x3C]; s32 count; } G2;

extern G1 D_800F05A8;
extern G2 D_800EFD70;
extern Ent D_800E37C0[];
extern s32 D_800EFD28[];
extern s32 D_800E76D0;
extern void func_8004E6D8(s32 a0, s32 a1, s32 a2);

void func_800635A4(void)
{
    Ent *p;
    s32 v;
    s32 idx;

    if (D_800F05A8.f2C < 0x78) return;
    for (p = D_800E37C0; p < D_800E37C0 + D_800EFD70.count; p++) {
        v = p->f668 >= 0 ? p->f668 : p->f662;
        if (v != p->f664) {
            if (p->f666 > 0x180) {
                p->f666 = 0x300 - p->f666;
            }
            p->f666 -= 0x20;
            if (p->f666 <= 0) {
                if (p->f6E8 == 0 && p->f668 < 0) {
                    if (D_800E76D0 - D_800EFD28[p - D_800E37C0] > 300 && D_800F05A8.f2C > 600) {
                        func_8004E6D8(p - D_800E37C0, v, p->f664);
                    }
                }
                p->f666 = 1;
                p->f664 = v;
            }
        } else {
            if (p->f666 < 0x200) {
                p->f666 += 0x10;
            }
        }
    }
}
