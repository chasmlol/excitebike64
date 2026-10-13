#include "ctx.h"

typedef struct { s32 a, b, c; } T3;
typedef struct { u8 pad[0x20]; f32 pos; f32 vel; } S28;

extern u8 D_800E7968[];
extern u8 D_800E7FA8[];
extern s32 *D_800E80C0;
extern s32 D_800EE490;
extern u8 D_800D6A70[];
extern u8 D_800E8030[];
extern u8 D_800E7998[];
extern u8 D_800E7988[];
extern u8 D_800E7FC8[];
extern S28 D_800E78C8[];
extern u8 D_800E8010[];
extern f32 D_800C9688;
extern f32 D_800C968C;
extern s32 D_800C9654[];
extern T3 D_800E8090[];
extern s32 D_800E7FE8;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_80028790(void);
s32 *func_800BC908(s32 a, s32 b, void *c, s32 d);

void func_80028600(void)
{
    s32 i, j;

    func_8000ED10(D_800E7968, 0, 0x10);
    func_8000ED10(D_800E7FA8, 0, 0x20);
    D_800E80C0 = func_800BC908(D_800EE490 << 1, 0, D_800D6A70, 0x239);
    func_80028790();
    func_8000ED10(D_800E8030, 0, 0x60);
    func_8000ED10(D_800E7998, 0, 0x600);
    func_8000ED10(D_800E7988, 0, 0x10);
    func_8000ED10(D_800E7FC8, 0, 0x20);
    func_8000ED10((u8 *)D_800E78C8, 0, 0xa0);
    func_8000ED10(D_800E8010, 0, 0x10);
    D_800E7FE8 = 0;
    for (i = 0; i < 4; i++) {
        D_800E8090[i].a = 0;
        D_800E8090[i].b = 0xbb0;
        D_800E8090[i].c = 0;
        D_800C9654[i] = -1;
        for (j = 0; j < 0x78; j++) {
            D_800E78C8[i].vel += (2000.0f - D_800E78C8[i].pos) * D_800C9688;
            D_800E78C8[i].pos += D_800E78C8[i].vel;
            D_800E78C8[i].vel *= D_800C968C;
        }
    }
}
