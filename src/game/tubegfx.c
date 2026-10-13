#include "game.h"

typedef struct {
    s32 id;
    s32 x0, z0, x1, z1;
    s32 pad14, pad18;
    u32 mask;
} Item;

typedef struct {
    u32 n;
    Item it[1];
} ItemList;

typedef struct {
    s32 pad0;
    void *seg;
} SegRef;

extern SegRef *D_800E783C_2;
extern ItemList *D_800C9644;
extern ItemList *D_800C9640_2;

static inline MtxS *getmtx(s32 x, s32 y, s32 z, s32 view)
{
    MtxS *m;

    if (D_800E77D0 >= 300) {
        return NULL;
    }
    x = x * 16 - D_800E8090[view].x;
    y = y * 16 - D_800E8090[view].y;
    z = z * 16 - D_800E8090[view].z;
    m = &D_800E7798[D_800E30A8][D_800E77D0];
    func_80011490(m, 0, 0, 0, x, y, z, 0x100);
    D_800E77D0++;
    return m;
}

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 pad[6];
    s32 f28;
} Blk5CC0;

extern Blk5CC0 D_800F5CC0;

void func_80022BD0(void)
{
    s32 iVar1;
    s32 t;

    D_800C23F0 = 0x4000;
    iVar1 = (s32)(D_800EFD64 - D_800CB210_2) * -0x33333333 >> 5;
    D_800E7718 = 0;
    if (iVar1 == 0x18) {
        D_800E7718 = 1;
    }
    if (iVar1 == 0x16) {
        D_800E7718 = 1;
    }
    func_800BFC40(D_800D6A30, (s32)D_800248EC);
    func_800BFC40(D_800D6A34, (s32)D_80031D08);
    if ((*(u32 *)(D_800EFD64 + 0x50) & 2) != 0) {
        func_800A4BEC();
        func_800A5A74();
        if (D_800F5CC0.f28 != 0) {
            D_800C9574 = D_800F5CC0.f28;
        }
        D_800C9648 = 0;
        D_800E7824 = 0;
        D_800E783C = D_800F5CC0.f4;
        D_800E781C = D_800F5CC0.fC;
        D_800E7720 = func_800BF79C(D_800E7728, D_800D07C0);
        D_800E7710 = func_800BF79C(D_800E7728, D_800D06F4);
        D_800E771C = func_800BF79C(D_800E7728, D_800D07D4);
        D_800E7808[1] = 0;
        D_800E7808[2] = 0;
        D_800E77F8 = 0;
        D_800E7808[0] = D_800E781C;
        D_800C9644_2 = (s32 *)D_800F5CC0.f0;
        D_800E78A0 = func_80027574(D_800F5CC0.f0, (s32)&D_800E783C, 0, 0);
        D_800C9640 = (s32 *)D_800F5CC0.f8;
        D_800E7828 = func_80027574(D_800F5CC0.f8, (s32)D_800E7808, 0xC, 1);
        func_80024490((s32)D_800C9644_2);
    } else {
        D_800C9574 = func_8005AED8(D_800D1518);
        D_800C9648 = func_8005AED8(D_800D14E4);
        D_800E7824 = func_8005AED8(D_800D1510);
        D_800E783C = func_8005AED8(D_800D1504);
        D_800E781C = func_8005AED8(D_800D1508);
        D_800E7720 = func_8005AED8(D_800D1584);
        D_800E7710 = func_8005AED8(D_800D1588);
        D_800E771C = func_8005AED8(D_800D158C);
        D_800E77F8 = func_8005AED8(D_800D1500);
        D_800E7808[1] = D_800E77F8;
        D_800E7808[2] = 0;
        D_800E7808[0] = D_800E781C;
        D_800C9644_2 = (s32 *)func_8005AED8(D_800D14DC);
        D_800E78A0 = func_80027574((s32)D_800C9644_2, (s32)&D_800E783C, 0, 0);
        D_800C9640 = (s32 *)func_8005AED8(D_800D14E0);
        D_800E7828 = func_80027574((s32)D_800C9640, (s32)D_800E7808, 0xC, 1);
    }
    func_80011490(D_800E7730, 0, 0, 0, 0, 0, 0, 300);
    func_8002DA60((s32)D_800C9644_2, (s32)D_800C9640, D_800C9574, 0);
    iVar1 = func_8005B58C(D_800D1528);
    func_80065B10(D_800E7728, iVar1, 1, (s32)D_800C9644_2, D_800E781C);
    if ((D_800E1898 == 1) && (D_800E7718 == 0)) {
        t = *D_800C9644_2;
        D_800E7724 = t << 1;
        D_800E7714 = func_800BC908(t << 2, 0, (s32)D_800D6A38, 0xC9);
    } else {
        D_800E7714 = 0;
    }
}

typedef struct {
    u8 pad[0x58];
    u16 fogmin[4];
    u16 fogmax[4];
} FogTab;

extern SegRef *D_800E771C_2;
extern SegRef *D_800E7710_2;
extern ItemList *D_800E7720_2;

Gfx *func_80022FD8(Gfx *gdl, s32 a1, s32 a2, s32 view)
{
    u32 i, n;
    s32 j;
    s32 k;
    u32 m;
    FogTab *fog;
    Item *it;

    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    if (D_800C2350 & 0x20) {
        gSPSegment(gdl++, 6, D_800C95B8);
        gSPSegment(gdl++, 7, D_800C95D8);
        for (i = 0; i < D_800C9640_2->n; i++) {
            m = D_800C9640_2->it[i].mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gdl = func_800281F4(gdl, i);
                gSPDisplayList(gdl++, D_800E7828[i]);
            }
        }
    }
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPLoadTLUT_pal256(gdl++, D_800E783C_2->seg);
    if (D_800E7714_2 != NULL && view == 0) {
        for (j = 0; j < D_800E7724; j++) {
            k = D_800E7714_2[j];
            if (k < 0) continue;
            it = &D_800C9644->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, D_800E78A0[k]);
            }
        }
    } else {
        for (k = 0; k < D_800C9644->n; k++) {
            it = &D_800C9644->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && !(m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, D_800E78A0[k]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_FOG);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetFogColor(gdl++, D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 255);
    fog = D_800EFD64;
    gSPFogPosition(gdl++, fog->fogmin[D_800E1898 - 1], fog->fogmax[D_800E1898 - 1]);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFFFFF);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    if (D_800E7714_2 != NULL && view == 0) {
        for (j = 0; j < D_800E7724; j++) {
            k = D_800E7714_2[j];
            if (k < 0) continue;
            it = &D_800C9644->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && (m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, D_800E78A0[k]);
            }
        }
    } else {
        for (k = 0; k < D_800C9644->n; k++) {
            it = &D_800C9644->it[k];
            m = it->mask;
            if ((m & (1 << (view * 2))) && (m & (2 << (view * 2)))) {
                gSPDisplayList(gdl++, D_800E78A0[k]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_FOG);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800E771C_2->seg);
    if (D_800C2594 < 3) {
        for (i = 0; i < D_800E7720_2->n; i++) {
            if (D_800C2350 & 0x10) {
                gdl = func_80013D70(4, D_800E7720_2->it[i].id, &D_800E771C_2, gdl);
            }
        }
    } else {
        gdl = func_80013D70(4, (s32)D_800E7710_2->seg, &D_800E771C_2, gdl);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    Gfx *dl;
    u8 pad[0x18];
    u32 mask;
} Ent_14;

typedef struct {
    u32 n;
    Ent_14 e[1];
} Tbl;

typedef struct {
    u8 pad[0x58];
    u16 fogMax[4];
    u16 fogMin[4];
} Fog;

extern Tbl *D_800C9648_2;
extern Fog *D_800EFD64_18;

Gfx *func_800238F0(Gfx *gdl, s32 p2, s32 p3, u32 sh)
{
    u32 i;
    u32 off;
    u8 pad[0x48];
    Tbl *tbl = D_800C9648_2;


    if (tbl == NULL) {
        return gdl;
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetCombine(gdl++, 0x119823, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, D_800EFD64_18->fogMin[D_800E1898 - 1], D_800EFD64_18->fogMax[D_800E1898 - 1]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    D_800C8C74 = 1;
    for (i = 0; i < D_800C9648_2->n; i++) {
        off = i * 32 + 4;
        if ((*(u32 *)((u8 *)D_800C9648_2 + off + 0x1C) >> (sh << 1)) & 1) {
            gdl = func_80013D70(0, D_800C9648_2->e[i].dl, D_800E7824_2, gdl);
        }
    }
    gDPPipeSync(gdl++);
    D_800C8C74 = 0;
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPClearGeometryMode(gdl++, G_FOG);
    return gdl;
}

typedef struct Ent_15 {
    u32 pad[8];
} Ent_15;

typedef struct List {
    u32 n;
    u32 pad[7];
    struct { u32 flags; u32 pad[7]; } e[1];
} List;

extern List *D_800C9640_4;

Gfx *func_80023C0C(Gfx *gdl, s32 a1, s32 a2, s32 mode)
{
    s32 pos[4];
    s32 out[3];
    s32 mtx[10];
    u16 *fog;
    u32 i;

    func_80011A58(mtx, 0, 0, D_800E8000[mode]);
    pos[0] = 0;
    pos[1] = -30;
    pos[2] = 0;
    func_80012DC8(pos, mtx, out);
    func_80011490(D_800E7730, 0, 0, 0, out[0], out[1], out[2], 0x100);

    gSPViewport(gdl++, &D_800E19C0[mode + 4]);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, 0x200004);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 255);
    fog = (u16 *)((u8 *)D_800EFD64 + ((D_800E1898 - 1) << 1));
    gSPFogPosition(gdl++, fog[0x2C], fog[0x30]);
    gSPSegment(gdl++, 6, D_800C9588);
    gSPSegment(gdl++, 7, D_800C95A0);

    for (i = 0; i < D_800C9640_4->n; i++) {
        if ((D_800C9640_4->e[i].flags & (1 << (mode * 2))) && (D_800C9640_4->e[i].flags & (2 << (mode * 2)))) {
            gdl = func_800281F4(gdl, i);
            gSPDisplayList(gdl++, D_800E7828[i]);
        }
    }

    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    return gdl;
}

typedef struct {
    u8 pad[0x4C];
    s32 mode;
} Game_12;

typedef struct {
    s32 pad0;
    s32 x0;
    s32 z0;
    s32 x1;
    s32 z1;
    s32 pad14;
    s32 flag;
    u32 bits;
} Rec_10;

typedef struct {
    u32 count;
    Rec_10 recs[1];
} List_2;

extern Game_12 D_800EFD70_27;

void func_8002400C(u32 *list, s32 p, s32 strict, s16 *slots, s32 nslots)
{
    s32 unused[10];
    s32 d[4];
    s32 r[4];
    s32 cam0;
    s32 cam1;
    s32 cam2;
    s32 far;
    s32 near;
    s32 j;
    s32 sn;
    s32 cs;
    s32 k17;
    s32 k5;
    s32 n;
    u32 i;
    u32 m2;
    u32 m1;
    s32 a;
    s32 h;
    s32 idx;
    s32 ex;
    s32 ey;
    Rec_10 *rec;

    far = D_800C9550[D_800E1898 - 1];
    near = D_800C9560[D_800E1898 - 1];
    if (slots != NULL) {
        for (sn = 0; sn < nslots; sn++) {
            slots[sn] = -1;
        }
    }
    cam0 = D_800E8090_3[p][0] >> 4;
    sn = D_800C2C60[(D_800E8000[p] >> 4) & 0xFFF];
    cs = D_800C2C60[((D_800E8000[p] >> 4) - 0x400) & 0xFFF];
    cam1 = D_800E8090_3[p][1] >> 4;
    cam2 = D_800E8090_3[p][2] >> 4;
    for (i = 0; i < list[0]; i++) {
        rec = (Rec_10 *)(list + 1) + i;
        ex = ((rec->x1 + rec->x0) >> 1) - cam0;
        ey = ((rec->z1 + rec->z0) >> 1) - cam2;
        rec->flag = 1;
        rec->bits &= ~(2 << (p * 2));
        d[0] = ex;
        d[1] = -cam1;
        d[2] = ey;
        r[0] = (sn * ex + cs * ey) >> 14;
        r[2] = (sn * ey - cs * ex) >> 14;
        if (far < r[2] && strict != 0) {
            rec->bits |= 2 << (p * 2);
        }
        if (D_800EFD70_27.mode == 9 || D_800EFD70_27.mode == 2 || D_800E1898 == 2) {
            if (r[2] >= -799 && r[2] < 1200 && r[0] >= -799 && r[0] < 800) goto visible;
        } else {
            if (r[2] >= -399 && r[2] < 800 && r[0] >= -399 && r[0] < 400) goto visible;
        }
        if (r[2] < 0) {
            goto reject;
        }
        a = r[0];
        if (a < -180) {
            a += 360;
        } else if (a > 180) {
            a -= 360;
        }
        h = r[2];
        h /= 2;
        if (D_800E1898 == 2 && D_800C2384 == 0 && D_800C2388 == 0) {
            h = h * 3 >> 1;
            if (D_800E7978[0] > 0x1500) {
                h = (h * 4) / 3;
            }
        }
        if (h < a) {
            goto reject;
        }
        if (a < -h) {
            goto reject;
        }
        if (r[2] > near && strict != 0) {
            goto hidden;
        }
    visible:
        rec->flag = 1;
        rec->bits |= 1 << (p * 2);
        if (slots != NULL) {
            idx = r[2] * nslots / D_800C9560[D_800E1898 - 1];
            if (idx < 0) {
                idx = 0;
            }
            if (idx >= nslots) {
                idx = nslots - 1;
            }
            while (slots[idx] != -1) {
                idx = (idx + 1) % nslots;
            }
            slots[idx] = i;
        }
        continue;
    reject:
        rec->flag = 0;
    hidden:
        rec->bits &= ~(1 << (p * 2));
    }
}

typedef struct {
    s16 x, y, z;
    u8 pad[6];
    u8 r, g, b;
    u8 pad2;
} V16;

typedef struct {
    u32 count;
    V16 *v;
} Mesh_2;

typedef struct {
    Mesh_2 *mesh;
    s32 minA, minB, maxA, maxB;
    u8 pad[0xC];
} Item_2;

struct Model_2 {
    u32 count;
    Item_2 items[1];
};

void func_80024490(Model_2 *m)
{
    s32 minB = 9999999;
    s32 minA = 9999999;
    s32 maxB = -9999999;
    s32 maxA = -9999999;
    s32 i, off, off2;
    u32 k;
    Item_2 *it;
    u8 *buf;
    u8 *out;
    s32 dx, dy, ix, iy;
    Mesh_2 *mesh;
    V16 *vt;
    u8 c;

    for (i = 0; i < m->count; i++) {
        it = (Item_2 *)((u8 *)m + (i * 0x20 + 4));
        if (it->minA < minA) minA = it->minA;
        if (it->minB < minB) minB = it->minB;
        if (maxA < it->maxA) maxA = it->maxA;
        if (maxB < it->maxB) maxB = it->maxB;
    }
    while (func_80065858() != 0) {
    }
    buf = D_800C29E0;
    out = buf + 0x10000;
    for (i = 0; i < 0x10000; i++) {
        buf[i] = func_8000EFE8();
    }
    for (i = 0; i < 0x10000; i++) {
        out[i] = (buf[i] + buf[i + 0x100] + buf[i - 0x100] + buf[i + 1] + buf[i - 1]) / 5;
    }
    i = 0;
    if (m->count != 0) {
        dx = maxA - minA;
        dy = maxB - minB;
        off2 = 4;
        do {
            it = (Item_2 *)((u8 *)m + off2);
            mesh = it->mesh;
            for (k = 0; k < mesh->count; k++) {
                vt = &mesh->v[k];
                ix = vt->x;
                ix = (ix - minA) * 64 / dx + 5;
                iy = vt->z;
                iy -= minB;
                iy = iy * 64 / dy + 5;
                c = out[(ix << 8) + iy];
                vt->r = c;
                vt->g = c;
                vt->b = c;
            }
            off2 += 0x20;
            i++;
        } while (i < m->count);
    }
}

void func_80024708(void)
{
    u32 i;

    i = 0;
    if (*D_800C9644_2 != 0) {
        do {
            func_800BCB04(D_800E78A0[i]);
            i = i + 1;
        } while (i < (u32)*D_800C9644_2);
    }
    i = 0;
    if (*D_800C9640 != 0) {
        do {
            func_800BCB04(D_800E7828[i]);
            i = i + 1;
        } while (i < (u32)*D_800C9640);
    }
    func_800BCB04((s32)D_800E78A0);
    func_800BCB04((s32)D_800E7828);
    if ((D_800EFD64_3[20] & 2) != 0) {
        func_800A5840();
    }
    func_8002E190();
    func_80028278();
    if (D_800E7714 != 0) {
        func_800BCB04(D_800E7714);
    }
}

void func_8002482C(s32 param_1)
{
    s32 f = (D_800E7718 == 0);
    void *p3 = D_800E7714;

    if (p3 != NULL && param_1 == 0) {
        func_8002400C(D_800C9644, 0, f, p3, D_800E7724);
    } else {
        func_8002400C(D_800C9644, param_1, f, NULL, 0);
    }
    func_8002400C(D_800C9640, param_1, D_800EFDBC != 2, NULL, 0);
    if (D_800C9648 != NULL) {
        func_8002400C(D_800C9648, param_1, 1, NULL, 0);
    }
}

typedef struct {
    u32 f0, f4, f8, fc, f10, f14, f18, f1c;
} Ent_16;

void func_800248EC(u32 *param)
{
    u32 *base = (u32 *)*param;
    Ent_16 *e;
    u32 i;
    u32 *q;
    u32 j;
    u32 n;
    if (*base != 0) {
        i = 0;
        do {
            e = (Ent_16 *)base + i;
            e->f4 = (u32)base + e->f4;
            e->f18 = (u32)base + e->f18;
            q = (u32 *)((volatile Ent_16 *)e)->f4;
            q[1] = (u32)base + q[1];
            q[3] = (u32)base + q[3];
            if (q[5] != 0) {
                q[5] = (u32)base + q[5];
            }

            j = 0;
            if (q[0] != 0) {
                do {
                    j++;
                } while (j < q[0]);
            }
            i++;
        } while (i < *base);
    }
}

typedef struct { u8 pad[0x40]; } Obj40;

typedef struct {
    s32 a, b, c, d;
    u8 pad10[8];
    s32 f18;
    s32 pad1C;
} MEnt;

typedef struct {
    u32 n;
    u32 pad4;
    MEnt e[1];
} Mdl;

typedef struct {
    s32 f0;
    u16 *pal;
} Pal_5;

extern Pal_5 *D_800E781C_2;
extern Mdl *D_800C9640_5;
extern Mdl *D_800C9644_6;

void func_80024980(void)
{
    s32 i, j;
    u32 *p;

    for (i = 0; i < 2; i++) {
        D_800E7798[i] = func_800BC908(0x4B00, 0, D_800D6A50, 0xCA);
        for (j = 0; j < 300; j++) {
            func_80011490(&D_800E7798[i][j], 0, 0, 0, 0, 0, 0, 0x100);
        }
    }
    D_800E7820 = -1;
    D_800C23F0 = 8000;
    func_800BFC40((u8 *)D_800D6A5C, (s32)func_800248EC);
    func_800BFC40((u8 *)D_800D6A60, (s32)func_80031D08);
    D_800E781C_2 = (Pal_5 *)func_8005AED8(D_800D1508);
    p = &D_800E783C;
    *p = func_8005AED8(D_800D1504);
    D_800E7824 = func_8005AED8(D_800D1510);
    D_800E77F8 = func_8005AED8(D_800D1500);
    D_800E77C4 = func_8005AED8(D_800D14FC);
    D_800C9640_5 = (Mdl *)func_8005AED8(D_800D14E0);
    D_800E7808[0] = (u32)D_800E781C_2;
    D_800E7808[1] = D_800E77C4;
    D_800E7808[2] = D_800E77F8;
    D_800E7828 = func_80027574(D_800C9640_5, D_800E7808, 0x4C, 1);
    D_800C9644_6 = (Mdl *)func_8005AED8(D_800D14DC);
    D_800E78A0 = func_80027574(D_800C9644_6, p, 0, 0);
    D_800C9648 = (Mdl *)func_8005AED8(D_800D14E4);
    if (D_800C2280 & 0x80) {
        func_80028330(D_800C9644_6);
        func_80028330(D_800C9640_5);
    }
    D_800E7834 = func_800BC908(D_800C9640_5->n * 4, 0, D_800D6A50, 0xF9);
    for (i = 0; i < D_800C9640_5->n; i++) {
        D_800E7834[i] = D_800C9640_5->e[i].f18;
    }
    func_80066494(D_800C9640_5);
    D_800E77D4 = func_800BC908(D_800C9644_6->n * 16, 0, D_800D6A50, 0x103);
    func_80027B7C();
    for (i = 0; i < 256; i++) {
        if (D_800E781C_2->pal[i] == 0xF83F) {
            D_800E781C_2->pal[i] = 0;
        }
    }
    p = &D_800C9574;
    *p = func_8005AED8(D_800D1518);
    func_80072AD0(D_800C9640_5, *p);
    func_8002DA60(D_800C9644_6, D_800C9640_5, *p, func_8005AED8(D_800D14F4));
    D_800E7814 = func_800BC908(D_800C9644_6->n * 16, 0, D_800D6A50, 0x11D);
    for (i = 0; i < D_800C9644_6->n; i++) {
        D_800E7814[i].x = D_800C9644_6->e[i].a;
        D_800E7814[i].z = D_800C9644_6->e[i].b;
        D_800E7814[i].y = D_800C9644_6->e[i].c;
        D_800E7814[i].w = D_800C9644_6->e[i].d;
    }
    func_800270F8(D_800C9640_5, D_800C9644_6);
    if (D_800C9648 != NULL) {
        func_800270F8(D_800C9648, D_800C9644_6);
    }
    func_80065B10(D_800E7728, func_8005B58C(D_800D1528), 0, D_800C9644_6, D_800E781C_2);
}

void func_80024DA4(void)
{
    u32 i;

    func_80067AC4();
    i = 0;
    func_800BCB04(D_800E7814);
    func_800BCB04(D_800E77D4);
    func_800BCB04(D_800E7834);
    if (*D_800C9644_3 != 0) {
        do {
            func_800BCB04(D_800E78A0[i]);
            i++;
        } while (i < *D_800C9644_3);
    }
    i = 0;
    if (*D_800C9640 != 0) {
        do {
            func_800BCB04(D_800E7828[i]);
            i++;
        } while (i < *D_800C9640);
    }
    func_800BCB04(D_800E78A0);
    func_800BCB04(D_800E7828);
    func_80028278();
    func_800BCB04(D_800E7798_2.a);
    func_800BCB04(D_800E7798_2.b);
    func_800BCB04(D_800C963C);
    func_800BCB04(D_800C9638);
    D_800C963C = 0;
    D_800C9638 = 0;
    func_8002E190();
    func_80072E7C();
    D_800E77F8 = 0;
    D_800E77C4 = 0;
    D_800C9648 = 0;
    D_800E7824 = 0;
    D_800C9644_3 = (u32 *)0;
}

typedef struct {
    u8 pad[0x1C];
    u32 f1C;
} Item_3;

typedef struct {
    u32 count;
    Item_3 items[1];
} List_3;

extern List_3 *D_800C9640_6;
extern List_3 *D_800C9644_7;
extern List_3 *D_800C9648_3;

void func_80024F24(s32 n)
{
    u32 i;
    Item_3 *it;

    func_80027DE0(n, &D_800E7770[n], &D_800E7780[n]);
    func_80026924(D_800C9644_7, n);
    for (i = 0; i < D_800C9640_6->count; i++) {
        it = (Item_3 *)&((u32 *)D_800C9640_6)[i * 8 + 1];
        it->f1C = (it->f1C & 0xFFFF0000) | D_800C9644_7->items[it->f1C >> 16].f1C;
    }
    if (D_800C9648_3 != NULL) {
        for (i = 0; i < D_800C9648_3->count; i++) {
            it = (Item_3 *)&((u32 *)D_800C9648_3)[i * 8 + 1];
            it->f1C = (it->f1C & 0xFFFF0000) | D_800C9644_7->items[it->f1C >> 16].f1C;
        }
    }
    if (D_800E30A8 != D_800E7820) {
        D_800E77D0 = 0;
    }
    D_800E7820 = D_800E30A8;
}

Gfx *func_80025060(Gfx *gdl, s32 mtx, s32 a2, s32 view, s32 fog)
{
    u32 i;
    MtxS *m;
    Item *it;
    u32 sh;

    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPLoadTLUT_pal256(gdl++, D_800E783C_2->seg);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    if (fog == 0) {
        gDPSetRenderMode(gdl++, 0x442230, 0);
    } else {
        gSPSetGeometryMode(gdl++, G_FOG);
        gDPSetFogColor(gdl++, D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 255);
        gSPFogPosition(gdl++, D_800E7770[view], D_800E7780[view]);
        gDPSetRenderMode(gdl++, 0xC8004030, 0);
    }

    for (i = 0; i < D_800C9644->n; i++) {
        it = &D_800C9644->it[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) continue;
        if (it->mask & (0x100 << view)) {
            if (fog == 0) continue;
        } else {
            if (fog != 0) continue;
        }
        {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            D_800E77D4[i * 4 + view] = m;
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, D_800E78A0[i]);
            }
        }
    }
    gDPPipeSync(gdl++);
    if (fog != 0) {
        gdl = func_80066070(gdl, (Mtx *)mtx, a2, view, 1);
        gDPPipeSync(gdl++);
    }
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    if (fog == 0) {
        gSPSegment(gdl++, 6, D_800C95B8);
        gSPSegment(gdl++, 7, D_800C95D8);
    } else {
        gSPSegment(gdl++, 6, D_800C9588);
        gSPSegment(gdl++, 7, D_800C95A0);
    }

    for (i = 0; i < D_800C9640_2->n; i++) {
        if (D_800E7834[i] & 8) continue;
        it = &D_800C9640_2->it[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) continue;
        if (it->mask & (0x100 << view)) {
            if (fog == 0) continue;
        } else {
            if (fog != 0) continue;
        }
        {
            s32 x, z;
            gdl = func_800281F4(gdl, i);
            x = (it->x0 + it->x1) / 2;
            z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                if (D_800E7834[i] & 1) {
                    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
                }
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, D_800E7828[i]);
                if (D_800E7834[i] & 1) {
                    gSPSetGeometryMode(gdl++, D_800E19CC);
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    return gdl;
}

extern SegRef *D_800E781C_3;
extern SegRef *D_800E77C4_2;
extern SegRef *D_800E77F8_2;

Gfx *func_800258F4(Gfx *gdl, s32 mtx, s32 a2, s32 view)
{
    u32 i;
    MtxS *m;
    Item *it;
    u32 sh;
    s32 tmp[18];

    gSPSegment(gdl++, 1, D_800E781C_3->seg);
    if (D_800E77C4_2 != NULL) {
        gSPSegment(gdl++, 2, D_800E77C4_2->seg);
    }
    if (D_800E77F8_2 != NULL) {
        gSPSegment(gdl++, 3, D_800E77F8_2->seg);
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, D_800DC0E0[0], D_800DC0E0[1], D_800DC0E0[2], 255);
    gSPFogPosition(gdl++, D_800E7770[view], D_800E7780[view]);
    gDPSetCombine(gdl++, 0x127E24, 0xFFFFF3F9);
    gDPSetRenderMode(gdl++, 0xC8004030, 0);
    gDPLoadTLUT_pal256(gdl++, D_800E783C_2->seg);
    gSPViewport(gdl++, &D_800E19C0[view + 4]);

    for (i = 0; i < D_800C9644->n; i++) {
        it = &D_800C9644->it[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            D_800E77D4[i * 4 + view] = m;
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, D_800E78A0[i]);
            }
        }
    }
    gDPPipeSync(gdl++);
    gdl = func_80066070(gdl, (Mtx *)mtx, a2, view, 3);
    gDPSetBlendColor(gdl++, 0, 0, 0, 255);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPSegment(gdl++, 6, D_800C9588);
    gSPSegment(gdl++, 7, D_800C95A0);

    for (i = 0; i < D_800C9640_2->n; i++) {
        if (D_800E7834[i] & 8) continue;
        it = &D_800C9640_2->it[i];
        sh = it->mask >> (view * 2);
        if (!(sh & 1)) continue;
        if (sh & 2) {
            s32 x, z;
            gdl = func_800281F4(gdl, i);
            x = (it->x0 + it->x1) / 2;
            z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                if (D_800E7834[i] & 1) {
                    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
                }
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gdl++, D_800E7828[i]);
                if (D_800E7834[i] & 1) {
                    gSPSetGeometryMode(gdl++, D_800E19CC);
                }
            }
        }
    }
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPClearGeometryMode(gdl++, G_FOG);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    return gdl;
}

typedef struct {
    s32 a;
    u32 img;
} TexEnt;

typedef struct {
    s32 pad0, pad4;
    TexEnt e[1];
} TexTab;

extern ItemList *D_800C9648_4;
extern TexTab *D_800E92C8;

Gfx *func_80026120(Gfx *gdl, s32 mtx, s32 a2, s32 view)
{
    s32 i;
    MtxS *m;
    Item *it;
    s32 tmp[18];

    if (D_800C9648_4 == NULL) {
        return gdl;
    }
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetCombine(gdl++, 0x119823, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetAlphaCompare(gdl++, G_AC_NONE);
    gSPFogPosition(gdl++, D_800E7780[view], D_800E7770[view]);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    D_800C8C74 = 1;

    for (i = 0; i < D_800C9648_4->n; i++) {
        it = &D_800C9648_4->it[i];
        if ((it->mask >> (view * 2)) & 1) {
            s32 x = (it->x0 + it->x1) / 2;
            s32 z = (it->z0 + it->z1) / 2;
            m = getmtx(x, 0, z, view);
            if (m != NULL) {
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gdl = func_80013D70(0, D_800C9648_4->it[i].id, D_800E7824_2, gdl);
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    for (i = 0; i < 8; i++) {
        if (D_800E77D8[i] == 0) continue;
        it = &D_800C9644->it[D_800E77A0[i]];
        if ((it->mask >> (view * 2)) & 1) {
            s32 a = D_800E77D8[i] / 2;
            s32 f;
            if (a >= 256) {
                a = 255;
            }
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, a);
            f = (120 - D_800E77D8[i]) / 12;
            if (f >= 5) {
                f = (f - 2) % 3 + 2;
            }
            gDPLoadTextureBlock(gdl++, D_800E92C8->e[f].img, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                                G_TX_MIRROR, G_TX_MIRROR, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
            m = getmtx(D_800E7840[i].x, D_800E7840[i].y, D_800E7840[i].z, view);
            if (m != NULL) {
                func_80011490(m, 0, 0, 0, D_800E7840[i].x * 16 - D_800E8090[view].x, D_800E7840[i].y * 16 - D_800E8090[view].y,
                              D_800E7840[i].z * 16 - D_800E8090[view].z, 0x100);
                gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPVertex(gdl++, D_800C95F8, 4, 0);
                gSP2Triangles(gdl++, 0, 1, 2, 0, 0, 2, 3, 0);
            }
        }
    }
    D_800C8C74 = 0;
    gDPPipeSync(gdl++);
    gSPMatrix(gdl++, mtx + 0x400, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPClearGeometryMode(gdl++, G_SHADING_SMOOTH | G_SHADE);
    gSPClearGeometryMode(gdl++, G_FOG);
    func_80014A68(0);
    return gdl;
}

typedef struct { s32 f0, f4, f8, fC, f10, f14, f18, f1C; } Rec_11;
struct List_4 { u32 count; Rec_11 r[1]; };
typedef struct { u8 pad[0x5AA]; s16 f5AA; u8 pad2[0x7E0 - 0x5AC]; } Player_9;

extern Player_9 D_800E37C0_22[];

void func_80026924(List_4 *l, s32 n)
{
    s32 k;
    s32 i;
    s32 base;
    s32 px, py, pz;
    s32 cnt;
    s32 bit;
    u16 *e;
    s32 n1, tot;
    s32 h;
    s32 lim;
    s32 ang;
    s32 sn, cs;
    s32 d[4];
    s32 w[4];

    k = n;
    if (D_800C9650 != -1) {
        k = D_800C9650;
    }
    D_800C964C = 0x68;
    if (D_800E1898 == 2) {
        D_800C964C = 0x9C;
    }
    if (D_800EFD64_2 - D_800CB210 == 20 && D_800E3D6A == 4) {
        n1 = D_800E15E0[0];
        if (n1 >= 0 && n1 < l->count) {
            s32 ref = l->r[n1].f8 + l->r[n1].f10;
            for (i = 0; i < l->count; i++) {
                s32 v = l->r[i].f1C & (-4 << (n * 2));
                s32 dd = l->r[i].f8 + l->r[i].f10;
                l->r[i].f1C = v;
                if (dd >= ref - 7500 && dd <= ref + 2500) {
                    l->r[i].f1C = v | (1 << (n * 2));
                }
            }
        }
    } else {
        base = 0;
        if (D_800E1898 != 1) {
            base = l->count;
        }
        px = D_800E8090[n].x;
        py = D_800E8090[n].y;
        pz = D_800E8090[n].z;
        cnt = 0;
        if (D_800E37C0_22[n].f5AA == 9) {
            D_800E77FC = D_800E15E0[k];
        } else {
            D_800E8508 = 0;
            func_8002E920(px, py, pz);
            if (D_800E8508 == 0) {
                D_800E77FC = D_800E15E0[k];
                D_800E8508 = 1;
            } else {
                D_800E77FC = D_800E8958;
            }
        }
        bit = 1 << (n * 2);
        for (i = 0; i < l->count; i++) {
            l->r[i].f1C &= ~(3 << (n * 2));
        }
        e = D_800C963C_2[D_800E77FC + base];
        n1 = e[0];
        tot = n1 + e[1];
        for (i = 0; i < tot; i++) {
            l->r[e[i + 2]].f1C |= bit;
        }
        for (i = 0; i < n1; i++) {
            l->r[e[i + 2]].f1C |= 2 << (n * 2);
        }
        px >>= 4;
        pz >>= 4;
        l->r[D_800E77FC].f1C |= bit;
        lim = 0xC48A / (0x3EE - D_800E7770[n]);
        ang = D_800E8000[n] >> 4;
        sn = D_800C2C60[ang & 0xFFF];
        cs = D_800C2C60[(ang - 0x400) & 0xFFF];
        for (i = 0; i < l->count; i++) {
            Rec_11 *r = &l->r[i];
            Quad *q = &D_800E7814[i];
            if (r->f1C & bit) {
                cnt++;
                if (D_800E8508 == 0 && cnt >= 16) {
                    r->f1C &= ~bit;
                    cnt--;
                } else {
                    s32 t, u, f, x, z;
                    x = r->f4;
                    z = r->f8;
                    t = (sn * (z - pz) - cs * (x - px)) >> 14;
                    x = r->fC - x;
                    h = (-cs * x) >> 14;
                    f = (sn * (r->f10 - z)) >> 14;
                    u = t + h;
                    d[0] = t;
                    d[1] = u;
                    d[2] = t + f;
                    d[3] = u + f;
                    if (d[0] < 11 && d[1] < 11 && d[2] < 11 && d[3] < 11) {
                        cnt--;
                        r->f1C &= ~(3 << (n * 2));
                    } else {
                        if (d[0] >= lim || d[1] >= lim || d[2] >= lim || d[3] >= lim) {
                            r->f1C |= 0x100 << n;
                        } else {
                            r->f1C &= ~(0x100 << n);
                        }
                        if (D_800EFD64_2 - D_800CB210 != 20 && d[0] > 300 && d[1] > 300 && d[2] > 300 && d[3] > 300) {
                            s32 y = q->a;
                            s32 u2;
                            t = (sn * (y - px) + cs * (q->c - pz)) >> 14;
                            y = q->b - y;
                            h = (sn * y) >> 14;
                            u2 = t + h;
                            f = (cs * (q->d - q->c)) >> 14;
                            w[0] = (t << 8) / d[0];
                            w[1] = (u2 << 8) / d[1];
                            w[2] = ((t + f) << 8) / d[2];
                            w[3] = ((u2 + f) << 8) / d[3];
                            if ((w[0] < -D_800C964C && w[1] < -D_800C964C && w[2] < -D_800C964C && w[3] < -D_800C964C) ||
                                (D_800C964C < w[0] && D_800C964C < w[1] && D_800C964C < w[2] && D_800C964C < w[3])) {
                                cnt--;
                                r->f1C &= ~(3 << (n * 2));
                            }
                        }
                    }
                }
            }
        }
    }
}

typedef struct {
    s32 x0, z0, x1, z1;
    s32 pad10, pad14;
    s32 idx;
    s32 pad1c;
} Ent_17;

struct Lst {
    u32 n;
    s32 pad;
    Ent_17 e[1];
};

typedef struct {
    s32 x0, x1, z0, z1;
} Box;

extern Box *D_800E7814_2;

void func_800270F8(Lst *a, Lst *b)
{
    Ent_17 *e;
    Ent_17 *f;
    u32 i, k;
    s32 x, z, fx, fz, d, dx, dz;
    s32 best, bd;

    for (i = 0; i < a->n; i++) {
        D_800E8508 = 0;
        x = (a->e[i].x0 + a->e[i].x1) / 2;
        z = (a->e[i].z0 + a->e[i].z1) / 2;
        func_8002E920(x << 4, 999999, z << 4);
        if (D_800E8508 != 0) {
            { s32 n = D_800E8958; if (n) dz = 1; best = n; }
        } else {
            x /= 4;
            z /= 4;
            for (k = 0; k < b->n; k++) {
                fx = (b->e[k].x0 + b->e[k].x1) / 2;
                fz = (b->e[k].z0 + b->e[k].z1) / 2;
                fx /= 4;
                fz /= 4;
                dx = x - fx;
                dz = z - fz;
                { s32 t; t = dx * dx; do { d = dz * dz; } while (0); dz = d; d = t + dz; }
                if (d < bd || k == 0) {
                    bd = d;
                    best = k;
                }
            }
        }
        do {
            a->e[i].idx = best << 16;
        } while (0);
        if (a->e[i].x0 < ((Box *)((best << 4) + (u32)D_800E7814_2))->x0) {
            D_800E7814_2[best].x0 = a->e[i].x0;
        }
        if (a->e[i].z0 < D_800E7814_2[best].z0) {
            D_800E7814_2[best].z0 = a->e[i].z0;
        }
        if (D_800E7814_2[best].x1 < a->e[i].x1) {
            D_800E7814_2[best].x1 = a->e[i].x1;
        }
        if (D_800E7814_2[best].z1 < a->e[i].z1) {
            D_800E7814_2[best].z1 = a->e[i].z1;
        }
    }
}

typedef struct Obj_12 {
    s32 f0;
    u8 *base;
    s32 f8;
    u8 *bytes;
} Obj_12;

typedef struct Ent_18 {
    Obj_12 *obj;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;
    s32 pad[3];
} Ent_18;

typedef struct Tbl_2 {
    u32 n;
    Ent_18 e[1];
} Tbl_2;

extern Tbl_2 *D_800C9648_5;

s32 func_8002733C(s32 x, s32 y, s32 *out)
{
    u32 i;
    s32 k;
    s32 la[4];
    s32 lb[4];
    s32 fx, fy;
    Ent_18 *e;
    u8 *pb;
    u8 *base;

    if (D_800C9648_5 == NULL) {
        return 0;
    }
    x >>= 4;
    y >>= 4;
    for (i = 0; i < D_800C9648_5->n; i++) {
        e = &D_800C9648_5->e[i];
        if (x < e->x0 || e->x1 < x || y < e->y0 || e->y1 < y) {
            continue;
        }
        if (D_800EFD64_3[0xb] == 1) {
            fx = x - (e->x0 + e->x1) / 2;
            fy = y - (e->y0 + e->y1) / 2;
        } else {
            fx = x;
            fy = y;
        }
        for (pb = e->obj->bytes; pb[0] != 0; pb += 4) {
            if ((pb[0] >> 4) == 3) {
                for (k = 0; k < 3; k++) {
                    la[k] = *(s16 *)((u32)(pb[k + 1] << 4) + (u32)base);
                    lb[k] = *(s16 *)((u32)(pb[k + 1] << 4) + (u32)base + 4);
                }
                if (func_80031D54(la, lb, fx, fy)) {
                    *out = *(s16 *)((u32)(pb[1] << 4) + (u32)base + 2) << 4;
                    return 1;
                }
            } else if ((pb[0] >> 4) == 1) {
                base = e->obj->base + (*(u16 *)(pb + 2) << 4);
            }
        }
    }
    return 0;
}

typedef struct {
    u8 pad0[0xC];
    s32 fC;
} Sub_5;

struct Ent_19 {
    u32 n;
    Sub_5 *sub;
    u8 pad8[0x18];
};

s32 func_80013D70_3(u32 a0, Sub_5 *a1, s32 a2, s32 a3);

s32 **func_80027574(Ent_19 *h, s32 p2, u32 p3, s32 p4)
{
    u8 *buf;
    s32 off;
    s32 n;
    u32 i;
    s32 **tbl;
    s32 *t;
    s32 sz;
    s32 *nb;
    s32 j;
    s32 d;

    buf = D_800C29D8[1 - D_800E30A8];
    if (!(*(s32 *)(D_800EFD64 + 0x50) & 2)) {
        i = h->sub->fC - (s32)h;
        n = func_800BD924(h);
        func_800AEC34(buf, (u8 *)h + i, n - i);
        h = func_800BD798(h, i, 0, D_800D6A50, 0x51B);
        do { } while (0);
        sz = 0;
        d = (s32)buf - ((s32)h + i);
        if (h->n != 0) {
            do {
                h[sz].sub->fC += d;
                sz++;
            } while (sz < h->n);
        }
    }
    tbl = func_800BC908(h->n * 4, 0, D_800D6A50, 0x524);
    func_80014A68(p4 ? func_800285C4 : NULL);
    if (p4 && D_800E77F8) {
        func_80027AB4();
        D_800C9584 = func_800BC908((h->n + 1) * 4, 0, D_800D6A50, 0x52E);
        D_800E77C8 = D_800C9580 = (s32 *)(D_800C29E0 + 0x1E000);
    }
    for (i = 0; i < h->n; i++) {
        D_800C8C7C = -1;
        t = func_800BC908(func_80013D70_3(p3 | 1, h[i].sub, p2, D_800C29E0) - D_800C29E0, 0, D_800D6A50, 0x541);
        tbl[i] = t;
        if (t == NULL) {
            for (;;) {
            }
        }
        if (p4 && D_800E77F8) {
            func_80014A74(func_80027934);
            D_800C9584[i] = (s32)D_800E77C8;
            D_800E7790 = 0;
        }
        D_800C8C7C = -1;
        func_80013D70_3(p3 | 1, h[i].sub, p2, (s32)tbl[i]);
        if (p4 && D_800E77F8) {
            D_800E77C8 += D_800E7790;
            func_80014A74(NULL);
        }
    }
    if (p4 && D_800E77F8) {
        D_800C9584[i] = (s32)D_800E77C8;
        i++;
        sz = (s32)D_800E77C8 - (s32)D_800C9580;
        if (sz != 0) {
            sz *= 4;
            nb = func_800BC908(sz, 0, D_800D6A50, 0x565);
            func_800AEC34(nb, D_800C9580, sz);
            for (j = 0; j < (s32)i; j++) {
                D_800C9584[j] = (s32)(nb + ((D_800C9584[j] - (s32)D_800C9580) >> 2));
            }
            D_800C9580 = nb;
        } else {
            D_800C9580 = NULL;
        }
    }
    func_80014A68(NULL);
    return tbl;
}

void func_80027934(u32 *param_1)
{
    s32 *piVar1;
    s32 iVar4;
    s32 iVar5;
    s32 iVar6;
    u8 *pb;
    u32 uVar3;

    uVar3 = *param_1;
    iVar5 = D_800E77F8_3[0] - 1;
    if (((u32)D_800E77F8_3[3] <= uVar3) && (uVar3 <= *(u32 *)((u8 *)D_800E77F8_3 + (iVar5 << 3) + 0xC)) &&
        (-1 < iVar5)) {
        iVar6 = 0;
        piVar1 = D_800E77F8_3;
        while (iVar6 <= iVar5) {
            if ((u32)piVar1[3] == uVar3) {
                break;
            }
            iVar6 = iVar6 + 1;
            piVar1 = piVar1 + 2;
        }
        if ((iVar6 <= iVar5) && (1 < *(s32 *)(D_800E782C + iVar6 * 4))) {
            iVar5 = 0;
            while (iVar5 < D_800E7790) {
                if (D_800E77C8_2[iVar5 * 4 + 2] + D_800E77C8_2[iVar5 * 4] == iVar6) {
                    break;
                }
                iVar5 = iVar5 + 1;
            }
            if (iVar5 == D_800E7790) {
                if (iVar5 <= 7) goto store;
                do {
                } while (1);
store:
                D_800E77C8_2[D_800E7790 * 4] = D_800E7830[iVar6 * 4 + 3];
                D_800E77C8_2[D_800E7790 * 4 + 1] = D_800E782C[iVar6 * 4 + 3];
                D_800E77C8_2[D_800E7790 * 4 + 2] = (u8)(iVar6 - D_800E7830[iVar6 * 4 + 3]);
                D_800E7790 = D_800E7790 + 1;
            }
            *param_1 = (u32)((iVar5 + 8) << 24);
        }
    }
}

typedef struct { u8 pad[2]; u8 flags; u8 pad2[5]; } Ent_20;
typedef struct { u32 count; u32 pad; Ent_20 e[1]; } Tbl_3;

extern Tbl_3 *D_800E77F8_4;

void func_80027AB4(void)
{
    s32 i, start, k;
    Tbl_3 *t;

    D_800E7830_2 = (s32 *)(D_800C29E0 + 0x18000);
    D_800E782C_2 = D_800E7830_2 + D_800E77F8_4->count;
    start = 0;
    for (i = 0; i < D_800E77F8_4->count; i++) {
        if (!(D_800E77F8_4->e[i].flags & 0x34)) {
            for (k = start; k <= i; k++) {
                D_800E7830_2[k] = start;
                D_800E782C_2[k] = i - start + 1;
            }
            start = i + 1;
        }
    }
}

void func_80027B7C(void)
{
    s32 idx;
    s32 i;
    u32 j;
    s32 n;
    s32 a, b;
    u8 *dst;
    s32 *offs;
    s32 sz;
    s32 *q;
    u16 *p;
    s32 t1, t2;

    while (func_80065858() != 0) {
    }
    D_800C9638_2 = (u16 *)func_8005AED8(D_800D1514);
    if (*D_800C9644_3 != *D_800C9638_2) {
        func_8000ECB8(1);
    }
    idx = 1;
    dst = D_800C29E0;
    offs = (s32 *)(D_800C29E0 + 0x1E000);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < *D_800C9644_3; j++) {
            a = D_800C9638_2[idx];
            b = D_800C9638_2[idx + 1];
            if (i == 0 || i == D_800EFD70_4[15] - 1) {
                n = a + b;
                *offs++ = dst - D_800C29E0;
                func_800AEC34(dst, (u16 *)((idx << 1) + (s32)D_800C9638_2), (n + 2) * 2);
                dst += n * 2 + 4;
            }
            t1 = idx + 2;
            t2 = a + b;
            idx = t1 + t2;
        }
    }
    idx = 0;
    func_800BFEA4((u32 *)(D_800E7728_2[1] + func_8005B58C(D_800D1514) * 4));
    i = dst - D_800C29E0;
    D_800C9638_2 = func_800BC908(i, idx, D_800D6A50, 0x61B);
    func_800AEC34(D_800C9638_2, D_800C29E0, i);
    i = (s32)offs + (-0x1E000 - (s32)D_800C29E0);
    D_800C963C = func_800BC908(i, idx, D_800D6A50, 0x620);
    func_800AEC34(D_800C963C, D_800C29E0 + 0x1E000, i);
    p = D_800C9638_2;
    q = D_800C963C;
    for (; idx < i / 4; idx++) {
        *q++ += (s32)p;
    }
}

typedef struct { u8 pad[0x628]; s16 f628; u8 rest[0x7E0 - 0x62A]; } Rec7E0;
typedef struct { u8 pad[0x46]; s16 next; u8 pad2[0x60 - 0x48]; u32 bits; u8 rest[0x6C - 0x64]; } Node_2;
typedef struct { u8 pad[0x58]; u16 a[4]; u16 b[4]; u8 rest[0xA0 - 0x68]; } Trk_7;

extern Rec7E0 D_800E37C0_23[];
extern Trk_7 *D_800EFD64_19;
extern Trk_7 D_800CB210_8[];

void func_80027DE0(s32 a, s32 *b, s32 *c)
{
    f32 v[3];
    s32 n;
    f32 t, fa, fb, fx, fy;
    s32 i, sh, sh4;
    s32 a0, a1, b0, b1;
    u32 w, w2;
    Node_2 *nd, *base;
    s32 x, y, j, g;

    v[0] = (f32)D_800E8090[a].x / 1000.0f;
    v[1] = (f32)D_800E8090[a].y / 1000.0f;
    v[2] = (f32)D_800E8090[a].z / 1000.0f;
    func_8004AB24(v, &n, &t, D_800E37C0_23[D_800C9654[a]].f628);
    base = D_800EE498;
    g = D_800E1898;
    i = g - 1;
    nd = (Node_2 *)(n * sizeof(Node_2) + (u32)base);
    j = nd->next;
    if (i > 2) {
        i = 2;
    }
    i *= 8;
    sh4 = i + 4;
    w = nd->bits;
    a0 = (w >> sh4) & 0xF;
    if (a0 >= 8) a0 -= 16;
    a1 = (w >> i) & 0xF;
    if (a1 >= 8) a1 -= 16;
    w2 = ((Node_2 *)(j * sizeof(Node_2) + (u32)base))->bits;
    b0 = (w2 >> sh4) & 0xF;
    if (b0 >= 8) b0 -= 16;
    b1 = (w2 >> i) & 0xF;
    if (b1 >= 8) b1 -= 16;
    b0 -= a0;
    b0 *= 16;
    b1 -= a1;
    b1 *= 2;
    fa = (f32)b0 * t;
    fb = (f32)b1 * t;
    a0 *= 16;
    a1 *= 2;
    fx = (f32)a0 + fa;
    fy = (f32)a1 + fb;
    x = (s32)fx;
    y = (s32)fy;
    x = D_800EFD64_19->a[g - 1] - x;
    y = D_800EFD64_19->b[g - 1] - y;
    if (y < x) {
        x = y;
    }
    if (D_800EFD64_19 - D_800CB210_8 == 8 && g == 1) {
        x = 1000;
        y = 0x41A;
    }
    *b = x;
    *c = y;
}

/* G_MOVEWORD 0xDB, index 6 (segment table), offset seg*4 */
void func_8002803C(Gfx *gdl, s32 a, s32 b, s32 c) {
    Gfx *q;

    gDma1p(gdl++, 0xDB, D_800E781C_4[1], 0x4, 0x6);
    if (D_800E77C4_3 != 0) {
        gDma1p(gdl++, 0xDB, D_800E77C4_3[1], 0x8, 0x6);
    }
    if (D_800E77F8_5 != 0) {
        gDma1p(gdl++, 0xDB, D_800E77F8_5[1], 0xC, 0x6);
    }
    q = func_80025060(gdl, a, b, c, 0);
    func_80025060(q, a, b, c, 1);
}

short *func_8002811C(s32 p1, s32 p2, s32 p3, s32 p4)
{
    short *r;
    s32 n;
    s32 t;

    n = D_800E77D0;
    if (n >= 300) {
        return 0;
    }
    p2 = p2 * 0x10 - D_800E8090_4[p1].a;
    p3 = p3 * 0x10 - D_800E8090_4[p1].b;
    p4 = p4 * 0x10 - D_800E8090_4[p1].c;
    t = n * 0x40;
    r = (short *)(D_800E7798_3[D_800E30A8] + t);
    func_80011490(r, 0, 0, 0, p2, p3, p4, 0x100);
    D_800E77D0 = D_800E77D0 + 1;
    return r;
}

typedef struct {
    u32 size;
    void *addr;
} Ent_21;

typedef struct {
    u32 h0;
    u32 h1;
    Ent_21 e[1];
} Bank;

extern Bank *D_800E77F8_6;

Gfx *func_800281F4(Gfx *gdl, s32 idx)
{
    if (D_800E77F8_6 != NULL && D_800C9584 != NULL) {
        u8 *cur = D_800C9584[idx];
        u8 *end = D_800C9584[idx + 1];
        s32 i;

        for (i = 0; cur != end; cur += 4, i++) {
            gSPSegment(gdl++, i + 8, D_800E77F8_6->e[cur[2] + cur[0]].addr);
        }
    }
    return gdl;
}

void func_80028278(void)
{
    if (D_800C9580 != 0) {
        func_800BCB04(D_800C9580);
        D_800C9580 = 0;
    }
    if (D_800C9584 != 0) {
        func_800BCB04(D_800C9584);
        D_800C9584 = 0;
    }
}

void func_800282C8(void)
{
    u32 *p = D_800C9640;
    u32 *c;
    u32 i = 0;
    u32 *q;
    u32 m;

    if (*p != 0) {
        m = 0xfffffff7;
        c = p;
        q = D_800E7834;
        do {
            i++;
            *q = *q & m;
            q++;
        } while (i < *c);
    }
    func_800679C0();
    func_80072EF0();
}

typedef struct { u32 a, b, c; u32 val; } Rec_12;
typedef struct { u32 count; Rec_12 *recs; } Sub_6;
struct Ent_22 { u32 count; Sub_6 *sub; u32 pad[6]; };

void func_80028330(Ent_22 *p)
{
    u32 i, j;
    for (i = 0; i < p->count; i++) {
        for (j = 0; j < p[i].sub->count; j++) {
            u32 v = p[i].sub->recs[j].val;
            v = (v & 0xFF) | ((v >> 16) & 0xFF00) | ((v << 8) & 0xFFFF0000);
            p[i].sub->recs[j].val = v;
        }
    }
}

void func_800283C0(void)
{
    volatile s32 pad[10];
    s32 v = D_800E76D0 / 2;
    if (v == D_800E78AC) return;
    D_800E78AC = v;
    if (D_800C2358 != 0 && D_800E1B70 == 0x100) return;
    if (D_800E77F8 != 0) {
        func_800147C4(D_800E77F8, v);
    }
    if (D_800E7824 != 0) {
        D_800C8C74 = 1;
        func_800147C4(D_800E7824, D_800E78AC);
        D_800C8C74 = 0;
    }
}

typedef struct {
    u8 pad0[0x4F8];
    s32 f4F8;
    u8 rest[0x7E0 - 0x4F8 - 4];
} Rec37;

extern Rec37 D_800E37C0_24[];

void func_8002845C(s32 param_1, s32 param_2, s32 param_3, s32 param_4)
{
    if (D_800E77D8[param_1] == 0 && D_800E37C0_24[param_1].f4F8 >= 0) {
        D_800E8508 = 0;
        func_8002E920(param_2, param_3, param_4);
        if (D_800E8508 != 0) D_800E77A0[param_1] = D_800E8958;
        else D_800E77A0[param_1] = D_800E15E0[param_1];
        D_800E77D8[param_1] = 0x78;
        D_800E7840_2[param_1].a = param_2 >> 4;
        D_800E7840_2[param_1].b = param_3 >> 4;
        D_800E7840_2[param_1].c = param_4 >> 4;
        func_80010994(6, param_1, 0xff);
    }
}

void func_80028590(void)
{
    s32 i;
    s32 *p;

    i = 0;
    p = D_800E77D8;
    do {
        i = i + 1;
        if (*p != 0) {
            *p = *p - 1;
        }
        p = p + 1;
    } while (i < 8);
}

Gfx *func_800285C4(Gfx *gdl, s32 flag)
{
    if (flag) {
        gSPDisplayList(gdl++, 0x07000000);
    } else {
        gSPDisplayList(gdl++, 0x06000000);
    }
    return gdl;
}
