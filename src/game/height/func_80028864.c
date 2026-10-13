#include "ctx.h"

typedef struct {
    s32 x, y, z;
    u8 pad0C[0x54 - 0xC];
    f32 f54;
    u8 pad58[0x3FF - 0x58];
    u8 b3FF;
    u8 pad400[0x5A8 - 0x400];
    s16 f5A8;
    s16 f5AA;
    u8 pad5AC[0x628 - 0x5AC];
    s16 f628;
    u8 pad62A[0x7E0 - 0x62A];
} Player;

typedef struct {
    s32 f0;
    u8 pad4[0x1C - 4];
    s32 f1C[4];
    u8 pad2C[0x3C - 0x2C];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    s32 f4C;
    u8 pad50[0x2C8 - 0x50];
    s8 f2C8[4];
    u8 pad2CC[0x760 - 0x2CC];
    u8 f760[4];
} Game;

typedef struct {
    s32 f0;
    u8 pad4[0x26 - 4];
    s16 f26;
    u8 pad28[0x40 - 0x28];
    s32 f40;
    u8 pad44[0x58 - 0x44];
    s32 f58;
} Cam;

typedef struct {
    u16 flags;
    s16 mode;
    u8 pad[4];
} CamFlags;

typedef struct {
    void (*f0)(void);
    void (*f4)(void *, s32 *);
    void (*f8)(void);
    u16 fC;
    u16 pad;
} CamMode;

typedef struct {
    s32 x, y, z, w, f10, f14;
} CamPos;

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    u8 pad[0x64];
    u8 b64;
    u8 pad2[0x6C - 0x65];
} Area;

extern s32 D_800C2358;
extern s32 D_800C2384;
extern s32 D_800C8CD8;
extern s32 D_800C8CE8;
extern s32 D_800C9650;
extern s32 D_800C9654[];
extern s32 D_800C966C[];
extern s32 D_800C9684;
extern CamMode D_800C9698[];
extern u8 D_800CB7B0[];
extern s32 D_800CCD64;
extern s32 D_800E36E0[];
extern s32 D_800E3700;
extern s32 D_800E3770[];
extern Player D_800E37C0[];
extern s32 D_800E78B0;
extern s32 D_800E78B8[];
extern s32 D_800E7968[];
extern s32 D_800E7978[];
extern s32 D_800E7FA8[];
extern CamFlags D_800E7FC8[];
extern s32 D_800E7FE8;
extern s32 D_800E7FF0[];
extern s32 D_800E8000[];
extern CamPos D_800E8030[];
extern Vec3i D_800E8090[];
extern s32 D_800EE140;
extern Area *D_800EE498;
extern u8 *D_800EFD64;
extern Game D_800EFD70;
extern s32 D_800EFDBC;
extern Cam D_800F05A8;

void func_8000BD10(void);
void func_8004F5C8(s32 param_1);
void func_80029830(Player *p, s32 *out, s32 changed);
s32 func_8002B870(void);
void func_8002D888(Player *p, s32 *out);
void func_8002D890(void);
void func_8002BFE0(Player *p, s32 *out);
void func_8002BD40(Player *p, s32 *out);
void func_8002B6F4(s32 *p, f32 *v);
void func_8002B5F8(s32 *src);
void func_8002CC30(s32 a);
void func_8004AB24(Vec3f *pos, s32 *area, f32 *h, s32 p4);


void func_80028864(void) {
    s32 buf[6];
    Vec3f pos;
    s32 area;
    f32 hh;
    s32 changed, sel;
    f32 *s5;
    s32 m;
    s32 m2;
    s32 mode;
    s32 t, k;
    CamFlags *e;
    u16 f;
    CamMode *cm;
    Player *pl;
    CamPos *c;
    Vec3i *v;
    f32 dx, dz, d;

    sel = 0;
    changed = 0;
    s5 = NULL;
    if (D_800C2358 != 0) {
        func_8000BD10();
        return;
    }
    if (D_800C9684 < D_800EFD70.f3C && D_800EFD70.f4C != 7 && D_800EFD70.f4C != 2) {
        if (D_800E37C0[D_800C9684].f5AA != 10) {
            D_800EFD70.f760[D_800EFD70.f2C8[D_800C9684]] = D_800E37C0[D_800C9684].f5AA;
        }
    }
    if (D_800C9650 == -1) {
        D_800E78B0 = D_800C9654[D_800C9684];
    } else {
        D_800E78B0 = D_800C9650;
    }
    if (D_800E7FA8[D_800E78B0] != 0) return;
    if (D_800C9684 == 1) {
        if (D_800C2384 != 0) {
            D_800E78B0 = 0;
        }
    }
    if (D_800F05A8.f0 == 0) {
        m2 = D_800F05A8.f26;
        if (m2 == 0xFF && D_800C9684 == 0) {
            func_8004F5C8(0);
        }
        if ((D_800E3770[0] & 0x10) || D_800EFD64 == D_800CB7B0) {
            if (m2 >= 2) {
                m2 = 1;
            }
            D_800F05A8.f26 = m2;
            changed = 1;
        }
    }
    if (D_800EFD70.f0 & 0x10) {
        t = D_800EFD70.f1C[D_800C9684];
        D_800E78B0 = t;
        func_80029830(&D_800E37C0[t], buf, t != D_800C9654[D_800C9684]);
        D_800C9654[D_800C9684] = D_800E78B0;
    } else {
        {
            s32 u = D_800E78B0;
            s32 *bs = D_800C9654;
            s32 kk = D_800C9684;
            s32 n;
            if (u != bs[kk]) changed = 1;
            bs[kk] = u;
            n = D_800E37C0[u].f5AA;
            if (n != D_800C966C[kk]) changed = 1;
            D_800C966C[kk] = n;
        }
        if (D_800EFD70.f4C == 9 && (D_800F05A8.f58 & 0x80)) {
            changed = 1;
            D_800F05A8.f58 &= ~0x80;
        }
        if (func_8002B870() != 0) {
            D_800E7FC8[D_800C9684].flags |= 2;
        } else {
            D_800E7FC8[D_800C9684].flags &= ~2;
        }
        if (D_800EE140 != 0) {
            func_8002D888(&D_800E37C0[D_800E78B0], buf);
        } else {
            if ((D_800C8CE8 != 0 || D_800CCD64 != 0 || (D_800EFD70.f0 & 0x81)) &&
                (D_800E7FC8[D_800C9684].flags & 2)) {
                if (!(D_800E7FC8[D_800C9684].flags & 1)) {
                    D_800E7FC8[D_800C9684].flags |= 1;
                    func_8002D890();
                }
                switch (D_800E7FC8[D_800C9684].mode) {
                case 0:
                    func_8002BFE0(&D_800E37C0[D_800E78B0], buf);
                    break;
                case 1:
                    func_8002BD40(&D_800E37C0[D_800E78B0], buf);
                    break;
                }
            } else {
                { CamFlags *bs = D_800E7FC8; e = &bs[D_800C9684]; }
                f = e->flags;
                if (f & 1) {
                    mode = e->mode;
                    e->flags = f & 0xFFFE;
                    if (mode < 2) if (mode >= 0) {
                        changed = 1;
                    }
                    m = D_800E37C0[D_800E78B0].f5AA;
                    sel = 1;
                } else {
                    k = D_800C9684;
                    if (D_800C8CD8 != 0 && D_800E3700 == 0) {
                        k = 1;
                    }
                    if ((D_800E3770[k] & 0x80) && !(D_800E36E0[k] & 0x80) && !(D_800F05A8.f58 & 0x800) &&
                        D_800F05A8.f0 != 0 && D_800F05A8.f40 == 0) {
                        s32 md = D_800E37C0[D_800E78B0].f5AA;
                        if (md != 9 && D_800EFDBC != 7) {
                            sel = 1;
                            { Player *b = (Player *)((u32)D_800E37C0 + ((md & 1) >> 1)); s5 = &b[D_800E78B0].f54; }
                            m = md;
                            if (D_800EFDBC == 6 && D_800E37C0[D_800E78B0].b3FF == 0) {
                                m ^= 6;
                            } else {
                                do {
                                    m++;
                                    if (m >= 12) {
                                        m = 0;
                                    }
                                } while (D_800C9698[m].fC & 8);
                            }
                        }
                    }
                }
            }
            if (sel) {
                cm = &D_800C9698[D_800E37C0[D_800E78B0].f5AA];
                if (cm->fC & 2) {
                    changed = 1;
                }
                if (cm->f8 != NULL) {
                    cm->f8();
                }
                pl = &D_800E37C0[D_800E78B0];
                pl->f5A8 = m;
                pl->f5AA = m;
                D_800C966C[D_800C9684] = (s16)m;
                cm = &D_800C9698[pl->f5AA];
                if (cm->fC & 1) {
                    changed = 1;
                }
                if (cm->f0 != NULL) {
                    cm->f0();
                }
            }
            {
            if (!(D_800E7FC8[D_800C9684].flags & 1)) {
                cm = &D_800C9698[D_800E37C0[D_800E78B0].f5AA];
                if (D_800C2384 != 0 && D_800C9684 == 1) {
                    cm = &D_800C9698[7];
                }
                if (cm->f4 != NULL) {
                    cm->f4(&D_800E37C0[D_800C9650 != -1 ? D_800C9650 : D_800E78B0], buf);
                    if (changed) {
                        if (s5 != NULL) {
                            func_8002B6F4(buf, s5);
                        } else {
                            func_8002B5F8(buf);
                        }
                    }
                }
            }
            }
        }
    }
    t = D_800C9684;
    D_800E8090[t].x = D_800E8030[t].x / 16;
    D_800E8090[t].y = D_800E8030[t].y / 16 + D_800E7968[t];
    D_800E8090[t].z = D_800E8030[t].z / 16;
    D_800E7978[t] = D_800E8030[t].w;
    if (D_800E7968[t] != 0) {
        dx = (f32)D_800E37C0[t].x - (f32)D_800E8090[t].x;
        dz = (f32)D_800E37C0[t].z - (f32)D_800E8090[t].z;
        d = __builtin_sqrtf(dx * dx + dz * dz);
        D_800E7978[D_800C9684] += (s32)((f32)D_800E7968[D_800C9684] / d * 32768.0f / 3.1415927f);
    }
    D_800E78B8[D_800C9684] = D_800E8030[D_800C9684].f10;
    D_800E8000[D_800C9684] = D_800E8030[D_800C9684].f14;
    D_800E7FE8++;
    if (!(D_800E7FC8[D_800C9684].flags & 1) && D_800E37C0[D_800C9684].f5AA != 9) {
        D_800E7FF0[D_800C9684] = 0;
        func_8002CC30(D_800E78B0);
        return;
    }
    pos.x = (f32)D_800E8090[D_800C9684].x / 1000.0f;
    pos.y = (f32)D_800E8090[D_800C9684].y / 1000.0f;
    pos.z = (f32)D_800E8090[D_800C9684].z / 1000.0f;
    func_8004AB24(&pos, &area, &hh, D_800E37C0[D_800E78B0].f628);
    D_800E7FF0[D_800C9684] = D_800EE498[area].b64;
}
