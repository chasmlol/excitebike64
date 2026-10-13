#include "ctx.h"

typedef struct {
    s32 state;
    u8 f04[0x1C];
    u8 f20[2];
    u8 flag22;
    u8 pad23;
    void *p24;
    s32 f28, f2C, f30, f34;
    s32 f38, f3C, f40, f44, f48;
} Ent;

typedef void (*FnPtr)(s32, s32, s32);

extern Ent D_80100720[];
extern s32 D_80100704;
extern s32 D_801009A0;
extern FnPtr D_800D5714;

extern void func_8000EA24(void);
extern s32 func_800B0610(void *a, s32 b, s32 c);
extern s32 func_800BF040(s32 a, void *b, s32 c);

void func_800BED20(void)
{
    s32 i;

    D_80100704 = 0;
    D_801009A0 = 0;
    for (i = 0; i < 8; i++) {
        if (D_80100720[i].state == 1) {
            if (func_800B0610(D_80100720[i].f04, 0, 0) == 0) {
                D_80100720[i].state = 3;
            } else {
                D_80100704++;
                return;
            }
        } else if (D_80100720[i].state == 2) {
            if (D_800D5714 == 0) {
                D_80100720[i].p24 = D_80100720[i].f04;
                D_801009A0++;
                D_80100720[i].flag22 = 0;
                D_80100720[i].f28 = D_80100720[i].f3C;
                D_80100720[i].f2C = D_80100720[i].f40;
                D_80100720[i].f30 = D_80100720[i].f44;
                func_8000EA24();
                if (func_800BF040(D_80100720[i].f38, D_80100720[i].f20, D_80100720[i].f48) == 0) {
                    D_80100720[i].state = 1;
                    return;
                }
            } else {
                D_800D5714(D_80100720[i].f40, D_80100720[i].f3C, D_80100720[i].f44);
                D_80100720[i].state = 3;
            }
        }
    }
}
