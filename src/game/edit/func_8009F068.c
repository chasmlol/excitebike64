#include "ctx.h"

typedef struct { u8 pad[0x3E8]; void *ptr; } GS;
typedef struct { u8 b[0x310]; } Blk;

extern GS D_800EFD70;
extern Blk D_800F1DF4;
extern char D_800DAB1C[];

void *func_800BC908(s32 a, s32 b, char *c, s32 d);
void func_800A42D0(void *p);
void func_8006F5F8(s32 x, s32 y);
void func_800BCB04(void *p);

void func_8009F068(void)
{
    D_800EFD70.ptr = func_800BC908(0x400, 0, D_800DAB1C, 0x6FE);
    func_800A42D0(D_800EFD70.ptr);
    D_800F1DF4 = *(Blk *)D_800EFD70.ptr;
    func_8006F5F8(4, 0);
    func_800BCB04(D_800EFD70.ptr);
    D_800EFD70.ptr = 0;
}

