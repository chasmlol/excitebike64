#include "ctx.h"

typedef struct {
    u32 size;
    u32 code;
    u16 company;
    u8 ext;
    u8 padB[0x1E - 0xB];
    s16 fileNo;
    u8 pad20[0x74 - 0x20];
} Entry;

typedef struct {
    Entry *e[16];
} EntryRow;

typedef struct {
    s32 mode;
    s32 state;
    s32 sub;
    s32 pak;
    s32 sel;
    s32 f14;
    s32 scroll;
    s32 pages;
    s32 f20;
    u8 *buf;
    s32 (*cb)(u8 *, s32);
    s32 f2C;
} PakState;

typedef struct {
    char **lines;
    char *text;
    char *extra;
} Msg;

extern s32 D_800F4A04;
extern s32 D_800F4A00;
extern PakState D_800F4A28;
extern PakState D_800F4838;
extern Msg D_800CE068[];
extern char *D_800CE05C[];
extern char *D_800CDF64[];
extern char *D_800CDF6C[];
extern char *D_800CDF78[];
extern s16 D_800F01F8;
extern char D_800CE034[];
extern char D_800CE01C[];
extern u32 D_800E30B0;
extern u32 D_800F49FC;
extern s32 D_800F4A78;
extern u8 D_800F4A5C;
extern s32 D_800F4A08;
extern Entry *D_800F4A10[];
extern s32 D_800E2004;
extern s32 D_800E2008;
extern s32 D_800E1C60;
extern u8 D_800C292C;
extern s32 D_800F4AE0[];
extern s32 D_800F4A68[];
extern Entry *D_800F48E0[4][17];
extern Entry D_800CE168;
extern s32 D_800F4AF4;
extern s32 D_800CDE70;
extern s32 D_800F4AF0;
extern s32 D_800F4A7C;
extern char *D_800CDF00[];
extern s32 D_800CDEFC;
extern u8 D_800F47D0[];
extern u8 *D_800F4A80;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8001102C(s32 param_1);
s32 func_8008C55C(u32 param_1);
s32 func_8008CA68(u8 *param_1);
s32 func_8008C2BC(u32 param_1, s32 param_2, u32 param_3, u32 param_4, u8 *param_5);
s32 func_8008C370(u32 p1, s32 p2, u32 p3, u32 p4, u8 *p5);
s32 func_8008C1FC(s32 channel, s32 file_no);
void func_80089E30(s32 p);
void func_8008B9BC(s32 p);
void func_80088500(s32 p, s32 q);
void func_8008BC30(void);
s32 func_8008C0E4(s32 pak, s32 company, u8 *ext, s32 name, s32 code, s32 size);
u8 *func_800BC908(s32 size, s32 flags, char *file, s32 line);
void func_800BCB04(u8 *p);

static inline void SET(s32 s, s32 m) {
    D_800F4A28.state = s;
    D_800F4A28.sub = 0;
    D_800F4A00 = m;
}

static inline void SND(s32 m) {
    D_800F4A00 = m;
}

static inline void SET2(s32 s) {
    D_800F4A28.state = s;
    D_800F4A28.sub = 0;
}

static inline void compact(s32 pak) {
    if (D_800F48E0[pak][0] == &D_800CE168 && D_800F4AE0[pak] > 0) {
        *(EntryRow *)&D_800F48E0[pak][0] = *(EntryRow *)&D_800F48E0[pak][1];
        D_800F4AE0[pak]--;
    }
}

void func_80089F2C(void) {
    u8 st[8];
    u32 pad;
    u32 prev;
    s32 i;
    s32 n;
    s32 j;
    s32 cnt;
    s32 bit;
    Entry *e;

    if (D_800F4A04 != 0) {
        return;
    }
    D_800F4A00 = -1;
    D_800CE068[5].text = D_800CE05C[D_800F4A28.mode];
    if (D_800F4A28.mode == 0) {
        D_800CE068[12].lines = D_800CDF64;
    } else if (D_800F01F8 == 1) {
        D_800CE068[12].lines = D_800CDF6C;
    } else {
        D_800CE068[12].lines = D_800CDF78;
    }
    if (D_800F4A28.mode == 2) {
        D_800CE068[0].extra = D_800CE034;
    } else {
        D_800CE068[0].extra = D_800CE01C;
    }
    pad = D_800E30B0;
    if (pad & 0x2000) {
        pad |= 4;
    }
    if (pad & 0x4000) {
        pad |= 8;
    }
    if (pad & 0x800) {
        pad |= 1;
    }
    if (pad & 0x1000) {
        pad |= 2;
    }
    i = D_800F49FC;
    D_800F49FC = pad;
    pad &= ~i;
    D_800F4A78++;
    if (D_800F4A78 % 15 == 0 && func_8008CA68(st) == 0 && st[0] != D_800F4A5C) {
        for (i = 0; i < 4; i++) {
            if (((st[0] ^ D_800F4A5C) >> i) & 1) {
                D_800F4A08 &= ~(1 << i);
                func_8008B9BC(i);
                if (D_800E2004 & (1 << i)) {
                    func_80089E30(i);
                } else {
                    func_8000ED10((u8 *)D_800F4A10[i], 0, 0x740);
                }
                if ((D_800E2008 & (1 << i)) && (D_800F4A28.state == 0 || D_800F4A28.state == 14)) {
                    D_800F4A28.pak = i;
                    SET(1, 3);
                }
            }
        }
        D_800F4A5C = st[0];
    }
    D_800E2008 &= D_800C292C;
    D_800E2004 &= D_800C292C;
    D_800E1C60 &= D_800C292C;
    D_800F4A08 &= D_800E2008;
    if (D_800F4A28.state == 0 || D_800F4A28.state == 14) {
        for (i = 0; i < 4; i++) {
            n = (D_800F4A28.pak + i) % 4;
            if ((D_800E2008 & (1 << n)) && !((D_800F4A08 >> n) & 1)) {
                D_800F4A28.pak = n;
                SET(1, 3);
            }
        }
    }
    if (D_800F4A28.state == 5 && D_800F4AE0[D_800F4A28.pak] == 0) {
        D_800F4A28.state = 0;
        D_800F4A28.sub = 0;
    }
    if ((D_800F4A28.state < 14 || D_800F4A28.state > 16) && D_800F4A28.state != 20) {
        if ((D_800E2004 | D_800E2008) == 0 && D_800F4A28.state != 14) {
            D_800F4A28.state = 14;
            D_800F4A28.sub = 0;
        } else if (!((D_800E2004 | D_800E2008) & (1 << D_800F4A28.pak)) && D_800F4A28.state != 0) {
            D_800F4A28.state = 0;
            D_800F4A28.sub = 0;
        }
    }
    for (i = 0; i < 4; i++) {
        if (!((D_800E2008 | D_800E2004) & (1 << D_800F4A28.pak))) {
            D_800F4A28.pak = (D_800F4A28.pak + 1) % 4;
        }
    }
    if (D_800F4838.state != -1 && D_800F4A28.state == 0 && D_800F4A28.mode == 0) {
        D_800F4A28 = D_800F4838;
        D_800F4838.state = -1;
        D_800F4A28.state = 0;
        D_800F4A28.sub = 0;
    }
    switch (D_800F4A28.state) {
    case 0:
        compact(D_800F4A28.pak);
        D_800F4A28.sel = 0;
        D_800F4A28.scroll = 0;
        if (pad & 4) {
            for (i = D_800F4A28.pak - 1; i >= 0; i--) {
                if ((D_800E2004 | D_800E2008) & (1 << i)) {
                    break;
                }
            }
            if (i < 0) {
                D_800F4A00 = 3;
            } else {
                D_800F4A28.pak = i;
                D_800F4A00 = 0;
            }
        } else if (pad & 8) {
            for (i = D_800F4A28.pak + 1; i < 4; i++) {
                if ((D_800E2004 | D_800E2008) & (1 << i)) {
                    break;
                }
            }
            if (i >= 4) {
                D_800F4A00 = 3;
            } else {
                D_800F4A28.pak = i;
                D_800F4A00 = 0;
            }
        }
        if (pad & 0x20) {
            if (D_800F4A28.mode == 1) {
                SET(18, 2);
            } else if (D_800F4A28.mode == 2) {
                SET(17, 2);
            } else {
                D_800F4AF4 = 0;
                if (D_800CDE70 != 0) {
                    D_800F4A04 = 1;
                    D_800F4A00 = 1;
                    D_800CDE70 = 0;
                    break;
                }
                SET(20, 2);
            }
        } else if (pad & 0x10) {
            if (D_800E2004 & (1 << D_800F4A28.pak)) {
                n = 0;
                for (i = 0; i < 16; i++) {
                    if (D_800F4A10[D_800F4A28.pak][i].fileNo != -1) {
                        n++;
                    }
                }
                D_800F4A28.sel = 0;
                if (D_800F4A28.mode == 2) {
                    if (n == 16) {
                        SET(8, 1);
                    } else {
                        func_80088500(D_800F4A28.pak, 1);
                        SET(5, 1);
                    }
                } else if (D_800F4A28.mode == 0 && n == 0) {
                    SET(12, 3);
                } else if (D_800F4A28.mode == 1 && D_800F4AE0[D_800F4A28.pak] == 0) {
                    SET(12, 3);
                } else {
                    SET(5, 1);
                }
            } else if (D_800E2008 & (1 << D_800F4A28.pak)) {
                SET(1, 3);
            }
        }
        break;
    case 5:
        if (D_800F4A28.sel >= D_800F4AE0[D_800F4A28.pak]) {
            D_800F4A28.sel = D_800F4AE0[D_800F4A28.pak] - 1;
        }
        if (D_800F4A28.sel < 0) {
            D_800F4A28.sel = 0;
        }
        j = D_800F4A28.sel;
        n = 0;
        if (pad & 2) {
            n = 1;
        }
        i = j;
        if (pad & 1) {
            n = -1;
        }
        j += n;
        cnt = D_800F4AE0[D_800F4A28.pak];
        do {
            if (j < 0) {
                if (n != 0) {
                    j = i;
                    goto done;
                }
                j = D_800F4AE0[D_800F4A28.pak] - 1;
            }
            if (j >= D_800F4AE0[D_800F4A28.pak]) {
                if (n == 0) {
                    j = 0;
                } else {
                    j = i;
                    goto done;
                }
            }
            if (D_800F4A28.mode != 2 || D_800F48E0[D_800F4A28.pak][j] == &D_800CE168 ||
                D_800F48E0[D_800F4A28.pak][j]->code == 0x4E4D5847) {
                goto done;
            }
            j += n;
        } while (n != 0 || (n = 1, --cnt != 0));
    done:
        D_800F4A28.sel = j;
        if (j < D_800F4A28.scroll) {
            D_800F4A28.scroll = j;
        }
        if (D_800F4A28.scroll < j - 7) {
            D_800F4A28.scroll = j - 7;
        }
        if (D_800F4AE0[D_800F4A28.pak] - 8 < D_800F4A28.scroll) {
            D_800F4A28.scroll = D_800F4AE0[D_800F4A28.pak] - 8;
        }
        if (D_800F4A28.scroll < 0) {
            D_800F4A28.scroll = 0;
        }
        if (pad & 3) {
            if (i == j) {
                D_800F4A00 = 3;
            } else {
                D_800F4A00 = 0;
            }
        }
        if (pad & 0x20) {
            SET(0, 2);
        }
        if (pad & 0x10) {
            if (D_800F4A28.mode == 0) {
                SET(7, 1);
            } else if (D_800F4A28.mode == 2) {
                e = D_800F48E0[D_800F4A28.pak][D_800F4A28.sel];
                if (e == &D_800CE168) {
                    if (D_800F4A68[D_800F4A28.pak] < D_800F4A28.pages) {
                        SET(8, 1);
                        break;
                    }
                } else {
                    j = (e->size + 0xFF) >> 8;
                    if (D_800F4A68[D_800F4A28.pak] + j < D_800F4A28.pages) {
                        SET(8, 1);
                        break;
                    }
                }
                if (e != &D_800CE168) {
                    SET(6, 1);
                } else {
                    SET(10, 1);
                }
            } else {
                SET(9, 1);
            }
        }
        break;
    case 7:
        if (pad & 0x20) {
            SET(5, 2);
        }
        if (pad & 0x10) {
            SET(13, 1);
        }
        break;
    case 13:
        switch (D_800F4A28.sub) {
        case 0:
            D_800F4AF0 = 30;
            D_800F4A28.sub = 1;
            break;
        case 1:
            if (D_800F4AF0 == 0) {
                D_800F4A28.sub = 2;
            }
            break;
        case 2:
            if (func_8008C1FC(D_800F4A28.pak, D_800F48E0[D_800F4A28.pak][D_800F4A28.sel]->fileNo) != 0) {
                SET(11, 3);
            } else if (D_800F4A28.mode != 0) {
                func_80089E30(D_800F4A28.pak);
                SET(0, 1);
            } else {
                func_80089E30(D_800F4A28.pak);
                if (D_800F48E0[D_800F4A28.pak] == NULL) {
                    SET(0, 1);
                } else {
                    SET(5, 1);
                }
            }
            break;
        }
        break;
    case 1:
        D_800CDF00[0][0x11] = D_800F4A28.pak + '1';
        if (pad & 0x10) {
            SET(2, 1);
        }
        if (pad & 0x20) {
            SND(2);
            SET2(0);
            D_800F4A08 |= 1 << D_800F4A28.pak;
        }
        break;
    case 12:
        if (pad & 0x10) {
            SET(0, 1);
        }
        break;
    case 2:
        switch (D_800F4A28.sub) {
        case 0:
            D_800F4AF0 = 30;
            D_800F4A28.sub = 1;
            break;
        case 1:
            if (D_800F4AF0 == 0) {
                D_800F4A28.sub = 2;
            }
            break;
        case 2:
            if (func_8008C55C(D_800F4A28.pak) != 0) {
                SET(4, 3);
            } else {
                D_800F4A28.state = 3;
                D_800F4A00 = 1;
                D_800F4A28.sub = 0;
                D_800F4A5C &= ~(1 << D_800F4A28.pak);
            }
            break;
        }
        break;
    case 4:
        if (pad & 0x10) {
            SET(2, 1);
        }
        if (pad & 0x20) {
            SET(0, 1);
            D_800F4A08 |= 1 << D_800F4A28.pak;
        }
        break;
    case 3:
        if (pad & 0x10) {
            SET(0, 1);
        }
        break;
    case 6:
        if (pad & 0x20) {
            SET(5, 2);
        }
        if (pad & 0x10) {
            e = D_800F48E0[D_800F4A28.pak][D_800F4A28.sel];
            if (func_8008C1FC(D_800F4A28.pak, e->fileNo) != 0) {
                SET(11, 3);
            } else {
                if (e->code == 0x4E4D5847) {
                    D_800F4A7C = e->ext;
                } else {
                    D_800F4A7C = -1;
                }
                SET(10, 1);
            }
        }
        break;
    case 11:
        if (pad & 0x10) {
            SET(5, 1);
            D_800F4A5C = 0;
            D_800E2004 = 0;
            D_800F4A78 = -1;
        }
        break;
    case 14:
        if (D_800E2008 | D_800E2004) {
            D_800F4A28.state = 0;
            D_800F4A28.sub = 0;
        } else if (pad & 0x20) {
            D_800F4AF4 = 1;
            D_800F4A04 = 1;
            D_800F4A00 = 2;
        }
        break;
    case 9:
        switch (D_800F4A28.sub) {
        case 0:
            D_800F4AF0 = 30;
            D_800F4A28.sub = 1;
            break;
        case 1:
            if (D_800F4AF0 == 0) {
                D_800F4A28.sub = 2;
            }
            break;
        case 2:
            e = D_800F48E0[D_800F4A28.pak][D_800F4A28.sel];
            if (D_800F4A28.pages < (e->size + 0xFF) >> 8) {
                SET(19, 3);
                return;
            }
            func_8000ED10(D_800F4A28.buf, 0, D_800F4A28.pages << 8);
            if (func_8008C2BC(D_800F4A28.pak, e->fileNo, 0, (e->size + 0xFF) & ~0xFF, D_800F4A28.buf) != 0) {
                SET(11, 3);
                return;
            }
            if (D_800F4A28.cb != NULL && D_800F4A28.cb(D_800F4A28.buf, (e->size + 0xFF) & ~0xFF) != 0) {
                SET(19, 3);
                return;
            }
            SET2(16);
            break;
        }
        break;
    case 10:
        switch (D_800F4A28.sub) {
        case 0:
            D_800F4AF0 = 30;
            D_800F4A28.sub = 1;
            break;
        case 1:
            if (D_800F4AF0 == 0) {
                D_800F4A28.sub = 2;
            }
            break;
        case 2:
            if (D_800F4A7C == -1) {
                n = 'A';
                for (;;) {
                    for (i = 0; i < D_800F4AE0[D_800F4A28.pak]; i++) {
                        e = D_800F48E0[D_800F4A28.pak][i];
                        if (e->code == 0x4E4D5847 && e->ext == n) {
                            break;
                        }
                    }
                    if (i == D_800F4AE0[D_800F4A28.pak]) {
                        break;
                    }
                    if (++n > 'Z') {
                        break;
                    }
                }
            } else {
                n = D_800F4A7C;
            }
            D_800F4A7C = -1;
            func_8000ED10(D_800F47D0, 0, 4);
            D_800F47D0[0] = n;
            j = func_8008C0E4(D_800F4A28.pak, D_800CDEFC, D_800F47D0, 0x3031, 0x4E4D5847, D_800F4A28.pages << 8);
            if (j < 0) {
                SET(11, 3);
            }
            if (func_8008C370(D_800F4A28.pak, j, 0, D_800F4A28.pages << 8, D_800F4A28.buf) != 0) {
                func_8008C1FC(D_800F4A28.pak, j);
                SET(11, 3);
            }
            D_800F4A80 = func_800BC908(D_800F4A28.pages << 8, 0, "pak.c", 0x5E6);
            n = func_8008C2BC(D_800F4A28.pak, j, 0, D_800F4A28.pages << 8, D_800F4A80);
            if (n == 0) {
                for (i = 0; i < D_800F4A28.pages << 8; i++) {
                    if (D_800F4A80[i] != D_800F4A28.buf[i]) {
                        n = 1;
                    }
                }
            }
            func_800BCB04(D_800F4A80);
            if (n != 0) {
                func_8008C1FC(D_800F4A28.pak, j);
                SET(11, 3);
            }
            func_80089E30(D_800F4A28.pak);
            D_800F4A28.state = 15;
            D_800F4A28.sub = 0;
            break;
        }
        break;
    case 19:
        if (pad & 0x10) {
            SET(7, 1);
        } else if (pad & 0x20) {
            SET(0, 1);
            D_800F4A5C = 0;
            D_800E2004 = 0;
            D_800F4A78 = -1;
        }
        break;
    case 8:
        if (pad & 0x20) {
            SET(0, 2);
        } else if (pad & 0x10) {
            D_800F4838 = D_800F4A28;
            D_800F4A28.mode = 0;
            func_80088500(D_800F4A28.pak, 0);
            compact(D_800F4A28.pak);
            SET(5, 1);
        }
        break;
    case 17:
        D_800F4AF4 = 1;
        if (pad & 0x10) {
            SET(20, 1);
        } else if (pad & 0x20) {
            SET(0, 2);
        }
        break;
    case 18:
        D_800F4AF4 = 1;
        if (pad & 0x10) {
            SET(20, 1);
        } else if (pad & 0x20) {
            SET(0, 2);
        }
        break;
    case 15:
    case 16:
        D_800F4AF4 = 0;
    case 20:
        if (pad & 0x10) {
            D_800F4A04 = 1;
            D_800F4A00 = 1;
        }
        break;
    }
    if (D_800F4A04 != 0 && D_800F4838.state != -1) {
        D_800F4A28 = D_800F4838;
        D_800F4838.state = -1;
        D_800F4A28.state = 0;
        D_800F4A28.sub = 0;
        D_800F4A04 = 0;
    }
    if (D_800F4A00 != -1) {
        func_8001102C(D_800F4A00);
    }
    if (D_800F4A04 != 0) {
        func_8008BC30();
        if (D_800F4A04 != 0) {
            D_800CDE70 = 0;
        }
    }
}
