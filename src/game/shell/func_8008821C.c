#include "ctx.h"

typedef struct {
    u8 p0[0x22];
    u16 h22;
    u8 p24[0x50 - 0x24];
    u8 b50[6];
} SrcA;

typedef struct {
    u8 p0[0x418];
    u16 h418;
    u8 b41A[6];
    u32 w420[6];
    u8 p438[0x764 - 0x438];
    u8 b764[6];
    u8 b76A[6];
} DstB;

typedef struct {
    u8 p0[0x669];
    u8 f669;
    u8 p66A[0x68C - 0x66A];
    u32 f68C;
    u8 pad[0x7E0 - 0x690];
} EntE;

extern SrcA D_800F05A8;
extern DstB D_800EFD70;
extern EntE D_800E37C0[];
extern u8 *D_800CA930;

void func_8008821C(void)
{
    s32 i = 0;

    D_800EFD70.h418 = D_800F05A8.h22;
    do {
        D_800EFD70.b41A[i] = D_800F05A8.b50[i];
        D_800EFD70.w420[i] = D_800E37C0[i].f68C;
        D_800EFD70.b76A[i] = D_800E37C0[i].f669;
        if (D_800CA930 != 0) {
            D_800EFD70.b764[i] = (D_800CA930 + i)[0x6C];
        }
        i++;
    } while (i < 6);
}
