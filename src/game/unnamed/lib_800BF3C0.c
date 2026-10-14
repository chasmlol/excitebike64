#include "game.h"

/* SRAM: a PI handle and whole-SRAM reads and writes. dma_init sets up the handle; nothing calls sram_read or
   sram_write. */

/* An OSPiHandle (sSramPiHandle). */
struct Blk_29 {
    s32 next;
    u8 type, latency, pageSize, relDuration, pulse, domain;
    u8 pad[2];
    u32 baseAddress;
    u32 speed;
    u8 transferInfo[0x50]; /* libultra's is 0x60 bytes, which sram_init clears */
};

/* Sets up the SRAM handle the first time (until then its base address is not set) and links it into
   __osPiTable. */
Blk_29 *sram_init(void)
{
    u32 saveMask;
    if (sSramPiHandle.baseAddress != PHYS_TO_K1(PI_DOM2_ADDR2)) {
        sSramPiHandle.type = DEVICE_TYPE_SRAM;
        sSramPiHandle.latency = SRAM_LATENCY;
        sSramPiHandle.pulse = SRAM_PULSE;
        sSramPiHandle.pageSize = SRAM_PAGE_SIZE;
        sSramPiHandle.relDuration = SRAM_REL_DURATION;
        sSramPiHandle.baseAddress = PHYS_TO_K1(PI_DOM2_ADDR2);
        sSramPiHandle.domain = PI_DOMAIN2;
        mem_fill(sSramPiHandle.transferInfo, 0, 0x60);
        saveMask = __osDisableInt();
        sSramPiHandle.next = __osPiTable;
        __osPiTable = (s32)&sSramPiHandle;
        __osRestoreInt(saveMask);
    }
    return &sSramPiHandle;
}

/* Reads all of SRAM into `buf`, meant to return 1 when no DMA slot is free and 0 when done. dma_start's result is
   never stored: `req` is read uninitialised (a bug of the original). */
int sram_read(u32 buf)
{
    s32 req;

    osInvalDCache(buf, SRAM_SIZE);
    dma_start(sSramHandle, OS_READ, PI_DOM2_ADDR2, buf, SRAM_SIZE);
    if (req < 0) {
        return 1;
    }
    dma_wait(req);
    return 0;
}

/* Writes `buf` to all of SRAM, with the same bug as sram_read. */
s32 sram_write(s32 buf)
{
    s32 req;

    osWritebackDCache(buf, SRAM_SIZE);
    dma_start(sSramHandle, OS_WRITE, PI_DOM2_ADDR2, buf, SRAM_SIZE);
    if (req < 0) {
        return 1;
    }
    dma_wait(req);
    return 0;
}
