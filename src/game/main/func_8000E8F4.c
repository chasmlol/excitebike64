#include "ctx.h"

typedef struct {
    void *next;
    u8 f4, f5, f6, f7, f8, f9;
    u8 pad[2];
    u32 fc;
} Obj;

extern Obj D_800E2138;
extern void *D_800D1E0C;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
s32 func_800AC890(void);
void func_800AC900(s32);

void func_8000E8F4(void)
{
    s32 r;

    func_8000ED10((u8 *)&D_800E2138, 0, 0x74);
    D_800E2138.fc = 0xAFF70000;
    D_800E2138.f5 = 5;
    D_800E2138.f8 = 0xC;
    D_800E2138.f6 = 0xD;
    D_800E2138.f7 = 2;
    D_800E2138.f4 = 0;
    D_800E2138.f9 = 1;
    r = func_800AC890();
    D_800E2138.next = D_800D1E0C;
    D_800D1E0C = &D_800E2138;
    func_800AC900(r);
}
