#include "boot.h"

void boot_log_write(u8 *param_1, u32 param_2)
{
    if (sBootLogFd == -1) {
        sBootLogFd = sn_pc_open(sBootLogFileName, 0);
    }
    sn_pc_write(sBootLogFd, param_1, param_2);
}

void boot_log_print(u8 *p)
{
    u32 n;

    if (sBootLogFd == -1) {
        sBootLogFd = sn_pc_open(sBootLogFileName, 0);
    }
    n = strlen((char *)p);
    sn_pc_write(sBootLogFd, p, n);
}
