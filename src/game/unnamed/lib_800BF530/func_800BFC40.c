#include "ctx.h"

typedef struct {
    u8 name[3];
    u8 pad;
    s32 val;
} Ent;

extern s32 D_800D5738;
extern Ent D_80100AB8[16];
extern char D_800DB47C[];
s32 func_8000ECE0(s32 a0);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8000EB00(s32 a, ...);
void func_8000ECB8() __attribute__((noreturn));

void func_800BFC40(u8 *name, s32 val)
{
    s32 i;

    if (D_800D5738 == 0) {
        func_8000ED10((u8 *)D_80100AB8, 0, 0x80);
        D_800D5738 = 1;
    }
    for (i = 0; i < 16; i++) {
        if (func_8000ECE0(D_80100AB8[i].name[0]) == func_8000ECE0(name[0]) &&
            func_8000ECE0(D_80100AB8[i].name[1]) == func_8000ECE0(name[1]) &&
            func_8000ECE0(D_80100AB8[i].name[2]) == func_8000ECE0(name[2])) {
            break;
        }
    }
    if (i == 16) {
        for (i = 0; i < 16; i++) {
            if (D_80100AB8[i].val == 0) {
                break;
            }
        }
        if (i == 16) {
            func_8000EB00((s32)D_800DB47C);
            func_8000ECB8(1);
        }
    }
    __builtin_memcpy(D_80100AB8[i].name, name, 3);
    D_80100AB8[i].val = val;
}
