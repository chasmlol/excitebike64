#include "game.h"

typedef struct { u8 pad[0x20]; f32 pos; f32 vel; } S28;

struct Rec6 {
    s32 x, y, z;
    s32 rx, ry, rz;
};

struct Rec_4 {
    s32 x, y, z, a, b, c;
};

struct Ent_2 { s32 w[6]; };

typedef struct {
    s16 x, z;
} Pt;

typedef struct {
    u16 f0, f2, f4;
    u16 n;
    Pt pts[1];
} Poly;

typedef struct { u16 n; u16 id[1]; } Cell;

typedef struct {
    u8 pad[4];
    s16 x, z;
} Ent_28;

typedef struct {
    s32 x, y, z, w, h, f14, f18;
} Node_5;

typedef struct { s16 x, z, y, h; } Vtx8;

typedef struct {
    s32 x, d, y;
} Pt_3;

extern Ent_2 D_800E8030[];
extern S28 D_800E78C8[];
extern Poly **D_800E8E6C;
extern Cell **D_800E8D60;
extern Ent_28 *D_800E8538[];
extern Node_5 D_800E8D78[];
extern Pt_3 *D_800C9840;

void func_80028600(s32 arg0)
{
    s32 i, j;

    func_8000ED10(D_800E7968, 0, 0x10);
    func_8000ED10(D_800E7FA8, 0, 0x20);
    D_800E80C0_2 = func_800BC908_2(D_800EE490 << 1, 0, D_800D6A70, 0x239);
    func_80028790();
    func_8000ED10(D_800E8030, 0, 0x60);
    func_8000ED10(D_800E7998, 0, 0x600);
    func_8000ED10(D_800E7988, 0, 0x10);
    func_8000ED10(D_800E7FC8_2, 0, 0x20);
    func_8000ED10((u8 *)D_800E78C8, 0, 0xa0);
    func_8000ED10(D_800E8010, 0, 0x10);
    D_800E7FE8 = 0;
    for (i = 0; i < 4; i++) {
        D_800E8090_4[i].a = 0;
        D_800E8090_4[i].b = 0xbb0;
        D_800E8090_4[i].c = 0;
        D_800C9654[i] = -1;
        for (j = 0; j < 0x78; j++) {
            D_800E78C8[i].vel += (2000.0f - D_800E78C8[i].pos) * D_800C9688;
            D_800E78C8[i].pos += D_800E78C8[i].vel;
            D_800E78C8[i].vel *= D_800C968C;
        }
    }
}

typedef struct {
    u8 unk0[0x46];
    s16 next;
    s16 branch;
    s16 unk4A;
    s16 unk4C;
    u8 unk4E[0x6C - 0x4E];
} Seg_5;

extern Seg_5 *D_800EE498_5;

void func_80028790(void) {
    s32 j;
    s32 i;
    s32 n;

    i = 0;
    n = 0;
    do {
        D_800E80C0[i] = n;
        if (D_800EE498_5[i].branch != -1) {
            j = D_800EE498_5[i].branch;
            n++;
            do {
                D_800E80C0[j] = n;
                j = D_800EE498_5[j].next;
                n++;
            } while (D_800EE498_5[j].unk4C == -1);
        } else {
            n++;
        }
        i = D_800EE498_5[i].next;
    } while (i != 0);
}

struct Player_10 {
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
};

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
} Game_13;

typedef struct {
    s32 f0;
    u8 pad4[0x26 - 4];
    s16 f26;
    u8 pad28[0x40 - 0x28];
    s32 f40;
    u8 pad44[0x58 - 0x44];
    s32 f58;
} Cam_2;

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
    u8 pad[0x64];
    u8 b64;
    u8 pad2[0x6C - 0x65];
} Area;

extern CamMode D_800C9698_2[];
extern Player_10 D_800E37C0_25[];
extern CamFlags D_800E7FC8_3[];
extern CamPos D_800E8030_2[];
extern Area *D_800EE498_6;
extern Game_13 D_800EFD70_28;
extern Cam_2 D_800F05A8_12;

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
    Player_10 *pl;
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
    if (D_800C9684 < D_800EFD70_28.f3C && D_800EFD70_28.f4C != 7 && D_800EFD70_28.f4C != 2) {
        if (D_800E37C0_25[D_800C9684].f5AA != 10) {
            D_800EFD70_28.f760[D_800EFD70_28.f2C8[D_800C9684]] = D_800E37C0_25[D_800C9684].f5AA;
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
    if (D_800F05A8_12.f0 == 0) {
        m2 = D_800F05A8_12.f26;
        if (m2 == 0xFF && D_800C9684 == 0) {
            func_8004F5C8(0);
        }
        if ((D_800E3770[0] & 0x10) || D_800EFD64 == D_800CB7B0) {
            if (m2 >= 2) {
                m2 = 1;
            }
            D_800F05A8_12.f26 = m2;
            changed = 1;
        }
    }
    if (D_800EFD70_28.f0 & 0x10) {
        t = D_800EFD70_28.f1C[D_800C9684];
        D_800E78B0 = t;
        func_80029830(&D_800E37C0_25[t], buf, t != D_800C9654[D_800C9684]);
        D_800C9654[D_800C9684] = D_800E78B0;
    } else {
        {
            s32 u = D_800E78B0;
            s32 *bs = D_800C9654;
            s32 kk = D_800C9684;
            s32 n;
            if (u != bs[kk]) changed = 1;
            bs[kk] = u;
            n = D_800E37C0_25[u].f5AA;
            if (n != D_800C966C[kk]) changed = 1;
            D_800C966C[kk] = n;
        }
        if (D_800EFD70_28.f4C == 9 && (D_800F05A8_12.f58 & 0x80)) {
            changed = 1;
            D_800F05A8_12.f58 &= ~0x80;
        }
        if (func_8002B870() != 0) {
            D_800E7FC8_3[D_800C9684].flags |= 2;
        } else {
            D_800E7FC8_3[D_800C9684].flags &= ~2;
        }
        if (D_800EE140 != 0) {
            func_8002D888(&D_800E37C0_25[D_800E78B0], buf);
        } else {
            if ((D_800C8CE8 != 0 || D_800CCD64 != 0 || (D_800EFD70_28.f0 & 0x81)) &&
                (D_800E7FC8_3[D_800C9684].flags & 2)) {
                if (!(D_800E7FC8_3[D_800C9684].flags & 1)) {
                    D_800E7FC8_3[D_800C9684].flags |= 1;
                    func_8002D890();
                }
                switch (D_800E7FC8_3[D_800C9684].mode) {
                case 0:
                    func_8002BFE0(&D_800E37C0_25[D_800E78B0], buf);
                    break;
                case 1:
                    func_8002BD40(&D_800E37C0_25[D_800E78B0], buf);
                    break;
                }
            } else {
                { CamFlags *bs = D_800E7FC8_3; e = &bs[D_800C9684]; }
                f = e->flags;
                if (f & 1) {
                    mode = e->mode;
                    e->flags = f & 0xFFFE;
                    if (mode < 2) if (mode >= 0) {
                        changed = 1;
                    }
                    m = D_800E37C0_25[D_800E78B0].f5AA;
                    sel = 1;
                } else {
                    k = D_800C9684;
                    if (D_800C8CD8 != 0 && D_800E3700 == 0) {
                        k = 1;
                    }
                    if ((D_800E3770[k] & 0x80) && !(D_800E36E0_3[k] & 0x80) && !(D_800F05A8_12.f58 & 0x800) &&
                        D_800F05A8_12.f0 != 0 && D_800F05A8_12.f40 == 0) {
                        s32 md = D_800E37C0_25[D_800E78B0].f5AA;
                        if (md != 9 && D_800EFDBC != 7) {
                            sel = 1;
                            { Player_10 *b = (Player_10 *)((u32)D_800E37C0_25 + ((md & 1) >> 1)); s5 = &b[D_800E78B0].f54; }
                            m = md;
                            if (D_800EFDBC == 6 && D_800E37C0_25[D_800E78B0].b3FF == 0) {
                                m ^= 6;
                            } else {
                                do {
                                    m++;
                                    if (m >= 12) {
                                        m = 0;
                                    }
                                } while (D_800C9698_2[m].fC & 8);
                            }
                        }
                    }
                }
            }
            if (sel) {
                cm = &D_800C9698_2[D_800E37C0_25[D_800E78B0].f5AA];
                if (cm->fC & 2) {
                    changed = 1;
                }
                if (cm->f8 != NULL) {
                    cm->f8();
                }
                pl = &D_800E37C0_25[D_800E78B0];
                pl->f5A8 = m;
                pl->f5AA = m;
                D_800C966C[D_800C9684] = (s16)m;
                cm = &D_800C9698_2[pl->f5AA];
                if (cm->fC & 1) {
                    changed = 1;
                }
                if (cm->f0 != NULL) {
                    cm->f0();
                }
            }
            {
            if (!(D_800E7FC8_3[D_800C9684].flags & 1)) {
                cm = &D_800C9698_2[D_800E37C0_25[D_800E78B0].f5AA];
                if (D_800C2384 != 0 && D_800C9684 == 1) {
                    cm = &D_800C9698_2[7];
                }
                if (cm->f4 != NULL) {
                    cm->f4(&D_800E37C0_25[D_800C9650 != -1 ? D_800C9650 : D_800E78B0], buf);
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
    D_800E8090[t].x = D_800E8030_2[t].x / 16;
    D_800E8090[t].y = D_800E8030_2[t].y / 16 + D_800E7968[t];
    D_800E8090[t].z = D_800E8030_2[t].z / 16;
    D_800E7978[t] = D_800E8030_2[t].w;
    if (D_800E7968[t] != 0) {
        dx = (f32)D_800E37C0_25[t].x - (f32)D_800E8090[t].x;
        dz = (f32)D_800E37C0_25[t].z - (f32)D_800E8090[t].z;
        d = __builtin_sqrtf(dx * dx + dz * dz);
        D_800E7978[D_800C9684] += (s32)((f32)D_800E7968[D_800C9684] / d * 32768.0f / 3.1415927f);
    }
    D_800E78B8[D_800C9684] = D_800E8030_2[D_800C9684].f10;
    D_800E8000[D_800C9684] = D_800E8030_2[D_800C9684].f14;
    D_800E7FE8++;
    if (!(D_800E7FC8_3[D_800C9684].flags & 1) && D_800E37C0_25[D_800C9684].f5AA != 9) {
        D_800E7FF0_2[D_800C9684] = 0;
        func_8002CC30_2(D_800E78B0);
        return;
    }
    pos.x = (f32)D_800E8090[D_800C9684].x / 1000.0f;
    pos.y = (f32)D_800E8090[D_800C9684].y / 1000.0f;
    pos.z = (f32)D_800E8090[D_800C9684].z / 1000.0f;
    func_8004AB24(&pos, &area, &hh, D_800E37C0_25[D_800E78B0].f628);
    D_800E7FF0_2[D_800C9684] = D_800EE498_6[area].b64;
}

typedef struct {
    f32 x, y, z;
    u8 pad0C[0x46 - 0x0C];
    s16 next;
    u8 pad48[0x64 - 0x48];
    u8 flags;
    u8 pad65[0x6C - 0x65];
} Seg_6;

struct Obj_13 {
    u8 pad0[0x54];
    f32 vx;
    f32 f58;
    f32 vz;
    u8 pad60[0xB8 - 0x60];
    f32 dx;
    f32 fBC;
    f32 dz;
    u8 padC4[0x416 - 0xC4];
    s16 f416;
    u8 pad418[0x494 - 0x418];
    f32 px, py, pz;
    u8 pad4A0[0x4C0 - 0x4A0];
    f32 f4C0;
    f32 f4C4;
    f32 f4C8;
    u8 pad4CC[0x628 - 0x4CC];
    s16 seg;
};

extern Seg_6 *D_800EE498_7;

f32 func_800292E4(Obj_13 *self)
{
    s32 i;
    s32 k;
    s32 n;
    f32 arr[3];
    f32 f0;
    f32 f3;
    f32 t;
    f32 dx, dz;
    f32 q1, q2;

    i = self->seg;
    f3 = 0.0f;
    if ((D_800EE498_7[i].flags & 1) == 0) {
        f0 = self->vx * self->dx + self->vz * self->dz;
        if (f0 < 2.0f) {
            f0 = 2.0f;
        }
        if (16.0f < f0) {
            f0 = 16.0f;
        }
        arr[0] = self->px + self->dx * f0;
        arr[2] = self->pz + self->dz * f0;
        arr[1] = self->py;
        t = func_800228A0(arr);
        if (D_800E8508 != 0) {
            f3 = (self->py - t) * (8192.0f / f0);
        }
    } else {
        k = i;
        for (n = 5; n >= 0; n--) {
            k = D_800EE498_7[k].next;
        }
        dx = D_800EE498_7[k].x - self->px;
        dz = D_800EE498_7[k].z - self->pz;
        t = __builtin_sqrtf(dx * dx + dz * dz);
        f3 = -(D_800EE498_7[k].y - self->py) * (8192.0f / t);
    }
    if (f3 < -2048.0f) {
        f3 = -2048.0f;
    } else if (2048.0f < f3) {
        f3 = 2048.0f;
    }
    if (self->f416 == 0) {
        f3 = f3 + 2000.0f;
    } else {
        f3 = f3 + 0.0f;
    }
    q1 = self->vx;
    q2 = self->vz;
    f0 = __builtin_sqrtf(q1 * q1 + q2 * q2);
    if (f0 == 0.0f) {
        f0 = 1.0f;
    }
    q1 = q1 / f0;
    q2 = q2 / f0;
    f0 = self->f4C0 * q1 + self->f4C8 * q2;
    if (0.2f < f0) {
        f0 = (f32)((s16)func_80013010((s32)(f0 * 16384.0f)) + 0x400);
        if (f3 < f0) {
            f3 = f0;
        }
    }
    return f3;
}

#define FLD(off) (*(f32 *)(p + (off)))

f32 func_800295E0(u8 *p)
{
    f32 fVar2, fVar3;
    f32 fVar4, r;
    f32 fVar5;
    f32 A[3];
    f32 B[3];
    s32 iVar1;
    f32 tt;



    if (*(s32 *)(p + 0x744) > 0 && 0.25f < FLD(0x6f4)) {
        fVar4 = 0.5f;
        A[0] = (FLD(0x6fc) + FLD(0x71c)) * fVar4 - FLD(0x6c);
        A[1] = (FLD(0x700) + FLD(0x720)) * fVar4 - FLD(0x70);
        A[2] = (FLD(0x704) + FLD(0x724)) * fVar4 - FLD(0x74);
        fVar2 = func_80013098(__builtin_sqrtf(A[0] * A[0] + A[2] * A[2]) + 1.0f, -A[1] - 1.0f);
        fVar2 = (fVar2 * 32768.0f) / 3.1415927f;
        if (FLD(0x58) < 0.0f) {
            B[0] = FLD(0x708) + FLD(0x728);
            B[1] = FLD(0x70c) + FLD(0x72c);
            B[2] = FLD(0x710) + FLD(0x730);
            fVar3 = func_80013408(B, A);
            r = fVar3 * fVar4;
            r = r / __builtin_sqrtf(A[0] * A[0] + A[1] * A[1] + A[2] * A[2]);
            if (-0.25f < r) {
                tt = r + 0.25f;
                iVar1 = func_80013010((s32)(tt * 16384.0f));
                fVar2 = fVar2 + (f32)(s32)(s16)iVar1;
            }
        }
        D_800E8020[D_800C9684] = fVar2;
    } else if (*(s32 *)(p + 0x744) >= -31) {
        fVar2 = D_800E8020[D_800C9684];
    } else {
        fVar2 = 2000.0f;
    }
    return fVar2;
}

typedef struct {
    s32 flags;
    u8 pad0[0x3C - 4];
    s32 mode;
    u8 pad1[0x2A8 - 0x40];
    s32 tab[16][2];
    u8 pad2[0x408 - 0x2A8 - 0x80];
    s16 f408;
    s16 pad3;
    u8 f40C;
} Ctl_6;

struct Obj_14 {
    u8 pad[0x6C];
    f32 x, y, z;
};

extern Ctl_6 D_800EFD70_29;

void func_80029830(Obj_14 *p, s32 *out, s32 arg2)
{
    s32 buf[10];
    s32 in[3];
    s32 res[3];
    s32 amp, yoff;
    s32 r, z;
    s32 *e;
    Ctl_6 *g = &D_800EFD70_29;

    if (g->flags & (0x400 << D_800C9684)) {
        amp = 4000;
        yoff = 0x672;
    } else {
        amp = 6000;
        if (g->mode == 2 && D_800C2388 != 0) {
            amp = 7200;
        }
        yoff = 0x8FC;
    }
    e = D_800EFD70_29.tab[D_800C9684];
    out[0] = p->x * 1000.0f + (f32)((D_800C2C60[((e[0] >> 4) - 0x400) & 0xFFF] * amp) >> 14);
    out[1] = p->y * 1000.0f + (f32)yoff;
    r = 0;
    out[2] = p->z * 1000.0f + (f32)((D_800C2C60[(e[0] >> 4) & 0xFFF] * amp) >> 14);
    z = 0;
    out[3] = 0xE10;
    out[4] = 0;
    out[5] = (0x8000U - e[0]) & 0xFFFF;
    func_80011A58(buf, out[3], out[4], out[5]);
    switch (D_800EFD70_29.mode) {
    case 1:
        switch (D_800EFD70_29.f40C) {
        case 1:
            r = 0x3E - D_800EFD70_29.f408;
            break;
        case 2:
            r = 0x3E;
            break;
        case 3:
            r = D_800EFD70_29.f408;
            break;
        }
        break;
    case 2:
        r = 0x19;
        if (D_800C2388 != 0) {
            z = -0x226;
        } else {
            r = -0x1E;
            z = -0x64;
        }
        break;
    default:
        r = 0x1E;
        z = -0x1F4;
        break;
    }
    r = (r * -0x564) / 0x3E;
    if (D_800EFD70_29.flags & (0x400 << D_800C9684)) {
        in[0] = 0;
        in[1] = 0;
        in[2] = 0;
    } else {
        in[0] = r + 600;
        in[1] = z;
        in[2] = 0;
    }
    func_80012DC8(in, buf, res);
    out[0] += res[0];
    out[1] += res[1];
    out[2] += res[2];
    func_8002B5F8(out);
}

#define FL(o) (*(f32 *)(s + (o)))
#define HW(o) (*(s16 *)(s + (o)))
#define WD(o) (*(s32 *)(s + (o)))
#define BY(o) (*(u8 *)(s + (o)))
#define SB(o) (*(s8 *)(s + (o)))

int func_80029B1C(u8 *s)
{
    struct { u16 a[2]; s32 b; u16 c[4]; } L;
    s32 iv1, iv2, a;

    if (BY(0x44) != 0) goto done;
    func_8001254C((s32)(-FL(0xB8) * 1000.0f), (s32)(FL(0xC0) * 1000.0f), L.a, &L.b);
    func_8001254C((s32)(-FL(0x54) * 1000.0f), (s32)(FL(0x5C) * 1000.0f), L.c, &L.b);
    L.b = (s16)(L.a[0] - L.c[0]);
    if (((WD(0x5B0) & 0x10000) == 0) || (WD(0x744) >= 0)) goto normal;
    a = SB(0x5D0);
    if (a < 0) a = -a;
    if (a < 6) goto normal;
    if (HW(0x36) < 0x10) HW(0x36) = HW(0x36) + 1;
    goto after;
normal:
    if (BY(0x41B) == 1) {
        if (HW(0x36) < 0x18) HW(0x36) = HW(0x36) + 1;
    } else {
        a = L.b;
        if (a < 0) a = -a;
        if (a < 0x800 && HW(0x36) != 0) HW(0x36) = HW(0x36) - 1;
    }
after:
    iv2 = 0;
    if (WD(0x744) < 0) iv2 = HW(0x36);
    if (FL(0x424) < 2.0f) iv2 = 0x20;
    iv1 = L.b * iv2;
    iv2 = (s16)L.c[0];
    if (iv1 < 0) iv1 = iv1 + 0x1f;
    L.b = iv2 + (iv1 >> 5);
    HW(0x34) = ((u16 *)&L.b)[1];
done:
    return HW(0x34);
}

#define COST(i) D_800C2C60[(((i) >> 4) - 0x400) & 0xFFF]
#define SINT(i) D_800C2C60[((i) >> 4) & 0xFFF]

struct Bike_16 {
    u8 pad0[0x48];
    f32 f48;
    f32 f4C;
    f32 f50;
    u8 pad54[0x7E0 - 0x54];
};

typedef struct {
    u8 pad0[0x58];
    s32 f58;
    u8 pad5C[0x6C - 0x5C];
    s16 f6C;
} State_3;

extern State_3 D_800F05A8_13;
extern Bike_16 D_800E37C0_26[];

s32 func_80029CE0(Bike_16 *bike, Rec6 *out) {
    u16 a1;
    s32 ratio;
    u16 a2;
    f32 s;
    f32 fz;
    s32 n;
    s32 z;
    s32 i;
    s32 ang, q, ang3, q3;

    n = D_800F05A8_13.f58 & 0xF;
    if (n != 0) {
    i = n - 1;
    if (i == ((bike - D_800E37C0_26) >> ((D_800EFDAC - 1) >> 1))) {
        switch (n) {
        case 1:
            s = 1000.0f;
            q = D_800F05A8_13.f6C * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = D_800C2C60;
            ang = D_800F05A8_13.f6C * 15000 / 180 - 8000;
            out->x = D_800F1510.x * s + (f32)(((r - q) * tb[(((ang) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)D_800F05A8_13.f6C * s * 3.0f / 180.0f;
            fz = D_800F1510.z * s - (f32)(((0x4844 - D_800F05A8_13.f6C * 10500 / 180) * SINT(D_800F05A8_13.f6C * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            func_8001254C(D_800F1510.x * s - (f32)out->x, D_800F1510.z * s - (f32)z, &a1, &ratio);
            func_8001254C(ratio, D_800F1510.y * s - (f32)out->y, &a2, &ratio);
            out->ry = 0;
            out->rx = a2 - 0x4000;
            out->rz = -a1;
            break;
        case 2:
            s = 1000.0f;
            q3 = D_800F05A8_13.f6C * 10500 / 180;
            { s32 r = 0x4844; s16 *tb = D_800C2C60;
            ang3 = D_800F05A8_13.f6C * 15000 / 180 - 8000;
            out->x = D_800F1510.x * s - (f32)(((r - q3) * tb[(((ang3) >> 4) - 0x400) & 0xFFF]) >> 14); }
            out->y = 6000.0f - (f32)D_800F05A8_13.f6C * s * 3.0f / 180.0f;
            fz = D_800F1510.z * s + (f32)(((0x4844 - D_800F05A8_13.f6C * 10500 / 180) * SINT(D_800F05A8_13.f6C * 15000 / 180 - 8000)) >> 14);
            out->z = z = fz;
            func_8001254C(D_800F1510.x * s - (f32)out->x, D_800F1510.z * s - (f32)z, &a1, &ratio);
            func_8001254C(ratio, D_800F1510.y * s - (f32)out->y, &a2, &ratio);
            out->ry = 0;
            out->rx = a2 - 0x4000;
            out->rz = -a1;
            break;
        default:
            func_8000ECB8(1);
        }
    } else switch (n) {
    case 1: {
        Bike_16 *p = &D_800E37C0_26[i];
        out->x = p->f48 * 1000.0f + (f32)((COST(D_800F05A8_13.f6C * 14000 / 180 - 7000) * 625) >> 11);
        out->y = 1000;
        out->z = p->f50 * 1000.0f - (f32)((SINT(D_800F05A8_13.f6C * 14000 / 180 - 7000) * 625) >> 11);
        out->rx = 1000;
        out->rz = D_800F05A8_13.f6C * 14000 / 180 - 7000;
        out->ry = 0;
        break; }
    case 2: {
        Bike_16 *p = &D_800E37C0_26[i];
        out->x = p->f48 * 1000.0f + (f32)((COST(7000 - D_800F05A8_13.f6C * 14000 / 180) * 625) >> 11);
        out->y = 1000;
        out->z = p->f50 * 1000.0f + (f32)((SINT(7000 - D_800F05A8_13.f6C * 14000 / 180) * 625) >> 11);
        out->rx = 1000;
        out->rz = D_800F05A8_13.f6C * 14000 / 180 + 0x64A8;
        out->ry = 0;
        break; }
    }
    return func_8002B5F8_2(out);
    } else {
        return func_8002A8C0_2(bike, out, -7000);
    }
}

f32 func_8002A4EC(u8 *param_1)
{
    f32 fs;
    f32 v[3];
    f32 len;
    f32 d;
    f32 t;
    f32 k8;
    f32 s2;
    u8 fl;
    s32 *gp = &D_800C9760;

    fl = D_800EE498[*(s16 *)(param_1 + 0x628) * 0x6C + 0x64];
    fs = (f32)*gp;
    if ((fl & 1) == 0) {
        s2 = *(f32 *)(param_1 + 0xB8) * *(f32 *)(param_1 + 0xB8) +
             *(f32 *)(param_1 + 0xC0) * *(f32 *)(param_1 + 0xC0);
        k8 = 8.0f;
        len = sqrtf(s2);
        v[0] = *(f32 *)(param_1 + 0x494) + (*(f32 *)(param_1 + 0xB8) * k8) / len;
        v[2] = *(f32 *)(param_1 + 0x49C) + (*(f32 *)(param_1 + 0xC0) * k8) / len;
        v[1] = *(f32 *)(param_1 + 0x498);
        d = func_800228A0(v);
        v[1] = d;
        if (D_800E8508 != 0) {
            t = *(f32 *)(param_1 + 0x498) - d;
            fs = t * 1024.0f;
            if ((D_800C2280 & 8) == 0) {
                if (fs < -2048.0f) fs = -2048.0f;
                else if (fs > 1024.0f) fs = 1024.0f;
            } else {
                if (fs < -1024.0f) fs = -1024.0f;
                else if (fs > 4096.0f) fs = 4096.0f;
            }
        }
    }
    return fs + 2000.0f;
}

typedef struct {
    s32 x, y, z, w3, w4, w5;
} TblEnt;

struct Obj_15 {
    u8 pad0[0x34];
    u16 s34;
    u8 pad1[0x0E];
    u8 f44;
    u8 pad2[0x03];
    V3 pos;
    u8 pad3[0x5D0];
    u16 c624;
};

void func_8002A680(Obj_15 *p, s32 *out)
{
    s32 v[4];
    s32 r[4];
    s32 m[10];
    s16 ang;
    s32 dist;
    TblEnt *tb;
    TblEnt *base;
    s32 z2, a, b;
    V3 *pv;
    s32 z, d;
    s16 rot, h;

    if (p->f44 == 0) {
        rot = p->s34;
        h = func_8002A4EC((u8 *)p);
        func_80011A58(m, h, 0, rot);
        v[0] = 0;
        v[1] = 0;
        v[2] = -D_800C9758;
        func_80012DC8(v, m, r);
        pv = &p->pos;
        out[0] = r[0] + pv->x * 1000.0f;
        out[1] = r[1] + pv->y * 1000.0f + D_800C975C;
        out[2] = r[2] + pv->z * 1000.0f;
        out[3] = h;
        out[4] = 0;
        out[5] = rot;
    } else {
        base = D_800E8030;
        tb = &base[D_800C9684];
        out[0] = tb->x / 16;
        out[1] = tb->y / 16;
        z = tb->z;
        if (z < 0) {
            z += 0xF;
        }
        z2 = z >> 4;
        out[2] = z2;
        a = out[0] - (s32)(p->pos.x * 1000.0f);
        b = (s32)(p->pos.z * 1000.0f) - z2;
        d = (s32)(p->pos.y * 1000.0f) + D_800C975C - out[1];
        func_8001254C(a, b, &ang, &dist);
        if (dist > 10000) {
            p->c624 += 3;
        }
        out[5] = ang;
        func_8001254C(-d, dist, &ang, &dist);
        out[3] = ang;
        out[4] = 0;
    }
    func_8002B348(out);
}

typedef struct { u32 flags; u8 pad4[0x48]; s32 mode; } Game_14;
typedef struct { s32 active; u8 pad4[0x22]; s16 t; } Rep;

struct Bike_17 {
    u8 pad0[0x34];
    u16 h34;
    u8 pad36[0x12];
    f32 pos[3];
    f32 v54[3];
    u8 pad60[0x48C - 0x60];
    f32 f48C;
    u8 pad490[0x4E4 - 0x490];
    Trk *p4E4;
    u8 pad4E8[0x5B0 - 0x4E8];
    u32 f5B0;
    u32 f5B4;
    u8 pad5B8[0x5CD - 0x5B8];
    u8 b5CD;
    u8 pad5CE[0x6F4 - 0x5CE];
    f32 f6F4;
    u8 pad6F8[0x744 - 0x6F8];
    s32 f744;
    u8 pad748[0x77C - 0x748];
    s16 h77C;
};

extern Game_14 D_800EFD70_30;
extern Rep D_800F05A8_14;

void func_8002A8C0(Bike_17 *p, s32 *out, s32 z)
{
    s32 n, k, ang, tv, zero;
    s32 rot;
    f32 f, g, nv, fc, a, b, c, d;
    s32 dy, dz;
    s32 vec[3];
    s32 res[3];
    s32 mtx[10];
    f32 *pp;
    S28 *e, *eb;
    s32 *q, *cnt;
    s32 tb;
    s32 *q2;
    f32 cs;

    k = p->f744;
    cnt = D_800E8010_2;
    n = cnt[D_800C9684];
    if (p->f6F4 * 60.0f + (f32)k > 60.0f) {
        if (n < 0x20) {
            n++;
        }
    } else if (k < 0) {
        if (n == 0) goto first;
        n--;
    }
    if (n == 0) {
first:
        f = func_800292E4(p);
    } else if (n == 0x20) {
        f = func_800295E0(p);
    } else {
        a = func_800292E4(p);
        b = func_800295E0(p);
        f = (a * (f32)(0x20 - n) + b * (f32)n) * 0.03125f;
    }
    D_800E8010_2[D_800C9684] = n;
    eb = D_800E78C8;
    e = &eb[D_800C9684];
    f = f - e->pos;
    if (f > 0.0f) {
        nv = e->vel;
        nv += f * D_800C9688;
        fc = D_800C968C;
        e->vel = nv;
        do { d = nv; } while (0);
    } else {
        nv = e->vel;
        nv += f * D_800C9690;
        fc = D_800C9694;
        e->vel = nv;
        do { d = nv; } while (0);
    }
    d *= fc;
    e->pos += nv;
    do { e->vel = d; } while (0);
    if ((zero = D_800EFD70_30.mode, rot = p->h34, zero != 9) && !(D_800EFD70_30.flags & 0x8000)) {
        if ((p->f5B0 & 0x200) && D_800F05A8_14.active != 0) {
            rot += -0x8000;
        }
    }
    ang = D_800E78C8[D_800C9684].pos;
    if (p->h77C != 0) {
        ang = -500;
    }
    vec[0] = 0;
    vec[1] = 0;
    vec[2] = z;
    if (D_800F05A8_14.active == 0) {
        g = (f32)D_800F05A8_14.t * 0.00390625f;
        if (g < 0.5f) {
            g = 2.0f * (g * g);
        } else {
            g = 1.0f - g;
            g = 2.0f * (g * g);
            g = 1.0f - g;
        }
        tv = g * 36864.0f;
        tb = D_800C2C60[(tv >> 4) & 0xFFF] / 8;
        dy = 0;
        dz = -12000;
        if (!(D_800EFD70_30.flags & 0x8000)) {
            rot += tv;
            vec[0] = (f32)tb * g;
            vec[1] = g * (f32)dy;
            vec[2] = (f32)z + g * (f32)dz;
        }
    }
    func_80011A58(mtx, ang, 0, (s16)rot);
    func_80012DC8(vec, mtx, res);
    pp = p->pos;
    out[0] = (f32)res[0] + pp[0] * 1000.0f;
    out[1] = (f32)res[1] + pp[1] * 1000.0f + 600.0f;
    out[2] = (f32)res[2] + pp[2] * 1000.0f;
    out[3] = ang - 500;
    out[5] = (s16)rot;
    out[4] = 0;
    if (D_800EFD70_30.mode != 9) {
        if ((p->f5B0 & 0x200) || ((D_800E3770_2 & 0x10) && D_800F05A8_14.active == 0 && !(D_800EFD70_30.flags & 0x8000))) {
            func_8002B5F8(out);
        } else if (p->f5B4 & 0x200) {
            func_8002B6F4(out, p->v54);
        } else {
            goto other;
        }
    } else {
other:
        func_8002B348(out);
    }
    D_800E7968[D_800C9684] = 0;
    if (p->p4E4 != NULL && p->f744 < 6 && (p->p4E4->flags & 0x10)) {
        D_800E7968[D_800C9684] = -(s32)(p->f48C * 1000.0f) * 2;
    }
    if (p->b5CD != 0) {
        f = p->b5CD;
        cs = func_80013774(-f * 0.0625f);
        q2 = D_800E7968;
        f = f * 3.1415927f;
        q = &q2[D_800C9684];
        *q += (s32)(func_800AEA30(f / 6.0f) * 1024.0f * cs);
    }
}

struct Ent_23 {
    u8 pad0[4];
    s32 y;
    u8 pad1[0x44C - 8];
    f32 f44C;
    u8 pad2[0x624 - 0x450];
    s16 h624;
    u8 pad3[0x7E0 - 0x626];
};

extern Ent_23 D_800E37C0_27[];

void func_8002AE5C(Ent_23 *e, Rec_4 *r)
{
    Rec_4 *b;
    s32 i;
    s32 d;

    b = D_800E8030;
    i = e - D_800E37C0_27;
    r->x = b[i].x / 16;
    r->y = b[i].y / 16;
    r->z = b[i].z / 16;
    d = e->y - r->y;
    r->a = b[i].a;
    r->c = b[i].c;
    r->b = 0;
    func_8002B5F8(r);
    if (D_800EFDBC == 7) {
        e->f44C = 0.5f;
        if (d < -12000) {
            e->h624 = 10000;
            e->f44C = 1.0f;
        }
    }
}

typedef struct { f32 v[16]; } Row_3;

extern Row_3 D_800C9080_5[];

void func_8002AF64(Ent_24 *p)
{
    Rec_4 loc;
    s32 m[9];
    s32 v[3];
    s32 o[3];
    s16 ang;
    s32 dist;
    f32 f;
    s32 idx;

    if (p->f5AA != 9) {
        D_800C966C[p - D_800E37C0_28] = 9;
        p->f5AA = 9;
        if (D_800EFD64 == D_800CB5D0) {
            loc = D_800C9764;
            func_8001254C(loc.x - p->f0, p->f8 - loc.z, &ang, &dist);
            loc.c = ang;
        } else {
            func_8004AC98(p);
            func_8001254C(-(D_800EE480 * 1000.0f), D_800EE484 * 1000.0f, &ang, &dist);
            if (0.0f < p->f638) {
                ang += 0x4000;
            } else {
                ang -= 0x4000;
            }
            p->f34 = ang;
            func_80011A58(m, 0x1000, 0, ang);
            f = D_800C9080_5[(D_800E84F8 >> 4) & 0xF].v[7];
            if (-0.1f < f) {
                f = -0.1f;
            }
            v[0] = 0;
            v[1] = 0;
            f *= 100000.0f;
            v[2] = f;
            func_80012DC8(v, m, o);
            loc.x = (s32)(p->x * 1000.0f) + o[0];
            loc.y = (s32)(p->y * 1000.0f) + o[1] - 1000;
            loc.z = (s32)(p->z * 1000.0f) + o[2];
            loc.a = 0x1000;
            loc.b = 0;
            loc.c = ang;
        }
        D_800C9684 = idx = p - D_800E37C0_28;
        func_8002B5F8(&loc);
        func_8004F064(idx);
    }
}

struct Bike_18 {
    u8 pad00[0x2C];
    s32 f2C;
    u8 pad30[0x6C - 0x30];
    f32 x, y, z;
    u8 pad78[0x3FE - 0x78];
    s8 f3FE;
    u8 pad3FF[0x744 - 0x3FF];
    s32 f744;
    u8 pad748[0x7E0 - 0x748];
};

void func_8002B210(Bike_18 *p, Rec6 *out)
{
    s32 a[3];
    s32 b[3];
    s32 m[10];

    if (p->f3FE != 0) {
        if (p->f744 <= 0) {
            p->f3FE--;
        }
        func_80011A58(m, 0x800, 0, p->f2C + D_800E1B40);
        D_800E1B40 -= 0x40;
        if (D_800E1B40 < 9000) {
            D_800E1B40 = 9000;
        }
        a[0] = 0;
        a[1] = 0;
        a[2] = -4000;
        func_80012DC8(a, m, b);
        out->x = (f32)b[0] + p->x * 1000.0f;
        out->y = (f32)b[1] + p->y * 1000.0f;
        out->z = (f32)b[2] + p->z * 1000.0f;
        out->rx = 0x800;
        out->rz = p->f2C + D_800E1B40;
        out->ry = 0;
        func_8002B5F8(out);
    }
}

extern Ent_2 D_800E7998_2[][16];

void func_8002B348(Ent_2 *p)
{
    s32 a, b, c, k, j, idx;
    Ent_2 *h, *s, *q;

    if (D_800E7FE8 == 0) {
        func_8002B5F8(p);
        return;
    }
    c = b = a = 0;
    idx = D_800E7988[D_800C9684];
        D_800E8030[D_800C9684].w[0] -= D_800E7998_2[D_800C9684][idx].w[0];
    D_800E8030[D_800C9684].w[1] -= D_800E7998_2[D_800C9684][idx].w[1];
    D_800E8030[D_800C9684].w[2] -= D_800E7998_2[D_800C9684][idx].w[2];
    D_800E7998_2[D_800C9684][idx] = *p;
        D_800E8030[D_800C9684].w[0] += D_800E7998_2[D_800C9684][idx].w[0];
    D_800E8030[D_800C9684].w[1] += D_800E7998_2[D_800C9684][idx].w[1];
    D_800E8030[D_800C9684].w[2] += D_800E7998_2[D_800C9684][idx].w[2];
    j = idx - 1;
    for (k = 0; k < 15; k++) {
        if (j < 0) j += 16;
                a += (s16)(D_800E7998_2[D_800C9684][j].w[5] - D_800E7998_2[D_800C9684][idx].w[5]);
        b += (s16)(D_800E7998_2[D_800C9684][j].w[4] - D_800E7998_2[D_800C9684][idx].w[4]);
        c += (s16)(D_800E7998_2[D_800C9684][j].w[3] - D_800E7998_2[D_800C9684][idx].w[3]);
        j--;
    }
    D_800E8030[D_800C9684].w[3] = c / 16 + D_800E7998_2[D_800C9684][idx].w[3];
    D_800E8030[D_800C9684].w[4] = b / 16 + D_800E7998_2[D_800C9684][idx].w[4];
    D_800E8030[D_800C9684].w[5] = a / 16 + D_800E7998_2[D_800C9684][idx].w[5];
    D_800E7988[D_800C9684] = (idx + 1) % 16;
}

void func_8002B5F8(Ent_2 *src)
{
    s32 i;
    s32 t2;
    s32 off;
    s32 v;
    u8 *base;
    s32 o;
    Ent_2 *d;

    i = 0;
    base = D_800E7998;
    do {
        v = D_800C9684;
        o = (i * 0x18 + v * 0x180) + (s32)base;
        i++;
        *(Ent_2 *)o = *src;
    } while (i < 16);
    D_800E8030[D_800C9684] = *src;
    d = &D_800E8030[D_800C9684];
    d->w[0] <<= 4;
    d->w[1] <<= 4;
    d->w[2] <<= 4;
}

struct S {
    s32 x, y, z, w, u, v;
};

extern S D_800E7998_3[][16];
extern S D_800E8030_3[];

void func_8002B6F4(S *p, f32 *v)
{
    s32 i;
    s32 sx, sy, sz;

    sx = sy = sz = 0;
    for (i = 15; i >= 0; i--) {
        D_800E7998_3[D_800C9684][i] = *p;
        sx += p->x;
        sy += p->y;
        sz += p->z;
        p->x -= (s32)(v[0] * 16.666666f);
        p->y -= (s32)(v[1] * 16.666666f);
        p->z -= (s32)(v[2] * 16.666666f);
    }
    D_800E8030_3[D_800C9684] = *p;
    D_800E8030_3[D_800C9684].x = sx;
    D_800E8030_3[D_800C9684].y = sy;
    D_800E8030_3[D_800C9684].z = sz;
    D_800E7988[D_800C9684] = 0;
}

typedef struct RecA {
    u8 pad0[0x14];
    s16 i0;
    s16 i1;
    u32 flags;
} RecA;

typedef struct RecB {
    u8 pad0[0x18];
    u32 flags;
    s32 i0;
    s32 i1;
} RecB;

typedef struct Obj_16 {
    u8 pad0[0x74];
    s16 nA;
    s16 nB;
    RecA *la;
    RecB *lb;
} Obj_16;

typedef struct Flag {
    u16 f0;
    s16 h2;
    void *p4;
} Flag;

typedef struct G {
    u8 pad0[0x3C];
    s32 f3C;
} G;

typedef struct Big {
    u8 pad0[0x628];
    s16 f628;
    u8 pad1[0x7E0 - 0x62A];
} Big;

typedef struct Node_3 {
    u8 pad0[0x4E];
    s16 next;
    u8 pad1[0x6C - 0x50];
} Node_3;

extern Flag D_800E7FC8_4[];
extern Node_3 *D_800EE498_8;
extern Obj_16 *D_800EFD64_20;
extern Obj_16 *objB_800EFD64;
extern Big D_800E37C0_29[];

s32 func_8002B870(void)
{
    s32 pass = 0;
    s32 cur;
    s32 k;
    s32 a, b, v;
    s32 d;
    s32 off;
    s16 *pv;
    Obj_16 *P;
    u16 *pf;
    Flag *fl;
    G *g;
    s16 *tab;
    void *rec;
    s32 a2, b2;
    void *rec2;

    if (D_800C9684 == 0) {
        D_800C97E4 = -1;
    }
    g = &D_800F05A8;
    fl = D_800E7FC8_4;
    P = D_800EFD64_20;
    tab = D_800E80C0;
    cur = D_800E37C0_29[D_800E78B0].f628;
    do {
        if (cur < 0) {
            goto done;
        }
        if (P->la != NULL && (k = 0, P->nA > 0)) {
            pv = (s16 *)(cur * 2 + (s32)tab);
            pf = (u16 *)(D_800C9684 * 8 + (s32)fl);
            off = k;
        loopA:
            {
                if (pass == 0 || ((*(RecA *)(off + (s32)P->la)).flags & 0x40)) {
                    b = tab[(*(RecA *)(off + (s32)P->la)).i1];
                    a = tab[(*(RecA *)(off + (s32)P->la)).i0];
                    if ((b < a && (*pv >= a || *pv < b)) ||
                        (a < b && *pv >= a && *pv < b)) {
                        if (g->f3C != 0 || !((*(RecA *)(off + (s32)P->la)).flags & 2)) {
                            ((s16 *)pf)[1] = 1;
                            rec = (RecA *)((u8 *)P->la + off);
                            v = rec != *(void **)(pf + 2);
                            *(void **)(pf + 2) = rec;
                            if (v || !(*pf & 2)) {
                                *pf |= 4;
                            }
                            if (pass > 0) {
                                *pf |= 8;
                            } else {
                                *pf &= ~8;
                            }
                            return 1;
                        }
                    }
                }
                if (pv == (s16 *)tab) v = 1; else v = 2;
                off += sizeof(RecA);
                k++;
            }
            if (k < D_800EFD64_20->nA) goto loopA;
        }
        if ((void *)g == (void *)P) k = 1; else k = 2;
        if (pass == 0) k = 1; else k = 2;
        if (tab == 0) k = 1; else k = 2;
        if (P->lb != NULL && (k = 0, P->nB > 0)) {
            pv = (s16 *)(cur * 2 + (s32)tab);
            d = D_800C9684;
            pf = (u16 *)(d * 8 + (s32)fl);
            off = k;
        loopB:
            {
                if (pass == 0 || ((*(RecB *)(off + (s32)P->lb)).flags & 0x40)) {
                    b2 = tab[(*(RecB *)(off + (s32)P->lb)).i1];
                    a2 = tab[(*(RecB *)(off + (s32)P->lb)).i0];
                    if ((b2 < a2 && (*pv >= a2 || *pv < b2)) ||
                        (a2 < b2 && *pv >= a2 && *pv < b2)) {
                        if (g->f3C != 0 || !((*(RecB *)(off + (s32)P->lb)).flags & 2)) {
                            ((s16 *)pf)[1] = 0;
                            rec2 = (RecB *)((u8 *)P->lb + off);
                            v = rec2 != *(void **)(pf + 2);
                            *(void **)(pf + 2) = rec2;
                            if (v || !(*pf & 2)) {
                                *pf |= 4;
                            }
                            if (pass > 0) {
                                *pf |= 8;
                            } else {
                                *pf &= ~8;
                            }
                            if (d == 0) {
                                D_800C97E4 = k;
                            }
                            return 1;
                        }
                    }
                }
                off += sizeof(RecB);
                k++;
            }
            if (k < objB_800EFD64->nB) goto loopB;
        }
        pass++;
        cur = D_800EE498_8[cur].next;
    } while (pass < 2);
done:
    return 0;
}

struct Obj_17 {
    u8 pad0[0x48];
    Vec3f pos;
    u8 pad54[0x628 - 0x54];
    s16 seg;
    u8 pad62A[0x63C - 0x62A];
    f32 frac;
};

typedef struct {
    u8 pad0[0x4E];
    s16 start;
    u8 pad50[0x6C - 0x50];
} Seg_7;

typedef struct {
    u16 flags;
    u8 pad2[6];
} Trk_8;

extern Trk_8 D_800E7FC8_5[];
extern Seg_7 *D_800EE498_9;

f32 func_8002BBD4(Obj_17 *p, s32 a, s32 b)
{
    s32 seg;
    s32 len;
    s32 d;
    f32 frac;

    if (D_800E7FC8_5[D_800C9684].flags & 8) {
        len = D_800E80C0[b] - D_800E80C0[a];
        seg = func_8004B5F0(&p->pos, D_800EE498_9[p->seg].start, 15, 1);
        func_8004A944(&p->pos, seg);
        frac = D_800EE4A0;
    } else {
        seg = p->seg;
        seg = (s16)seg;
        frac = p->frac;
        len = D_800E80C0[b] - D_800E80C0[a];
    }
    if (len < 0) {
        len += D_800EE490;
    }
    d = D_800E80C0[seg] - D_800E80C0[a];
    if (d < 0) {
        d += D_800EE490;
    }
    return ((f32)d + frac) / (f32)len;
}

struct Obj_18 {
    u8 pad0[0x2E];
    u16 h2e;
    u8 pad1[0x3C];
    f32 f6c;
    f32 f70;
    f32 f74;
};

typedef struct {
    s32 w0;
    u16 h4;
    u16 h6;
    s32 w8;
    s32 w12;
    s32 w16;
    s16 s20;
    s16 s22;
} Track_7;

typedef struct {
    s16 h[2];
    s32 w;
} Tmp30;

typedef struct {
    u16 flags;
    u16 pad;
    Track_7 *node;
} Slot_5;

extern Slot_5 D_800E7FC8_6[];

void func_8002BD40(Obj_18 *param_1, s32 *param_2)
{
    u16 uVar1;
    Track_7 *piVar2;
    f32 fT;
    f32 fA;
    f32 fB;
    u32 uA;
    u32 uB;
    u32 sum4;
    s32 vS;
    s32 iVar5;
    s32 local_78[4];
    s32 local_68[3];
    s32 aiStack_58[10];
    Tmp30 t30;
    s16 local_28[4];

    uVar1 = D_800E7FC8_6[D_800C9684].flags;
    if (uVar1 & 4) {
        D_800E7FC8_6[D_800C9684].flags = uVar1 & 0xFFFB;
    }
    piVar2 = D_800E7FC8_6[D_800C9684].node;
    fT = func_8002BBD4(param_1, piVar2->s20, piVar2->s22);
    vS = piVar2->w0 + (s32)((f32)(piVar2->w8 - piVar2->w0) * fT);
    fA = (f32)piVar2->w12 * fT;
    uA = (u32)fA;
    fB = (f32)piVar2->w16 * fT;
    sum4 = piVar2->h4 + uA;
    uB = (u32)fB;
    func_80011A58(aiStack_58, (-((piVar2->h6 + uB) & 0xFFFF)) + 1000, 0,
                  (param_1->h2e + sum4) & 0xFFFF);
    local_78[0] = 0;
    local_78[1] = 0;
    local_78[2] = vS;
    func_80012DC8(local_78, aiStack_58, local_68);
    param_2[0] = (s32)(param_1->f6c * 1000.0f + (f32)local_68[0]);
    param_2[1] = (s32)(param_1->f70 * 1000.0f + (f32)local_68[1] + 1000.0f);
    iVar5 = (s32)(param_1->f74 * 1000.0f + (f32)local_68[2]);
    param_2[2] = iVar5;
    func_8001254C((s32)(param_1->f6c * 1000.0f - (f32)param_2[0]),
                  (s32)(param_1->f74 * 1000.0f - (f32)iVar5), t30.h, &t30.w);
    func_8001254C(t30.w, (s32)((param_1->f70 + 0.7f) * 1000.0f - (f32)param_2[1]), local_28, &t30.w);
    param_2[4] = 0;
    {
        s32 ta = local_28[0] + -0x4000;
        s32 tb = -t30.h[0];
        param_2[3] = ta;
        param_2[5] = tb;
    }
    func_8002B5F8(param_2);
}

typedef struct {
    s32 x, y, z;
    s32 rx, ry, rz;
    s32 flags;
    s32 p7, p8;
} Obj_19;

typedef struct {
    u16 flags;
    u16 pad;
    Obj_19 *obj;
} Slot_6;

struct Actor {
    u8 pad[0x6C];
    f32 fx, fy, fz;
};

extern Slot_6 D_800E7FC8_7[];

void func_8002BFE0(Actor *act, Rec6 *out)
{
    s32 m[3];
    s32 mtx[10];
    s16 a1;
    s32 r1;
    s16 a2;
    s16 b1;
    s32 r2;
    s16 b2;
    Slot_6 *sl;
    s32 idx;
    Obj_19 *o;
    u16 sf;
    s32 f;
    s32 rx, rz;
    f32 t, u, v;

    sl = D_800E7FC8_7;
    sl += D_800C9684;
    sf = sl->flags;
    o = sl->obj;
    if (sf & 4) {
        sl->flags = sf & ~4;
        f = o->flags;
        if (f & 8) {
            if (f & 1) {
                if (f & 4) {
                    o->x = (f32)D_800E8090[D_800C9684].x - act->fx * 1000.0f;
                    o->y = (f32)D_800E8090[D_800C9684].y - act->fy * 1000.0f;
                    o->z = (f32)D_800E8090[D_800C9684].z - act->fz * 1000.0f;
                } else {
                    o->x = D_800E8090[D_800C9684].x;
                    o->y = D_800E8090[D_800C9684].y;
                    o->z = D_800E8090[D_800C9684].z;
                }
            } else {
                o->x = D_800E8090[D_800C9684].x;
                o->y = D_800E8090[D_800C9684].y;
                o->z = D_800E8090[D_800C9684].z;
            }
            f = o->flags;
        }
        if (f & 0x10) {
            func_80011A58(mtx, o->rx, 0, o->rz);
            m[0] = 0;
            m[1] = 0;
            m[2] = o->ry;
            func_80012DC8(m, mtx, &D_800C97E8);
        } else if (f & 4) {
            func_8001254C(-o->x, o->z, &a1, &r1);
            func_8001254C(r1, -o->y + 700, &a2, &r1);
            func_80011A58(mtx, a2 - 0x4000, 0, -a1);
            m[0] = 0;
            m[1] = 0;
            m[2] = o->ry;
            func_80012DC8(m, mtx, &D_800C97E8);
        }
    }
    t = func_8002BBD4(act, o->p7, o->p8);
    f = o->flags;
    if (f & 4) {
        out->x = (f32)o->x + act->fx * 1000.0f;
        out->y = (f32)o->y + act->fy * 1000.0f;
        out->z = (f32)o->z + act->fz * 1000.0f;
        if (o->flags & 0x20) {
            u = 1.0f - t;
            out->x = (f32)out->x + (f32)D_800C97E8.x * u;
            out->y = (f32)out->y + (f32)D_800C97E8.y * u;
            out->z = (f32)out->z + (f32)D_800C97E8.z * u;
        } else {
            out->x = (f32)out->x + (f32)D_800C97E8.x * t;
            out->y = (f32)out->y + (f32)D_800C97E8.y * t;
            out->z = (f32)out->z + (f32)D_800C97E8.z * t;
        }
    } else if (f & 1) {
        out->x = (f32)o->x + (f32)(o->rx - o->x) * t;
        out->y = (f32)o->y + (f32)(o->ry - o->y) * t;
        out->z = (f32)o->z + (f32)(o->rz - o->z) * t;
    } else if (f & 0x10) {
        if (f & 0x20) {
            out->x = (f32)(o->x + D_800C97E8.x) - (f32)D_800C97E8.x * t;
            out->y = (f32)(o->y + D_800C97E8.y) - (f32)D_800C97E8.y * t;
            out->z = (f32)(o->z + D_800C97E8.z) - (f32)D_800C97E8.z * t;
        } else {
            out->x = (f32)o->x + (f32)D_800C97E8.x * t;
            out->y = (f32)o->y + (f32)D_800C97E8.y * t;
            out->z = (f32)o->z + (f32)D_800C97E8.z * t;
        }
    } else {
        out->x = o->x;
        out->y = o->y;
        out->z = o->z;
    }
    if (o->flags & 0x10) {
        out->rx = o->rx;
        out->ry = 0;
        out->rz = o->rz;
    } else {
        func_8001254C(act->fx * 1000.0f - (f32)out->x, act->fz * 1000.0f - (f32)out->z, &b1, &r2);
        func_8001254C(r2, (act->fy + 0.7f) * 1000.0f - (f32)out->y, &b2, &r2);
        out->ry = 0;
        out->rx = b2 - 0x4000;
        out->rz = -b1;
    }
    func_8002B5F8(out);
}

typedef struct {
    u8 pad0[0x14];
    f32 fa;
    f32 fb;
    u8 pad1[0x50];
} Rec_13;

extern Rec_13 *D_800EE498_10;

void func_8002C5A4(s16 p1, f32 p2, u16 *p3)
{
    s32 i1, i2;
    s32 aux[2];
    f32 xa, xb;
    if (0.0f <= p2) {
        i2 = p1;
        i1 = i2 + 1;
        if (i1 >= D_800EE490) i1 -= D_800EE490;
    } else {
        s32 t = p1;
        i2 = t - 1;
        i1 = t;
        if (i2 < 0) i2 += D_800EE490;
        p2 = p2 + 1.0f;
    }
    xb = (D_800EE498_10[i2].fb * (1.0f - p2) + D_800EE498_10[i1].fb * p2) * 16384.0f;
    xa = (D_800EE498_10[i2].fa * (1.0f - p2) + D_800EE498_10[i1].fa * p2) * 16384.0f;
    func_8001254C((s32)xb, (s32)xa, p3, aux);
}

void func_8002C6B4(s16 param_1, f32 param_2, f32 *param_3, f32 *param_4, f32 *param_5)
{
    s32 i0, i1;
    s32 t;

    if (0.0f <= param_2) {
        i0 = param_1;
        i1 = i0 + 1;
        if (i1 >= D_800EE490) {
            i1 -= D_800EE490;
        }
    } else {
        t = param_1;
        i0 = t - 1;
        i1 = t;
        if (i0 < 0) {
            i0 = D_800EE490 - 1;
        }
        param_2 = param_2 + 1.0f;
    }
    *param_3 = D_800EE498_2[i0].x + (D_800EE498_2[i1].x - D_800EE498_2[i0].x) * param_2;
    *param_4 = D_800EE498_2[i0].y + (D_800EE498_2[i1].y - D_800EE498_2[i0].y) * param_2;
    *param_5 = D_800EE498_2[i0].z + (D_800EE498_2[i1].z - D_800EE498_2[i0].z) * param_2;
}

typedef struct {
    u8 pre[0x48];
    Vec3f pos;
    u8 post[0x7E0 - 0x48 - 12];
} Node_4;

void func_8002C7C0(s32 a, s32 b, s32 c)
{
    Node_4 *nd, *nd0;
    s32 *tb;
    Vec3f p;
    f32 pt[3];
    f32 n[3];
    f32 q[4];
    f32 tt[2];
    f32 sum, len, L, t;
    s32 *dst;

    nd0 = D_800E37C0;
    nd = nd0 + a;
    tb = D_800E8090;
    p = nd->pos;
    dst = &tb[b * 3];
    pt[0] = (f32)dst[0] / 1000.0f;
    pt[1] = (f32)dst[1] / 1000.0f;
    pt[2] = (f32)dst[2] / 1000.0f;
    pt[0] = pt[0] - p.x;
    pt[1] = pt[1] - p.y;
    pt[2] = pt[2] - p.z;
    sum = pt[0] * pt[0] + pt[1] * pt[1] + pt[2] * pt[2];
    L = __builtin_sqrtf(sum);
    if (0.0f < L) {
        n[0] = pt[0] / L;
        n[1] = pt[1] / L;
        n[2] = pt[2] / L;
    }
    L = L + 0.2f;
    pt[0] = p.x + n[0] * L;
    pt[1] = p.y + n[1] * L;
    pt[2] = p.z + n[2] * L;
    func_80030ECC((s32)((p.x - L) * 1000.0f), (s32)((p.z - L) * 1000.0f),
                  (s32)((p.x + L) * 1000.0f), (s32)((p.z + L) * 1000.0f));
    tt[0] = 1.0f;
    if (func_80030658(&p, pt, q, tt, c)) {
        t = tt[0] * L - 0.2f;
        pt[0] = p.x + t * n[0];
        pt[1] = p.y + t * n[1];
        pt[2] = p.z + t * n[2];
        dst[0] = (s32)(pt[0] * 1000.0f);
        dst[1] = (s32)(pt[1] * 1000.0f);
        tt[0] = t;
        dst[2] = (s32)(pt[2] * 1000.0f);
    }
    func_8002D1F0(a, b);
}

typedef struct {
    u8 pad[0x6C];
    f32 x;
    f32 y;
    f32 z;
    u8 pad2[0x7E0 - 0x78];
} Bike_19;

typedef struct {
    s16 v;
} Ang;

typedef struct {
    s32 x, y, z;
    s32 pad0C, pad10;
    s32 w14;
} Seg_8;

extern Bike_19 D_800E37C0_30[];
extern Seg_8 D_800E7998_4[][16];
extern Seg_8 D_800E8030_4[];

void func_8002CA70(s32 p1, s32 p2, s32 p3, s32 p4)
{
    Ang a;
    s32 b;
    Ang c;
    s32 dx, dz, d;
    s32 i;

    dx = D_800E37C0_30[p2].x * 1000.0f;
    dx -= D_800E8090[p1].x;
    dz = D_800E37C0_30[p2].z * 1000.0f;
    dz -= D_800E8090[p1].z;
    func_8001254C(dx, dz, &a.v, &b);
    func_8001254C(dx - p3, dz - p4, &c.v, &b);
    D_800E8090[p1].x += p3;
    D_800E8090[p1].z += p4;
    D_800E8000[p1] -= (d = c.v - a.v);
    if (b > 0x1000) {
        for (i = 0; i < 16; i++) {
            D_800E7998_4[p1][i].x += p3;
            D_800E7998_4[p1][i].z += p4;
            D_800E7998_4[p1][i].w14 -= d;
        }
        D_800E8030_4[p1].x += p3 * 16;
        D_800E8030_4[p1].z += p4 * 16;
    }
}

#define REC(i) ((Rec_14 *)((i) * sizeof(Rec_14) + (u32)D_800EE498))
#define REC2(i) ((Rec_14 *)((u32)D_800EE498 + (i) * sizeof(Rec_14)))

typedef struct Rec_14 {
    f32 x;
    f32 y;
    f32 z;
    u8 p0c[8];
    f32 f14;
    f32 f18;
    u8 p1c[0x2a];
    s16 s46;
    u8 p48[10];
    s16 s52;
    s16 s54;
    s16 s56;
    u8 p58[12];
    u8 b64;
    u8 p65[7];
} Rec_14;

typedef struct Blk_8 {
    u8 p0[0x6c];
    f32 f6c;
    f32 f70;
    f32 f74;
    u8 p78[0x628 - 0x78];
    s16 s628;
    u8 p62a[0x744 - 0x62a];
    s32 w744;
    u8 p748[0x7e0 - 0x748];
} Blk_8;

typedef struct Ent_25 {
    s32 f00, f04, f08, f0c, f10, f14;
} Ent_25;

typedef struct { u8 p0[0x2C]; s32 f2C; u8 p30[0xA0 - 0x30]; } Trk_9;

extern Trk_9 *D_800EFD64_21;
extern Blk_8 D_800E37C0_31[];
extern Ent_25 D_800E8030_5[];
extern Ent_25 D_800E7998_5[][16];
extern Trk_9 D_800CB210_9[];

static inline f32 lerp(f32 a, f32 b, f32 t)
{
    return a + (b - a) * t;
}

static inline f32 lerpi(s32 a, s32 b, f32 t)
{
    return (f32)a + (f32)(b - a) * t;
}

void func_8002CC30(s32 m, s32 n)
{
    Vec3f v;
    s32 idx;
    f32 t;
    s16 a0;
    s32 ratio;
    s16 a1;
    s32 lim;
    Blk_8 *pb;
    Vec3i *q;
    Rec_14 *pr, *pn;
    s32 nx;
    Rec_14 *tab;
    f32 ax, az, bx, bz, qx, qz;
    s32 k, h, top, d, r, dz;
    f32 f, g, sx, sz;

    if (D_800EE140 != 0) {
        return;
    }
    if (D_800EFD64_21->f2C == 2) {
        func_8002D1F0(m, n);
        return;
    }
    {
        v.x = (f32)D_800E8090[n].x / 1000.0f;
        v.y = (f32)D_800E8090[n].y / 1000.0f;
        v.z = (f32)D_800E8090[n].z / 1000.0f;
        func_8004AB24(&v, &idx, &t, D_800E37C0_31[m].s628);
        tab = D_800EE498;
        pr = (Rec_14 *)(idx * sizeof(Rec_14) + (u32)tab);
        nx = pr->s46;
        D_800E7FF0_2[n] = pr->b64;
        if (D_800EFD64_21->f2C == 0) {
            s32 id = D_800EFD64_21 - D_800CB210_9;
            if (id == 0x18 || id == 0x15 || id == 0x16) {
                func_8002C7C0(m, n, 1);
            }
            return;
        }
        D_800E7F98 = 0;
        if (pr->s56 < 0) {
            func_8002C7C0(m, n, 0);
            return;
        }
        if (pr->s56 > 0) {
            pn = (Rec_14 *)(nx * sizeof(Rec_14) + (u32)tab);
            if (pn->s56 > 0) {
                f32 ya, yb;
                D_800E7F98 = 1;
                ya = pr->y * 1000.0f + (f32)pr->s56 * 16.0f;
                yb = pn->y * 1000.0f + (f32)pn->s56 * 16.0f;
                lim = (s32)(lerpi(pr->s52, pn->s52, t) * 16.0f);
                top = (s32)(lerpi(pr->s54, pn->s54, t) * 16.0f);
                h = (s32)lerp(ya, yb, t);
                if (h < D_800E8090[n].y && D_800E37C0_31[m].w744 < 0x1e) {
                    f32 ex, ez;
                    ex = D_800E37C0_31[m].f6c * 1000.0f - (f32)D_800E8090[n].x;
                    ez = D_800E37C0_31[m].f74 * 1000.0f - (f32)D_800E8090[n].z;
                    ex = __builtin_sqrtf(ex * ex + ez * ez);
                    g = D_800E37C0_31[m].f70;
                    g *= 1000.0f;
                    r = (s32)ex;
                    g -= (f32)D_800E8090[n].y;
                    func_8001254C((s32)g, r, &a0, &ratio);
                    g += (f32)(D_800E8090[n].y - h);
                    func_8001254C((s32)g, r, &a1, &ratio);
                    D_800E7978[n] -= a1 - a0;
                    for (k = 0; k < 16; k++) {
                        D_800E7998_5[n][k].f04 += h - D_800E8090[n].y;
                        D_800E7998_5[n][k].f0c -= a1 - a0;
                    }
                    D_800E8030_5[n].f04 += (h - D_800E8090[n].y) * 16;
                    D_800E8090[n].y = h;
                }
                {
                Rec_14 *pr2, *pn2;
                Rec_14 *tab2;
                f32 ax, az;
                tab2 = D_800EE498;
                pr2 = (Rec_14 *)(idx * sizeof(Rec_14) + (u32)tab2);
                nx = pr2->s46;
                pn2 = (Rec_14 *)(nx * sizeof(Rec_14) + (u32)tab2);
                g = lerp(pr2->f14, pn2->f14, t);
                f = lerp(pr2->f18, pn2->f18, t);
                ax = pr2->x * 1000.0f;
                az = pr2->z * 1000.0f;
                dz = (s32)(lerp((f32)D_800E8090[n].x - ax, (f32)D_800E8090[n].x - pn2->x * 1000.0f, t) * g +
                           lerp((f32)D_800E8090[n].z - az, (f32)D_800E8090[n].z - pn2->z * 1000.0f, t) * f);
                }
                if (top < dz) {
                    d = top - dz;
                } else if (dz < -lim) {
                    d = -lim - dz;
                } else {
                    func_8002D1F0(m, n);
                    return;
                }
                func_8002CA70(n, m, (s32)((f32)d * g), (s32)((f32)d * f));
            }
        }
    }
    func_8002D1F0(m, n);
}

typedef struct {
    u8 pad0[0x6C];
    f32 fA;
    u8 pad1[4];
    f32 fB;
    u8 pad2[0x7E0 - 0x78];
} Rec37_2;

extern Rec37_2 D_800E37C0_32[];

void func_8002D1F0(s32 param_1, s32 param_2)
{
    u32 bp = (u32)D_800E8090;
    s32 off = param_2 * 12;
    s32 *t = (s32 *)(off + (s32)bp);
    s32 iVar1;
    f32 r;
    f32 fVar2, fVar4;
    f32 fVar3;
    f32 dz;
    s32 bw;
    s32 iStack2c;

    iVar1 = func_8002E920(t[0], t[1] + 0x1F4, t[2]);
    iVar1 = iVar1 + 200;
    if (D_800E8508 != 0 && t[1] < iVar1) {
        fVar3 = D_800E37C0_32[param_1].fA * 1000.0f - (f32)t[0];
        dz = D_800E37C0_32[param_1].fB * 1000.0f - (f32)t[2];
        r = sqrtf(fVar3 * fVar3 + dz * dz);
        fVar4 = (f32)D_800E7978[param_2] * 3.1415925f * 3.0517578e-05f;
        fVar3 = r;
        fVar2 = func_800AEA30(fVar4);
        fVar4 = func_800ADFC0(fVar4);
        fVar4 = (fVar2 / fVar4) * fVar3 - (f32)(t[1] - iVar1);
        func_8001254C((s32)fVar4, (s32)fVar3, (s16 *)&bw, &iStack2c);
        D_800E7978[param_2] = (s32)((s16 *)&bw)[0];
        t[1] = iVar1;
    }
}

typedef struct {
    s32 a;
    s32 b;
    s32 c;
    s32 pad[3];
} E18;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
    u8 pad[0x7E0 - 12];
} R7E0;

extern E18 D_800E8030_6[];
extern E18 D_800E7998_6[][16];
extern R7E0 D_800E37C0_33[];

void func_8002D384(s32 p, s32 q)
{
    s32 i;

    D_800E8030_6[p].c += q * 16;
    for (i = 0; i < 16; i++) {
        D_800E7998_6[p][i].c += q;
    }
    D_800C9684 = p;
    func_80028864();
    D_800E8508 = 0;
    func_8002E920(D_800E37C0_33[p].x, D_800E37C0_33[p].y, D_800E37C0_33[p].z);
    if (D_800E8508 != 0) {
        D_800E15E0[p] = D_800E8958;
    }
}

void func_8002D464(int param_1, int *param_2)
{
    func_8002A8C0(param_1, param_2, -7000);
}

void func_8002D480(s32 a, s32 *b)
{
    func_8002A8C0(a, b, -10000);
}

struct Obj_20 {
    u8 pad[0x70];
    f32 f70;
    f32 f74;
};

void func_8002D49C(Obj_20 *param_1, s32 *param_2)
{
    f32 k = 1000.0f;

    param_2[0] = (s32)-D_800C9794[0];
    param_2[1] = (s32)(param_1->f70 * k + D_800C9794[1]);
    param_2[2] = (s32)(param_1->f74 * k - D_800C9794[2]);
    param_2[3] = (s32)D_800C9794[4];
    param_2[5] = (s32)-D_800C9794[3];
    param_2[4] = 0;
    func_8002B5F8(param_2);
}

struct Obj_21 {
    u8 pad0[0x6C];
    f32 f6c;
    u8 pad1[4];
    f32 f74;
};

struct Out_2 {
    s32 w0, w4, w8, wC, w10, w14;
};

void func_8002D528(Obj_21 *o, Out_2 *out) {
    out->w0 = o->f6c * 1000.0f + -28000.0f;
    out->w4 = 0x5BCC;
    out->w8 = o->f74 * 1000.0f - 10000.0f;
    out->wC = 0x1800;
    out->w14 = -0x4000;
    out->w10 = 0;
    func_8002B348(out);
}

void func_8002D5A0(u8 *p, s32 *p2) {
    s32 v = -0x9C4;

    if (p[0x44] != 0) {
        v = -5000;
    }
    if (*(f32 *)(p + 0x424) < 0.0f) {
        v = (s32)((f32)v + *(f32 *)(p + 0x424) * 250.0f);
    }
    func_8002A8C0(p, p2, v);
}

void func_8002D604(s32 param_1, s32 *param_2)
{
    func_8002A8C0(param_1, param_2, -5000);
}

void func_8002D620(s32 a, s32 *b) {
    func_8002A8C0(a, b, -10000);
}

struct Obj_22 { u8 p0[0x22]; u16 f22; u8 p1[0x6C - 0x24]; f32 f6c; f32 f70; f32 f74; u8 p2[0x3FF - 0x78]; u8 f3ff; u16 f400; };
typedef struct { u8 p[0x2C]; s32 f2c; } GS;
typedef struct { s32 l60[4]; s32 l50; s32 l4c; s32 l48; s32 pad[1]; s32 arr[10]; } Stk;

extern GS *D_800EFD64_22;

void func_8002D63C(Obj_22 *o, s32 *out)
{
    Stk st;
    s16 it;
    s16 sv;
    s32 a;

    it = 2000;
    if (D_800EFD64_22->f2c == 1) {
        it = 6000;
    }
    a = o->f22 + 0x7000;
    sv = a;
    if (o->f3ff > 1) {
        sv = a - o->f400 * 0x40;
        o->f400 = o->f400 + 1;
    }
    st.l60[0] = 0;
    st.l60[1] = 0;
    st.l60[2] = -5000;
    func_80011A58(st.arr, it, 0, sv);
    func_80012DC8(st.l60, st.arr, &st.l50);
    out[0] = (s32)((f32)st.l50 + o->f6c * 1000.0f);
    out[1] = (s32)((f32)st.l4c + o->f70 * 1000.0f + 750.0f);
    out[2] = (s32)((f32)st.l48 + o->f74 * 1000.0f);
    out[3] = it;
    out[5] = sv;
    out[4] = 0;
    func_8002B5F8(out);
}

void func_8002D788(s32 *p1, s32 *p2)
{
    s32 t[4];
    s32 o[3];
    s32 a[10];
    f32 *pf;
    s32 v;

    func_80011A58(a, p1[6], p1[7], p1[8]);
    t[0] = 0;
    t[1] = 400;
    t[2] = 0;
    func_80012DC8(t, a, o);
    pf = (f32 *)p1;
    p2[0] = (s32)((f32)o[0] + pf[27] * 1000.0f);
    p2[1] = (s32)((f32)o[1] + pf[28] * 1000.0f);
    p2[2] = (s32)((f32)o[2] + pf[29] * 1000.0f);
    p2[3] = p1[6] / 2;
    p2[5] = p1[8];
    v = p1[7];
    if (v < 0) {
        v += 3;
    }
    p2[4] = v >> 2;
    func_8002B5F8(p2);
}

void func_8002D888(Player_10 *arg0, s32 *arg1)
{
}

void func_8002D890(void)
{
}

void func_8002D898(void)
{
}

void func_8002D8A0(void) {
    func_800BCB04(D_800E80C0);
}

void func_8002D8C4(s16 p1, f32 f, u16 *p3)
{
    u16 loc[2];
    s32 i0, i1;

    i0 = p1 - 1;
    if (i0 < 0) {
        i0 += D_800EE490;
    }
    i1 = p1 + 1;
    if (D_800EE490 <= i1) {
        i1 -= D_800EE490;
    }
    func_8002C5A4((s16)i0, f, &loc[0]);
    func_8002C5A4((s16)i1, f, &loc[1]);
    *p3 = loc[1] - loc[0];
}

void func_8002D964(s32 a0, f32 f, s16 *out)
{
    s16 i;
    s32 i1, i2;
    f32 r;
    u8 *base;
    i = a0;
    if (0.0f <= f) {
        i2 = i;
        i1 = i + 1;
        if (D_800EE490 <= i1) {
            i1 = i1 - D_800EE490;
        }
    } else {
        s32 t;
        t = i;
        i2 = t - 1;
        i1 = t;
        if (i2 < 0) {
            i2 = i2 + D_800EE490;
        }
        f = f + 1.0f;
    }
    base = D_800EE498;
    r = *(f32 *)(base + i2 * 0x6c + 0x24) * (1.0f - f) + *(f32 *)(base + i1 * 0x6c + 0x24) * f;
    *out = (s16)((r * 32768.0f) / 3.1415927f);
}

void func_8002DA48(s32 i, s32 v)
{
    D_800E7FA8[i] = v;
}

#define SX(v) (((s32)(v) << 4) >> 4)

typedef struct {
    void *hdr;
    s32 x0, z0, x1, z1;
    void *grid;
    s32 f18;
    s32 f1C;
} Sub_7;

struct Area_2 {
    u32 count;
    Sub_7 subs[1];
};

typedef struct {
    u32 count;
} PolyHdr;

typedef struct { u16 n; u16 ids[1]; } List_5;

extern List_5 **D_800E8D60_2;

void func_8002DA60(Area_2 *area, s32 b, s32 c, u32 *polys)
{
    u32 i;
    s32 n;
    s32 cell;
    s32 x, z;
    s32 j, k, total;
    u16 *out;
    Sub_7 *s;
    Area_2 *r;
    Poly *p;
    Poly **tbl;
    s16 *buf;

    D_800E8948_2 = area;
    D_800E8D6C = c;
    D_800E8958 = -1;
    if (area != NULL) {
        for (i = 0; i < D_800E8948_2->count; i++) {
            s = (Sub_7 *)((u8 *)D_800E8948_2 + (i * 32 + 4));
            if (s->x0 < D_800E8940[0] || i == 0) D_800E8940[0] = s->x0;
            if (s->z0 < D_800E8940[1] || i == 0) D_800E8940[1] = s->z0;
            if (s->x1 > D_800E8950[0] || i == 0) D_800E8950[0] = s->x1;
            if (s->z1 > D_800E8950[1] || i == 0) D_800E8950[1] = s->z1;
        }
        func_8000ED10((u8 *)D_800C29E0_2, 0, 0x200);
        i = 0;
        total = 0;
        out = D_800C29E0_2 + 0x100;
        for (k = 0; k < 16; k++) {
            for (j = 0; j < 16; j++) {
                for (n = 0; n < D_800E8948_2->count; n++) {
                    r = (Area_2 *)((u8 *)D_800E8948_2 + n * 32);
                    x = r->subs[0].x0 << 4;
                    x >>= 4;
                    x = ((x - D_800E8940[0]) << 4) / (D_800E8950[0] - D_800E8940[0]);
                    z = r->subs[0].z0 << 4;
                    z >>= 4;
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    z = ((z - D_800E8940[0]) << 4) / (D_800E8950[1] - D_800E8940[1]);
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    cell = z * 16 + x;
                    z = cell >> 4;
                    if ((cell & 0xF) > j) continue;
                    if (z > k) continue;
                    z = r->subs[0].x1 << 4;
                    z >>= 4;
                    z = ((z - D_800E8940[0]) << 4) / (D_800E8950[0] - D_800E8940[0]);
                    x = r->subs[0].z1 << 4;
                    x >>= 4;
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    x = ((x - D_800E8940[0]) << 4) / (D_800E8950[1] - D_800E8940[1]);
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    cell = x * 16 + z;
                    z = cell >> 4;
                    if ((cell & 0xF) < j) continue;
                    if (z < k) continue;
                    D_800C29E0_2[i]++;
                    total++;
                    *out++ = n;
                }
                i++;
            }
        }
        D_800E8510 = func_800BC908((total + 0x100) * 2, 0, (s32)D_800D6C20, 0xDA);
        D_800E8D60_2 = func_800BC908(0x400, 0, (s32)D_800D6C20, 0xDB);
        n = 0;
        out = D_800C29E0_2 + 0x100;
        for (i = 0; (s32)i < 0x100; i++) {
            D_800E8D60_2[i] = (List_5 *)(D_800E8510 + n);
            D_800E8D60_2[i]->n = D_800C29E0_2[i];
            func_800AEC34(D_800E8D60_2[i]->ids, out, D_800E8D60_2[i]->n * 2);
            n++;
            n += D_800E8D60_2[i]->n;
            out += D_800E8D60_2[i]->n;
        }
    } else {
        D_800E8510 = NULL;
        D_800E8D60_2 = NULL;
    }
    D_800E8E6C = NULL;
    D_800E8500 = NULL;
    if (polys != NULL) {
        D_800E8E6C = func_800BC908(*polys * 4, 0, (s32)D_800D6C20, 0xF6);
        i = 0;
        p = (Poly *)(polys + 1);
        z = *polys;
        if (z != 0) {
            do {
                D_800E8E6C[i] = p;
                p = (Poly *)((u8 *)p + (p->n * 4 + 8));
            } while (++i < *polys);
        }
        func_8000ED10((u8 *)D_800C29E0_2, 0, D_800E8948_2->count * 2);
        i = 0;
        out = D_800C29E0_2 + D_800E8948_2->count;
        for (; i < D_800E8948_2->count; i++) {
            for (cell = 0; cell < *polys; cell++) {
                p = D_800E8E6C[cell];
                for (n = 0; n < p->n; n++) {
                    if (func_8002E29C(i, p->pts[n].x << 6, 0, p->pts[n].z << 6)) break;
                }
                if (n != p->n) {
                    *out++ = cell;
                    D_800C29E0_2[i]++;
                }
            }
        }
        i = 0;
        D_800E8500 = func_800BC908(D_800E8948_2->count * 4, 0, (s32)D_800D6C20, 0x11C);
        func_8000ED10((u8 *)D_800E8500, 0, D_800E8948_2->count * 4);
        out = D_800C29E0_2 + D_800E8948_2->count;
        for (; i < D_800E8948_2->count; i++) {
            if (D_800C29E0_2[i] != 0) {
                buf = func_800BC908((D_800C29E0_2[i] + 1) * 2, 0, (s32)D_800D6C20, 0x123);
                D_800E8500[i] = buf;
                func_800AEC34(buf, out, D_800C29E0_2[i] * 2);
                out += D_800C29E0_2[i];
                D_800E8500[i][D_800C29E0_2[i]] = -1;
            }
        }
    }
}

void func_8002E190(void)
{
    u32 i;
    if (D_800C9800 != 0) {
        func_800BCB04(D_800C9800);
        D_800C9800 = 0;
    }
    if (D_800E8D60 != 0) {
        func_800BCB04(D_800E8D60);
        D_800E8D60 = 0;
    }
    if (D_800E8510 != 0) {
        func_800BCB04(D_800E8510);
        D_800E8510 = 0;
    }
    if (D_800E8E6C != 0) {
        func_800BCB04(D_800E8E6C);
        D_800E8E6C = 0;
    }
    if (D_800E8500 != 0) {
        i = 0;
        if (*D_800E8948_3 != 0) {
            do {
                if (D_800E8500[i] != 0) {
                    func_800BCB04(D_800E8500[i]);
                }
                i++;
            } while (i < (u32)*D_800E8948_3);
        }
        func_800BCB04(D_800E8500);
        D_800E8500 = 0;
    }
}

typedef struct {
    s32 f0;
    Vtx16 *verts;
} Hdr_3;

typedef struct {
    u16 bits[4];
    u16 v[3];
    u16 flags;
} GTri;

typedef struct {
    u16 flags;
    u16 count;
    GTri tris[1];
} Grid;

typedef struct {
    Hdr_3 *hdr;
    s32 x0, z0, x1, z1;
    Grid *grid;
    s32 f18;
} Sub_8;

typedef struct {
    s32 f0;
    Sub_8 sub;
} Area_3;

s32 func_8002E29C(s32 id, s32 x, s32 y, s32 z)
{
    Sub_8 *a;
    Grid *g2;
    s32 px, pz;
    s32 mask, word, count;
    GTri *tri;
    Vtx16 *verts;
    s32 x0, z0, x1, z1;
    s32 w, d;
    s32 i, start;
    s32 v[3][4];
    Grid *grid;
    s32 dxp, dzp;
    s32 nx, ny, nz, h;
    s32 k;
    s32 c;

    D_800E8E7C++;
    a = (Sub_8 *)((u8 *)D_800E8948 + (id * 32 + 4));
    x0 = a->x0;
    if (x < x0 * 16) return 0;
    z0 = a->z0;
    if (z < z0 * 16) return 0;
    x1 = a->x1;
    if (x1 * 16 < x) return 0;
    z1 = a->z1;
    if (z1 * 16 < z) return 0;
    grid = a->grid;
    if (grid == NULL) return 0;
    if (!(grid->flags & 1)) func_8000ECB8(1);
    px = x;
    g2 = grid;
    pz = z;
    if (g2->flags & 4) {
        px = x - (x1 + x0) * 8;
        pz = z - (z1 + z0) * 8;
    }
    w = (x1 - x0) * 16;
    if (w == 0) return 0;
    d = (z1 - z0) * 16;
    if (d == 0) return 0;
    i = ((z - z0 * 16) * 8 / d) * 8 + (x - x0 * 16) * 8 / w;
    verts = a->hdr->verts;
    word = i / 16;
    mask = 1 << (i % 16);
    start = D_800E8D64;
    count = g2->count;
    if (start < 0) start = 0;
    if (start >= count) start = 0;
    i = start;
    do {
        tri = &g2->tris[i];
        if (tri->bits[word] & mask) {
            v[0][0] = verts[tri->v[0]].x * 16;
            v[0][1] = verts[tri->v[1]].x * 16;
            v[0][2] = verts[tri->v[2]].x * 16;
            v[2][0] = verts[tri->v[0]].z * 16;
            dzp = pz - v[2][0];
            v[2][1] = verts[tri->v[1]].z * 16;
            dxp = px - v[0][0];
            v[2][2] = verts[tri->v[2]].z * 16;
            if ((c = dzp * (v[0][1] - v[0][0]) - dxp * (v[2][1] - v[2][0])) >= 0 &&
                (c = (pz - v[2][1]) * (v[0][2] - v[0][1]) - (px - v[0][1]) * (v[2][2] - v[2][1])) >= 0 &&
                (c = (pz - v[2][2]) * (v[0][0] - v[0][2]) - (px - v[0][2]) * (v[2][0] - v[2][2])) >= 0) {
                v[1][0] = verts[tri->v[0]].y * 16;
                v[1][1] = verts[tri->v[1]].y * 16;
                v[1][2] = verts[tri->v[2]].y * 16;
                v[0][1] -= v[0][0];
                v[0][2] -= v[0][0];
                v[1][1] -= v[1][0];
                v[1][2] -= v[1][0];
                v[2][1] -= v[2][0];
                v[2][2] -= v[2][0];
                v[0][1] >>= 4;
                v[1][1] >>= 4;
                v[2][1] >>= 4;
                v[0][2] >>= 4;
                v[2][2] >>= 4;
                v[1][2] >>= 4;
                nx = v[2][1] * v[1][2] - v[1][1] * v[2][2];
                ny = v[0][1] * v[2][2] - v[2][1] * v[0][2];
                nz = v[1][1] * v[0][2] - v[0][1] * v[1][2];
                if (ny != 0) {
                    c = -nz;
                    h = (dxp * -nx + dzp * c) / ny + v[1][0];
                    if (tri->flags & 0x400) {
                        if (h <= y + 500 && h >= D_800E80D0) {
                            D_800E80D0 = h;
                            D_800E8508 = 1;
                            D_800E8958 = id;
                            D_800E8D64 = i;
                            D_800E80D4 = h;
                            D_800E84F8 = tri->flags;
                        }
                    } else if (D_800E80D0 == -99999) {
                        D_800E8508 = 1;
                        D_800E8958 = id;
                        D_800E8D64 = i;
                        D_800E80D4 = h;
                        D_800E84F8 = tri->flags;
                        if (D_800E8500 != NULL) {
                            if (D_800E8500[id] == NULL) return 1;
                            k = 0;
                            while (D_800E8500[id][k] != -1) {
                                if (func_80031EC4(D_800E8E6C_2[D_800E8500[id][k]], x, z)) {
                                    D_800E84F8 = *D_800E8E6C_2[D_800E8500[id][k]];
                                    break;
                                }
                                k++;
                            }
                        }
                        return 1;
                    }
                }
            }
        }
        i++;
        if (i >= count) i = 0;
    } while (i != start);
    return 0;
}

s32 func_8002E920(s32 x, s32 y, s32 z)
{
    s32 ix, iz, i, k;

    D_800E80D0 = -99999;
    D_800E8508 = 0;
    D_800E84F8 = 0;
    D_800E8938++;
    if (D_800EFD64_5->mode == 2) {
        return func_8002EB5C(x, z);
    }
    if (D_800E8958 != -1 && func_8002E29C(D_800E8958, x, y, z) != 0) {
        return D_800E80D4;
    }
    ix = x >> 4;
    ix = (ix - D_800E8940_2.a) * 16 / (D_800E8950_2.a - D_800E8940_2.a);
    iz = z >> 4;
    if (ix < 0) ix = 0;
    if (ix > 15) ix = 15;
    iz = (iz - D_800E8940_2.a) * 16 / (D_800E8950_2.b - D_800E8940_2.b);
    if (iz < 0) iz = 0;
    if (iz > 15) iz = 15;
    k = iz * 16 + ix;
    for (i = 0; i < D_800E8D60[k]->n; i++) {
        if (D_800E8D60[k]->id[i] != D_800E8958 && func_8002E29C(D_800E8D60[k]->id[i], x, y, z) != 0) {
            return D_800E80D4;
        }
    }
    if (D_800E80D0 != -99999) {
        return D_800E80D0;
    }
    return D_800E80D4;
}

s32 func_8002EB5C(s32 x, s32 z)
{
    struct {
        s32 q;
        s32 rx;
        s32 rz;
    } L;
    s32 *qp;
    s32 ix, lt, v, fx, fz, h0, h1, h2;

    qp = &L.q;
    D_800E8508 = 1;
    func_8004262C(x, 2000, qp, &L.rx);
    ix = L.q;
    func_8004262C(z, 2000, qp, &L.rz);
    z = L.q;
    D_800E8958 = ((ix >> 2) & 0xffff) + ((z >> 2) << 16);
    v = ((ix & 3) + (z & 3) * 4) * 2;
    D_800E8D64 = v;
    if (L.rx < 1999 - L.rz) {
        D_800E8D64 = v + 1;
    }
    ix &= 0x1ff;
    z &= 0x1ff;
    fx = L.rx;
    fz = L.rz;
    if (fx < 1999 - fz) {
        h0 = D_800EA990[z * 0x200 + ix];
        h1 = D_800EA990[((z + 1) & 0x1ff) * 0x200 + ix];
        h2 = D_800EA990[z * 0x200 + ((ix + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * fz + (h2 - h0) * fx) / 2000;
    } else {
        h0 = D_800EA990[((z + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = D_800EA990[((z + 1) & 0x1ff) * 0x200 + ix];
        h2 = D_800EA990[z * 0x200 + ((ix + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * (2000 - L.rx) + (h2 - h0) * (2000 - L.rz)) / 2000;
    }
    return L.q;
}

float func_8002ED38(float x, float z)
{
    s32 ix, iz, h0, h1, h2;
    float rz, f0, f2;

    x *= 0.5f;
    z *= 0.5f;
    ix = x;
    D_800E8508 = 1;
    if (ix < 0) {
        ix--;
    }
    iz = z;
    x -= ix;
    if (iz < 0) {
        iz--;
    }
    D_800E8958 = ((ix >> 2) & 0xffff) + ((iz >> 2) << 16);
    z -= iz;
    rz = 1.0f - z;
    D_800E8D64 = ((ix & 3) + (iz & 3) * 4) * 2;
    if (x < rz) {
        D_800E8D64 = D_800E8D64 + 1;
    }
    ix &= 0x1ff;
    iz &= 0x1ff;
    if (x < rz) {
        float f1;
        h0 = D_800EA990[iz * 0x200 + ix];
        h1 = D_800EA990[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        f1 -= f0;
        f2 = D_800EA990[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + f1 * z + (f2 - f0) * x) / 1000.0f;
    } else {
        float f1, d;
        h0 = D_800EA990[((iz + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = D_800EA990[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        d = f1 - f0;
        f1 = 1.0f - x;
        f2 = D_800EA990[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + d * f1 + (f2 - f0) * rz) / 1000.0f;
    }
}

typedef struct { u8 pad[8]; u16 v[3]; u16 pad2; } Tri3;
typedef struct { u16 flags; u16 pad; Tri3 t[1]; } Shape;
typedef struct { u32 pad; Vtx_t *vtx; } Mesh_3;
typedef struct { Mesh_3 *mesh; s32 x0, z0, x1, z1; Shape *shape; u32 pad[2]; } Ent_26;
typedef struct { u32 n; Ent_26 e[1]; } EntList;

extern EntList *D_800E8948_4;

void func_8002EF1C(s32 x, s32 z, s32 *out)
{
    s32 m[3][4];
    Ent_26 *e;
    Shape *s;
    Tri3 *t;
    Vtx_t *vb;
    s32 ee, k, q, tt, a, a2, b, d, num, sx, sz, px, pz;

    if (D_800E8958 < 0 || D_800E8958 >= D_800E8948_4->n) return;
    e = &D_800E8948_4->e[D_800E8958];
    s = e->shape;
    if (!(s->flags & 1)) return;
    t = &s->t[D_800E8D64];
    sx = (e->x1 + e->x0) * 8;
    sz = (e->z1 + e->z0) * 8;
    if (s->flags & 4) {
        px = x - sx;
        pz = z - sz;
    } else {
        px = x;
        pz = z;
    }
    vb = e->mesh->vtx;
    m[0][0] = vb[t->v[0]].ob[0];
    m[0][1] = vb[t->v[1]].ob[0];
    m[0][2] = vb[t->v[2]].ob[0];
    m[2][0] = vb[t->v[0]].ob[2];
    m[2][1] = vb[t->v[1]].ob[2];
    m[2][2] = vb[t->v[2]].ob[2];
    m[0][1] -= m[0][0];
    m[2][1] -= m[2][0];
    m[0][2] -= m[0][0];
    m[2][2] -= m[2][0];
    m[0][0] <<= 4;
    m[2][0] <<= 4;
    for (k = 0; k < 3; k++) {
        m[1][0] = vb[t->v[0]].cn[k];
        m[1][1] = vb[t->v[1]].cn[k];
        m[1][2] = vb[t->v[2]].cn[k];
        m[1][1] -= m[1][0];
        m[1][2] -= m[1][0];
        tt = m[2][1] * m[1][2];
        a = m[1][1] * m[2][2] - tt;
        b = px - m[0][0];
        q = b * a;
        tt = m[1][1] * m[0][2];
        a2 = m[0][1] * m[1][2] - tt;
        d = m[0][1] * m[2][2];
        q += (pz - m[2][0]) * a2;
        ee = m[2][1] * m[0][2];
        q = q / (d - ee);
        q /= 16; q += m[1][0];
        if (q < 0) q = 0;
        if (q > 255) q = 255;
        out[k] = q;
    }
}

#define IDX(a, b) (((((a) & 0x1FF)) << 9) | ((b) & 0x1FF))

typedef struct { u8 pad[8]; u16 vi[3]; u16 pad2; } MTri;
typedef struct { u16 flags; u16 pad; MTri tri[1]; } Mesh_4;
typedef struct { u8 pad[4]; Vtx16 *verts; } Model_3;
typedef struct { Model_3 *model; s32 a, b, c, d; Mesh_4 *mesh; } Obj_23;

s32 func_8002F1D0(s32 p1, s32 p2, Vec3i *v)
{
    s32 n, q, u12, u13, lo4;
    Obj_23 *o;
    Mesh_4 *m;
    Vtx16 *vt;
    s32 k;
    u32 f; s32 j;
    u16 *pb, *pv;

    if (*(s32 *)(D_800EFD64 + 0x2c) == 2) {
        n = p2 / 2;
        lo4 = (p1 << 16) >> 14;
        q = n / 4;
        u12 = lo4 + (n - q * 4);
        u13 = (p1 >> 16) * 4 + q;
        if (p2 & 1) {
            v[0].x = u12 * 2000;
            v[0].z = u13 * 2000;
            v[0].y = D_800EA990[IDX(u13, u12)] << 6;
            v[1].x = (u12 + 1) * 2000;
            v[1].z = u13 * 2000;
            v[1].y = D_800EA990[IDX(u13, u12 + 1)] << 6;
            v[2].x = u12 * 2000;
            v[2].z = (u13 + 1) * 2000;
            v[2].y = D_800EA990[IDX(u13 + 1, u12)] << 6;
        } else {
            v[0].x = (u12 + 1) * 2000;
            v[0].z = (u13 + 1) * 2000;
            v[0].y = D_800EA990[IDX(u13 + 1, u12 + 1)] << 6;
            v[1].x = u12 * 2000;
            v[1].z = (u13 + 1) * 2000;
            v[1].y = D_800EA990[IDX(u13 + 1, u12)] << 6;
            v[2].x = (u12 + 1) * 2000;
            v[2].z = u13 * 2000;
            v[2].y = D_800EA990[IDX(u13, u12 + 1)] << 6;
        }
        return 0;
    }

    o = (Obj_23 *)(D_800E8948 + (p1 * 0x20 + 4));
    m = o->mesh;
    f = m->flags;
    if (!(f & 1)) {
        goto hang;
    }
    pb = (u16 *)m;
    vt = o->model->verts;
    if (f & 4) {
        u12 = (o->c + o->a) * 8;
        n = (o->d + o->b) * 8;
    } else {
        n = 0;
        u12 = 0;
    }
    pv = pb + (p2 * 8 + 2);
    for (p1 = 0; p1 < 3; p1++) {
        v[p1].x = vt[pv[p1 + 4]].x * 16 + u12;
        v[p1].y = vt[pv[p1 + 4]].y * 16;
        v[p1].z = vt[pv[p1 + 4]].z * 16 + n;
    }
    return 0;
hang:
    for (;;) {
    }
}

void func_8002F4B0(f32 *param_1)
{
    s32 v[9];
    f32 len;
    func_8002F1D0(D_800E8958, D_800E8D64, v);
    v[3] -= v[0];
    v[6] -= v[0];
    v[4] -= v[1];
    v[5] -= v[2];
    v[7] -= v[1];
    v[8] -= v[2];
    param_1[0] = (f32)v[5] * (f32)v[7] - (f32)v[4] * (f32)v[8];
    param_1[1] = (f32)v[3] * (f32)v[8] - (f32)v[5] * (f32)v[6];
    param_1[2] = (f32)v[4] * (f32)v[6] - (f32)v[3] * (f32)v[7];
    len = param_1[0] * param_1[0] + param_1[1] * param_1[1] + param_1[2] * param_1[2];
    len = sqrtf(len);
    param_1[0] = param_1[0] / len;
    param_1[1] = param_1[1] / len;
    param_1[2] = param_1[2] / len;
}

typedef struct { s32 pad; Vtx16 *v; } Mesh_5;
typedef struct { u32 m0, m1; u16 v[3]; u16 flags; } Ent_27;
typedef struct { u16 flags; u16 n; Ent_27 e[1]; } TList;
typedef struct { Mesh_5 *m; s32 x0, z0, x1, z1; TList *tl; u8 pad[8]; } Obj_24;
typedef struct { s32 pad; Obj_24 o; } Rec_15;
typedef struct { s32 id; s32 n; s32 v[3][3]; } CTri;

extern CTri D_800E80D8[];

static inline void tri(s32 id, s32 n, s32 ax, s32 az, s32 bx, s32 bz, s32 cx, s32 cz) {
    D_800E80D8[D_800E8D68].id = id;
    D_800E80D8[D_800E8D68].n = n;
    D_800E80D8[D_800E8D68].v[0][0] = ax * 2000;
    D_800E80D8[D_800E8D68].v[0][1] = D_800EA990[((az & 0x1FF) << 9) | (ax & 0x1FF)] << 6;
    D_800E80D8[D_800E8D68].v[0][2] = az * 2000;
    D_800E80D8[D_800E8D68].v[1][0] = bx * 2000;
    D_800E80D8[D_800E8D68].v[1][1] = D_800EA990[((bz & 0x1FF) << 9) | (bx & 0x1FF)] << 6;
    D_800E80D8[D_800E8D68].v[1][2] = bz * 2000;
    D_800E80D8[D_800E8D68].v[2][0] = cx * 2000;
    D_800E80D8[D_800E8D68].v[2][1] = D_800EA990[((cz & 0x1FF) << 9) | (cx & 0x1FF)] << 6;
    D_800E80D8[D_800E8D68].v[2][2] = cz * 2000;
    D_800E8D68++;
}

void func_8002F62C(s32 x0, s32 z0, s32 x1, s32 z1, s32 hy) {

    s32 r[4];
    union { u16 h[4]; u32 w[2]; } mask;
    s32 code[3];
    s32 idx;
    s32 hmin;
    s32 x, z, gx, gz, cx, cz, cell, k, j, n;
    s32 ox, oz, dx, dz;
    s32 bx0, bz0, bx1, bz1, bit;
    u16 f;
    Obj_24 *o;
    TList *tl;
    Ent_27 *en;
    Vtx16 *vt;
    u16 *vi;
    u16 *mp;
    CTri *t;
    s32 *p;

    x0 >>= 4;
    x1 >>= 4;
    z0 >>= 4;
    z1 >>= 4;
    D_800E8D68 = 0;
    hy >>= 4;
    if (D_800EFD64_4->f2C == 2) {
        func_8004262C(x0, 125, &r[0], &idx);
        func_8004262C(z0, 125, &r[2], &idx);
        func_8004262C(x1 + 124, 125, &r[1], &idx);
        func_8004262C(z1 + 124, 125, &r[3], &idx);
        for (k = r[2]; k < r[3]; k++) {
            for (n = r[0]; n < r[1]; n++) {
                tri(idx, n * 2 + k * 8, n, k, n + 1, k, n + 1, k + 1);
                if (D_800E8D68 == 24) return;
                tri(idx, n * 2 + k * 8, n, k, n + 1, k + 1, n, k + 1);
                if (D_800E8D68 == 24) return;
            }
        }
        return;
    }
    gx = (x0 + x1) >> 1;
    gz = (z0 + z1) >> 1;
    gx = ((gx - D_800E8940[0]) << 4) / (D_800E8950[0] - D_800E8940[0]);
    if (gx < 0) gx = 0;
    if (gx >= 16) gx = 15;
    gz = ((gz - D_800E8940[0]) << 4) / (D_800E8950[1] - D_800E8940[1]);
    if (gz < 0) gz = 0;
    if (gz >= 16) gz = 15;
    cell = gz * 16 + gx;
    for (idx = 0; idx < D_800E8D60[cell]->n; idx++) {
        o = (Obj_24 *)(D_800E8948 + ((D_800E8D60[cell]->id[idx] << 5) + 4));
        if (x1 < o->x0) continue;
        if (z1 < o->z0) continue;
        if (o->x1 < x0) continue;
        if (o->z1 < z0) continue;
        if (!(o->tl->flags & 1)) func_8000ECB8(1);
        tl = o->tl;
        if (tl->flags & 4) {
            cx = (o->x1 + o->x0) / 2;
            cz = (o->z1 + o->z0) / 2;
        } else {
            cx = cz = 0;
        }
        r[0] = x0 - cx;
        r[1] = x1 - cx;
        r[2] = z0 - cz;
        r[3] = z1 - cz;
        dx = o->x1 - o->x0;
        ox = o->x0;
        bx0 = (x0 - ox) * 8 / dx;
        oz = o->z0;
        dz = o->z1 - oz;
        if (bx0 >= 8) bx0 = 7;
        if (bx0 < 0) bx0 = 0;
        bz0 = (z0 - oz) * 8 / dz;
        if (bz0 >= 8) bz0 = 7;
        if (bz0 < 0) bz0 = 0;
        bx1 = (x1 - ox) * 8 / dx;
        if (bx1 >= 8) bx1 = 7;
        if (bx1 < 0) bx1 = 0;
        bz1 = (z1 - oz) * 8 / dz;
        if (bz1 >= 8) bz1 = 7;
        if (bz1 < 0) bz1 = 0;
        mp = mask.h;
        for (n = bz0; n <= bz1; n++) {
            for (k = bx0; k <= bx1; k++) {
                bit = n * 8 + k;
                mp[bit / 16] |= 1 << (bit % 16);
            }
        }
        for (n = 0; n < tl->n; n++) {
            en = &tl->e[n];
            if (!((en->m0 & mask.w[0]) || (en->m1 & mask.w[1]))) continue;
            if (en->flags & 0x100) continue;
            vi = en->v;
            vt = o->m->v;
            for (k = 0; k < 3; k++) {
                code[k] = 0;
                if (vt[vi[k]].x < r[0]) code[k] = 1;
                if (r[1] < vt[vi[k]].x) code[k] |= 2;
                if (vt[vi[k]].z < r[2]) code[k] |= 4;
                if (r[3] < vt[vi[k]].z) code[k] |= 8;
            }
            if (code[0] & code[1] & code[2]) continue;
            if (en->flags & 0x400) {
                for (k = 0; k < 3; k++) {
                    if (!(hy < vt[vi[k]].y)) break;
                }
                if (k == 3) continue;
                for (k = 0; k < 3; k++) {
                    if (hy - 250 < vt[vi[k]].y) break;
                }
                if (k == 3) continue;
            }
            { CTri *b = D_800E80D8; t = &b[D_800E8D68]; }
            t->n = n;
            t->id = idx;
            for (k = 0; k < 3; k++) {
                D_800E80D8[D_800E8D68].v[k][0] = (cx + vt[vi[k]].x) * 16;
                D_800E80D8[D_800E8D68].v[k][1] = vt[vi[k]].y << 4;
                D_800E80D8[D_800E8D68].v[k][2] = (cz + vt[vi[k]].z) * 16;
            }
            if (++D_800E8D68 == 24) return;
        }
    }
}

typedef struct {
    u8 pad[0x1C];
    u8 *base;
} Hdr_4;

extern Hdr_4 *D_800E8D6C_2;

void func_8002FF44(f32 *pos)
{
    s32 i;
    Ent_28 *e;
    f32 f;

    func_80030ECC((pos[0] - 1.0f) * 1000.0f, (pos[2] - 1.0f) * 1000.0f,
                  (pos[0] + 1.0f) * 1000.0f, (pos[2] + 1.0f) * 1000.0f);
    for (i = 0; i < D_800E84FC; i++) {
        e = D_800E8538[i];
        if (!(D_800C9800[((u8 *)e - D_800E8D6C_2->base) >> 3] & 0x20)) {
            f = __builtin_sqrtf(e->x * e->x + e->z * e->z);
            pos[0] += e->x / f;
            pos[2] += e->z / f;
            return;
        }
    }
}

typedef struct {
    s16 x, y, z;
} Face;

extern Face D_800C981C[];

s32 func_800300B0(f32 *p1, f32 *p2, f32 *p3, f32 *p4)
{
    Face *bestFace;
    s32 best;
    s32 ax, ay, az, bx, by, bz;
    s32 n, i;
    s32 ex, ey, ez;

    ax = (s32)(p1[0] * 1000.0f);
    ay = (s32)(p1[1] * 1000.0f);
    az = (s32)(p1[2] * 1000.0f);
    bx = (s32)(p2[0] * 1000.0f);
    by = (s32)(p2[1] * 1000.0f);
    bz = (s32)(p2[2] * 1000.0f);
    best = 0;
    bestFace = 0;
    for (n = 0; n < D_800E850C; n++) {
        Node_5 *nd = &D_800E8D78[n];
        for (i = 0; i < 5; i++) {
            Face *f = &D_800C981C[i];
            s32 hw = nd->w;
            s32 hh = nd->h;
            s32 nx = nd->x;
            s32 ny = nd->y;
            s32 nz = nd->z;
            s32 px, py, pz;
            s32 d1, d2, dd;
            f32 t;
            s32 dx;
            px = ((hw * f->x) >> 14) + nx;
            py = ((hh * f->y) >> 14) + ny;
            pz = ((hw * f->z) >> 14) + nz;

            d1 = (bx - px) * f->x + (by - py) * f->y + (bz - pz) * f->z;
            if (d1 >= best) continue;
            pz = (ax - px) * f->x + (ay - py) * f->y + (az - pz) * f->z;
            if (pz < 0) continue;
            py = d1 - pz;
            if (py == 0) continue;
            dx = bx - ax;
            t = -(f32)pz / (f32)py;
            px = (f32)ax + t * (f32)dx;
            dx = by - ay;
            py = (f32)ay + t * (f32)dx;
            dx = bz - az;
            pz = (f32)az + t * (f32)dx;
            if (i < 2) {
                if (ny + hh < py || pz < nz - hw || nz + hw < pz) continue;
            } else if (i < 4) {
                if (ny + hh < py || px < nx - hw || nx + hw < px) continue;
            } else {
                if (px < nx - hw || nx + hw < px || pz < nz - hw || nz + hw < pz) continue;
            }
            bestFace = &D_800C981C[i];
            best = d1;
        }
    }
    if (bestFace) {
        *p4 = -((f32)best / 16384000.0f);
        p3[0] = bestFace->x * 6.1035156e-05f;
        p3[1] = bestFace->y * 6.1035156e-05f;
        p3[2] = bestFace->z * 6.1035156e-05f;
        return 1;
    }
    return 0;
}

typedef struct { s32 x; s32 y; s32 z; s32 pad[4]; } Ent_29;

void func_80030478(f32 *p)
{
    s32 nx, ny, i, dx, dy, ax, d2, lim;
    f32 r;

    nx = (s32)(p[0] * 1000.0f);
    ny = (s32)(p[2] * 1000.0f);
    func_80030ECC(nx - 0x1000, ny - 0x1000, nx + 0x1000, ny + 0x1000);
    if (D_800E850C > 0) {
    i = 0;
    lim = 0x900000;
    do {
        dx = D_800E8D78[i].x - nx;
        if (abs(dx) < 0xc01) {
            dy = D_800E8D78[i].z - ny;
            if (abs(dy) < 0xc01) {
                d2 = dx * dx + dy * dy;
                if (d2 <= lim) {
                    r = sqrtf((f32)d2);
                    nx = (s32)((f32)D_800E8D78[i].x - (f32)(dx * 0xc00) / r);
                    ny = (s32)((f32)D_800E8D78[i].z - (f32)(dy * 0xc00) / r);
                }
            }
        }
    } while (++i < D_800E850C);
    }
    p[0] = (f32)nx / 1000.0f;
    p[2] = (f32)ny / 1000.0f;
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

typedef struct { s16 a, b, nx, nz; } Edge;
typedef struct { s16 a, b, c, nx, ny, nz; } Tri12;
typedef struct { u8 pad[0x14]; Vtx8 *verts; u8 pad2[4]; Edge *edges; } Track_8;

extern Track_8 *D_800E8D6C_3;
extern Tri12 *D_800E8960[];
extern Edge *D_800E8530;

u8 func_80030658(f32 *start, f32 *end, f32 *out, f32 *tmax, s32 wantFlag)
{
    u8 result = 0;
    f32 sx, sy, sz, ex, ey, ez;
    s32 i;
    f32 t;
    f32 vx, vy, vz;
    f32 d1, d0, px;
    Vtx8 *a, *b;
    s32 iax, iaz, ibx, ibz;
    f32 unused[3];

    if (D_800EFD64_5->mode == 2) {
        return func_800300B0(start, end, out, tmax);
    }
    if (D_800E8D6C_3 == NULL) {
        return 0;
    }
    sx = start[0] * 1000.0f;
    sy = start[1] * 1000.0f;
    sz = start[2] * 1000.0f;
    ex = end[0] * 1000.0f;
    ey = end[1] * 1000.0f;
    ez = end[2] * 1000.0f;
    D_800E8D70 = 0;
    if (D_800E84FC != 0) {
        for (i = 0; i < D_800E84FC; i++) {
            Edge *e = D_800E8538[i];
            u8 fl = D_800C9800[(e - D_800E8D6C_3->edges)];
            f32 ax, az, fax, faz, dz, dx, py, y0, y1, len;

            if (fl & 0x20) continue;
            if (wantFlag && !(fl & 0x40)) continue;
            a = &D_800E8D6C_3->verts[e->a];
            iax = a->x;
            fax = iax;
            iaz = a->z;
            faz = iaz;
            ax = fax * 64.0f;
            az = faz * 64.0f;
            b = &D_800E8D6C_3->verts[e->b];
            ibz = b->z;
            dz = ibz - iaz;
            ibx = b->x;
            dx = iax - ibx;
            vx = ex - ax;
            vz = ez - az;
            d1 = vx * dz + vz * dx;
            if (d1 >= 0.0f) continue;
            vx = sx - ax;
            vz = sz - az;
            d0 = vx * dz + vz * dx;
            if (d0 < 0.0f) continue;
            t = d0 / (d0 - d1);
            if (*tmax < t) continue;
            px = start[0] + (end[0] - start[0]) * t;
            py = start[1] + (end[1] - start[1]) * t;
            vz = start[2] + (end[2] - start[2]) * t;
            px *= 15.625f;
            py *= 1000.0f;
            vz *= 15.625f;
            if (__builtin_abs(e->nz) > __builtin_abs(e->nx)) {
                if (px < fax && px < ibx) continue;
                if (fax < px && ibx < px) continue;
                vz = (px - fax) / (ibx - fax);
            } else {
                if (vz < faz && vz < ibz) continue;
                if (faz < vz && ibz < vz) continue;
                vz = (vz - faz) / (ibz - faz);
            }
            y0 = a->y + vz * (b->y - a->y);
            y0 *= 16.0f;
            y1 = a->h + vz * (b->h - a->h);
            y1 *= 16.0f;
            if (py < y0) continue;
            if (y0 + y1 <= py) continue;
            *tmax = t;
            len = __builtin_sqrtf(dz * dz + dx * dx);
            out[0] = dz / len;
            out[1] = 0.0f;
            out[2] = dx / len;
            result = 1;
            D_800E8530 = e;
            D_800E8D70 = D_800C9800[(e - D_800E8D6C_3->edges)];
            D_800E8E78 = (D_800E8D70 >> 1) & 3;
        }
    }
    if (D_800E8504 != 0) {
        for (i = 0; i < D_800E8504; i++) {
            Tri12 *tr = D_800E8960[i];
            Vtx8 *v = D_800E8D6C_3->verts;
            f32 fax, faz, ax, ay, az, nx, ny, nz, c, len;
            Vtx8 *cc;
            s32 pos, neg, jz;
            a = &v[tr->a];

            ibx = a->x;
            fax = ibx;
            ax = fax * 64.0f;
            ay = (a->y + a->h) * 16.0f;
            jz = a->z;
            faz = jz;
            az = faz * 64.0f;
            nx = tr->nx;
            ny = tr->ny;
            nz = tr->nz;
            b = &v[tr->b];
            vx = ex - ax;
            vy = ey - ay;
            vz = ez - az;
            d1 = vx * nx + vy * ny + vz * nz;
            cc = &v[tr->c];
            if (d1 >= 0.0f) continue;
            vx = sx - ax;
            vy = sy - ay;
            vz = sz - az;
            d0 = vx * nx + vy * ny + vz * nz;
            if (d0 < 0.0f) continue;
            t = d0 / (d0 - d1);
            if (*tmax < t) continue;
            pos = neg = 0;
            px = (sx + (ex - sx) * t) * 0.015625f;
            vz = (sz + (ez - sz) * t) * 0.015625f;
            c = (b->x - ibx) * (vz - faz) - (b->z - jz) * (px - fax);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            c = (cc->x - b->x) * (vz - b->z) - (cc->z - b->z) * (px - b->x);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            c = (a->x - cc->x) * (vz - cc->z) - (a->z - cc->z) * (px - cc->x);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            if (pos == 3 || neg == 3) {
                len = __builtin_sqrtf(tr->nx * tr->nx + tr->ny * tr->ny + tr->nz * tr->nz);
                out[0] = tr->nx / len;
                out[1] = tr->ny / len;
                out[2] = tr->nz / len;
                result = 1;
                *tmax = t;
            }
        }
    }
    return result;
}

#define MIN_2(a, b) ((a) < (b) ? (a) : (b))
#define MAX_2(a, b) ((a) < (b) ? (b) : (a))

typedef struct {
    s32 x, y, z, r, a, b, c;
} Ent_30;

typedef struct {
    s32 cx;             /* 0x00 */
    s32 cz;             /* 0x04 */
    s32 f8, fC;
    s32 off;            /* 0x10 */
    u8 pad14[0x320 - 0x14];
    s32 count;          /* 0x320 */
    Ent_30 e[8];           /* 0x324 */
} Tile;

typedef struct {
    s16 v0, v1;
    s16 f4;
    s16 flags;          /* 0x06 */
} CEdge;

typedef struct {
    s16 v[3];
    s16 f6, f8;
    s16 flags;          /* 0x0A */
} CTri_2;

typedef struct {
    u16 *tris;
    u16 *edges;
} CCell;

typedef struct {
    s32 minx, minz, maxx, maxz;
    s32 f10;
    Vtx8 *verts;        /* 0x14 */
    s32 f18;
    CEdge *edges;       /* 0x1C */
    s32 f20;
    CTri_2 *tris;         /* 0x24 */
    CCell cells[257];   /* 0x28 */
} Grid_2;

extern Tile *D_800EE138_2;
extern Ent_30 D_800E8D78_2[];
extern Grid_2 *D_800E8D6C_4;
extern CEdge *D_800E8538_2[];
extern CTri_2 *D_800E8960_2[];

void func_80030ECC(s32 x0, s32 z0, s32 x1, s32 z1)
{
    s32 px[3];
    s32 py[3];
    s32 pz[3];
    s32 qz[3];
    s32 i, j, k, m;
    s32 ix0, iz0, ix1, iz1;
    s32 n, mask, v, lo, hi;
    CCell *cell;
    CEdge *e;
    CTri_2 *t;
    Vtx8 *a, *b;

    D_800E850C = 0;
    if (D_800EFD64_5->mode == 2) {
        D_800E850C = 0;
        for (i = 0; i < 16; i++) {
            if (D_800EE138_2[i].off != 0) continue;
            if (x1 < D_800EE138_2[i].cx * 256000) continue;
            if (z1 < D_800EE138_2[i].cz * 256000) continue;
            if ((D_800EE138_2[i].cx + 1) * 256000 < x0) continue;
            if ((D_800EE138_2[i].cz + 1) * 256000 < z0) continue;
            for (j = 0; j < D_800EE138_2[i].count; j++) {
                if (x1 >= D_800EE138_2[i].e[j].x - D_800EE138_2[i].e[j].r
                    && z1 >= D_800EE138_2[i].e[j].z - D_800EE138_2[i].e[j].r
                    && D_800EE138_2[i].e[j].x + D_800EE138_2[i].e[j].r >= x0
                    && D_800EE138_2[i].e[j].z + D_800EE138_2[i].e[j].r >= z0) {
                    D_800E8D78_2[D_800E850C] = D_800EE138_2[i].e[j];
                    D_800E850C++;
                    if (D_800E850C == 8) return;
                }
            }
            if (D_800E850C == 8) return;
        }
    } else if (D_800E8D6C_4 != NULL) {
        D_800E84FC = 0;
        D_800E8504 = 0;
        ix0 = ((x0 / 16 - D_800E8D6C_4->minx) * 16) / (D_800E8D6C_4->maxx - D_800E8D6C_4->minx);
        iz0 = ((z0 / 16 - D_800E8D6C_4->minz) * 16) / (D_800E8D6C_4->maxz - D_800E8D6C_4->minz);
        ix1 = ((x1 / 16 - D_800E8D6C_4->minx) * 16) / (D_800E8D6C_4->maxx - D_800E8D6C_4->minx);
        iz1 = ((z1 / 16 - D_800E8D6C_4->minz) * 16) / (D_800E8D6C_4->maxz - D_800E8D6C_4->minz);
        if (ix0 > 16 || iz0 > 16 || ix1 < 0 || iz1 < 0) return;
        if (ix0 < 0) ix0 = 0;
        if (ix0 > 15) ix0 = 15;
        if (iz0 < 0) iz0 = 0;
        if (iz0 > 15) iz0 = 15;
        if (ix1 > 15) ix1 = 15;
        if (iz1 > 15) iz1 = 15;
        for (i = ix0; i <= ix1; i++) {
            for (j = iz0; j <= iz1; j++) {
                cell = &D_800E8D6C_4->cells[j * 16 + i];
                n = (u32)D_800E8D6C_4->cells[j * 16 + i + 1].edges - (u32)cell->edges;
                n = (u32)n >> 1;
                for (k = 0; k < n; k++) {
                    if (D_800E84FC >= 0x100) break;
                    e = &D_800E8D6C_4->edges[cell->edges[k]];
                    if (e->flags & 1) continue;
                    a = &D_800E8D6C_4->verts[e->v0];
                    b = &D_800E8D6C_4->verts[e->v1];
                    if (a->x < b->x) {
                        if ((a->x << 6) >= x1 || x0 >= (b->x << 6)) continue;
                    } else {
                        if ((b->x << 6) >= x1 || x0 >= (a->x << 6)) continue;
                    }
                    if (a->z < b->z) {
                        if ((a->z << 6) >= z1 || z0 >= (b->z << 6)) continue;
                    } else {
                        if ((b->z << 6) >= z1 || z0 >= (a->z << 6)) continue;
                    }
                    {
                        {
                            D_800E8538_2[D_800E84FC] = e;
                            D_800E84FC++;
                            e->flags |= 1;
                        }
                    }
                }
                cell = &D_800E8D6C_4->cells[j * 16 + i];
                n = (u32)D_800E8D6C_4->cells[j * 16 + i + 1].tris - (u32)cell->tris;
                n = (u32)n >> 1;
                for (k = 0; k < n + ((n & 1) >> 1); k++) {
                    if (D_800E8504 >= 0x100) break;
                    t = &D_800E8D6C_4->tris[cell->tris[k]];
                    if (t->flags & 1) continue;
                    mask = 0xF;
                    for (m = 0; m < 3; m++) {
                        a = &D_800E8D6C_4->verts[t->v[m]];
                        v = a->x << 6;
                        if (v >= x0) mask &= ~1;
                        if (x1 >= v) mask &= ~2;
                        v = a->z << 6;
                        if (v >= z0) mask &= ~4;
                        if (z1 >= v) mask &= ~8;
                    }
                    if (mask == 0) {
                        D_800E8960_2[D_800E8504++] = t;
                        t->flags |= 1;
                    }
                }
            }
        }
        for (i = 0; i < D_800E84FC; i++) {
            D_800E8538_2[i]->flags &= ~1;
        }
        for (i = 0; i < D_800E8504; i++) {
            D_800E8960_2[i]->flags &= ~1;
        }
        if (D_800C2350 & 0x200) {
            for (i = 0; i < D_800E84FC; i++) {
                px[0] = D_800E8D6C_4->verts[D_800E8538_2[i]->v0].x << 6;
                px[1] = D_800E8D6C_4->verts[D_800E8538_2[i]->v0].x << 6;
                px[2] = D_800E8D6C_4->verts[D_800E8538_2[i]->v1].x << 6;
                pz[0] = D_800E8D6C_4->verts[D_800E8538_2[i]->v0].z << 6;
                pz[1] = D_800E8D6C_4->verts[D_800E8538_2[i]->v0].z << 6;
                pz[2] = D_800E8D6C_4->verts[D_800E8538_2[i]->v1].z << 6;
                func_8002E920(px[0], 10000, pz[0]);
                func_8002E920(px[2], 10000, pz[2]);
                py[0] = D_800E8D6C_4->verts[D_800E8538_2[i]->v0].y * 16;
                py[1] = (D_800E8D6C_4->verts[D_800E8538_2[i]->v0].y + D_800E8D6C_4->verts[D_800E8538_2[i]->v0].h) * 16;
                py[2] = D_800E8D6C_4->verts[D_800E8538_2[i]->v1].y * 16;
                px[0] = D_800E8D6C_4->verts[D_800E8538_2[i]->v0].x << 6;
                px[1] = D_800E8D6C_4->verts[D_800E8538_2[i]->v1].x << 6;
                px[2] = D_800E8D6C_4->verts[D_800E8538_2[i]->v1].x << 6;
                pz[0] = D_800E8D6C_4->verts[D_800E8538_2[i]->v0].z << 6;
                pz[1] = D_800E8D6C_4->verts[D_800E8538_2[i]->v1].z << 6;
                pz[2] = D_800E8D6C_4->verts[D_800E8538_2[i]->v1].z << 6;
                py[0] = (D_800E8D6C_4->verts[D_800E8538_2[i]->v0].y + D_800E8D6C_4->verts[D_800E8538_2[i]->v0].h) * 16;
                py[1] = (D_800E8D6C_4->verts[D_800E8538_2[i]->v1].y + D_800E8D6C_4->verts[D_800E8538_2[i]->v1].h) * 16;
            }
            for (i = 0; i < D_800E8504; i++) {
                px[0] = D_800E8D6C_4->verts[D_800E8960_2[i]->v[0]].x << 6;
                px[1] = D_800E8D6C_4->verts[D_800E8960_2[i]->v[1]].x << 6;
                px[2] = D_800E8D6C_4->verts[D_800E8960_2[i]->v[2]].x << 6;
                py[0] = (D_800E8D6C_4->verts[D_800E8960_2[i]->v[0]].y + D_800E8D6C_4->verts[D_800E8960_2[i]->v[0]].h) * 16;
                py[1] = (D_800E8D6C_4->verts[D_800E8960_2[i]->v[1]].y + D_800E8D6C_4->verts[D_800E8960_2[i]->v[1]].h) * 16;
                py[2] = (D_800E8D6C_4->verts[D_800E8960_2[i]->v[2]].y + D_800E8D6C_4->verts[D_800E8960_2[i]->v[2]].h) * 16;
                qz[0] = D_800E8D6C_4->verts[D_800E8960_2[i]->v[0]].z << 6;
                qz[1] = D_800E8D6C_4->verts[D_800E8960_2[i]->v[1]].z << 6;
                qz[2] = D_800E8D6C_4->verts[D_800E8960_2[i]->v[2]].z << 6;
            }
        }
    }
}

typedef struct {
    s16 x;
    s16 y;
    s32 pad;
} Pt_2;

typedef struct {
    s16 a;
    s16 b;
    s16 nx;
    s16 ny;
} Edge_2;

typedef struct {
    u8 pad[10];
    s16 flags;
} Rec_16;

struct Hdr_5 {
    s32 pad[5];
    Pt_2 *pts;
    s32 nedges;
    Edge_2 *edges;
    s32 nrecs;
    Rec_16 *recs;
    Pair tab[0x101];
};

void func_80031A78(Hdr_5 **param_1, s32 param_2)
{
    Hdr_5 *h = *param_1;
    s32 i;
    s32 dx;
    s32 dy;
    s32 len;

    if (param_2 != 0) {
        h->pts = (Pt_2 *)((s32)h->pts + (s32)h);
        h->edges = (Edge_2 *)((s32)h->edges + (s32)h);
        h->recs = (Rec_16 *)((s32)h->recs + (s32)h);
        for (i = 0; i < 0x101; i++) {
            h->tab[i].a += (s32)h;
            h->tab[i].b += (s32)h;
        }
    }
    D_800C9800 = func_800BC908(h->nedges, 0, D_800D6C20, 0x87F);
    func_8000ED10(D_800C9800, 0, h->nedges);
    for (i = 0; i < h->nedges; i++) {
        D_800C9800[i] |= 1;
        if (h->edges[i].nx == -0x8000) {
            D_800C9800[i] = ((u8 *)&h->edges[i])[7] ^ 1;
        }
        dx = h->pts[h->edges[i].b].x - h->pts[h->edges[i].a].x;
        dy = h->pts[h->edges[i].b].y - h->pts[h->edges[i].a].y;
        len = func_8000EB18_4(dx * dx + dy * dy);
        if (len != 0) {
            dx = (dx * 0x4000) / len;
            dy = (dy * 0x4000) / len;
        }
        h->edges[i].nx = dy;
        h->edges[i].ny = -dx;
    }
    for (i = 0; i < h->nedges; i++) {
        h->edges[i].ny &= ~1;
    }
    for (i = 0; i < h->nrecs; i++) {
        h->recs[i].flags &= ~1;
    }
}

void func_80031D08(int *param_1)
{
    func_80031A78(param_1, 1);
}

void func_80031D24(void)
{
    func_800BCB04(D_800C9800);
    D_800C9800 = 0;
}

s32 func_80031D54(s32 *p1, s32 *p2, s32 x, s32 y)
{
    s32 b0 = p2[0];
    s32 a1 = p1[1];
    s32 a0 = p1[0];
    s32 b1 = p2[1];
    s32 a2;
    s32 b2;

    if ((y - b0) * (a1 - a0) - (x - a0) * (b1 - b0) <= 0) {
        return 0;
    }
    a2 = p1[2];
    b2 = p2[2];
    if ((y - b1) * (a2 - a1) - (x - a1) * (b2 - b1) <= 0) {
        return 0;
    }
    return (y - b2) * (a0 - a2) - (x - a2) * (b0 - b2) > 0;
}

int func_80031DF8(int x, int y)
{
    int a, b;

    b = y >> 4;
    a = x >> 4;
    a = (a - D_800E8940[0]) * 16 / (D_800E8950[0] - D_800E8940[0]);
    if (a < 0) a = 0;
    if (a > 15) a = 15;
    b = (b - D_800E8940[0]) * 16 / (D_800E8950[1] - D_800E8940[1]);
    if (b < 0) b = 0;
    if (b > 15) b = 15;
    return b * 16 + a;
}

struct Poly_2 {
    u8 hdr[6];
    u16 n;
    Pt pts[1];
};

s32 func_80031EC4(Poly_2 *poly, s32 px, s32 pz)
{
    s32 pos = 0;
    s32 neg;
    s32 i = 0, j, n, cr;

    n = poly->n;
    neg = 0;
    px = px >> 6;
    pz = pz >> 6;
    if (n != 0) {
        do {
            j = (i + 1) % n;
            cr = (pz - poly->pts[i].z) * (poly->pts[j].x - poly->pts[i].x);
            cr -= (px - poly->pts[i].x) * (poly->pts[j].z - poly->pts[i].z);
            if (cr >= 0) {
                pos++;
            }
            if (cr <= 0) {
                neg++;
            }
            n = poly->n;
            i++;
        } while (i < n);
    }
    if (pos == poly->n) {
        goto ret1;
    }
    if (neg != poly->n) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

s32 func_80031FA0(s32 p1, s32 p2, s32 p3)
{
    s32 t, v;

    if (p3 >= D_800E8518.b && p3 >= D_800E8518.d) {
        return 0;
    }
    if (p3 < D_800E8518.b && p3 < D_800E8518.d) {
        return 0;
    }
    t = ((p3 - D_800E8518.b) << 8) / (D_800E8518.d - D_800E8518.b);
    v = D_800E8518.a + ((D_800E8518.c - D_800E8518.a) * t) / 256;
    if (v < p1 || p2 < v) {
        return 0;
    }
    return 1;
}

s32 func_8003207C(s32 p1, s32 p2, s32 p3)
{
    s32 t, v;

    if (p3 >= D_800E8518.a && p3 >= D_800E8518.c) {
        return 0;
    }
    if (p3 < D_800E8518.a && p3 < D_800E8518.c) {
        return 0;
    }
    t = ((p3 - D_800E8518.a) << 8) / (D_800E8518.c - D_800E8518.a);
    v = D_800E8518.b + ((D_800E8518.d - D_800E8518.b) * t) / 256;
    if (v < p1 || p2 < v) {
        return 0;
    }
    return 1;
}

typedef struct {
    s32 flags;
    u8 pad[0x20];
} Ent_31;

typedef struct {
    s32 f0;
    s32 n;
    Ent_31 e[1];
} Hdr_6;

typedef struct {
    u8 pad[4];
    s16 v;
    u8 pad2[0xA];
} Cell_3;

typedef struct {
    u8 w, h;
    u8 pad[2];
    u8 *buf;
} Item_4;

typedef struct {
    u32 n;
    u32 pad;
    Item_4 it[1];
} Tab;

typedef struct {
    u8 pad[0x2C];
    s32 mode;
    u8 pad2[0x20];
    s32 flags;
} Cfg_6;

extern Hdr_6 *D_800C9860_2;
extern Cell_3 *D_800E8E88_2[2];
extern Tab *D_800E8E80;
extern Cfg_6 *D_800EFD64_23;

void func_80032150(void)
{
    s32 v;
    s32 i, j, k, n, a, b, c;
    s32 x, y;

    D_800C9840 = 0;
    D_800E8E88_2[0] = 0;
    D_800E8E88_2[1] = 0;
    D_800C9860_2 = 0;
    if (D_800EFD64_23->mode == 2) {
        v = D_800D0F78;
    } else {
        v = func_8005B58C(D_800D14EC);
    }
    if (v < 0) {
        D_800C9860_2 = 0;
        return;
    }
    a = 0;
    D_800C9860_2 = func_800BF79C(D_800E7728, v);
    for (; a < D_800C9860_2->n; a++) {
        D_800C9860_2->e[a].flags &= ~0x18;
    }
    for (j = 0; j < 2; j++) {
        D_800E8E88_2[j] = func_800BC908(0x800, 0, D_800D6C80, 0xCB);
        for (n = 0; n < 0x80; n++) {
            D_800E8E88_2[j][n].v = 1000;
        }
    }
    D_800E8E80 = func_800BF79C(D_800E7728, D_800C9860_2->f0);
    for (a = 0; a < D_800E8E80->n; a++) {
        n = D_800E8E80->it[a].w * D_800E8E80->it[a].h * 8;
        for (k = 0; k < n; k++) {
            D_800E8E80->it[a].buf[k] |= 0xF0;
        }
    }
    if (D_800EFD64_23->flags & 0x20) {
        D_800E8E98 = func_800BF79C(D_800E7728, D_800D0E00);
        D_800C9840 = func_800BC908(0x300, 0, D_800D6C80, 0xE3);
        for (i = 0; i < 0x40; i++) {
            x = (func_8000EFE8() - 0x80) * 16;
            y = (func_8000EFE8() - 0x80) * 16;
            D_800C9840[i].d = func_8000EB18_4(0x400000 - (x * x + y * y));
            D_800C9840[i].x = x;
            D_800C9840[i].y = y;
        }
    }
    D_800E8E9C = 0;
}

typedef struct {
    u32 flags;
    s32 a1, a2, a3;
    s32 p10;
    s32 v14, v18, v1C;
    s32 p20;
} Rec_17;

typedef struct {
    s32 h0;
    s32 count;
    Rec_17 r[1];
} Tab_2;

typedef struct {
    s32 A, B, C, D, E;
} Locs;

extern Tab_2 *D_800C9860_3;

void func_8003240C(s32 param_1, s32 param_2, s32 arg2)
{
    s32 A, B, C, D, E;
    s32 i;
    Rec_17 *rp;
    u32 fl;

    if (D_800C9860_3 != NULL) {
        for (i = 0; i < D_800C9860_3->count; i++) {
            rp = &D_800C9860_3->r[i];
            fl = rp->flags;
            rp->flags = fl & ~8;
            if (fl & 1) {
                if (func_800353EC(&A, &B, param_1, param_2) == 0) {
                    rp->v1C = 0x7FFF;
                    rp->flags |= 8;
                    rp->v14 = A;
                    rp->v18 = B;
                }
            } else if (fl & 2) {
                if (func_80033AC8(0, 0x14, -0x12C, &A, &B, &C, &D, param_1, 0) == 0) {
                    rp->v1C = 0x7FFF;
                    rp->flags |= 8;
                    rp->v14 = A;
                    rp->v18 = B;
                }
            } else {
                if (func_80033AC8(rp->a1, rp->a2, rp->a3, &A, &B, &C, &E, param_1, param_2) == 0) {
                    rp->v14 = A;
                    rp->v18 = B;
                    rp->v1C = C;
                    rp->flags |= 8;
                }
            }
            if (rp->v14 < -0x32 || rp->v18 < -0x32 ||
                D_800F13EC + 0x32 < rp->v14 || D_800F13F0 + 0x32 < rp->v18) {
                rp->flags &= ~8;
            }
        }
    }
}

typedef struct { u32 flags; s32 pad[4]; s32 x, y, z; s32 res; } Obj_25;
typedef struct { s32 pad; s32 n; Obj_25 o[1]; } Lst_2;

extern Lst_2 *D_800C9860_4;

void func_800325FC(s32 a0)
{
    s32 i, r, c;
    s32 x, y, thr, v, cnt, tot;
    s32 x0, y0, w, h, xe, ye;
    Obj_25 *o;
    u16 *tb;

    if (D_800C9860_4 == NULL) {
        return;
    }
    func_800AD7E0(0, 0x400000);
    w = D_800E1718_2.w;
    h = D_800E1718_2.h;
    x0 = D_800E1718_2.x;
    y0 = D_800E1718_2.y;
    i = 0;
    if (i < D_800C9860_4->n) {
        do {
            o = &D_800C9860_4->o[i];
            o->res = -1;
            if (!(o->flags & 8)) {
                o->flags &= ~0x10;
            } else {
                tot = 0;
                cnt = 0;
                x = o->x - 4;
                y = o->y - 4;
                thr = o->z - 0x19;
                for (r = 0; r < 8; r++) {
                    if (y < y0) {
                        y = y0;
                    }
                    if (y >= y0 + h) {
                        y = y0 + h - 1;
                    }
                    for (c = 0; c < 8; c++) {
                        if (x < x0) {
                            x = x0;
                        }
                        if (x >= x0 + w) {
                            x = x0 + w - 1;
                        }
                        tb = &D_800C29E0_2[y * D_800F13EC + x];
                        v = func_80033DA0(*tb);
                        if (v > 0x7FFF) {
                            v = 0x7FFF;
                        }
                        if (v >= thr) {
                            cnt++;
                        }
                        tot++;
                        x++;
                    }
                    x -= 8;
                    y++;
                }
                if (tot != 0) {
                    cnt = cnt * 7 / tot;
                }
                cnt--;
                if (cnt > 5) {
                    cnt = 5;
                }
                o->res = cnt;
                o->flags |= 0x10;
            }
            i++;
        } while (i < D_800C9860_4->n);
    }
}

#define SINS(a) D_800C2C60[((a) >> 4) & 0xFFF]
#define COSS(a) D_800C2C60[(((a) >> 4) - 0x400) & 0xFFF]

typedef struct {
    s32 flags;  /* 0x00 */
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 x;      /* 0x14 */
    s32 y;      /* 0x18 */
    s32 f1C;
    s32 tex;    /* 0x20 */
} Ent_32;

typedef struct {
    s32 f0;
    s32 n;
    Ent_32 e[1];
} EntList_2;

typedef struct {
    s16 x, y, z;
    u16 flag;
    s16 s, t;
    u8 c[4];
} V;

extern EntList_2 *D_800C9860_5;
extern V *D_800E8E88_3[];

static inline V *allocVtx(s32 idx, s32 n) {
    s32 c = D_800E8E90[idx];
    if (c + n > 128) {
        return NULL;
    }
    D_800E8E90[idx] = c + n;
    return &D_800E8E88_3[idx][c];
}

Gfx *func_80032854(Gfx *gdl, u8 *mtx, s32 idx) {
    s32 i, j, k;
    Ent_32 *e;
    V *v;
    TexInfo *t;
    s32 a, b, p, q, x, y, ang, c, s, m, n, w;
    s32 bb;
    s32 aa;
    V *vv;

    if (D_800C9860_5 == NULL) {
        return gdl;
    }
    D_800E8E90[idx] = 0;
    if (D_800C2280 & 0x40) {
        return gdl;
    }

    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPViewport(gdl++, &D_800C25E8);
    gSPMatrix(gdl++, mtx + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718_2.x, D_800E1718_2.y, D_800E1718_2.x + D_800E1718_2.w - 1,
                  D_800E1718_2.y + D_800E1718_2.h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);

    D_800E8E9C = 0;
    for (i = 0; i < D_800C9860_5->n; i++) {
        e = &D_800C9860_5->e[i];
        if (e->flags & 8) { if (e->flags & 0x10) { if (e->tex >= 0) {
            p = D_800F13EC / 2 - e->x;
            q = D_800F13F0 / 2 - e->y;
            a = 0x80 - func_8000EB18_2(p * p + q * q);
            if (a >= 0) {
                a = a * e->tex / 6;
                if (a != 0) {
                    a = a * 192 / 128;
                    if (i == 0) {
                        D_800E8E9C = a;
                    }
                    v = allocVtx(idx, 28);
                    if (v != NULL) {
                        x = e->x - D_800F13EC / 2;
                        y = e->y - D_800F13F0 / 2;
                        p = x / 3;
                        q = y / 3;
                        x -= p;
                        y -= q;
                        for (j = 0; j < 4; j++) {
                            v[j * 7].x = x;
                            v[j * 7].y = -y;
                            v[j * 7].z = 50;
                            for (k = 0; k < 6; k++) {
                                b = -y;
                                ang = ((k * 2 + 1) << 15) / 6;
                                v[j * 7 + k + 1].x = x + COSS(ang) * D_800C9850[j] / 16384;
                                v[j * 7 + k + 1].y = b + SINS(ang) * D_800C9850[j] / 16384;
                                v[j * 7 + k + 1].z = 50;
                            }
                            x -= p;
                            y -= q;
                            if (j & 1) {
                                x -= p;
                                y -= q;
                            }
                        }
                        gSPVertex(gdl++, v, 28, 0);
                        for (j = 0; j < 4; j++) {
                            gDPPipeSync(gdl++);
                            gDPSetPrimColor(gdl++, 0, 0, D_800C9844[j * 3], D_800C9844[j * 3 + 1],
                                            D_800C9844[j * 3 + 2], a);

                            for (k = 0; k < 3; k++) {
                                s32 n;
                                w = k * 2 + 2;
                                m = k * 2 + 1;
                                n = w % 6 + 1;
                                gSP2Triangles(gdl++, j * 7, j * 7 + w, j * 7 + m, 0, j * 7, j * 7 + n, j * 7 + w, 0);
                            }
                        }
                    }
                }
            }
        }
    }}}

    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0x11FE23, 0xFFFFF3F9);
    gDPSetColorDither(gdl++, G_CD_NOISE);
    gDPSetAlphaDither(gdl++, G_AD_NOISE);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    if (D_800EFD64_4->f2C == 0) {
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
    } else {
        gDPSetTexturePersp(gdl++, G_TP_NONE);
    }

    for (i = 0; i < D_800C9860_5->n; i++) {
        e = &D_800C9860_5->e[i];
        if (e->flags & 8) { if (e->flags & 0x10) { if ((n = e->tex) >= 0) {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            t = &D_800E8E80_2[n + 1];
            if (D_800EFD64_4->f2C == 0) {
                x = e->x - D_800F13EC / 2;
                y = D_800F13F0 / 2 - e->y;
                q = t->h - 1;
                c = (COSS((x + y) * 32) * q >> 14) * 2 / 3;
                s = (SINS((x + y) * 32) * q >> 14) * 2 / 3;
                vv = allocVtx(idx, 4);
                if (vv != NULL) {
                    vv[0].x = x + c;
                    vv[0].y = y + s;
                    vv[0].s = 0;
                    vv[0].t = 0;
                    vv[1].x = x + s;
                    vv[1].y = y - c;
                    vv[1].s = q << 6;
                    vv[1].t = 0;
                    vv[2].x = x - c;
                    vv[2].y = y - s;
                    vv[2].s = q << 6;
                    vv[2].t = q << 6;
                    vv[3].x = x - s;
                    vv[3].y = y + c;
                    vv[3].s = 0;
                    vv[3].t = q << 6;
                    gSPVertex(gdl++, vv, 4, 0);
                    gDPLoadTextureBlock(gdl++, t->img, G_IM_FMT_IA, G_IM_SIZ_8b, t->w * 8, t->h, 0, G_TX_CLAMP,
                                        G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                    gSP2Triangles(gdl++, 0, 1, 2, 0, 0, 2, 3, 0);
                }
            } else {
                bb = aa = 0;
                x = e->x - t->w * 4;
                y = e->y - t->h / 2;
                p = x + t->w * 8;
                q = y + t->h;
                if (x < 0) {
                    bb = -x;
                    x = 0;
                }
                if (y < 0) {
                    aa = -y;
                    y = 0;
                }
                if (p >= D_800F13EC) {
                    p = D_800F13EC - 1;
                }
                if (q >= D_800F13F0) {
                    q = D_800F13F0 - 1;
                }
                if (x < p && y < q) {
                    gDPLoadTextureBlock(gdl++, t->img, G_IM_FMT_IA, G_IM_SIZ_8b, t->w * 8, t->h, 0, G_TX_CLAMP,
                                        G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                    gSPTextureRectangle(gdl++, x << 2, y << 2, p << 2, q << 2, G_TX_RENDERTILE, bb << 5, aa << 5,
                                        0x400, 0x400);
                }
            }
        }
    }}}

    gSPSetGeometryMode(gdl++, D_800E19CC);
    gDPPipeSync(gdl++);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gDPSetAlphaDither(gdl++, G_AD_DISABLE);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gSPMatrix(gdl++, mtx, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

Gfx *func_8003371C(Gfx *gdl, s32 arg)
{
    s32 c, i;
    u8 *img;
    s32 sx, sy, sz, sw;

    if (D_800C9860 == 0) return gdl;
    if (*(s32 *)(D_800EFD64 + 0x50) & 0x20) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        { Gfx *_g = gdl++; _g->words.w0 = 0xFC11FE23; _g->words.w1 = 0xFFFFFFFF; }
        img = *(u8 **)(D_800E8E98 + 0xC) + 0x28;
        gDPLoadTextureBlock(gdl++, img, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0, c = 0x80; i < 0x40; i++) {
            if (func_80033AC8(D_800C9840_2[i].x + (D_800E8090_2.x >> 4), D_800C9840_2[i].y + (D_800E8090_2.y >> 4),
                              D_800C9840_2[i].z + (D_800E8090_2.z >> 4), &sx, &sy, &sz, &sw, arg, 0) == 0
                && sx >= 0 && sy >= 0 && sx <= D_800F13EC && sy <= D_800F13F0) {
                gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
                gSPTextureRectangle(gdl++, (sx - 1) << 2, (sy - 1) << 2, (sx + 1) << 2, (sy + 1) << 2,
                                    0, 0, 0, 0x400, 0x400);
            }
            c += 2;
        }
    }
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    return gdl;
}

s32 func_80033AC8(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9)
{
    f32 m[4][4];
    s16 *ps;
    f32 kx, ky, kz, kw;
    f32 fx, fy, fz, a, b, c, w;
    s32 ret;

    ps = (s16 *)(p9 * 16 + D_800E19C0_2);
    kx = (f32)ps[0] * 0.25f;
    ky = (f32)ps[1] * 0.25f;
    kw = (f32)ps[4] * 0.25f;
    kz = (f32)ps[5] * 0.25f;
    func_800B31C0(&m[0][0], (u32 *)(p8 + p9 * 0x40));

    fx = (f32)x;
    fy = (f32)y;
    fz = (f32)z;
    a = m[0][0] * fx + m[1][0] * fy + m[2][0] * fz + m[3][0];
    b = m[0][1] * fx + m[1][1] * fy + m[2][1] * fz + m[3][1];
    c = m[0][2] * fx + m[1][2] * fy + m[2][2] * fz + m[3][2];
    w = m[0][3] * fx + m[1][3] * fy + m[2][3] * fz + m[3][3];
    ret = 1;
    if (w != 0.0f) {
        a = a / w;
        b = b / w;
        c = c / w;
        if (c <= -1.0f) {
            return ret;
        }
        if (1.0f <= c) {
            return ret;
        }
        *p4 = (s32)(a * kx + kw);
        *p5 = (s32)(b * -ky + kz);
        *p6 = (s32)(c * 1023.0f * 0.5f + 511.5f) << 5;
        if (p7 != 0) {
            *p7 = (s32)w;
        }
        ret = 0;
    }
    return ret;
}

void func_80033D40(void) {
    if (D_800C9840 != 0) {
        func_800BCB04(D_800C9840);
        D_800C9840 = 0;
    }
    if (D_800E8E88[0] != 0) {
        func_800BCB04(D_800E8E88[0]);
        func_800BCB04(D_800E8E88[1]);
    }
    D_800C9860 = 0;
}

s32 func_80033DA0(s32 x) {
    s32 off = x >> 11;
    s32 v = (x >> 2) & 0x7FF;
    u8 *p1 = D_800C9864;
    s32 sh = *(s32 *)(p1 + (off & 0x1C));
    s32 add = *(s32 *)(D_800C9884 + (off & 0x1C));
    return ((v << sh) + add) >> 3;
}

s32 func_80033DE0(s32 a, s32 idx) {
    s32 n = D_800E8E90[idx] + a;
    if (n >= 0x81) {
        return 0;
    }
    D_800E8E90[idx] = n;
    return D_800E8E88[idx] + (n - a) * 16;
}
