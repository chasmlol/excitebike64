#include "ctx.h"

typedef struct { u32 w[8]; } Hdr;
typedef struct {
    u32 f0;
    u32 magic;
    u8 p8[2];
    u8 a[4];
    u8 b[0x10];
    s16 id;
    Hdr hdr;
    u8 rest[0x74 - 0x40];
} Slot;
typedef struct { u8 b[0x68]; } Ctl;

extern u32 D_800E2004;
extern Ctl D_800E1C78[];
extern Slot *D_800F4A10[];

void func_8000CD00(void);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
s32 func_800B44D0(Ctl *a0, s32 a1, Slot *a2);
void func_8008C670(u8 *buf, s32 n);
s32 func_8008C2BC(u32 param_1, s32 param_2, u32 param_3, u32 param_4, u8 *param_5);

s32 func_8008B7B4(u32 p)
{
    Hdr tmp;
    s32 i, k, r;

    if (!(D_800E2004 & (1 << p))) {
        return -1;
    }
    func_8000CD00();
    for (i = 0; i < 16; i++) {
        D_800F4A10[p][i].id = -1;
    }
    k = 0;
    i = 0;
    do {
        r = func_800B44D0(&D_800E1C78[p], i, &D_800F4A10[p][k]);
        if (r == 5) {
            func_8000ED10((u8 *)&D_800F4A10[p][k], 0, 0x74);
            D_800F4A10[p][k].id = -1;
        } else if (r == 0) {
            D_800F4A10[p][k].id = i;
            func_8008C670(D_800F4A10[p][k].b, 0x10);
            func_8008C670(D_800F4A10[p][k].a, 4);
            if (D_800F4A10[p][k].magic == 0x4E4D5847) {
                if (func_8008C2BC(p, i, 0, 0x20, (u8 *)&tmp) != 0) {
                    D_800F4A10[p][k].magic = 0;
                }
                D_800F4A10[p][k].hdr = tmp;
            }
            k++;
        } else {
            return -2;
        }
        i++;
    } while (i < 16);
    return 0;
}
