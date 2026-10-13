#include "game.h"

typedef struct { s32 x; s32 y; s32 d; s32 id; } Rec_44;
typedef struct { s32 n; Rec_44 r[1]; } List_8;

extern List_8 *D_800CCFE0;

void func_800716C0(void)
{
    s32 i, best, bi, t, dx, dy, id;
    u32 r;

    if (D_800CCFE0 != 0 && D_800CCFE0->n != 0 && D_800E1898 == 1) {
        for (i = 0; i < D_800CCFE0->n; i++) {
            dx = D_800CCFE0->r[i].x - (D_800E8090_2.x >> 4);
            dy = D_800CCFE0->r[i].y - (D_800E8090_2.z >> 4);
            r = func_8000EB18_2(dx * dx + dy * dy);
            t = 0x200 - (s32)(r << 9) / D_800CCFE0->r[i].d;
            if (best < t || i == 0) {
                best = t;
                bi = i;
            }
        }
        best = (best * D_800C2B58) / 255;
        if (best > 255) best = 255;
        if (best < 0) best = 0;
        id = D_800CCFE0->r[bi].id;
        if (id == -1) best = 0;
        if (D_800E3710 != 0) best = 0;
        if (D_800EFD64_2 - D_800CB210 == 0xd && id == 0x6c && D_800CCB20 != 0) {
            id = 0x72;
        }
        func_8004C5A0(id, best, -1);
    }
}

void func_800718F0(void)
{
    s32 i;
    u32 off;
    u32 r;
    D_800CCFE0_2 = (s32 *)func_8005AED8(D_800D1524);
    if (D_800CCFE0_2 != 0 && D_800CCFE0_2[0] > 0) {
        i = 0;
        do {
            off = (u32)i << 4;
            r = func_8004F3D0(*(u32 *)((u8 *)D_800CCFE0_2 + off + 0x10));
            i++;
            *(u32 *)((u8 *)D_800CCFE0_2 + off + 0x10) = r;
        } while (i < D_800CCFE0_2[0]);
    }
}

void func_80071978(void)
{
    D_800CCFE0 = 0;
}
