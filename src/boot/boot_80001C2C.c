#include "boot.h"

void sn_delay(s32 param_1) {
    sSnDelayCounter = 0;
    while (sSnDelayCounter < param_1) {
        sSnDelayCounter = sSnDelayCounter + 1;
        sSnDelayCounter;
    }
}

void sn_write_byte_wordwise(u32 addr, u32 val)
{
    u32 sh = (~addr & 3) << 3;
    u32 *p = (u32 *)(addr & ~3u);
    *p = (*p & ~(0xFFu << sh)) | ((val & 0xFFu) << sh);
}

void sn_pi_write_bytes(u8 *d, u8 *s, u32 n) {
    u8 *p6;
    u8 c;

    while (n != 0 && ((u32)d & 3) != 0) {
        sn_pi_write_byte((u32)d++, *s++);
        n = n - 1;
    }
    if (n > 3) {
        do {
            p6 = s + 3;
            sn_pi_write_word((u32 *)d, ((u32)s[0] << 24) | ((u32)p6[-2] << 16) | ((u32)p6[-1] << 8) | p6[0]);
            d = d + 4;
            s = s + 4;
            n = n - 4;
        } while (n > 3);
    }
    while (n != 0) {
        sn_pi_write_byte((u32)d++, *s++);
        n = n - 1;
    }
}

#define TB(o) (*(u32 *)((u32)sSnMemRegions + (o)))
#define TF(o) (*(u32 *)((u32)D_8000F4A4 + (o)))

void sn_host_mem_access(u32 p)
{
    u32 dst = 0xB1FE0000;
    u32 src = sn_pi_read_word(0xB1FFFFF4);
    u32 n = sn_pi_read_word(0xB1FFFFF8);
    u32 k, o, len, flags, sm;

    if (src > 0xEFFFFFFF)
        src += (u32)D_8F018110;

    while (n != 0) {
        k = 0;
        if (TB(0) < src) {
            o = 0;
            do {
                k++;
                o = k * 8;
            } while (TB(o) < src);
        }
        o = k * 8;
        sm = src - 1;
        len = TB(o) - sm;
        if (n < len)
            len = n;
        flags = TF(o);
        if (!(flags & p)) {
            if (p == 1)
                sn_pi_fill(dst, 0, len);
        } else if (flags & 4) {
            if (p == 1)
                sn_pi_copy_bytes(dst, src, len);
            else
                sn_pi_copy_bytes(src, dst, len);
        } else if (p == 1) {
            sn_pi_write_bytes(dst, src, len);
        } else if (flags & 8) {
            sn_pi_read_bytes_wordwise(dst, src, len);
        } else {
            sn_pi_read_bytes(dst, src, len);
        }
        dst += len;
        n -= len;
        src += len;
    }
}

void sn_debug_init(void)
{
    u32 r;
    u32 x;

    r = sn_disable_ints();
    sn_install_exception_hooks();
    sn_pi_write_word(0xB1FFFFF0, 0);
    sn_pi_write_word(0xB1FFFFFC, 0);
    D_80018430 = 1;
    D_80018432 = 0xFF;
    x = osMemSize;
    if (x > 0x3FFFFF && (x & 0x1FFFF) == 0 && x < 0x2000001) {
        x = x - 1;
        D_8000F4A8 = x - 0x80000000;
        D_8000F4B8 = x + 0xA0000000;
    }
    sn_restore_ints(r);
}

void sn_debug_command_loop(void)
{
    s32 bVar1;
    s32 iVar2;
    s32 iVar3;
    u32 uVar4;

    do {
        bVar1 = 1;
        while ((iVar2 = sn_pi_read_word(0xB1FFFFF0)) == 0) {
            sn_delay(1000);
        }
        if (iVar2 == 2) {
            sn_pi_write_word(0xB1FFFFFC, 0);
            sn_delay(1000);
            goto LAB_800027b8;
        }
        sn_pi_write_word(0xB1FFFFFC, 0x101);
        while ((iVar3 = sn_pi_read_word(0xB1FFFFF0)) == iVar2) {
            sn_delay(500);
        }
        iVar3 = sn_pi_read_word(0xB1FFFFF0);
        if (iVar3 == 2) {
            sn_pi_write_word(0xB1FFFFFC, 0);
            goto LAB_800027b8;
        }
        switch (iVar2) {
        case 0x10:
        default:
            break;
        case 0x11:
            bVar1 = 0;
            break;
        case 0x12:
            D_80018430 = 0;
            D_80018432 = 0;
            D_80018288 = D_80018288 & 0xfffffffe;
            uVar4 = sn_get_status_reg();
            sn_set_status_reg(uVar4 & 0xfffffffe);
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
            sn_pi_write_word(0xB1FE0000, (u32)D_80018432 << 24 | (u32)D_80018430 << 16);
            break;
        case 0x17:
            sn_host_mem_access(2);
            break;
        case 0x18:
            sn_host_mem_access(1);
            break;
        case 0x19:
            boot_func_800023EC();
            break;
        }
        sn_pi_write_word(0xB1FFFFFC, 0);
        while ((iVar2 = sn_pi_read_word(0xB1FFFFF0)) == 1) {
            sn_delay(500);
        }
LAB_800027b8:
        if (!bVar1 && D_80018432 != 0) {
            sn_pi_write_word(0xB1FFFFF0, 0);
            return;
        }
    } while (1);
}

u32 sn_pc_init(void)
{
    u32 a = sn_disable_ints();
    u32 r = sn_host_call_2(0x101, 0, 0, 0);
    sn_restore_ints_2(a);
    return r;
}

u32 sn_pc_close(u32 p)
{
    u32 s1, r;
    s1 = sn_disable_ints();
    r = sn_host_call_2(0x404, p, 0, 0);
    if ((r & 0x8000) != 0) {
        r = r | 0xffff0000;
    }
    sn_restore_ints(s1);
    return r;
}
