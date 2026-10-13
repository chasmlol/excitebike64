#include "ctx.h"

typedef struct { s32 a, b; u8 pad[0x1C]; } E;
typedef struct { u8 cpu[0x10000]; u8 pad[0x412C]; E e[4]; } Nes;
#define NES ((Nes *)0x80380000)

void func_800A1ADC(void)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        NES->e[i].a = 0;
        NES->e[i].b = 0;
    }
}
