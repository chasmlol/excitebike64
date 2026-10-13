#include "ctx.h"

typedef struct { s32 a; u8 pad[0x4C]; } Ent;
extern Ent D_800FC060[];
void func_8000EE20(void);
void func_8000EE28(void);

Ent *func_800BA0E0(void)
{
    s32 i;
    func_8000EE20();
    for (i = 0; i < 0x80; i++) {
        if (D_800FC060[i].a == -1) {
            D_800FC060[i].a = 0;
            break;
        }
    }
    func_8000EE28();
    if (i == 0x80) {
        return NULL;
    }
    return &D_800FC060[i];
}
