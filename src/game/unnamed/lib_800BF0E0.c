#include "game.h"

/* libultra's osPiGetCmdQueue, compiled with SN's compiler: the PI manager's command queue
   (__osPiDevMgr.cmdQueue, 0x800D1DF8), or NULL when it is not running (__osPiDevMgr.active, 0x800D1DF0).
   Numeric addresses (0x800D1DF0 / 0x800D1DF8) give one 'lw' macro each (lui+lw in the same reg);
   'queue + active - active' references `active` in the then-arm so flow marks it global (combine drops the use
   later), global-alloc then gives `queue` its $v0 preference and the flag gets $v1. */
s32 dma_pi_get_cmd_queue(void)
{
    s32 active = *(s32 *)0x800D1DF0;
    s32 queue = 0;
    if (active != 0) {
        queue = *(s32 *)0x800D1DF8 + active - active;
    }
    return queue;
}

/* A PI register, through KSEG1. */
#define PI_REG(reg) (*(volatile u32 *)PHYS_TO_K1(reg))

/* libultra's newer osCartRomInit, compiled with SN's compiler. The first call sets up the cartridge handle
   sCartRomHandleData (0x801009B0, an OSPiHandle): domain 1 at 0xB0000000 with the PI timings the ROM's first word
   gives (read with slow timings), linked into __osPiTable (0x800D1E0C). Returns the handle. The asm statements
   (keep them as they are) build the addresses the way the original does: pb4 is &handle.type, pb
   &handle.baseAddress, p0 the handle itself. */
u32 *dma_cart_rom_init(void)
{
    u32 timings;
    u32 baseAddr;
    u32 latency;
    u32 pageSize;
    u32 relDuration;
    u32 pulse;
    u32 old;
    u32 saveMask;
    u8 *pb4;
    u8 *tranxInfo;
    u32 *pb;
    register u32 *p0 asm("$16");

    __osPiGetAccess();
    if (sCartRomInitFirst != 0) {
        __asm__ __volatile__("lui %0, 0x8010
	addiu %0, %0, 0x9B4" : "=r"(pb4));
        tranxInfo = pb4 + 0x10;
        *(s32 *)0x800D5720 = 0;             /* sCartRomInitFirst */
        *pb4 = DEVICE_TYPE_CART;
        *(u32 *)0x801009BC = 0xB0000000;    /* baseAddress: PI_DOM1_ADDR2 through KSEG1 */
        *(u8 *)0x801009B9 = PI_DOMAIN1;     /* domain */
        *(s32 *)0x801009C0 = 0;             /* speed */
        bzero(tranxInfo, 0x60);
        while ((PI_REG(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) != 0) {
        }
        latency = PI_REG(PI_BSD_DOM1_LAT_REG);
        pageSize = PI_REG(PI_BSD_DOM1_PGS_REG);
        relDuration = PI_REG(PI_BSD_DOM1_RLS_REG);
        pulse = PI_REG(PI_BSD_DOM1_PWD_REG);
        __asm__ __volatile__("lui %0, 0x8010
	addiu %0, %0, 0x9BC" : "=r"(pb));
        baseAddr = *pb;
        __asm__ __volatile__("");
        /* Read the ROM's first word with slow timings, then restore them. */
        PI_REG(PI_BSD_DOM1_LAT_REG) = 0xFF;
        PI_REG(PI_BSD_DOM1_PGS_REG) = 0;
        PI_REG(PI_BSD_DOM1_RLS_REG) = 3;
        PI_REG(PI_BSD_DOM1_PWD_REG) = 0xFF;
        timings = *(u32 *)(baseAddr | 0xA0000000);
        PI_REG(PI_BSD_DOM1_LAT_REG) = latency;
        PI_REG(PI_BSD_DOM1_PGS_REG) = pageSize;
        PI_REG(PI_BSD_DOM1_RLS_REG) = relDuration;
        PI_REG(PI_BSD_DOM1_PWD_REG) = pulse;
        *(u8 *)0x801009B5 = (u8)timings;                /* latency */
        *(u8 *)0x801009B6 = (timings >> 16) & 0xF;      /* pageSize */
        *(u8 *)0x801009B7 = (timings >> 20) & 0xF;      /* relDuration */
        *(u8 *)0x801009B8 = (u8)(timings >> 8);         /* pulse */
        __asm__ ("" : "+r"(pb));
        p0 = pb - 3;
        saveMask = __osDisableInt();
        __asm__ __volatile__("lw %0, 0x800D1E0C" : "=r"(old));
        *(u32 *)0x800D1E0C = (u32)p0;   /* __osPiTable = handle */
        pb[-3] = old;                   /* handle.next */
        __osRestoreInt(saveMask);
    }
    __osPiRelAccess();
    {
        u32 *r = &sCartRomHandleData;
        __asm__ __volatile__("" : "+r"(r));
        return r;
    }
}
