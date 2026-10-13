#include "game.h"

struct Hdr {
    u32 n;
    u32 p;
    Entry e[1];
};

#define TILEBITS(s3, fp, t4) \
    ((((s3) & 2) ? G_TX_CLAMP : G_TX_WRAP) << 18 | ((fp) & 0xF) << 14 | \
     (((s3) & 8) ? G_TX_MIRROR : ((s3) & 1) ? G_TX_CLAMP : G_TX_WRAP) << 8 | ((t4) & 0xF) << 4)

#define CMS(b) (((b) & 8) ? G_TX_MIRROR : ((b) & 1) ? G_TX_CLAMP : G_TX_WRAP)
#define CMT(b) (((b) & 2) ? G_TX_CLAMP : G_TX_WRAP)

typedef struct { u8 w8, h, flags, pad; u32 addr; } Tex_2;
struct Seg_3 { u32 f0; u16 *pal; Tex_2 tex[1]; };
struct Model { u32 f0; Vtx *vtx; u32 f8; u8 *cmd; };

Gfx *func_80013D70(s32 flags, Model *mdl, Seg_3 **segs, Gfx *gdl) {
    u32 addr;
    Vtx *vtx;
    s32 blk, done;
    u8 c0;
    s32 vbase, mtx, h, rel;
    u8 *cmd;
    u8 a, b, c;
    s32 seg, w, fp, t4;
    u32 tl;
    s32 pl;

    mtx = 0;
    cmd = mdl->cmd;
    vtx = mdl->vtx;
    if (flags & 8) D_800E36D0 = -1;
    done = 0;
    rel = flags & 0x20;
    do {
        c0 = cmd[0];
        a = cmd[1];
        b = cmd[2];
        c = cmd[3];
        switch (c0 >> 4) {
        case 5:
            vbase = 0;
            cmd += 4;
            break;
        case 3:
            if ((cmd[4] >> 4) == 3) {
                gSP2Triangles(gdl++, a, b, c, 0, cmd[5], cmd[6], cmd[7], 0);
                cmd += 8;
            } else {
                gSP1Triangle(gdl++, a, b, c, 0);
                cmd += 4;
            }
            break;
        case 1:
            gSPVertex(gdl++, &vtx[*(u16 *)(cmd + 2)], a, 0);
            cmd += 4;
            break;
        case 4:
            gSPVertex(gdl++, &vtx[*(u16 *)(cmd + 2)], a, vbase);
            vbase += a;
            cmd += 4;
            break;
        case 6:
            if (D_800C8C70_2 != 0) {
                gSPMatrix(gdl++, &D_800C8C70_2[cmd[1]], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                mtx = 1;
            }
            cmd += 4;
            break;
        case 2:
            if (!(flags & 2)) {
                t4 = c & 0xF;
                seg = b >> 6;
                fp = c >> 4;
                addr = segs[seg]->tex[a].addr;
                blk = (b >> 2) & 1;
                w = segs[seg]->tex[a].w8 * 8;
                h = segs[seg]->tex[a].h;
                if (segs[seg]->tex[a].flags & 2) {
                    if (D_800C8C7C != 1) {
                        if (D_800C8C80 != NULL) gdl = D_800C8C80(gdl, 1, c);
                        D_800C8C7C = 1;
                    }
                } else if (D_800C8C7C != 0) {
                    if (D_800C8C80 != NULL) gdl = D_800C8C80(gdl, ((u32)&cmd[1] & 1) >> 1, c);
                    D_800C8C7C = 0;
                }
                addr = segs[seg]->tex[a].addr;
                if (rel) addr = ((addr - (u32)segs[seg]) & 0xFFFFFF) | ((seg + 2) << 24);
                if (D_800C8C84 != NULL) D_800C8C84(&addr);
                t4 -= D_800C8C78;
                fp -= D_800C8C78;
                pl = c0 & 0xF;
                if ((flags & 4) && seg != D_800E36D0) {
                    u32 pv = (u32)segs[seg]->pal;
                    if (pv != 0) {
                        tl = pv;
                        if (rel) { tl -= (u32)segs[seg]; tl &= 0xFFFFFF; tl |= (seg + 2) << 24; }
                        else if (flags & 0x40) tl = (seg + 1) << 24;
                        gDPLoadTLUT_pal256(gdl++, tl);
                    }
                }
                D_800E36D0 = seg;
                if (blk == 0) {
                    gDPLoadTextureBlock_4b(gdl++, addr, G_IM_FMT_CI, w, h, pl, CMS(b), CMT(b), t4, fp, 0, 0);
                } else {
                    gDPLoadTextureBlock(gdl++, addr, D_800C8C74 ? G_IM_FMT_IA : G_IM_FMT_CI, G_IM_SIZ_8b, w, h, 0,
                                        CMS(b), CMT(b), t4, fp, 0, 0);
                    if (D_800C8C74 != 0 && segs[seg]->pal != NULL) {
                        s32 col = segs[seg]->pal[a];
                        s32 r, g, bb;
                        r = (col >> 8) & 0xF8; g = (col >> 3) & 0xF8; bb = (col << 2) & 0xF8;
                        gDPPipeSync(gdl++);
                        gDPSetPrimColor(gdl++, 0, 0, r, g, bb, 0xFF);
                    }
                }
                if (segs[seg]->tex[a].flags & 8) {
                    gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, (-(D_800E2038 * 2) & 0xFF) >> D_800C8C78, 0, 0);
                }
            }
            cmd += 4;
            break;
        case 0:
            done = 1;
            break;
        }
    } while (!done);
    if (mtx) gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
    if (flags & 1) gSPEndDisplayList(gdl++);
    return gdl;
}

void func_800147C4(u32 *param_1, s32 param_2)
{
    u32 i;
    s32 a2;
    s32 a3;
    s32 j;
    u32 *dst;
    s32 jn;

    u32 idx;
    s32 g;
    s32 *tab;
    u16 t1;
    u32 t0;

    if (param_1[0] == 0) {
        return;
    }
    i = 0;
    tab = D_800C8CA8;
    g = D_800C8C74;
    do {
        a2 = *(u8 *)((s32)param_1 + (i << 3) + 0xA) & 0x34;
        if (a2 != 0) {
            idx = ((u32)a2 >> 5) << 2;
            if (a2 & 0x10) {
                idx = idx | 2;
            }
            a2 = (((u32)a2 >> 2) & 1) | idx;
            a2 = tab[a2];
            if (param_2 % a2 == 0) {
                a3 = i + 1;
                while (*(u8 *)((s32)param_1 + (a3 << 3) + 0xA) & 0x34) {
                    a3++;
                }
                if (g != 0) {
                    t1 = ((u16 *)param_1[1])[i];
                    for (a2 = i; a2 < a3; a2++) {
                        ((u16 *)param_1[1])[a2] = ((u16 *)param_1[1])[a2 + 1];
                    }
                    ((u16 *)param_1[1])[a3] = t1;
                }
                t0 = *(u32 *)((s32)param_1 + (i << 3) + 0xC);
                for (a2 = i; a2 < a3; a2++) {
                    *(u32 *)((s32)param_1 + (a2 << 3) + 0xC) = *(u32 *)((s32)param_1 + ((a2 + 1) << 3) + 0xC);
                }
                *(u32 *)((s32)param_1 + (a3 << 3) + 0xC) = t0;
                i = a3;
            }
        }
        i = i + 1;
    } while (i < param_1[0]);
}

typedef struct {
    u8 w;
    u8 h;
    u8 flags;
    u8 pad;
    u8 *data;
} Img_5;

struct ImgSet {
    u32 count;
    u32 pad;
    Img_5 img[1];
};

static inline void flip(u8 *pa, s32 row, s32 h)
{
    s32 j, k;
    u8 *pb;
    u8 tmp;

    pb = pa + row * (h - 1);
    for (j = 0; j < h / 2; j++) {
        for (k = 0; k < row; k++) {
            tmp = *pa;
            *pa++ = *pb;
            *pb++ = tmp;
        }
        pb -= row * 2;
    }
}

void func_80014948(ImgSet *s)
{
    u32 i;

    for (i = 0; i < s->count; i++) {
        if (!(s->img[i].flags & 1)) {
            flip(s->img[i].data, s->img[i].w * 4, s->img[i].h);
        } else {
            flip(s->img[i].data, s->img[i].w * 8, s->img[i].h);
        }
    }
}

void func_80014A68(s32 a0)
{
    D_800C8C80 = a0;
}

void func_80014A74(s32 a0)
{
    D_800C8C84 = a0;
}

void func_80014A80(void)
{
    func_800BFC40(D_800D614C, func_80014CCC);
    func_800BFC40(D_800D6150, func_80014D20);
    D_800E36D0 = -1;
    D_800C8C78 = 0;
}

void func_80014AD0(void)
{
}

void func_80014AD8(s32 x)
{
    D_800C8C78 = x;
}

void func_80014AE4(void **param_1, u32 *param_2, s32 param_3, u32 param_4)
{
    u32 r;
    u32 saved;

    r = func_80013D70(param_4 | 0x21, param_2, param_3, D_800C29E0);
    *param_1 = func_800BC908(r - D_800C29E0, 0, D_800D6154, 0x323);
    saved = param_2[1];
    param_2[1] = 0x01000000;
    func_80013D70(param_4 | 0x21, param_2, param_3, (u32)*param_1);
    param_2[1] = saved;
}

void func_80014B84(u32 *p1, u32 a1, u32 a2, u32 a3)
{
    u32 r;

    r = func_80013D70(a3 | 1, a1, a2, D_800C29E0);
    r = func_800BC908(r - D_800C29E0, 0, D_800D6154, 0x33B);
    *p1 = r;
    func_80013D70(a3 | 1, a1, a2, r);
}

void func_80014C14(u8 *p, s32 stride, s32 rows) {
    u8 *q = p + stride * (rows - 1);
    s32 i;
    s32 j;
    for (i = 0; i < rows / 2; i++) {
        for (j = 0; j < stride; j++) {
            u8 t = *p;
            *p = *q;
            p++;
            *q = t;
            q++;
        }
        q -= stride * 2;
    }
}

void func_80014C80(Hdr **pp)
{
    Hdr *h = *pp;
    u32 i;

    if (h->p != 0) {
        h->p -= (u32)h;
    }
    for (i = 0; i < h->n; i++) {
        h->e[i].b -= (u32)h;
    }
}

void func_80014CCC(Hdr **pp)
{
    Hdr *h = *pp;
    u32 i;

    h = (Hdr *)((u32)h & ~2);
    if (h->p != 0) {
        h->p += (u32)h;
    }
    for (i = 0; i < h->n; i++) {
        h->e[i].b += (u32)h;
    }
}

void func_80014D20(u32 *p)
{
    u32 b = *p & 0xFFFFFFFC;
    *(u32 *)(b + 4) = *(u32 *)(b + 4) + b;
    *(u32 *)(b + 12) = *(u32 *)(b + 12) + b;
}

s32 func_80014D48(u16 *p, s32 r, s32 g, s32 b, s32 flag, s32 n)
{
    s32 best = 99999999;
    s32 bi = 0;
    s32 i;

    for (i = 0; i < n; i++, p++) {
        if (flag == 0) {
            if ((*p & 1) == 0) {
                return i;
            }
        } else {
            if ((*p & 1) == 0) {
                continue;
            }
        }
        {
            u32 c = *p;
            s32 t;
            s32 d;
            t = (s32)(c >> 11) - r;
            d = t * t;
            t = (s32)((c >> 6) & 0x1F) - g;
            t = t * t;
            d += t;
            t = (s32)((c >> 1) & 0x1F) - b;
            t = t * t;
            d += t;
            if (d < best) {
                best = d;
                bi = i;
            }
        }
    }
    return bi;
}
