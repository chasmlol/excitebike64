#include "game.h"

s32 dma_epi_start(s32 a0, u16 *a1, s32 a2)
{
    void *p;
    s32 r;
    u16 t;

    if (__osPiDevMgr == 0) {
        return -1;
    }
    *(s32 *)(a1 + 10) = a0;
    if (a2 != 0) {
        goto L10;
    }
    t = 0xF;
    goto join;
L10:
    __asm__ __volatile__("" ::: "memory");
    t = 0x10;
join:
    a1[0] = t;
    __asm__ __volatile__("" ::: "memory");
    if (*((u8 *)a1 + 2) == 1) {
        p = dma_pi_get_cmd_queue();
        r = dma_jam_mesg(p, a1, 0);
    } else {
        p = dma_pi_get_cmd_queue();
        r = osSendMesg(p, a1, 0);
    }
    return r;
}
