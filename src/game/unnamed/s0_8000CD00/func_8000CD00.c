#include "ctx.h"

typedef struct {
    u8 pad[0x68];
} Rec68;

extern s32 D_800E1E18;
extern u8 D_800E1C48[];
extern s32 D_800E2010[];
extern Rec68 D_800E1E60[];
extern s32 D_800E2020;
extern s32 D_800E1C60;
extern s32 D_800C2924;
extern s32 D_800C2920;
extern s32 D_800C29E0;

void func_800B0610(void *a, void *b, s32 c);
void func_800B3704(void *a, void *b, s32 c);
void func_800B35C0(void *a, s32 c);

void func_8000CD00(void)
{
    s32 buf[2];
    s32 i;
    s32 v;

    if (D_800E1E18 != 0) {
        func_800B0610(D_800E1C48, buf, 1);
        for (i = 0; i < 8; i++) {
            func_800B0610(D_800E1C48, buf, 0);
        }
        for (i = 0; i < 4; i++) {
            if (D_800C2924 != 0) {
                func_800B3704(D_800E1C48, &D_800E1E60[i], i);
                func_800B35C0(&D_800E1E60[i], 0);
                D_800E2010[i] = 0;
                D_800E2020 &= ~(1 << i);
            }
            if (D_800E1C60 & (1 << i)) {
                if (D_800E2020 & (1 << i)) {
                    v = D_800E2010[i];
                    if (v == 0) {
                        func_800B35C0(&D_800E1E60[i], 0);
                        D_800E2010[i] = 0;
                        D_800E2020 &= ~(1 << i);
                    }
                } else {
                    if (D_800E2010[i] != 0) {
                        func_800B35C0(&D_800E1E60[i], 1);
                        D_800E2020 |= 1 << i;
                    }
                }
                if (D_800E2010[i] != 0) {
                    D_800E2010[i]--;
                }
            }
        }
        D_800C2924 = 0;
        D_800E1E18 = 0;
        if (D_800C2920 != 0) {
            D_800C29E0 = 0x80010000;
        }
    }
}
