#include "ctx.h"

typedef struct {
    u8 c[3];
} Trip;

typedef struct {
    s32 f0;
    u8 pad4[0x2C8 - 0x4];
    s8 f2C8[0x20];
    s32 f2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x454 - 0x3F0];
    s16 f454;
    s16 f456;
    s16 f458;
    u8 pad45A[0x460 - 0x45A];
    s32 f460;
    s16 f464;
    u8 f466[0x12];
    s32 f478;
    u8 pad47C[0x734 - 0x47C];
    Trip f734[12];
    s8 f758[4];
    u8 pad75C[0x7A8 - 0x75C];
    u8 f7A8;
    u8 f7A9;
    u8 pad7AA[0x84C - 0x7AA];
    Trip f84C[4];
} Game;

typedef struct {
    s32 f0;
    char *name;
} NameEnt;

typedef struct {
    s32 f0;
    s32 f4;
} Sub;

typedef struct {
    u8 pad0[0xC];
    Sub *fC;
    u8 pad10[0x16 - 0x10];
    s16 f16;
    u8 pad18[0x24 - 0x18];
    s32 f24;
} Obj;

extern Game D_800EFD70;
extern NameEnt D_800C2284[];
extern s32 D_800CD1A8;
extern Trip D_800F05BC[];
extern u8 *D_800CD1AC[];
extern u8 D_800CD79C[];
extern s32 D_800E30B0[];
extern s32 D_800E25B0[];

void func_8001102C(s32 param_1);
void func_8007AF38(s32 p1, s32 p2, s32 p3);
s32 func_800AEC10(u8 *s);

#define G D_800EFD70
#define BT D_800E30B0[pad]
#define PRESSED(pad, m) ((D_800E30B0[pad] & (m)) && !(D_800E25B0[pad] & (m)))

void func_80074684(Obj *o, s32 unused, s32 *btn) {
    s32 isNine;
    s32 pad;
    s32 sel;
    s32 found;
    s32 i;
    s32 snd;
    s32 b;
    s32 old;
    s32 lo, hi, lo2, hi2;
    u8 *p;
    u8 *q;
    s32 m;

    isNine = o->f16 == 9;
    if (++G.f3EC == 120) {
        G.f3EC = 0;
    }
    if (isNine) {
        pad = 0;
    } else {
        sel = G.f758[G.f460];
        pad = G.f2C8[sel];
    }
    if ((u32)(G.f2E8 - 0x11) < 2) {
        if (btn[pad] & 0x10) {
            func_8007AF38((s32)D_800CD79C, 0, 0);
        }
        return;
    }
    b = btn[pad];
    if (b & 0x10) {
        found = 0;
        if (G.f454 == 0x26 && G.f464 == 0) {
            snd = 3;
        } else if (isNine && G.f454 == 0x27) {
            {
                for (i = 0; i < 14; i++) {
                    p = (u8 *)D_800C2284[i].name;
                    q = G.f466;
                loop:
                    m = *p;
                    if (m == *q) {
                        if (m == 0) {
                            found = 1;
                            m = 1 << i;
                            G.f7A9 = i;
                            if (m == 4) {
                                G.f0 ^= 0x20000000;
                            } else {
                                D_800CD1A8 ^= m;
                            }
                        } else {
                            p++;
                            q++;
                            goto loop;
                        }
                    }
                    if (found) {
                        goto ok;
                    }
                }
                snd = 3;
            }
        } else {
        ok:
            snd = 1;
        }
        func_8001102C(snd);
        if (G.f454 == 0x26) {
            if (G.f464 != 0) {
                G.f466[G.f464--] = 0;
            }
        } else if (G.f454 == 0x27) {
            if (isNine) {
                if (found) {
                    G.f2E8 = 0x11;
                } else {
                    G.f2E8 = 0x12;
                }
                return;
            }
            for (i = G.f460 + 1; i < 4; i++) {
                if (G.f758[i] == G.f758[G.f460]) {
                    D_800F05BC[i] = *(Trip *)G.f466;
                    if (i == 3) {
                        G.f7A8 = G.f758[i];
                    }
                    G.f758[i] = -1;
                }
            }
            { s32 k = G.f460; D_800F05BC[k] = *(Trip *)G.f466; }
            {
                Trip *t = G.f734;
                t[sel] = *(Trip *)G.f466;
            }
            for (i = G.f460 + 1; i < 4; i++) {
                if (G.f758[i] != G.f758[G.f460] && G.f758[i] != -1) {
                    G.f460 = i;
                    break;
                }
            }
            if (i < 4) {
                s32 k = G.f758[G.f460];
                if (G.f734[k].c[0] != 0) {
                    Trip *t = G.f734;
                    *(Trip *)G.f466 = t[k];
                    G.f466[3] = 0;
                    G.f464 = func_800AEC10(G.f466);
                    G.f456 = 0x31F;
                    G.f458 = 0x31F;
                    G.f454 = 0x27;
                } else {
                    G.f466[0] = 0x41;
                    G.f466[1] = 0;
                    G.f466[2] = 0;
                    G.f466[3] = 0;
                    G.f464 = 0;
                    G.f456 = 0;
                    G.f458 = 0;
                    G.f454 = 0;
                }
                {
                    s32 t = o->f24;
                    o->f24 = G.f478;
                    G.f478 = t;
                }
                return;
            }
            func_8007AF38(o->fC->f4, 0, 0);
        } else {
            s32 n = (s16)++G.f464;
            if (n >= (isNine ? 10 : 3)) {
                G.f456 = 0x31F;
                G.f458 = 0x31F;
                G.f454 = 0x27;
            }
        }
        return;
    }
    if (b & 0x20) {
        if (G.f464 != 0) {
            G.f466[G.f464 - 1] = G.f466[G.f464];
            snd = 1;
            G.f466[G.f464--] = 0;
        } else {
            snd = 3;
        }
        func_8001102C(snd);
        return;
    }
    old = G.f454;
    if (G.f464 < (isNine ? 10 : 3)) {
        s32 x, v;
        s32 col, row;
        lo = 0;
        lo2 = lo;
        hi = 0x320;
        hi2 = hi;
        G.f456 += (BT & 0xC) ? ((BT & 8) ? ((BT >> 24) & 0xF) : -((BT >> 24) & 0xF)) : 0;
        if (PRESSED(pad, 0x2000)) {
            G.f456 -= 100;
        }
        if (PRESSED(pad, 0x4000)) {
            G.f456 += 100;
        }
        if (G.f456 >= hi) {
            G.f456 = lo;
        } else if (G.f456 < lo) {
            G.f456 = hi - 1;
        }
        G.f458 -= (BT & 3) ? ((BT & 1) ? ((s32)((u32)BT >> 28) * 3 >> 1) : (-(s32)((u32)BT >> 28) * 3) / 2) : 0;
        if (PRESSED(pad, 0x800)) {
            G.f458 -= 0xA0;
        }
        if (PRESSED(pad, 0x1000)) {
            G.f458 += 0xA0;
        }
        if (G.f458 >= hi2) {
            G.f458 = lo2;
        } else if (G.f458 < lo2) {
            G.f458 = hi2 - 1;
        }
        col = G.f456;
        row = G.f458;
        G.f454 = col / 100 + row / 160 * 8;
    } else if (btn[pad] & 0xC) {
        if (G.f454 == 0x27) {
            G.f456 = 0x258;
            G.f458 = 0x31F;
            G.f454 = 0x26;
        } else {
            G.f456 = 0x31F;
            G.f458 = 0x31F;
            G.f454 = 0x27;
        }
    }
    if (G.f454 < 0x26) {
        if (func_800AEC10(G.f466) == 0) {
            G.f466[1] = 0;
        }
        G.f466[G.f464] = *D_800CD1AC[G.f454];
    } else {
        G.f466[G.f464] = 0;
    }
    if (old != G.f454) {
        func_8001102C(0);
    }
}
