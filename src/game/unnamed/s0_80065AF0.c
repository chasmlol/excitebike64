#include "game.h"

/* Whether the IPL3 left the instruction sw $t1, 0($at) (0xAC290000) at 0xA0000200 of RDRAM. Nothing calls it. */
s32 cic_check_rdram_200(void) {
    return D_A0000200 == 0xAC290000;
}
