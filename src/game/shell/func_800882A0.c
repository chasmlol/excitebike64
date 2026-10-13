#include "ctx.h"

typedef struct {
    u8 pad0[0x668];
    u16 h668;
    u8 pad1[0x68C - 0x66A];
    u32 w68C;
    u8 pad2[0x7E0 - 0x690];
} Elem;

typedef struct {
    u8 pad0[0x418];
    u16 h418;
    u8 b41A[6];
    u32 w420[6];
    u8 pad1[0x764 - 0x438];
    u8 b764[6];
    u8 b76A[6];
} Src;

typedef struct {
    u8 pad0[0x22];
    u16 h22;
    u8 pad1[0x50 - 0x24];
    u8 b50[6];
} Info;

typedef struct {
    u8 pad0[0x6C];
    u8 b6C[6];
} Out;

extern Info D_800F05A8;
extern Src D_800EFD70;
extern Elem D_800E37C0[];
extern Out *D_800CA930;

void func_800882A0(void)
{
    s32 i;

    D_800F05A8.h22 = D_800EFD70.h418;
    for (i = 0; i < 6; i++) {
        D_800F05A8.b50[i] = D_800EFD70.b41A[i];
        D_800E37C0[i].w68C = D_800EFD70.w420[i];
        D_800E37C0[i].h668 = D_800EFD70.b76A[i];
        if (D_800CA930 != 0) {
            D_800CA930->b6C[i] = D_800EFD70.b764[i];
        }
    }
}
