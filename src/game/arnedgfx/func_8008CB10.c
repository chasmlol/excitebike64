#include "ctx.h"

typedef struct {
    s16 ob[3];
    u16 flag;
    s16 tc[2];
    u8 cn[4];
} V;

extern V *D_800F557C;
extern s32 D_800F4FD4;
extern s32 D_800F4F8C;
extern s32 D_800F4F90;

void func_8008CB10(s32 x, s32 z, s32 w, s32 h)
{
    D_800F557C[D_800F4FD4].ob[0] = x;
    D_800F557C[D_800F4FD4].ob[1] = 0;
    D_800F557C[D_800F4FD4].ob[2] = z;
    D_800F557C[D_800F4FD4].tc[0] = 0;
    D_800F557C[D_800F4FD4].tc[1] = 0;
    D_800F4FD4++;
    D_800F557C[D_800F4FD4].ob[0] = x + w;
    D_800F557C[D_800F4FD4].ob[1] = 0;
    D_800F557C[D_800F4FD4].ob[2] = z + h;
    D_800F557C[D_800F4FD4].tc[0] = 0x800;
    D_800F557C[D_800F4FD4].tc[1] = 0;
    D_800F4FD4++;
    if (w != 0) {
        z += w / 2;
        x = (x * D_800F4F8C + 1) / D_800F4F8C;
    }
    if (h != 0) {
        x -= h / 2;
        z = (z * D_800F4F90 + 1) / D_800F4F90;
    }
    D_800F557C[D_800F4FD4].ob[0] = x;
    D_800F557C[D_800F4FD4].ob[1] = 500;
    D_800F557C[D_800F4FD4].ob[2] = z;
    D_800F557C[D_800F4FD4].tc[0] = 0;
    D_800F557C[D_800F4FD4].tc[1] = 0x1000;
    D_800F4FD4++;
    D_800F557C[D_800F4FD4].ob[0] = x + w;
    D_800F557C[D_800F4FD4].ob[1] = 500;
    D_800F557C[D_800F4FD4].ob[2] = z + h;
    D_800F557C[D_800F4FD4].tc[0] = 0x800;
    D_800F557C[D_800F4FD4].tc[1] = 0x1000;
    D_800F4FD4++;
}
