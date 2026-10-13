#include "ctx.h"

typedef struct { u8 b[0x20]; } MQ;
typedef struct { u16 type; u8 status; u8 errnum; } CStat;
typedef struct { u8 b[0x68]; } Pfs;

extern s32 D_800E1E18;
extern MQ D_800E1C48;
extern CStat D_800E1C68[4];
extern Pfs D_800E1C78[4];
extern Pfs D_800E1E60[4];
extern u8 D_800F4878[];
extern u32 D_800E2004;
extern u32 D_800E2008;
extern u32 D_800E1C60;
extern u8 D_800C292C;
extern u8 D_800C292D;
extern u8 D_800C292E;

extern void func_8000CD00(void);
extern void func_800AD9C0(MQ *mq);
extern void func_800B0610(MQ *mq, void *msg, s32 flag);
extern void func_800ADA40(CStat *st);
extern s32 func_800B4D20(MQ *mq, Pfs *pfs, s32 chan);
extern s32 func_800B3704(MQ *mq, Pfs *pfs, s32 chan);
extern s32 func_800B33C0(MQ *mq, void *buf, s32 chan);
extern void func_800B35C0(Pfs *pfs, s32 x);

void func_8008B9BC(s32 chan)
{
    u32 bit = 1 << chan;
    s32 msg[2];
    s32 i;

    if (D_800E1E18 != 0) {
        func_8000CD00();
    }
    func_800AD9C0(&D_800E1C48);
    func_800B0610(&D_800E1C48, msg, 1);
    func_800ADA40(D_800E1C68);
    D_800C292C = 0;
    D_800C292D = 0;
    for (i = 0; i < 4; i++) {
        if (D_800E1C68[i].errnum == 0) {
            if (D_800C292C == 0) {
                D_800C292E = i;
            }
            D_800C292C |= 1 << i;
            D_800C292D++;
        }
    }
    if ((D_800C292C & bit) == 0) {
        return;
    }
    i = chan;
    D_800E2004 &= ~bit;
    D_800E1C60 &= ~bit;
    D_800E2008 &= ~bit;
    switch (func_800B4D20(&D_800E1C48, &D_800E1C78[i], i)) {
    case 0:
        D_800E2004 |= bit;
        break;
    case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
        break;
    case 11:
        if (func_800B3704(&D_800E1C48, &D_800E1E60[i], i) != 0) {
            if (func_800B33C0(&D_800E1C48, D_800F4878, i) != 0) {
                D_800E2008 |= 1 << i;
            }
        } else {
            func_800B35C0(&D_800E1E60[i], 0);
            D_800E1C60 |= bit;
        }
        break;
    case 10:
        if (func_800B3704(&D_800E1C48, &D_800E1E60[i], i) == 0) {
            func_800B35C0(&D_800E1E60[i], 0);
            D_800E1C60 |= bit;
        } else {
            D_800E2008 |= bit;
        }
        break;
    }
}
