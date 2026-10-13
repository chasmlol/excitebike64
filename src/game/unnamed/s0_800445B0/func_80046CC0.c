#include "ctx.h"

typedef struct {
    f32 x, y, z, w;
} Quat;

typedef struct Bike {
    u8 pad0[0x22 - 0x0];
    s16 f22;
    u8 pad24[0x34 - 0x24];
    s16 f34;
    u8 pad36[0x44 - 0x36];
    u8 f44;
    s8 f45;
    u8 pad46[0x48 - 0x46];
    Vec3f pos;
    f32 f54;
    f32 f58;
    f32 f5C;
    u8 pad60[0x78 - 0x60];
    Quat q;
    f32 f88;
    f32 f8C;
    f32 f90;
    u8 pad94[0xA0 - 0x94];
    f32 mtx[9];
    void * fC4;
    Vec3f hist[11];
    u8 pad14C[0x1B8 - 0x14C];
    f32 f1B8;
    f32 f1BC;
    f32 f1C0;
    u8 f1C4;
    u8 pad1C5[0x36C - 0x1C5];
    u8 f36C;
    u8 pad36D[0x370 - 0x36D];
    Quat q370;
    u8 pad380[0x3C4 - 0x380];
    s32 f3C4;
    u8 pad3C8[0x3D0 - 0x3C8];
    s8 f3D0;
    s8 f3D1;
    u8 pad3D2[0x3D3 - 0x3D2];
    s8 f3D3;
    u8 pad3D4[0x3FB - 0x3D4];
    u8 f3FB;
    u8 pad3FC[0x3FF - 0x3FC];
    u8 f3FF;
    u8 pad400[0x416 - 0x400];
    s16 f416;
    u8 pad418[0x41D - 0x418];
    u8 f41D;
    u8 pad41E[0x430 - 0x41E];
    f32 f430;
    f32 f434;
    f32 f438;
    u8 pad43C[0x44C - 0x43C];
    f32 f44C;
    u8 pad450[0x478 - 0x450];
    f32 f478;
    u8 pad47C[0x51C - 0x47C];
    f32 f51C;
    u8 pad520[0x5A8 - 0x520];
    s16 f5A8;
    s16 f5AA;
    u8 pad5AC[0x5CD - 0x5AC];
    u8 f5CD;
    u8 pad5CE[0x620 - 0x5CE];
    u8 f620;
    u8 f621;
    u8 pad622[0x624 - 0x622];
    s16 f624;
    s16 f626;
    s16 f628;
    u8 pad62A[0x648 - 0x62A];
    s32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 f654;
    u8 pad655[0x690 - 0x655];
    s32 f690;
    u8 pad694[0x6E8 - 0x694];
    u8 f6E8;
    u8 pad6E9[0x6ED - 0x6E9];
    u8 f6ED;
    u8 pad6EE[0x744 - 0x6EE];
    s32 f744;
    s32 f748;
    u8 pad74C[0x7A5 - 0x74C];
    u8 f7A5;
    u8 pad7A6[0x7A9 - 0x7A6];
    u8 f7A9;
    u8 f7AA;
    u8 pad7AB[0x7AC - 0x7AB];
    s16 f7AC;
    u8 pad7AE[0x7DC - 0x7AE];
    u8 f7DC;
    u8 pad7DD[0x7E0 - 0x7DD];
} Bike;

typedef struct {
    Vec3f pos;
    u8 padC[0x14 - 0xC];
    f32 f14;
    f32 f18;
    u8 pad1C[0x44 - 0x1C];
    s8 f44;
    u8 pad45;
    s16 next;
    u8 pad48[2];
    s16 prev;
    u8 pad4C[0x58 - 0x4C];
    f32 f58;
    f32 f5C;
    u8 pad60[0x6C - 0x60];
} Node;

typedef struct {
    u8 pad0[0x3C];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    u32 mode;
} Game;

typedef struct {
    s32 f0;
    u8 pad4[0x58 - 0x4];
    s32 f58;
} State;

extern Bike D_800E37C0[];
extern s8 D_800EE30C;
extern s32 D_800C9650;
extern s32 D_800C966C[];
extern Game D_800EFD70;
extern s32 D_800EFDBC;
extern State D_800F05A8;
extern Vec3f D_800F1510;
extern Node *D_800EE498;
extern s32 D_800EE490;
extern s32 D_800EE488;
extern s32 D_800C8CD8;
extern f32 D_800C8E14;

void func_8002DA48(s32 i, s32 v);
void func_8005E9A4(s32 a0);
void func_80061D54(void *p);
void func_80013568(f32 *m, f32 *q);
f32 func_80013098(f32 p1, f32 p2);
void func_80013438(f32 *out, f32 x, f32 y, f32 z);
void func_8002FF44(f32 *pos);
void func_80030478(f32 *p);
void func_8001C780(Bike *b);
void func_8001B808(Bike *b);
void func_8001BC98(Bike *b);
void func_8001FE1C(Bike *b);
s32 func_8002E920(s32 x, s32 y, s32 z);

void func_80046CC0(Bike *b) {
    s32 s;
    s32 v, i, k, idx;
    Node *n;
    Game *g;
    Bike *p;
    f32 z, r2, half, t, d, dx, dz, y;

    if (b->f6E8 == 2) {
        b->f7A5 = 1;
        return;
    }
    b->f36C = 0;
    b->f748 = 0;
    b->f744 = 0;
    b->f5CD = 0;
    b->f416 = 0;
    b->f45 = 0;
    b->f624 = 0;
    b->f44 = 0;
    b->f7AC = 0;
    b->f648 = 0;
    b->f654 = 0;
    b->f3D1 = -1;
    b->f3D3 = -1;
    b->f3FB = 0;
    b->f7DC = 0;
    if (b->f3FF == 0) {
        b->f620 = 40;
    }
    b->f621 = 16;
    b->f41D = 1;
    b->f44C = 1.0f;
    b->f1C4 = b->f6E8 != 0;
    if (b->f6E8 == 0) {
        D_800EE30C = 0;
        if (D_800C9650 == -1) {
            D_800C966C[b - D_800E37C0] = -1;
        } else if (D_800C9650 == b - D_800E37C0) {
            D_800C966C[0] = -1;
        }
    }
    s = -1;
    g = &D_800EFD70;
    switch (g->mode) {
    case 2:
        if ((D_800F05A8.f58 & 0xF) || (D_800F05A8.f0 != 7 && D_800F05A8.f0 != 11 && D_800F05A8.f0 != 9)) {
        b->f620 = 0;
        b->f621 = 0;
        if (g->f3C == 4) {
            switch (b - D_800E37C0) {
            case 0:
                b->pos.x = -1.0f;
                b->pos.z = -10.0f;
                func_80013438(&b->q.x, 0.0f, 0.0f, 0.0f);
                break;
            case 1:
                b->pos.x = 1.0f;
                b->pos.z = -10.0f;
                func_80013438(&b->q.x, 0.0f, 0.0f, 0.0f);
                break;
            case 2:
                b->pos.x = 1.0f;
                b->pos.z = 10.0f;
                func_80013438(&b->q.x, 0.0f, 0.0f, 3.1415927f);
                break;
            case 3:
                b->pos.x = -1.0f;
                b->pos.z = 10.0f;
                func_80013438(&b->q.x, 0.0f, 0.0f, 3.1415927f);
                break;
            }
        } else {
            switch (b - D_800E37C0) {
            case 0:
                b->pos.x = 0.015f;
                b->pos.z = -10.0f;
                func_80013438(&b->q.x, 0.0f, 0.0f, 0.00958738f);
                break;
            case 1:
                b->pos.x = 0.01f;
                b->pos.z = 10.0f;
                func_80013438(&b->q.x, 0.0f, 0.0f, 3.15118f);
                break;
            }
        }
        } else {
            func_8002FF44(&b->pos.x);
            func_80013438(&b->q.x, 0.0f, 0.0f, func_80013098(-(b->pos.z - D_800F1510.z), b->pos.x - D_800F1510.x));
        }
        b->f41D = 0;
        break;
    case 4:
    case 5:
        func_80030478(&b->pos.x);
        func_80013438(&b->q.x, 0.0f, 0.0f, (f32)b->f34 * 3.1415927f * 3.0517578e-05f);
        break;
    case 10:
        func_8002FF44(&b->pos.x);
        func_80013438(&b->q.x, 0.0f, 0.0f, (f32)b->f34 * 3.1415927f * 3.0517578e-05f);
        break;
    default:
        s = b->f626;
        if (D_800EFD70.mode == 6 && s > 0) {
            s--;
        }
        if (D_800EFD70.mode == 14) {
            s = 1;
        }
        while (D_800EE498[s].f44 > 0) {
            s = D_800EE498[s].next;
            if (s > D_800EE490) {
                s = 0;
            }
        }
        while (D_800EE498[s].f44 < 0) {
            s = D_800EE498[s].prev;
            if (s < 0) {
                s = D_800EE490 - 1;
            }
        }
        k = 0;
        v = b->f628;
        for (i = 0; i < 8; i++) {
            if (v == b->f626) {
                b->f6ED += k;
                break;
            }
            if (v == D_800EE488) {
                k = -1;
            }
            v = D_800EE498[v].prev;
        }
        b->f628 = s;
        if (D_800EFD70.mode == 6) {
            b->f7AA = 0;
            b->pos.x = (f32)b->f7A9 * 2.178f;
        } else {
            r2 = (D_800EE498[s].f58 * D_800EE498[s].f58 + D_800EE498[s].f5C * D_800EE498[s].f5C) / 36.0f;
            half = __builtin_sqrtf(r2) * 0.5f;
            t = 0.0f;
            while (t <= D_800EE498[s].f5C || -t <= D_800EE498[s].f58) {
                if (!(t > 0.0f ? D_800EE498[s].f5C < t : D_800EE498[s].f58 < -t)) {
                    b->pos.x = D_800EE498[s].pos.x + D_800EE498[s].f14 * t;
                    d = 10000.0f;
                    b->pos.z = D_800EE498[s].pos.z + D_800EE498[s].f18 * t;
                    if (D_800C8CD8 != 0) {
                        break;
                    }
                    for (p = D_800E37C0; p < &D_800E37C0[8]; p++) {
                        if (p == b || p->f7A5 != 0) {
                            continue;
                        }
                        dx = b->pos.x - p->pos.x;
                        dz = b->pos.z - p->pos.z;
                        d = dx * dx + dz * dz;
                        if (d < r2) {
                            break;
                        }
                    }
                    if (r2 <= d) {
                        break;
                    }
                }
                t += (t >= 0.0f ? half : 0.0f);
                t = -t;
            }
            if (d < r2) {
                b->pos = D_800EE498[s].pos;
            }
        }
        func_80013438(&b->q.x, 0.0f, 0.0f, func_80013098(D_800EE498[s].f14, D_800EE498[s].f18));
        break;
    }
    func_8001C780(b);
    func_80013568(b->mtx, &b->q.x);
    b->f3C4 = 0;
    b->f5AA = b->f5A8;
    z = 0.0f;
    b->f51C = z;
    b->f478 = z;
    b->f5C = z;
    b->f58 = z;
    b->f54 = z;
    b->f90 = z;
    b->f8C = z;
    b->f88 = z;
    b->f430 = z;
    b->f434 = z;
    b->f438 = z;
    b->f1B8 = 1.0f;
    if (b == D_800E37C0) {
        b->f690 &= ~0x10;
    }
    b->f3D0 = -1;
    b->f1BC = 250.0f;
    b->f1C0 = 250.0f;
    b->q370 = b->q;
    if (s > 0) {
        b->pos.y = D_800EE498[s].pos.y + 1.0f;
    }
    y = (f32)func_8002E920(b->pos.x * 1000.0f, b->pos.y * 1000.0f, b->pos.z * 1000.0f) / 1000.0f + z + 0.5f;
    b->pos.y = y - (D_800EFD70.mode == 6 ? 3675.0002f / (2.0f * (D_800C8E14 * 20000.0f)) : 0.12250001f);
    func_8001B808(b);
    func_8001B808(b);
    func_8001BC98(b);
    b->f34 = b->f22;
    for (i = 0; i < 11; i++) {
        b->hist[i] = b->pos;
    }
    idx = b - D_800E37C0;
    func_8005E9A4(idx);
    func_8002DA48(idx, 0);
    func_80061D54(b->fC4);
    func_8001FE1C(b);
}
