#include "boot.h"

typedef struct Rec_6 {
    s32 state;        /* 0x00 */
    u8 r4[0x1C];      /* 0x04 */
    u8 b20[2];        /* 0x20 */
    u8 flag;          /* 0x22 */
    u8 b23;           /* 0x23 */
    u8 *rp;           /* 0x24 */
    s32 f28;          /* 0x28 */
    s32 f2c;          /* 0x2C */
    s32 f30;          /* 0x30 */
    s32 f34;          /* 0x34 */
    s32 f38;          /* 0x38 */
    s32 f3c;          /* 0x3C */
    s32 f40;          /* 0x40 */
    s32 f44;          /* 0x44 */
    s32 f48;          /* 0x48 */
} Rec_6;

extern Rec_6 sBootDmaRequests[];

void boot_dma_update(void)
{
    s32 i;
    s32 iVar2;

    sBootDmaPendingCount = 0;
    sBootDmaStartedCount = 0;
    for (i = 0; i < 8; i++) {
        if (sBootDmaRequests[i].state == 1) {
            iVar2 = osRecvMesg(sBootDmaRequests[i].r4, 0, 0);
            if (iVar2 == 0) {
                sBootDmaRequests[i].state = 3;
            } else {
                sBootDmaPendingCount = sBootDmaPendingCount + 1;
                return;
            }
        } else if (sBootDmaRequests[i].state == 2) {
            if (sBootDmaCopyFunc == 0) {
                sBootDmaRequests[i].rp = sBootDmaRequests[i].r4;
                sBootDmaStartedCount = sBootDmaStartedCount + 1;
                sBootDmaRequests[i].flag = 0;
                sBootDmaRequests[i].f28 = sBootDmaRequests[i].f3c;
                sBootDmaRequests[i].f2c = sBootDmaRequests[i].f40;
                sBootDmaRequests[i].f30 = sBootDmaRequests[i].f44;
                iVar2 = osEPiStartDma(sBootDmaRequests[i].f38, sBootDmaRequests[i].b20, sBootDmaRequests[i].f48);
                if (iVar2 == 0) {
                    sBootDmaRequests[i].state = 1;
                    return;
                }
            } else {
                sBootDmaCopyFunc(sBootDmaRequests[i].f40, sBootDmaRequests[i].f3c, sBootDmaRequests[i].f44);
                sBootDmaRequests[i].state = 3;
            }
        }
    }
}

s32 boot_dma_init(void) {
    s32 n;
    s32 *base;
    s32 *s0;
    s32 *s1;
    s32 *s2;

    osCreatePiManager(0x96, &sBootPiCmdQueue, sBootPiCmdMsgs, 8);
    n = 0;
    base = D_80017984;
    s2 = base - 1;
    s1 = base + 6;
    s0 = base;
    do {
        osCreateMesgQueue(s0, s1, 1);
        *s2 = 0;
        s2 += 19;
        s1 += 19;
        n++;
        s0 += 19;
    } while (n < 8);
    sBootCartHandle = osCartRomInit();
    return 0;
}
