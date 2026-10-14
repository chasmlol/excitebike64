#include "game.h"

/* libultra's osEPiStartDma, compiled with SN's compiler: sends the I/O message `ioMesg` (an OSIoMesg) for the PI
   device `handle` to the PI manager as an EDMA read or write, at the front of its queue when the message's priority
   is high. Returns -1 when the PI manager is not running. The empty asm statements keep the original's order. */
s32 dma_epi_start(s32 handle, u16 *ioMesg, s32 direction)
{
    void *cmdQueue;
    s32 result;
    u16 type;

    if (__osPiDevMgr == 0) { /* __osPiDevMgr.active */
        return -1;
    }
    *(s32 *)(ioMesg + 10) = handle; /* ioMesg->piHandle */
    if (direction != OS_READ) {
        goto write;
    }
    type = OS_MESG_TYPE_EDMAREAD;
    goto set_type;
write:
    __asm__ __volatile__("" ::: "memory");
    type = OS_MESG_TYPE_EDMAWRITE;
set_type:
    ioMesg[0] = type; /* hdr.type */
    __asm__ __volatile__("" ::: "memory");
    if (*((u8 *)ioMesg + 2) == OS_MESG_PRI_HIGH) { /* hdr.pri */
        cmdQueue = dma_pi_get_cmd_queue();
        result = dma_jam_mesg(cmdQueue, ioMesg, OS_MESG_NOBLOCK);
    } else {
        cmdQueue = dma_pi_get_cmd_queue();
        result = osSendMesg(cmdQueue, ioMesg, OS_MESG_NOBLOCK);
    }
    return result;
}
