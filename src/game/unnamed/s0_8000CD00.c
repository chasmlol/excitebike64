#include "game.h"

void func_8000CD00(void)
{
    s32 buf[2];
    s32 i;
    s32 v;

    if (D_800E1E18 != 0) {
        osRecvMesg(&D_800E1C48, buf, 1);
        for (i = 0; i < 8; i++) {
            osRecvMesg(&D_800E1C48, buf, 0);
        }
        for (i = 0; i < 4; i++) {
            if (D_800C2924 != 0) {
                osMotorInit(&D_800E1C48, &D_800E1E60[i], i);
                __osMotorAccess(&D_800E1E60[i], 0);
                D_800E2010[i] = 0;
                D_800E2020 &= ~(1 << i);
            }
            if (D_800E1C60 & (1 << i)) {
                if (D_800E2020 & (1 << i)) {
                    v = D_800E2010[i];
                    if (v == 0) {
                        __osMotorAccess(&D_800E1E60[i], 0);
                        D_800E2010[i] = 0;
                        D_800E2020 &= ~(1 << i);
                    }
                } else {
                    if (D_800E2010[i] != 0) {
                        __osMotorAccess(&D_800E1E60[i], 1);
                        D_800E2020 |= 1 << i;
                    }
                }
                if (D_800E2010[i] != 0) {
                    D_800E2010[i]--;
                }
            }
        }
        D_800C2924 = 0;
        D_800E1E18 = 0;
        if (D_800C2920 != 0) {
            D_800C29E0 = 0x80010000;
        }
    }
}

typedef struct { u16 id; u8 a; u8 b; u8 flag; u8 pad; } Sel;
typedef struct { u16 id; u8 a; u8 b; } Out;

extern Sel D_800E1C00[];
extern Out D_800E1E40[];

void func_8000CEAC(void)
{
    u16 i;
    u16 cnt;
    u32 started;
    u8 bit;
    u8 nv;
    u8 old;
    Sel *e;

    if (D_800E1E18 != 0) {
        return;
    }
    cnt = 0;
    osContGetReadData(D_800E1C00);
    started = 0;
    D_800E2000 = D_800E1C00[i].a;
    D_800E2001 = D_800E1C00[i].b;
    i = 0;
    do {
        old = D_800C292C;
        bit = (u8)(1 << i);
        nv = old | bit;
        D_800C292C = nv;
        e = &D_800E1C00[i];
        if (e->flag != 0) {
            D_800C292C = nv & ~bit;
        } else {
            if (started == 0) {
                D_800C292E = (u8)i;
                started = 1;
            }
            D_800E1C40[i] = e->id;
            D_800E1E40[i].id = e->id;
            D_800E1E40[i].a = e->a;
            D_800E1E40[i].b = e->b;
            cnt = cnt + 1;
            if (cnt == 4) {
                return;
            }
        }
        i = i + 1;
    } while (i < 4);
}

int func_8000CFE8(void)
{
    int r;

    osCreateMesgQueue(&D_800E1C48, D_800E1E50, 4);
    osSetEventMesg(5, &D_800E1C48, 1);
    r = osContInit(&D_800E1C48, D_800C292C_2, D_800E1C68);
    if (r != 0) {
        return r;
    }
    func_8008BC30();
    D_800E1E18 = 0;
    return 0;
}

void start_controller_read(void) {
    if (D_800E1E18 == 0) {
        osContStartReadData(&D_800E1C48);
        D_800E1E18 = 1;
    }
}

void func_8000D0A0(u32 a0, s32 a1)
{
    if ((D_800E1C60 & (1u << a0)) != 0) {
        D_800E2010[a0] = a1 + 1;
    }
}

void func_8000D0D8(void)
{
    s32 i;
    u32 bit;

    func_8000CD00();
    for (i = 0; i < 4; i++) {
        if ((D_800E1C60 & (1u << i)) != 0) {
            bit = 1u << i;
            osMotorInit(&D_800E1C48, D_800E1E60_2 + i * 0x68, i);
            __osMotorAccess(D_800E1E60_2 + i * 0x68, 0);
            D_800E2010[i] = 0;
            D_800E2020 = D_800E2020 & ~bit;
        }
    }
}

void func_8000D1A8(s32 p1, u8 *p2, s32 p3)
{
    u8 loc[8];
    s32 i;
    s32 j;
    s32 k;
    s32 base;
    s32 w;

    func_8000CD00();
    osWritebackDCache(p2, p3 << 3);
    i = 0;
    if (p3 > 0) {
        do {
            w = p1 + i;
            base = i * 8;
            do {
                osEepromWrite(&D_800E1C48, (u8)w, p2 + base);
                for (j = 0; j < 8; j++) {
                    loc[j] = p2[base + j] ^ 0xAA;
                }
                osEepromRead(&D_800E1C48, (u8)w, loc);
                osInvalDCache(0, 0x400000);
                for (k = 0; k < 8; k++) {
                    if (loc[k] != p2[base + k]) {
                        break;
                    }
                }
            } while (k != 8);
            i++;
        } while (i < p3);
    }
}
