#include "game.h"

/* Only in the retail versions (this is USA Rev 1's address; LodgeNet has no such object): whether the RSP's data
   memory starts with 0xFFFFFFFF, as the cartridge's IPL3 leaves it. shell_init turns the lag generator on if not. */
s32 shell_check_sp_dmem_magic(void)
{
    return *(u32 *)0xA4000000 == 0xFFFFFFFF;
}
