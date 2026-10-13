#include "ctx.h"

typedef struct {
    s32 state;
    u8 pad[0x48];
} Ent;

extern Ent D_80100720[];
extern void (*D_800D5710)(void);
void func_800BED20(void);

void func_800BEFB4(s32 x) {
    do {
        if (D_800D5710 != NULL) {
            D_800D5710();
        }
        func_800BED20();
    } while (D_80100720[x].state != 3 && D_80100720[x].state != 0);
    D_80100720[x].state = 0;
}
