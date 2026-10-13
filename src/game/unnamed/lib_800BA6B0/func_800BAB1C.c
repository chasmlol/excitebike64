#include "ctx.h"

typedef struct {
    void *p0;
    s32 f4;
    s32 f8;
    s32 fc;
    s32 f10;
    volatile s32 f14;
    s32 f18;
} SamEnt;

extern void *D_800D5604;
extern SamEnt D_800FE8F0[];
extern s32 D_800FE8DC;
extern char D_800FE880[];
extern char D_800DB1D4[];
extern char D_800DB1E0[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800AEC60(void *a0, char *a1, s32 a2);
void *func_800BFF60(void *a0, void *a1, void *a2);

s32 func_800BAB1C(s32 param_1)
{
    s32 i;
    SamEnt *e;
    char *pcVar6;
    s32 *q;
    SamEnt *ent;
    s32 *pp;

    if (D_800D5604 == 0) {
        return -1;
    }
    for (i = 0; i < 0x100; i++) {
        if (D_800FE8F0[i].f4 == 0) {
            break;
        }
    }
    if (i == 0x100) {
        return -1;
    }
    func_8000ED10((u8 *)&D_800FE8F0[i], 0, 0x1c);
    if (D_800FE8DC & 2) {
        func_800AEC60(D_800FE880, D_800DB1D4, param_1);
    } else {
        func_800AEC60(D_800FE880, D_800DB1E0, param_1);
    }
    q = (s32 *)func_800BFF60(D_800D5604, D_800FE880, &D_800FE8F0[i].f18);
    ent = &D_800FE8F0[i];
    ent->f14 = (s32)q;
    ent->f4 = ((volatile s32 *)q)[1];
    e = (SamEnt *)ent->f14;
    pp = (s32 *)ent->f14;
    ent->f8 = ((s32 *)e)[2];
    ent->fc = ((s32 *)e)[3];
    pcVar6 = (char *)pp;
    pp = (s32 *)pp[4];
    ent->p0 = pcVar6 + 0x18;
    ent->f10 = (s32)pp;
    return i;
}
