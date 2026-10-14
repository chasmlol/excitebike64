#include "game.h"

s32 cic_check_rdram_200(void) {
    return D_A0000200 == 0xAC290000;
}
