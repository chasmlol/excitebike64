#include "ctx.h"

extern s32 D_800F0750;
typedef struct { f32 x, y, z; } V3;
extern V3 *D_800F0754;
extern u8 D_800CB17C[];
extern u8 D_800CB1D4[];
extern u8 D_800CB1F0[];
extern u8 D_800D8B44[];
extern u8 D_800D8B48[];

void func_8000EFB0();
void func_8000EE30();
u32 func_8002E920();

typedef struct {
    u8 b10[16];
    u8 b20[16];
    u8 b30[16];
    u8 b40[16];
} Frame5CB20;

void func_8005CB20(void)
{
    Frame5CB20 f;
    s32 k;
    u32 u;

    func_8000EFB0(D_800CB17C);
    func_8000EE30(D_800F0750, f.b40);
    func_8000EFB0(f.b40);
    func_8000EFB0(D_800CB1D4);
    for (k = 1; k <= D_800F0750; k++) {
        func_8000EE30((s32)(D_800F0754[k].x * 1000.0f), f.b10);
        func_8000EE30((s32)(D_800F0754[k].z * 1000.0f), f.b30);
        u = func_8002E920((s32)(D_800F0754[k].x * 1000.0f), (s32)(D_800F0754[k].y * 1000.0f), (s32)(D_800F0754[k].z * 1000.0f));
        func_8000EE30(u, f.b20);
        func_8000EFB0(f.b10);
        func_8000EFB0(D_800D8B44);
        func_8000EFB0(f.b30);
        func_8000EFB0(D_800D8B48);
    }
    func_8000EFB0(D_800CB1F0);
}
