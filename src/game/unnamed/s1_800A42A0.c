#include "game.h"

/* Whether the IPL3 left the instruction li $t3, 0x17D7 (0x240B17D7; 0x17D7 = 6103, the CIC) at 0xA0000284 of RDRAM.
   Nothing calls it. */
s32 cic_check_rdram_284(void)
{
    return D_A0000284 == 0x240B17D7;
}
