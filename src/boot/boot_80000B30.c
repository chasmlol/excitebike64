#include "boot.h"

/* The boot loader's own dma_update and dma_init (its dma_start, dma_get_status, dma_release and dma_wait are built
   from the game's unnamed/lib_800BEC20.c). A request slot, as dma_update reads it: */
typedef struct BootDmaRequest {
    s32 state;          /* 0x00: BOOT_DMA_* */
    u8 queue[0x1C];     /* 0x04: the OSMesgQueue and its message */
    u8 ioMesg[2];       /* 0x20: the OSIoMesg, from hdr.type */
    u8 ioPri;           /* 0x22 */
    u8 ioStatus;        /* 0x23 */
    u8 *ioRetQueue;     /* 0x24 */
    s32 ioDramAddr;     /* 0x28 */
    s32 ioDevAddr;      /* 0x2C */
    s32 ioSize;         /* 0x30 */
    s32 ioPiHandle;     /* 0x34 */
    s32 handle;         /* 0x38: boot_dma_start's arguments */
    s32 dramAddr;       /* 0x3C */
    s32 devAddr;        /* 0x40 */
    s32 size;           /* 0x44 */
    s32 direction;      /* 0x48 */
} BootDmaRequest;

extern BootDmaRequest sBootDmaRequests[];

/* Marks running requests whose message has arrived as done and starts queued ones (through sBootDmaCopyFunc when it
   is set), returning at the first one still running or just started. */
void boot_dma_update(void)
{
    s32 i;
    s32 result;

    sBootDmaPendingCount = 0;
    sBootDmaStartedCount = 0;
    for (i = 0; i < BOOT_DMA_SLOT_COUNT; i++) {
        if (sBootDmaRequests[i].state == BOOT_DMA_RUNNING) {
            result = osRecvMesg(sBootDmaRequests[i].queue, NULL, OS_MESG_NOBLOCK);
            if (result == 0) {
                sBootDmaRequests[i].state = BOOT_DMA_DONE;
            } else {
                sBootDmaPendingCount = sBootDmaPendingCount + 1;
                return;
            }
        } else if (sBootDmaRequests[i].state == BOOT_DMA_QUEUED) {
            if (sBootDmaCopyFunc == 0) {
                sBootDmaRequests[i].ioRetQueue = sBootDmaRequests[i].queue;
                sBootDmaStartedCount = sBootDmaStartedCount + 1;
                sBootDmaRequests[i].ioPri = OS_MESG_PRI_NORMAL;
                sBootDmaRequests[i].ioDramAddr = sBootDmaRequests[i].dramAddr;
                sBootDmaRequests[i].ioDevAddr = sBootDmaRequests[i].devAddr;
                sBootDmaRequests[i].ioSize = sBootDmaRequests[i].size;
                result = osEPiStartDma(sBootDmaRequests[i].handle, sBootDmaRequests[i].ioMesg,
                                       sBootDmaRequests[i].direction);
                if (result == 0) {
                    sBootDmaRequests[i].state = BOOT_DMA_RUNNING;
                    return;
                }
            } else {
                sBootDmaCopyFunc(sBootDmaRequests[i].devAddr, sBootDmaRequests[i].dramAddr, sBootDmaRequests[i].size);
                sBootDmaRequests[i].state = BOOT_DMA_DONE;
            }
        }
    }
}

/* Starts the PI manager, creates each slot's message queue (D_80017984 is sBootDmaRequests[0]'s) and sets up the
   cartridge handle. */
s32 boot_dma_init(void)
{
    s32 i;
    s32 *firstQueue;
    s32 *queue;
    s32 *msg;
    s32 *state;

    osCreatePiManager(OS_PRIORITY_PIMGR, &sBootPiCmdQueue, sBootPiCmdMsgs, 8);
    i = 0;
    firstQueue = D_80017984;
    state = firstQueue - 1;
    msg = firstQueue + 6;
    queue = firstQueue;
    do {
        osCreateMesgQueue(queue, msg, 1);
        *state = BOOT_DMA_FREE;
        state += BOOT_DMA_SLOT_WORDS;
        msg += BOOT_DMA_SLOT_WORDS;
        i++;
        queue += BOOT_DMA_SLOT_WORDS;
    } while (i < BOOT_DMA_SLOT_COUNT);
    sBootCartHandle = osCartRomInit();
    return 0;
}
