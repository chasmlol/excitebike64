#include "game.h"

/* libultra's osJamMesg, compiled with SN's compiler: puts `msg` at the front of the message queue `mq` (Q_3 is an
   OSMesgQueue: p0 mtqueue, f4 fullqueue, f8 validCount, f0c first, f10 msgCount, f14 msg). While the queue is full
   it waits with OS_MESG_BLOCK, else returns -1. Then it wakes the first thread waiting for a message. */
s32 dma_jam_mesg(Q_3 *mq, s32 msg, s32 flag)
{
    s32 saveMask;
    s32 slot;
    s32 blockFlag;

    blockFlag = flag;
    saveMask = __osDisableInt();
    if (mq->f8 >= mq->f10) {
        do {
            if (blockFlag != OS_MESG_BLOCK) {
                __osRestoreInt(saveMask);
                return -1;
            }
            /* __osRunningThread->state */
            *(u16 *)((u8 *)*(s32 **)0x800D1F40 + 0x10) = OS_STATE_WAITING;
            __osEnqueueAndYield(&mq->f4);
        } while (mq->f8 >= mq->f10);
    }
    slot = (mq->f0c + mq->f10 - 1) % mq->f10;
    mq->f0c = slot;
    mq->f14[slot] = msg;
    mq->f8 = mq->f8 + 1;
    if (*mq->p0 != 0) { /* mtqueue->next */
        s32 *thread = __osPopThread(mq);
        osStartThread(thread);
    }
    __osRestoreInt(saveMask);
    return 0;
}
