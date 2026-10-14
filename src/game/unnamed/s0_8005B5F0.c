#include "game.h"

void func_8005B5F0(u8 *p)
{
    s32 iVar1;
    f32 fVar2;
    f32 fVar3;
    u32 ev;
    u8 b;

    iVar1 = *(s16 *)(p + 0x628) + 1;
    fVar2 = *(f32 *)(D_800EE498 + *(s16 *)(p + 0x628) * 0x6C + 0x10) * (1.0f - *(f32 *)(p + 0x63C))
          + *(f32 *)(D_800EE498 + (iVar1 % D_800EE490) * 0x6C + 0x10) * *(f32 *)(p + 0x63C);
    ev = D_800EFDBC;
    if (ev == 10) goto L_e4;
    if (ev < 11) {
        if (ev == 7) goto L_e4;
    } else if (ev == 0xF) {
        fVar2 = 50.0f;
    }
    goto L_gen;

L_e4:
    if (!(0.5f < *(f32 *)(p + 0x424))) goto ZERO;
STORE20:
    *(s32 *)(p + 0x5B0) = 0x20;
    return;
ZERO:
    *(s32 *)(p + 0x5B0) = 0;
    return;

L_gen:
    b = *(u8 *)(p + 0x3FF);
    if (b == 1) goto C1;
    if (b == 2) goto C2;
    goto CS;
C1:
    fVar2 = 0.0f;
    goto CS;
C2:
    fVar2 = 5.0f;
CS:
    fVar3 = *(f32 *)(p + 0x420) - fVar2;
    if (0.5f < fVar3) {
        *(s16 *)(p + 0x55C) = 6;
        *(s16 *)(p + 0x4B8) = 6;
        goto STORE20;
    }
    if (fVar3 < -0.5f && *(s32 *)(p + 0x744) < 3 && *(u8 *)(p + 0x7A6) == 0) {
        if (fVar3 < -5.0f && *(f32 *)(p + 0x648) < 9500.0f) {
            if ((*(u32 *)(p + 0x5B4) & 0x20000) == 0) {
                *(s32 *)(p + 0x5B8) = D_800E76D0;
            }
            *(s32 *)(p + 0x5B0) = 0x20000;
            return;
        }
        *(s32 *)(p + 0x5B0) = 0x10;
    }
    return;

}

struct Bike_33 {
    u8 unk0[0x420];
    f32 unk420;
    u8 unk424[0x5A0 - 0x424];
    f32 unk5A0;
    u8 unk5A4[0x5D0 - 0x5A4];
    u8 unk5D0;
};

void func_8005B7F8(Bike_33 *b, f32 maxv) {
    f32 speed = b->unk420;
    s32 i = 0;
    f32 t;
    f32 r;

    while (D_800C9050[i + 1] < speed) {
        i++;
    }
    if (speed < 0.0f) {
        t = 0.0f;
    } else {
        t = (D_800C9050[i + 1] - speed) / (D_800C9050[i + 1] - D_800C9050[i]);
    }
    r = (D_800C901C[i] * t + D_800C901C[i + 1] * (1.0f - t)) * 0.18849557638168335f * 0.5f;
    if (r < maxv) {
        maxv = r;
    } else if (r < -maxv) {
        maxv = -r;
    }
    b->unk5A0 = maxv;
    b->unk5D0 = (s32)((maxv * 13.0f) / r);
}

typedef struct {
    Vec3f pos;
    u8 pad0C[8];
    f32 dx;
    f32 dz;
    u8 pad1C[0x45 - 0x1C];
    s8 unk45;
    s16 next;
    s16 alt;
    u8 pad4A[0x58 - 0x4A];
    f32 unk58;
    f32 unk5C;
    u8 pad60[4];
    u8 flags;
    u8 pad65[0x6C - 0x65];
} Node_15;

struct Bike_34 {
    u8 pad0[0x48];
    Vec3f pos;
    f32 vx;
    f32 vy;
    f32 vz;
    u8 pad60[0xA0 - 0x60];
    f32 unkA0;
    f32 unkA4;
    f32 unkA8;
    u8 padAC[0xB8 - 0xAC];
    f32 unkB8;
    f32 unkBC;
    f32 unkC0;
    u8 padC4[0x418 - 0xC4];
    u8 unk418;
    u8 pad419[2];
    u8 unk41B;
    u8 pad41C[4];
    f32 unk420;
    u8 pad424[8];
    f32 unk42C;
    u8 pad430[0x494 - 0x430];
    f32 unk494;
    f32 unk498;
    f32 unk49C;
    u8 pad4A0[0x538 - 0x4A0];
    f32 unk538;
    f32 unk53C;
    f32 unk540;
    u8 pad544[0x5B0 - 0x544];
    s32 unk5B0;
    u8 pad5B4[0x5C4 - 0x5B4];
    s32 unk5C4;
    u8 pad5C8[8];
    s8 unk5D0;
    u8 pad5D1[0x628 - 0x5D1];
    s16 unk628;
    u8 pad62A[0x638 - 0x62A];
    f32 unk638;
    f32 unk63C;
    u8 pad640[0x6EB - 0x640];
    u8 unk6EB;
    u8 unk6EC;
    u8 pad6ED[0x6FC - 0x6ED];
    Vec3f v6FC;
    u8 pad708[0x714 - 0x708];
    s16 unk714;
    u8 pad716[0x71C - 0x716];
    Vec3f v71C;
    u8 pad728[0x734 - 0x728];
    s16 unk734;
    u8 pad736[0x744 - 0x736];
    s32 unk744;
    u8 pad748[0x7E0 - 0x748];
};

extern Node_15 *D_800EE498_26;
extern Bike_34 D_800E37C0_72[];

void func_8005B928(Bike_34 *b) {
    f32 unused[8];
    s16 ang0;
    s32 ratio;
    s16 ang1;
    s32 idx;
    s32 cnt;
    s32 k;
    s32 j;
    s32 nx;
    f32 dist;
    f32 amp;
    f32 amp2;
    f32 amp3;
    f32 px, pz;
    f32 p0x, p0z;
    f32 ex;
    f32 f;
    f32 dx, dz;
    f32 len, l3;
    f32 ax, az, cx, cz, ez;
    f32 d, d3;
    f32 tx, tz, tl;
    f32 qx, qz, wz, wx;
    f32 vx, vz, sp;
    f32 r, steer;
    f32 ddx, ddz;
    f32 ang, s, c, q, sn;
    Vec3f *pos;
    Node_15 *n;

    switch (D_800EFDBC_2) {
    case 6:
        func_8006B2AC((u8 *)b);
        break;
    case 5:
        func_80047F7C((u8 *)b);
        return;
    case 10:
        func_8005B7F8(b, 0.0f);
        b->unk5D0 = 0;
        break;
    }

    switch (b->unk41B) {
    case 0: {
        s32 idx = b->unk628;
        s32 a;
        if (!(D_800EE498_26[idx].flags & 4) || D_800E76D0 - b->unk5C4 < 300) {
            break;
        }
        func_8001254C(-(b->unkB8 * 16384.0f), b->unkC0 * 16384.0f, &ang0, &ratio);
        func_8001254C(D_800EE498_26[idx].dz * 16384.0f, D_800EE498_26[idx].dx * 16384.0f, &ang1, &ratio);
        ang1 -= ang0;
        a = ang1 < 0 ? -ang1 : ang1;
        if (a > 0x1000) {
            break;
        }
        b->unk41B = 1;
    }
    case 1: {
        s32 idx, j, a;
        s16 d;
        if (b->unk420 < 5.0f) {
            b->unk41B = 2;
            break;
        }
        idx = b->unk628;
        if (!(D_800EE498_26[idx].flags & 4)) {
            b->unk41B = 2;
            break;
        }
        j = idx;
        do {
            j = D_800EE498_26[j].next;
        } while (D_800EE498_26[j].flags & 4);
        func_8001254C(-(b->unkB8 * 16384.0f), b->unkC0 * 16384.0f, &ang0, &ratio);
        func_8001254C(D_800EE498_26[j].dz * 16384.0f, D_800EE498_26[j].dx * 16384.0f, &ang1, &ratio);
        ang1 -= ang0;
        d = ang1;
        a = d < 0 ? -d : d;
        if (a > 0x400) {
            steer = D_800EE498_26[j].pos.x - b->pos.x;
            steer = (steer * D_800C2C60[(ang0 >> 4) & 0xFFF]
                 + (D_800EE498_26[j].pos.z - b->pos.z) * D_800C2C60[((ang0 >> 4) - 0x400) & 0xFFF]) * (1.0f / 16384.0f);
            if (d < 0) {
                b->unk5D0 = -13;
                steer = -steer;
            } else {
                b->unk5D0 = 13;
            }
            if (steer > -2.0f) {
                b->unk5D0 = 0;
            }
            b->unk41B = 1;
            b->unk5B0 = (b->unk5B0 & ~0x10) | 0x20;
        } else {
            b->unk41B = 2;
        }
        steer = b->unk5D0 > 0 ? 0.3f : -0.3f;
        func_8005B7F8(b, steer);
        return;
    }
    case 2:
        b->unk41B = 0;
        b->unk5C4 = D_800E76D0;
        b->unk42C = b->unk638;
        break;
    }

    dist = b->unk42C;
    if (b->unk744 < 3) {
        pos = &b->pos;
        idx = b->unk628;
        amp = b->unk63C;
        cnt = 2;
        if (D_800E76D0 - b->unk5C4 < 300) {
            cnt = 4;
        } else {
            k = D_800EE498_26[idx].unk45;
            if (cnt < k) {
                amp = 0.0f;
                cnt = k;
            }
        }
    } else if (D_800EFDBC_2 != 6) {
        pos = b->unk6EC ? &b->v6FC : &b->v71C;
        idx = b->unk6EC ? b->unk714 : b->unk734;
        r = func_8004A944(pos, idx);
        if (!(D_800EE498_26[idx].unk5C < r)) {
            if (!(r < -D_800EE498_26[idx].unk58)) {
                amp = D_800EE4A0;
                cnt = 2;
                if (cnt < D_800EE498_26[idx].unk45) {
                    amp = 0.0f;
                    cnt = D_800EE498_26[idx].unk45;
                }
            } else {
                b->unk5D0 = -8;
                return;
            }
        } else {
            b->unk5D0 = 8;
            return;
        }
    } else {
        b->unk5D0 = 0;
        return;
    }

    if ((u32)(b->unk418 - 1) < 2 && b->unk420 > 2.0f) {
        idx = b->unk628;
        for (k = (s8)b->unk418; k > 0; k--) {
            idx = D_800EE498_26[idx].next;
        }
        f = (b->vx * D_800EE498_26[idx].dx + b->vz * D_800EE498_26[idx].dz) / b->unk420;
        b->unk5D0 = f * 32.0f;
        if (b->unk5D0 >= 16) {
            b->unk5D0 = 15;
        } else if (b->unk5D0 < -15) {
            b->unk5D0 = -15;
        }
        func_8005B7F8(b, f * 8.0f / (b->unk420 + 5.0f));
        if (b == &D_800E37C0_72[D_800C9650]) {
            D_800EE150 = D_800EE498_26[idx].pos;
            D_800EE160.z = 2.0f * D_800EE498_26[idx].dx;
            D_800EE160.y = 0;
            D_800EE160.x = 2.0f * -D_800EE498_26[idx].dz;
        }
        return;
    }

    switch ((s32)b->unk6EB) {
    case 2:
        j = idx;
        while (cnt != 0) {
            cnt--;
            j = D_800EE498_26[j].alt >= 0 ? D_800EE498_26[j].alt : D_800EE498_26[j].next;
        }
        break;
    case 0:
    case 1:
    default:
        j = idx;
        while (cnt != 0) {
            cnt--;
            j = D_800EE498_26[j].next;
        }
        break;
    }

    b->unk42C = dist;
    nx = D_800EE498_26[j].next;
    p0x = D_800EE498_26[j].pos.x + D_800EE498_26[j].dx * dist;
    p0z = D_800EE498_26[j].pos.z + D_800EE498_26[j].dz * dist;
    dx = (D_800EE498_26[nx].pos.x + D_800EE498_26[nx].dx * dist) - p0x;
    dz = (D_800EE498_26[nx].pos.z + D_800EE498_26[nx].dz * dist) - p0z;
    amp2 = amp * amp;
    amp3 = amp2 * amp;
    len = __builtin_sqrtf(dx * dx + dz * dz);

    {
        f32 t1, s1, t2, s2, a1, b1, acx, acz, ds1;
        t1 = D_800EE498_26[j].dz * -3.0f;
        s1 = D_800EE498_26[nx].dz + D_800EE498_26[j].dz;
        d = len / 3.0f;
        ds1 = d * s1;
        cx = t1 * d;
        t2 = D_800EE498_26[j].dx * 3.0f;
        s2 = d * (D_800EE498_26[nx].dx + D_800EE498_26[j].dx);
        cz = t2 * d;
        a1 = (dx + ds1) * 3.0f;
        b1 = (dz - s2) * 3.0f;
        ax = a1 - cx;
        ex = (dx - ax) - cx;
        az = b1 - cz;
        ez = (dz - az) - cz;
        px = p0x + amp3 * ex + amp2 * ax + amp * cx;
        pz = p0z + amp3 * ez + amp2 * az + amp * cz;
    }
    d = __builtin_sqrtf((px - pos->x) * (px - pos->x) + (pz - pos->z) * (pz - pos->z));

    qx = amp2 * 3.0f * ex + 2.0f * amp * ax + cx;
    qz = amp2 * 3.0f * ez + 2.0f * amp * az + cz;
    sp = __builtin_sqrtf(qx * qx + qz * qz);
    qx /= sp;
    qz /= sp;

    vx = b->vx;
    vz = b->vz;
    sp = __builtin_sqrtf(vx * vx + vz * vz);
    if (sp < 2.0f) {
        vx = b->unkB8;
        vz = b->unkC0;
        sp = __builtin_sqrtf(vx * vx + vz * vz);
    }
    vx /= sp;
    d3 = d / 3.0f;
    ddx = pos->x + vx * d3;
    vz /= sp;
    ddz = pos->z + vz * d3;
    {
        f32 e1, e2;
        e1 = px - qx * d3;
        e2 = pz - qz * d3;
        wx = d * vx;
        tx = (e1 - ddx) * 3.0f;
        wz = d * vz;
        tz = (e2 - ddz) * 3.0f;
    }
    d = wx * wx + wz * wz;
    r = ((tz - wz) * wx - (tx - wx) * wz) / (d * __builtin_sqrtf(d));

    if (b->unk744 < 4) {
        dx = b->unk538 - b->unk494;
        dz = b->unk540 - b->unk49C;
        dx = r * __builtin_sqrtf(dx * dx + dz * dz);
        dx += func_80013098(wx, wz);
        sn = cosf(dx);
        c = sinf(dx);
        q = b->unkA8 * sn - b->unkA0 * c;
        tl = b->unkB8 * c - b->unkC0 * sn;
        steer = 0.0f;
        if (!(r * tl < 0.0f)) {
            steer = func_80013098(-q, tl);
        }
    } else {
        steer = r * 1.25f;
    }
    if (b->unk744 < 4) {
        func_8005B7F8(b, steer);
    } else {
        k = steer * 192.0f;
        if (k >= 16) {
            k = 15;
        } else if (k < -15) {
            k = -15;
        }
        b->unk5D0 = k / 2;
    }
}

#define FLD(t, p, o) (*(t *)((u8 *)(p) + (o)))

void func_8005C5BC(u8 *p, s32 m)
{
    s32 dir, i;
    u8 b;
    f32 x, lim, r, w, T, S, g;

    dir = FLD(s8, p, 0x5d0);
    if (FLD(s16, p, 0x5aa) == 4) {
        func_8006BC14(p);
    } else {
        b = (s32)(FLD(f32, p, 0x48) / 2.178f);
        FLD(u8, p, 0x7a9) = b;
        if (FLD(u8, p, 0x44) != 0) {
            FLD(s8, p, 0x7aa) = 0;
        } else {
            FLD(s8, p, 0x7aa) = (s32)((FLD(f32, p, 0x48) - b * 2.178f) * (D_800C8E00 / 2.178f));
            if (FLD(u8, p, 0x7a9) == 0 && FLD(s8, p, 0x7aa) < 0) {
                FLD(s8, p, 0x7aa) = 0;
            }
        }
    }
    if (FLD(s32, p, 0x744) >= 8) {
        dir = 0;
    }
    x = FLD(f32, p, 0x420);
    i = 0;
    while (D_800C9050[i + 1] < x) {
        i++;
    }
    if (x < 0.0f) {
        r = 0.0f;
    } else {
        r = (D_800C9050[i + 1] - x) / (D_800C9050[i + 1] - D_800C9050[i]);
    }
    S = 0.012566371f;
    T = dir * S;
    w = (D_800C901C[i] * r + D_800C901C[i + 1] * (1.0f - r)) * FLD(f32, p, 0x7d0);
    T *= w;
    lim = 0.18849556f;
    lim *= w;
    S = (D_800C8FD8[i] * r + D_800C8FD8[i + 1] * (1.0f - r)) * 0.327f * m * S;
    if (FLD(u8, p, 0x6e9) != 0) {
        T *= 0.25f;
    }
    if (T == 0.0f || T * FLD(f32, p, 0x5a0) < 0.0f) {
        S *= 5.0f;
    }
    if (FLD(u8, p, 0x41c) != 0) {
        T = D_800CB130;
        if (FLD(s8, p, 0x5d0) <= 0) {
            T = -T;
        }
    } else if (lim < T) {
        S = 0.1f;
        T = lim;
    } else {
        w = -lim;
        if (T < w) {
            S = 0.1f;
            T = w;
        }
    }
    if (D_800EFDBC == 7) {
        T += (FLD(f32, p, 0xb8) * FLD(f32, p, 0x56c) - FLD(f32, p, 0xc0) * FLD(f32, p, 0x564)) * 16.0f * S;
    }
    g = FLD(f32, p, 0x5a0);
    if (T < g) {
        FLD(f32, p, 0x5a0) = g - S;
        if (FLD(f32, p, 0x5a0) < T) {
            FLD(f32, p, 0x5a0) = T;
        }
    } else {
        FLD(f32, p, 0x5a0) = g + S;
        if (T < FLD(f32, p, 0x5a0)) {
            FLD(f32, p, 0x5a0) = T;
        }
    }
}

void func_8005C8F4(void)
{
}

void func_8005C8FC(void)
{
}
