#include "game.h"

s32 cic_check_rdram_284(void) {
    return D_A0000284 == 0x240B17D7;
}
