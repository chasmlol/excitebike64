#include "ctx.h"

typedef struct Ent {
    u8 pad0[8];
    void (*f8)(void);
    void (*fC)(struct Ent *);
    s16 f10;
    u16 f12;
    u8 pad1[4];
} Ent;

typedef struct { u8 pad[0x4100]; u16 pc; u8 pad2[14]; u32 cyc; } Cpu;
extern Ent D_800CED14[];
extern s32 D_800CEC1C;
extern u8 D_80394000;
extern u32 D_80394110;
extern volatile s32 D_803941BC;
extern void (*D_800D0660)(s32);
extern void func_800A2C50(void);

#define CUR (*(Ent **)(glob + 0x5BB4))
#define PC (cpu->pc)
#define CYC (cpu->cyc)
#define R2000 (ram[0x2000])
#define R2002 (ram[0x2002])

void func_800A2D64(void)
{
    s32 i = 0;
    s32 line;
    u8 *glob = (u8 *)0x800F0000;
    u8 *ram = (u8 *)0x80380000;
    Cpu *cpu = (Cpu *)0x80390000;

    do {
        line = (i + 0xf0) % 0xf8;
        if (*(u32 *)0x80394110 < 0x78) {
            Ent *tb = D_800CED14;
            u8 *dseg = (u8 *)0x800D0000;
            do {
                Ent *e;
                CUR = (Ent *)((s32)(*(u8 *)(PC + (s32)ram) * sizeof(Ent)) + (s32)tb);
                CUR->f8();
                e = CUR;
                PC += e->f12;
                e->fC(e);
                (*(s32 *)(dseg - 0x13E4))++;
                CYC += CUR->f10;
            } while (CYC < 0x78);
        }
        CYC -= 0x78;
        if ((line & 7) == 0) {
            if (line == 0) {
                R2002 = 0x10;
            } else if (line == 0xf0) {
                R2002 |= 0x80;
                if (R2000 & 0x80) func_800A2C50();
            }
        }
        if ((line & 7) == 4 && line < 0xf0 && D_800D0660 != 0) D_800D0660(line >> 3);
        if (line == *(u8 *)0x80394000 && line < 0xf0) {
            R2002 |= 0x40;
            if (R2000 & 0x40) func_800A2C50();
        }
        i++;
    } while (i < 0xf8);
    {
        u8 *c2 = (u8 *)0x80390000;
        (*(s32 *)(c2 + 0x41BC))++;
    }
}
