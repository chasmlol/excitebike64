#include "ctx.h"

extern s32 D_800C2358;
extern s32 D_800EA778[];
extern s32 D_800C99B0[];
extern s32 D_800EFD70;

typedef struct Hdr {
    u8 pad[0x10];
    u8 *p;
    u8 data[4];
} Hdr;

typedef struct Rec {
    s32 f00;
    s32 *f04;
    s32 *f08;
    Hdr *f0C;
    Hdr *f10;
    u8 pad[0x3C - 0x14];
} Rec;

typedef struct {
    u8 pad[0x3AC];
    s32 v;
    u8 pad2[0x7E0 - 0x3B0];
} Big;

extern Big D_800E37C0[];
extern Rec D_800C9A40[];

s32 func_800C0050(u8 *a, s32 b);
void func_8000ECB8(s32 x);
Hdr *func_800BF79C(u8 *a, s32 b);

void func_8003E1FC(u8 *param_1)
{
    s32 i;
    s32 v;
    Rec *p;
    s32 m;
    Hdr *r;

    if (D_800C2358 == 0) {
        for (i = 0; i < 0xF; i++) {
            v = func_800C0050(param_1, D_800C99B0[i]);
            D_800EA778[i] = v;
            if (v < 0) {
                func_8000ECB8(1);
            }
        }
        m = -1;
        for (i = 7; i >= 0; i--) {
            D_800E37C0[i].v = m;
        }
        for (i = 0; D_800C9A40[i].f04 != 0; i++) {
            if (i >= 16 || !(D_800EFD70 & 0x10)) {
                D_800C9A40[i].f0C = func_800BF79C(param_1, *D_800C9A40[i].f04);
                D_800C9A40[i].f0C->p = D_800C9A40[i].f0C->data;
                if (D_800C9A40[i].f08 != 0) {
                    D_800C9A40[i].f10 = func_800BF79C(param_1, *D_800C9A40[i].f08);
                    D_800C9A40[i].f10->p = D_800C9A40[i].f10->data;
                }
            }
        }
    }
}
