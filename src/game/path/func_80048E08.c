#include "ctx.h"

typedef struct Rec {
    u8 pad0[0x1C];
    f32 f1c;
    u8 pad20[0x25];
    u8 c45;
    s16 h46;
    s16 h48;
    u8 pad4a[0x6C - 0x4A];
} Rec;

extern s32 D_800EE490;
extern Rec *D_800EE498;
void func_80048ADC(s32 a, s32 b);

void func_80048E08(void)
{
    s32 i, j, cnt;
    f32 rem;
    Rec *base;

    for (i = 0; i < D_800EE490; i++) {
        func_80048ADC(i, D_800EE498[i].h46);
    }
    for (i = 0; i < D_800EE490; i++) {
        cnt = 0;
        j = i;
        rem = 0.06f;
        base = D_800EE498;
        while (rem > 0.0f) {
            if (base[j].h48 != -1) {
                cnt++;
                break;
            }
            rem -= ((base[j].f1c > 0.0f) ? base[j].f1c : -base[j].f1c) + 0.006f;
            j = base[j].h46;
            cnt++;
        }
        D_800EE498[i].c45 = cnt - 1;
    }
}
