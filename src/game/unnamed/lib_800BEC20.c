#include "game.h"

/* PI DMA requests in DMA_SLOT_COUNT slots (sDmaRequests): dma_start queues one, dma_update starts queued ones and
   marks finished ones done, and gDmaIdleCallback (the sound streams) runs while the game waits. The boot loader links
   dma_start, dma_get_status, dma_release and dma_wait again, built from this file. The slot structs below are views
   of the same 0x4C bytes, each declared the way its function's code needs. */

typedef struct DmaSlotWords {
    s32 state;                                              /* 0x00: DMA_* */
    s32 queue, queue08, queue0C, queue10, queue14, queue18; /* 0x04: the slot's OSMesgQueue */
    s32 msg;                                                /* 0x1C: the queue's one message */
    u8 ioHdr[4];                                            /* 0x20: the OSIoMesg: hdr.type, hdr.pri, hdr.status */
    s32 ioRetQueue;                                         /* 0x24 */
    s32 ioDramAddr;
    s32 ioDevAddr;
    s32 ioSize;
    s32 ioPiHandle;                                         /* 0x34: set by dma_epi_start */
    s32 handle;                                             /* 0x38: dma_start's arguments */
    s32 dramAddr;
    s32 devAddr;
    s32 size;
    s32 direction;
} DmaSlotWords;

/* Queues a DMA of `size` bytes between `devAddr` on the PI device `handle` and `dramAddr` (`direction` OS_READ or
   OS_WRITE) in the first free slot. Returns the slot, or -2 when none is free. */
s32 dma_start(s32 handle, s32 direction, s32 devAddr, s32 dramAddr, s32 size)
{
    s32 i;
    s32 offset;
    s32 queues;
    DmaSlotWords *slots;
    DmaSlotWords *slot;
    DmaSlotWords *req;

    dma_update();
    if (gDmaIdleCallback != 0) {
        gDmaIdleCallback();
    }
    i = 0;
    slots = sDmaRequests;
    slot = slots;
    for (; i < DMA_SLOT_COUNT; i++) {
        if (slot->state == DMA_FREE) {
            break;
        }
        slot++;
    }
    if (i == DMA_SLOT_COUNT) {
        return -2;
    }
    offset = i * DMA_SLOT_SIZE;
    req = (DmaSlotWords *)(offset + (s32)slots);
    req->dramAddr = dramAddr;
    req->devAddr = devAddr;
    req->size = size;
    req->handle = handle;
    req->direction = direction;
    req->ioHdr[2] = OS_MESG_PRI_NORMAL;
    queues = (s32)&slots->queue;
    req->ioRetQueue = offset + queues;
    req->state = DMA_QUEUED;
    req->ioDramAddr = req->dramAddr;
    req->ioDevAddr = req->devAddr;
    req->ioSize = req->size;
    return i;
}

typedef struct {
    s32 state;          /* 0x00 */
    u8 queue[0x1C];     /* 0x04: the OSMesgQueue and its message */
    u8 ioMesg[2];       /* 0x20: the OSIoMesg, from hdr.type */
    u8 ioPri;           /* 0x22 */
    u8 ioStatus;        /* 0x23 */
    void *ioRetQueue;   /* 0x24 */
    s32 ioDramAddr, ioDevAddr, ioSize, ioPiHandle;
    s32 handle, dramAddr, devAddr, size, direction;
} DmaRequest;

/* A copy routine that replaces the DMAs when set: (source, destination, size). */
typedef void (*DmaCopyFn)(s32, s32, s32);

extern DmaRequest sDmaRequests_2[];
extern DmaCopyFn sDmaCopyFunc;

/* Marks running requests whose message has arrived as done and starts queued ones: through sDmaCopyFunc when it is
   set (finished at once), else with dma_epi_start. It returns at the first request still running or just started,
   so one DMA runs at a time. */
void dma_update(void)
{
    s32 i;

    sDmaPendingCount = 0;
    sDmaStartedCount = 0;
    for (i = 0; i < DMA_SLOT_COUNT; i++) {
        if (sDmaRequests_2[i].state == DMA_RUNNING) {
            if (osRecvMesg(sDmaRequests_2[i].queue, NULL, OS_MESG_NOBLOCK) == 0) {
                sDmaRequests_2[i].state = DMA_DONE;
            } else {
                sDmaPendingCount++;
                return;
            }
        } else if (sDmaRequests_2[i].state == DMA_QUEUED) {
            if (sDmaCopyFunc == 0) {
                sDmaRequests_2[i].ioRetQueue = sDmaRequests_2[i].queue;
                sDmaStartedCount++;
                sDmaRequests_2[i].ioPri = OS_MESG_PRI_NORMAL;
                sDmaRequests_2[i].ioDramAddr = sDmaRequests_2[i].dramAddr;
                sDmaRequests_2[i].ioDevAddr = sDmaRequests_2[i].devAddr;
                sDmaRequests_2[i].ioSize = sDmaRequests_2[i].size;
                main_on_dma_start();
                if (dma_epi_start(sDmaRequests_2[i].handle, sDmaRequests_2[i].ioMesg,
                                  sDmaRequests_2[i].direction) == 0) {
                    sDmaRequests_2[i].state = DMA_RUNNING;
                    return;
                }
            } else {
                sDmaCopyFunc(sDmaRequests_2[i].devAddr, sDmaRequests_2[i].dramAddr, sDmaRequests_2[i].size);
                sDmaRequests_2[i].state = DMA_DONE;
            }
        }
    }
}

/* Starts the PI manager, creates each slot's message queue (D_80100724 is sDmaRequests[0]'s) and sets up the
   cartridge and SRAM handles. */
s32 dma_init(void)
{
    s32 i;
    u8 *firstQueue;
    s32 *state;
    u8 *msg;
    u8 *queue;

    osCreatePiManager(OS_PRIORITY_PIMGR, &sPiCmdQueue, sPiCmdMsgs, 8);
    i = 0;
    firstQueue = D_80100724;
    state = (s32 *)(firstQueue - 4);
    msg = firstQueue + 0x18;
    queue = firstQueue;
    do {
        osCreateMesgQueue(queue, msg, 1);
        *state = DMA_FREE;
        state = (s32 *)((u8 *)state + DMA_SLOT_SIZE);
        msg += DMA_SLOT_SIZE;
        i++;
        queue += DMA_SLOT_SIZE;
    } while (i < DMA_SLOT_COUNT);
    gCartRomHandle = dma_cart_rom_init();
    sSramHandle = (s32)sram_init();
    return 0;
}

/* Runs the idle callback and dma_update, then returns the slot's state (DMA_*). */
s32 dma_get_status(s32 slot)
{
    if (gDmaIdleCallback != NULL) {
        gDmaIdleCallback();
    }
    dma_update();
    return sDmaRequests[slot * DMA_SLOT_WORDS];
}

/* Frees a slot. */
void dma_release(s32 slot)
{
    sDmaRequests[slot * DMA_SLOT_WORDS] = DMA_FREE;
}

typedef struct {
    s32 state;
    u8 pad[0x48];
} DmaRequestState;

extern DmaRequestState sDmaRequests_3[];

/* Waits (running the idle callback and dma_update) until the request is done, then frees its slot. */
void dma_wait(s32 slot)
{
    do {
        if (gDmaIdleCallback != NULL) {
            gDmaIdleCallback();
        }
        dma_update();
    } while (sDmaRequests_3[slot].state != DMA_DONE && sDmaRequests_3[slot].state != DMA_FREE);
    sDmaRequests_3[slot].state = DMA_FREE;
}
