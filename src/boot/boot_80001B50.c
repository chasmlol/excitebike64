#include "boot.h"

/* SN Systems' debug runtime: the console's side of the development board (SN_* in boot.h), reached through PI
   reads and writes of whole words. */

/* Clears the status register's interrupt enable bit; returns its old value. */
u32 sn_disable_ints(void)
{
    u32 sr;

    sr = sn_get_status_reg();
    sn_set_status_reg(sr & 0xFFFFFFFE);
    return sr & SR_IE;
}

/* Sets the interrupt enable bit `ie` (from sn_disable_ints) back. */
void sn_restore_ints(u32 ie)
{
    u32 sr;

    sr = sn_get_status_reg();
    sn_set_status_reg(sr | ie);
}

/* Writes a word to PI space once the PI is idle. */
void sn_pi_write_word(s32 *addr, s32 value)
{
    while ((PI_REG(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) != 0) {
    }
    *addr = value;
}

/* Reads a word of PI space once the PI is idle. */
u32 sn_pi_read_word(u32 *addr)
{
    while ((PI_REG(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) != 0) {
    }
    return *addr;
}

/* Reads a byte of PI space (from the word that holds it; big-endian). Two copies. */
u32 sn_pi_read_byte(u32 addr)
{
    u32 shift = (~addr & 3) << 3;
    u32 word = sn_pi_read_word(addr & ~3u);
    return (word >> shift) & 0xFF;
}

u32 sn_pi_read_byte_alt(u32 addr)
{
    u32 word;
    u32 shift;

    shift = (~addr & 3) << 3;
    word = sn_pi_read_word((void *)(addr & ~3));
    return (word >> shift) & 0xFF;
}

/* Copies `n` bytes of PI space at `src` to `dst`: single bytes up to a word boundary, then words. */
void sn_pi_read_bytes(u32 src, u8 *dst, u32 n)
{
    u32 word;
    u32 from = src;
    u8 *to = dst;
    u32 left = n;

    if (left == 0) {
        goto words;
    }
loop:
    if ((from & 3) == 0) {
        goto words;
    }
    *to = (u8)sn_pi_read_byte(from++);
    to++;
    left--;
    if (left != 0) {
        goto loop;
    }
words:
    while (left >= 4) {
        word = sn_pi_read_word(from);
        *to = (u8)(word >> 24);
        to++;
        *to = (u8)(word >> 16);
        to++;
        *to = (u8)(word >> 8);
        to++;
        *to = (u8)word;
        to++;
        from += 4;
        left -= 4;
    }
    while (left != 0) {
        *to = (u8)sn_pi_read_byte(from++);
        to++;
        left--;
    }
}

/* As sn_pi_read_bytes, but stores each byte with sn_write_byte_wordwise (for memory that only takes whole
   words). */
void sn_pi_read_bytes_wordwise(u32 src, u32 dst, u32 count)
{
    u32 from = src;
    u32 to = dst;
    u32 left = count;
    u32 word;

    while (left != 0 && (from & 3) != 0) {
        sn_write_byte_wordwise(to++, sn_pi_read_byte(from++) & 0xFF);
        left--;
    }
    while (left > 3) {
        word = sn_pi_read_word(from);
        sn_write_byte_wordwise(to++, word >> 24);
        sn_write_byte_wordwise(to++, (word >> 16) & 0xFF);
        sn_write_byte_wordwise(to++, (word >> 8) & 0xFF);
        sn_write_byte_wordwise(to++, word & 0xFF);
        from += 4;
        left -= 4;
    }
    while (left != 0) {
        sn_write_byte_wordwise(to++, sn_pi_read_byte(from++) & 0xFF);
        left--;
    }
}

/* Writes a byte of PI space by reading, changing and writing back the word that holds it. Two copies. */
void sn_pi_write_byte(u32 addr, u8 value)
{
    u32 shift;
    u32 word;

    shift = (~addr & 3) << 3;
    addr = addr & 0xFFFFFFFC;
    word = sn_pi_read_word((void *)addr);
    sn_pi_write_word((void *)addr, (word & ~(0xFFU << shift)) | ((u32)value << shift));
}

void sn_pi_write_byte_alt(u32 addr, u8 value)
{
    u32 shift;
    u32 word;

    shift = (~addr & 3) << 3;
    addr = addr & 0xFFFFFFFC;
    word = sn_pi_read_word(addr);
    sn_pi_write_word(addr, (word & ~(0xFFu << shift)) | (value << shift));
}

/* Fills `count` bytes of PI space at `dst` with the byte `value`. */
void sn_pi_fill(u8 *dst, u32 value, u32 count)
{
    u8 *to = dst;
    u32 left = count;
    u32 c = value;
    u32 word;
    while (left != 0 && ((u32)to & 3) != 0) {
        sn_pi_write_byte((u32)to++, c & 0xFF);
        left--;
    }
    if (left > 3) {
        word = ((c & 0xFF) << 24) | ((c & 0xFF) << 16) | ((c & 0xFF) << 8) | (c & 0xFF);
        do {
            sn_pi_write_word((u32)to, word);
            left -= 4;
            to += 4;
        } while (left > 3);
    }
    while (left != 0) {
        sn_pi_write_byte((u32)to++, c & 0xFF);
        left--;
    }
}

/* Copies `n` bytes from PI space to PI space, a byte at a time. */
void sn_pi_copy_bytes(u8 *dst, u8 *src, s32 n)
{
    s32 left;
    u8 buf[8];

    left = n - 1;
    while (left != -1) {
        sn_pi_write_byte(dst++, sn_pi_read_byte(src++) & 0xFF);
        left--;
    }
}

/* Host command 0x19: copies the words at the cartridge address the host gives (SN_REG_HOST_ADDR, SN_REG_HOST_SIZE)
   to that address masked with 0xB07FFFFF, once the ROM header's word at 0x10 reads non-zero, then clears the
   address register. */
void sn_host_copy_rom(void)
{
    u8 buf[8];
    u32 *src;
    u32 size;
    u32 i;

    src = (u32 *)(sn_pi_read_word(SN_REG_HOST_ADDR) & 0xB1FFFFFC);
    size = sn_pi_read_word(SN_REG_HOST_SIZE) & 0x01FFFFFC;
    sn_pi_write_word(SN_REG_ACK, 0);
    while (sn_pi_read_word(0xB0000010) == 0) {
        sn_delay(500);
    }
    for (i = 0; i < size >> 2; i++) {
        sn_pi_write_word((u32)&((u32 *)((u32)src & 0xB07FFFFF))[i], sn_pi_read_word((u32)&src[i]));
    }
    sn_delay(2000);
    sn_pi_write_word(SN_REG_HOST_ADDR, 0);
}

/* Returns the length of `s` counting its terminating 0. */
s32 sn_strlen(char *s)
{
    char ch;
    char *end;
    ch = *s;
    end = s;
    while (end = end + 1, ch != 0) {
        ch = *end;
    }
    return end - s;
}

/* Calls the host: command `cmd` (SN_PC_*) with three arguments, then waits until the host has cleared the command
   register, stopping in the debugger (break 0x400) while the host has a command of its own. The result is read
   last; sn_host_call_2 to _5 are this function under other prototypes, which return it. */
void sn_host_call(s32 cmd, s32 arg1, s32 arg2, s32 arg3)
{
    s32 status;

    sn_pi_write_word(SN_REG_ARG1, arg1);
    sn_pi_write_word(SN_REG_ARG2, arg2);
    sn_pi_write_word(SN_REG_ARG3, arg3);
    sn_pi_write_word(SN_REG_COMMAND, cmd);

    do {
        sn_delay(1000);
        status = sn_pi_read_word(SN_REG_HOST_CMD);
        if (status != 0) {
            __asm__ volatile("break 0x400");
        }
        status = sn_pi_read_word(SN_REG_COMMAND);
    } while (status != 0);

    sn_pi_read_word(SN_REG_RESULT);
}

/* PCopen: opens the host file `name`; returns its descriptor, negative on error (the 16-bit result sign-extended). */
s32 sn_pc_open(u8 *name, s32 mode)
{
    s32 ie;
    s32 len;
    s32 ret;

    ie = sn_disable_ints();
    len = sn_strlen(name);
    sn_pi_write_bytes((void *)SN_DATA_PORT, name, len);
    ret = sn_host_call_3(SN_PC_OPEN, len, mode, 0);
    if (ret & 0x8000) {
        ret |= 0xFFFF0000;
    }
    sn_restore_ints(ie);
    return ret;
}

/* PCcreat: creates the host file `name`, as sn_pc_open. */
s32 sn_pc_creat(u8 *name, s32 mode)
{
    s32 ie;
    s32 len;
    s32 ret;

    ie = sn_disable_ints();
    len = sn_strlen(name);
    sn_pi_write_bytes((void *)SN_DATA_PORT, name, len);
    ret = sn_host_call_3(SN_PC_CREAT, len, mode, 0);
    if (ret & 0x8000) {
        ret |= 0xFFFF0000;
    }
    sn_restore_ints(ie);
    return ret;
}

/* PCread: reads `size` bytes of the host file `fd` into `buf`, at most SN_DATA_PORT_SIZE at a time. Returns the
   bytes read. */
s32 sn_pc_read(s32 fd, u8 *buf, u32 size)
{
    u32 ie;
    u32 chunk;
    u32 got;
    s32 total;

    ie = sn_disable_ints();
    total = 0;
    while (size != 0) {
        chunk = size;
        if (SN_DATA_PORT_SIZE < size) {
            chunk = SN_DATA_PORT_SIZE;
        }
        do {
            got = sn_host_call_4(SN_PC_READ, fd, chunk, 0);
        } while (0);
        if (got == 0) {
            sn_restore_ints(ie);
            return total;
        }
        sn_pi_read_bytes(SN_DATA_PORT, buf, got);
        buf += got;
        size -= got;
        total += got;
    }
    sn_restore_ints(ie);
    return total;
}

/* PCwrite: writes `size` bytes of `buf` to the host file `fd`, at most SN_DATA_PORT_SIZE at a time. Returns the
   bytes written. */
s32 sn_pc_write(s32 fd, u8 *buf, u32 size)
{
    u32 ie;
    u32 chunk;
    s32 got;
    s32 total;

    ie = sn_disable_ints();
    total = 0;
    while (size != 0) {
        chunk = size;
        if (SN_DATA_PORT_SIZE < size) {
            chunk = SN_DATA_PORT_SIZE;
        }
        sn_pi_write_bytes((void *)SN_DATA_PORT, buf, chunk);
        do {
            got = sn_host_call_5(SN_PC_WRITE, fd, chunk, 0);
        } while (0);
        if (got == 0) {
            sn_restore_ints(ie);
            return total;
        }
        buf += got;
        size -= got;
        total += got;
    }
    sn_restore_ints(ie);
    return total;
}

/* PClseek: moves the position in the host file `fd`; returns the new position. */
u32 sn_pc_lseek(u32 fd, u32 offset, u32 whence)
{
    u32 ie;
    u32 result;

    ie = sn_disable_ints();
    result = sn_host_call_2(SN_PC_LSEEK, fd, offset, whence);
    sn_restore_ints_2(ie);
    return result;
}
