#include "ctx.h"

typedef struct { u8 a; u8 b; u16 z; u8 *p; } Rec;
typedef struct { u32 n; u32 pad; Rec r[1]; } Tab;

extern s32 D_800D0E0C;
extern s32 D_800D0DB4;
extern s32 D_800D0D20;
extern s32 D_800D0924;
extern s32 D_800D0DAC;
extern s32 D_800D10B4;
extern Tab *D_800E92A0;
extern Tab *D_800E9290;
extern void *D_800E926C;
extern void *D_800E9270;
extern void *D_800E929C;
extern void *D_800E92C8;

extern void *func_800BF79C();
extern void func_80014948();

void func_800366AC(void *param_1)
{
    s32 iVar4;
    s32 j;
    s32 i;
    u8 *pb;

    D_800E92A0 = (Tab *)func_800BF79C(param_1, D_800D0E0C);
    iVar4 = D_800E92A0->r[0].a * (D_800E92A0->r[0].b << 3);
    if (D_800E92A0->n != 0) {
        i = 0;
        do {
            j = 0;
            if (iVar4 != 0) {
                                do {
                    pb = D_800E92A0->r[i].p;
                    pb = pb + j;
                    *pb = *pb >> 4;
                    pb = D_800E92A0->r[i].p;
                    pb = pb + j;
                    *pb = *pb & 0xf;
                    pb = D_800E92A0->r[i].p;
                    pb = pb + j;
                    j++;
                    *pb = *pb | 0xf0;
                } while (j < iVar4);
            }
            i++;
        } while (i < D_800E92A0->n);
    }
    D_800E926C = func_800BF79C(param_1, D_800D0DB4);
    D_800E9290 = (Tab *)func_800BF79C(param_1, D_800D0D20);
    i = 0;
    if (D_800E9290->n != 0) {
        do {
            iVar4 = D_800E9290->r[i].a * (D_800E9290->r[i].b << 3);
            j = 0;
            if (iVar4 != 0) {
                do {
                    pb = D_800E9290->r[i].p;
                    pb = pb + j;
                    j++;
                    *pb = *pb | 0xf0;
                } while (j < iVar4);
            }
            i++;
        } while (i < D_800E9290->n);
    }
    D_800E9270 = func_800BF79C(param_1, D_800D0924);
    func_80014948(D_800E9270);
    D_800E929C = func_800BF79C(param_1, D_800D0DAC);
    func_80014948(D_800E929C);
    D_800E92C8 = func_800BF79C(param_1, D_800D10B4);
    func_80014948(D_800E92C8);
}
