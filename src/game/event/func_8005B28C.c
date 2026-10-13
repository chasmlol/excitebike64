#include "ctx.h"

typedef struct {
    u8 pad[0x668];
    s16 x668;
    u8 pad2[0x7E0 - 0x66A];
} Rider;

typedef struct {
    u8 pad[0x22];
    s16 n;
} Game;

typedef struct {
    u8 pad[0x66];
    u8 a[1];
} Thing;

extern Game D_800F05A8;
extern Rider D_800E37C0[];
extern Thing *D_800CA930;

void func_8005B28C(void) {
    s32 i;
    for (i = 0; i < D_800F05A8.n; i++) {
        if (D_800E37C0[i].x668 != -1) {
            D_800CA930->a[i] = 0;
        }
    }
}
