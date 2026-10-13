#include "ctx.h"

typedef struct {
    u8 pad[0x22];
    u8 a;
    u8 b;
    u8 pad2[0xC];
} Ch;

extern s32 D_800DB8C0;
extern s32 *D_800DB8C4;
extern u8 D_800DB8DD;
extern s32 D_800FE8E0;
extern Ch *D_800FE8D4;
extern s32 D_800D3400;
extern s32 D_800FE8DC;
extern s32 D_800D5608;
extern u8 *D_800FE870;
extern s32 D_800FE874;
extern s32 D_800FE878;
extern s32 D_801004F0;
extern s8 D_80100500[];
extern u8 D_80100600[];

void func_800DBAA0(void);
void func_800DB8E0(void);

void func_800BB09C(s32 *param_1, s32 param_2)
{
    s32 i;
    s32 n;
    s32 ix;
    u8 *pb;

    D_800DB8C0 = param_2;
    D_800DB8C4 = param_1;
    D_800DB8DD = 1;
    for (i = 0; i < D_800FE8E0; i++) {
        D_80100600[i * 2] = D_800FE8D4[i].a;
        D_80100600[i * 2 + 1] = D_800FE8D4[i].b;
        D_800FE8D4[i].a = (D_800FE8D4[i].a * D_800D3400) >> 8;
        D_800FE8D4[i].b = (D_800FE8D4[i].a * D_800D3400) >> 8;
    }
    if (D_800FE8DC & 1) {
        func_800DBAA0();
    } else {
        func_800DB8E0();
    }
    if (D_800D5608 != 0) {
        n = param_2;
        if (D_800FE8DC & 1) {
            n = param_2 << 1;
        }
        if (D_800FE8DC & 2) {
            i = 0;
            if (0 < n) {
                s8 *tb;
                tb = D_80100500;
                do {
                    pb = D_800FE870;
                    ix = D_800FE874;
                    param_1[i] += *(s8 *)((s32)pb[ix] + (s32)tb) << 8;
                    pb[ix] = param_1[i] >> 10;
                    ix++;
                    D_800FE874 = ix;
                    if (ix == D_800FE878) {
                        D_800FE874 = 0;
                    }
                    i++;
                } while (i < n);
            }
        } else {
            i = 0;
            if (0 < n) {
                s8 *tb;
                tb = D_80100500;
                do {
                    pb = D_800FE870;
                    ix = D_800FE874;
                    param_1[i] += *(s8 *)((s32)pb[ix] + (s32)tb);
                    pb[ix] = param_1[i] >> 2;
                    ix++;
                    D_800FE874 = ix;
                    if (ix == D_800FE878) {
                        D_800FE874 = 0;
                    }
                    i++;
                } while (i < n);
            }
        }
    }
    D_801004F0 += param_2;
    for (i = 0; i < D_800FE8E0; i++) {
        D_800FE8D4[i].a = D_80100600[i * 2];
        D_800FE8D4[i].b = D_80100600[i * 2 + 1];
    }
}
