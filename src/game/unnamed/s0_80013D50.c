#include "game.h"

/* Whether the cartridge's CIC is a 6103 (osCicId 6103 = 0x17D7). Nothing calls it. */
s32 cic_is_6103(void)
{
    return osCicId == 0x17D7;
}
