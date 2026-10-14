#include "boot.h"

/* SN Systems' debug runtime, continued: the host's memory access and the command loop. */

/* Busy-waits `count` steps of the volatile sSnDelayCounter. */
void sn_delay(s32 count)
{
    sSnDelayCounter = 0;
    while (sSnDelayCounter < count) {
        sSnDelayCounter = sSnDelayCounter + 1;
        sSnDelayCounter;
    }
}

/* Writes a byte by reading, changing and writing back the word that holds it (big-endian). */
void sn_write_byte_wordwise(u32 addr, u32 val)
{
    u32 shift = (~addr & 3) << 3;
    u32 *word = (u32 *)(addr & ~3u);
    *word = (*word & ~(0xFFu << shift)) | ((val & 0xFFu) << shift);
}

/* Copies `n` bytes from `src` to PI space at `dst`: single bytes up to a word boundary, then words. */
void sn_pi_write_bytes(u8 *dst, u8 *src, u32 n)
{
    u8 *tail;
    u8 c;

    while (n != 0 && ((u32)dst & 3) != 0) {
        sn_pi_write_byte((u32)dst++, *src++);
        n = n - 1;
    }
    if (n > 3) {
        do {
            tail = src + 3;
            sn_pi_write_word((u32 *)dst, ((u32)src[0] << 24) | ((u32)tail[-2] << 16) | ((u32)tail[-1] << 8) | tail[0]);
            dst = dst + 4;
            src = src + 4;
            n = n - 4;
        } while (n > 3);
    }
    while (n != 0) {
        sn_pi_write_byte((u32)dst++, *src++);
        n = n - 1;
    }
}

/* The last address and the flags (SN_MEM_*) of the region at byte `o` of sSnMemRegions. */
#define REGION_END(o) (*(u32 *)((u32)sSnMemRegions + (o)))
#define REGION_FLAGS(o) (*(u32 *)((u32)D_8000F4A4 + (o)))

/* Host commands 0x17 and 0x18: the host writes (SN_MEM_WRITE) or reads (SN_MEM_READ) memory at SN_REG_HOST_ADDR,
   SN_REG_HOST_SIZE bytes, through SN_MEM_PORT. Each region of sSnMemRegions is accessed as its flags say; a region the
   host may not read reads as zeros, one it may not write is left alone. */
void sn_host_mem_access(u32 mode)
{
    u32 port = SN_MEM_PORT;
    u32 addr = sn_pi_read_word(SN_REG_HOST_ADDR);
    u32 left = sn_pi_read_word(SN_REG_HOST_SIZE);
    u32 region, offset, len, flags, before;

    if (addr > 0xEFFFFFFF)
        addr += (u32)D_8F018110;

    while (left != 0) {
        /* The region that holds addr, then as much of the access as it holds. */
        region = 0;
        if (REGION_END(0) < addr) {
            offset = 0;
            do {
                region++;
                offset = region * 8;
            } while (REGION_END(offset) < addr);
        }
        offset = region * 8;
        before = addr - 1;
        len = REGION_END(offset) - before;
        if (left < len)
            len = left;
        flags = REGION_FLAGS(offset);
        if (!(flags & mode)) {
            if (mode == SN_MEM_READ)
                sn_pi_fill(port, 0, len);
        } else if (flags & SN_MEM_PI) {
            if (mode == SN_MEM_READ)
                sn_pi_copy_bytes(port, addr, len);
            else
                sn_pi_copy_bytes(addr, port, len);
        } else if (mode == SN_MEM_READ) {
            sn_pi_write_bytes(port, addr, len);
        } else if (flags & SN_MEM_WORDS) {
            sn_pi_read_bytes_wordwise(port, addr, len);
        } else {
            sn_pi_read_bytes(port, addr, len);
        }
        port += len;
        left -= len;
        addr += len;
    }
}

/* Installs the exception hooks, clears the board's command registers and sets the RDRAM regions of sSnMemRegions
   (KSEG0 and KSEG1, D_8000F4A8 and D_8000F4B8) to end at the memory size, if it is a multiple of 128 KB from 4 MB
   to 32 MB. */
void sn_debug_init(void)
{
    u32 ie;
    u32 memSize;

    ie = sn_disable_ints();
    sn_install_exception_hooks();
    sn_pi_write_word(SN_REG_HOST_CMD, 0);
    sn_pi_write_word(SN_REG_ACK, 0);
    D_80018430 = 1;
    D_80018432 = 0xFF;
    memSize = osMemSize;
    if (memSize > 0x3FFFFF && (memSize & 0x1FFFF) == 0 && memSize < 0x2000001) {
        memSize = memSize - 1;
        D_8000F4A8 = memSize - 0x80000000;
        D_8000F4B8 = memSize + 0xA0000000;
    }
    sn_restore_ints(ie);
}

/* Serves the host's commands (SN_REG_HOST_CMD): answers each with 0x101 in SN_REG_ACK, runs it, and answers 0 when
   done; 2 cancels. Command 0x11 leaves the loop, once D_80018432 is set. 0x12-0x15 set or clear the flags
   D_80018430 / D_80018432 (0x12 also clears bit 0 of D_80018288 and the status register's interrupt enable bit),
   0x16 reports the two flags, 0x17 and 0x18 write and read memory, 0x19 copies ROM. */
void sn_debug_command_loop(void)
{
    s32 stay;
    s32 cmd;
    s32 status;
    u32 sr;

    do {
        stay = 1;
        while ((cmd = sn_pi_read_word(SN_REG_HOST_CMD)) == 0) {
            sn_delay(1000);
        }
        if (cmd == 2) {
            sn_pi_write_word(SN_REG_ACK, 0);
            sn_delay(1000);
            goto check_leave;
        }
        sn_pi_write_word(SN_REG_ACK, 0x101);
        while ((status = sn_pi_read_word(SN_REG_HOST_CMD)) == cmd) {
            sn_delay(500);
        }
        status = sn_pi_read_word(SN_REG_HOST_CMD);
        if (status == 2) {
            sn_pi_write_word(SN_REG_ACK, 0);
            goto check_leave;
        }
        switch (cmd) {
        case 0x10:
        default:
            break;
        case 0x11:
            stay = 0;
            break;
        case 0x12:
            D_80018430 = 0;
            D_80018432 = 0;
            D_80018288 = D_80018288 & 0xfffffffe;
            sr = sn_get_status_reg();
            sn_set_status_reg(sr & 0xfffffffe);
            break;
        case 0x13:
            D_80018432 = 0;
            D_80018430 = 0;
            break;
        case 0x14:
            D_80018432 = 0xff;
            break;
        case 0x15:
            D_80018432 = 0;
            break;
        case 0x16:
            sn_pi_write_word(SN_MEM_PORT, (u32)D_80018432 << 24 | (u32)D_80018430 << 16);
            break;
        case 0x17:
            sn_host_mem_access(SN_MEM_WRITE);
            break;
        case 0x18:
            sn_host_mem_access(SN_MEM_READ);
            break;
        case 0x19:
            sn_host_copy_rom();
            break;
        }
        sn_pi_write_word(SN_REG_ACK, 0);
        while ((cmd = sn_pi_read_word(SN_REG_HOST_CMD)) == 1) {
            sn_delay(500);
        }
check_leave:
        if (!stay && D_80018432 != 0) {
            sn_pi_write_word(SN_REG_HOST_CMD, 0);
            return;
        }
    } while (1);
}

/* PCinit: returns the host's result. */
u32 sn_pc_init(void)
{
    u32 ie = sn_disable_ints();
    u32 result = sn_host_call_2(SN_PC_INIT, 0, 0, 0);
    sn_restore_ints_2(ie);
    return result;
}

/* PCclose: closes the host file `fd`; returns the result, sign-extended from 16 bits. */
u32 sn_pc_close(u32 fd)
{
    u32 ie, result;
    ie = sn_disable_ints();
    result = sn_host_call_2(SN_PC_CLOSE, fd, 0, 0);
    if ((result & 0x8000) != 0) {
        result = result | 0xffff0000;
    }
    sn_restore_ints(ie);
    return result;
}
