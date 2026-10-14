#include "boot.h"

void boot_func_80001C2C(s32 param_1) {
    D_80018100 = 0;
    while (D_80018100 < param_1) {
        D_80018100 = D_80018100 + 1;
        D_80018100;
    }
}

void boot_func_80001DD0(u32 addr, u32 val)
{
    u32 sh = (~addr & 3) << 3;
    u32 *p = (u32 *)(addr & ~3u);
    *p = (*p & ~(0xFFu << sh)) | ((val & 0xFFu) << sh);
}

void boot_func_80002010(u8 *d, u8 *s, u32 n) {
    u8 *p6;
    u8 c;

    while (n != 0 && ((u32)d & 3) != 0) {
        boot_func_80001F20((u32)d++, *s++);
        n = n - 1;
    }
    if (n > 3) {
        do {
            p6 = s + 3;
            boot_func_80001BB8((u32 *)d, ((u32)s[0] << 24) | ((u32)p6[-2] << 16) | ((u32)p6[-1] << 8) | p6[0]);
            d = d + 4;
            s = s + 4;
            n = n - 4;
        } while (n > 3);
    }
    while (n != 0) {
        boot_func_80001F20((u32)d++, *s++);
        n = n - 1;
    }
}

#define TB(o) (*(u32 *)((u32)D_8000F4A0 + (o)))
#define TF(o) (*(u32 *)((u32)D_8000F4A4 + (o)))

void boot_func_8000223C(u32 p)
{
    u32 dst = 0xB1FE0000;
    u32 src = boot_func_80001BF0(0xB1FFFFF4);
    u32 n = boot_func_80001BF0(0xB1FFFFF8);
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
                boot_func_800020FC(dst, 0, len);
        } else if (flags & 4) {
            if (p == 1)
                boot_func_800021C8(dst, src, len);
            else
                boot_func_800021C8(src, dst, len);
        } else if (p == 1) {
            boot_func_80002010(dst, src, len);
        } else if (flags & 8) {
            boot_func_80001E0C(dst, src, len);
        } else {
            boot_func_80001CF4(dst, src, len);
        }
        dst += len;
        n -= len;
        src += len;
    }
}

void boot_func_800024EC(void)
{
    u32 r;
    u32 x;

    r = boot_func_80001B50();
    func_80002D20();
    boot_func_80001BB8(0xB1FFFFF0, 0);
    boot_func_80001BB8(0xB1FFFFFC, 0);
    D_80018430 = 1;
    D_80018432 = 0xFF;
    x = osMemSize;
    if (x > 0x3FFFFF && (x & 0x1FFFF) == 0 && x < 0x2000001) {
        x = x - 1;
        D_8000F4A8 = x - 0x80000000;
        D_8000F4B8 = x + 0xA0000000;
    }
    boot_func_80001B88(r);
}

void boot_func_800025B8(void)
{
    s32 bVar1;
    s32 iVar2;
    s32 iVar3;
    u32 uVar4;

    do {
        bVar1 = 1;
        while ((iVar2 = boot_func_80001BF0(0xB1FFFFF0)) == 0) {
            boot_func_80001C2C(1000);
        }
        if (iVar2 == 2) {
            boot_func_80001BB8(0xB1FFFFFC, 0);
            boot_func_80001C2C(1000);
            goto LAB_800027b8;
        }
        boot_func_80001BB8(0xB1FFFFFC, 0x101);
        while ((iVar3 = boot_func_80001BF0(0xB1FFFFF0)) == iVar2) {
            boot_func_80001C2C(500);
        }
        iVar3 = boot_func_80001BF0(0xB1FFFFF0);
        if (iVar3 == 2) {
            boot_func_80001BB8(0xB1FFFFFC, 0);
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
            uVar4 = boot_func_80002E04();
            boot_func_80002E10(uVar4 & 0xfffffffe);
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
            boot_func_80001BB8(0xB1FE0000, (u32)D_80018432 << 24 | (u32)D_80018430 << 16);
            break;
        case 0x17:
            boot_func_8000223C(2);
            break;
        case 0x18:
            boot_func_8000223C(1);
            break;
        case 0x19:
            boot_func_800023EC();
            break;
        }
        boot_func_80001BB8(0xB1FFFFFC, 0);
        while ((iVar2 = boot_func_80001BF0(0xB1FFFFF0)) == 1) {
            boot_func_80001C2C(500);
        }
LAB_800027b8:
        if (!bVar1 && D_80018432 != 0) {
            boot_func_80001BB8(0xB1FFFFF0, 0);
            return;
        }
    } while (1);
}

u32 boot_func_800028E0(void)
{
    u32 a = boot_func_80001B50();
    u32 r = boot_func_80002828_2(0x101, 0, 0, 0);
    boot_func_80001B88_2(a);
    return r;
}

u32 boot_func_80002A4C(u32 p)
{
    u32 s1, r;
    s1 = boot_func_80001B50();
    r = boot_func_80002828_2(0x404, p, 0, 0);
    if ((r & 0x8000) != 0) {
        r = r | 0xffff0000;
    }
    boot_func_80001B88(s1);
    return r;
}
