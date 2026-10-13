#include "ctx.h"

typedef struct { s32 x; s32 y; s32 d; s32 id; } Rec;
typedef struct { s32 n; Rec r[1]; } List;
typedef struct { s32 x; s32 y; s32 z; } Pos;

extern List *D_800CCFE0;
extern s32 D_800E1898;
extern Pos D_800E8090;
extern s32 D_800E3710;
extern s32 D_800C2B58;
typedef struct { u8 pad[0xA0]; } Trk;
extern Trk *D_800EFD64;
extern Trk D_800CB210[];
extern s32 D_800CCB20;
s32 func_8000EB18(u32 r);
void func_8004C5A0(s32 a, s32 b, s32 c);

void func_800716C0(void)
{
    s32 i, best, bi, t, dx, dy, id;
    u32 r;

    if (D_800CCFE0 != 0 && D_800CCFE0->n != 0 && D_800E1898 == 1) {
        for (i = 0; i < D_800CCFE0->n; i++) {
            dx = D_800CCFE0->r[i].x - (D_800E8090.x >> 4);
            dy = D_800CCFE0->r[i].y - (D_800E8090.z >> 4);
            r = func_8000EB18(dx * dx + dy * dy);
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
        if (D_800EFD64 - D_800CB210 == 0xd && id == 0x6c && D_800CCB20 != 0) {
            id = 0x72;
        }
        func_8004C5A0(id, best, -1);
    }
}
