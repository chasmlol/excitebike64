#include "boot.h"

/* Writes `size` bytes of `buf` to the log file on the host (sBootLogFileName, "output.log"), opening it the first
   time. */
void boot_log_write(u8 *buf, u32 size)
{
    if (sBootLogFd == -1) {
        sBootLogFd = sn_pc_open(sBootLogFileName, 0);
    }
    sn_pc_write(sBootLogFd, buf, size);
}

/* Writes the string `str` to the log file on the host. */
void boot_log_print(u8 *str)
{
    u32 len;

    if (sBootLogFd == -1) {
        sBootLogFd = sn_pc_open(sBootLogFileName, 0);
    }
    len = strlen((char *)str);
    sn_pc_write(sBootLogFd, str, len);
}
