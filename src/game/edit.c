#include "game.h"

#define CUR (*(s32 *)(D_800CEA40 + 0x16FC))

typedef struct {
    s32 x, y;
    u32 z;
} V3_3;

typedef struct { s16 f0; u8 pad[0x22]; } Ent24_2;
typedef struct { u8 pad0[6]; s16 f6; u8 pad8[8]; s16 f10; } Cfg_12;
typedef struct { u8 pad[0xC]; u32 flags; } Obj_2;

typedef struct {
    u8 pad0[6];
    s16 f6;
    u8 pad8[4];
    s32 fC;
    s16 f10;
} Menu_6;

typedef struct Node_24 {
    s32 f0;
    struct Node_24 *prev;
    struct Node_24 *next;
    s32 fC;
    u16 f10;
} Node_24;

typedef struct {
    s16 w, h;
} Dim;

struct Game_36 {
    u8 pad0[0x16C];
    s32 f16C;
    s32 f170;
    s32 f174;
    s32 f178;
    Node_24 *f17C;
    Node_24 *f180;
    Node_24 *f184;
    u8 pad188[8];
    s32 f190;
    Ent24_2 slots[150];
    Dim *f16AC;
    u8 pad16B0[6];
    s16 f16B6;
    s16 f16B8;
    char name[11];
    u8 f16C5[0xB];
    s32 f16D0, f16D4, f16D8, f16DC;
    Menu_6 *f16E0;
    Menu_6 *f16E4;
    u32 f16E8;
    u8 pad16EC[8];
    s32 f16F4;
    u8 pad16F8[0x2C];
    s32 f1724;
    s32 f1728;
    u8 pad172C[0xCC];
    s32 f17F8;
    s16 f17FC;
    s16 f17FE;
    s16 f1800;
};

typedef struct Ctl_23 {
    u8 pad0[0xC];
    u32 fC;
    s16 h10;
} Ctl_23;

typedef struct Ent_4 {
    s16 dim[2];
    s16 dir[2];
    s16 off[2];
    union {
        u32 w;
        struct {
            u16 hi;
            u16 lo;
        } h;
    } fC;
    u8 pad10[0x40 - 0x10];
} Ent_4;

typedef struct Node_25 {
    s16 used;
    s16 pad2;
    struct Node_25 *next;
    struct Node_25 *prev;
    s16 x;
    s16 y;
    u16 st;
    s16 idx;
    Ent_4 *pt;
    u8 pad18[8];
    s16 w;
    s16 h;
} Node_25;

typedef struct Edit {
    u8 pad0[0x16C];
    s32 f16C;
    s32 mode;
    u32 flags;
    u32 pad178;
    Node_25 *head;
    Node_25 *tail;
    Node_25 *cur;
    Node_25 *f188;
    Node_25 *f18C;
    Node_25 *f190;
    u8 pad194[0x16AC - 0x194];
    s16 *size;
    s16 f16B0;
    s16 f16B2;
    u8 pad16B4[0x16E0 - 0x16B4];
    Ctl_23 *f16E0;
    Ctl_23 *f16E4;
    u32 f16E8;
    u8 pad16EC[0x17B0 - 0x16EC];
    Ent_4 *f17B0;
} Edit;

typedef struct { s32 pad[4]; s32 x, y, w, h; } Scis;

typedef struct Sub_2 {
    u8 pad0[0x8];
    s32 f8;
    s16 c;
    s16 e;
    u8 pad1[0x2];
    s16 cur;
    u8 pad2[0x4];
    s16 arr[2];
} Sub_2;

typedef struct {
    u8 pad0[0x8];
    s32 f8;
    s16 x;
    s16 y;
    s16 pad10;
    s16 f12;
    u8 pad14[4];
    s16 sel[2];
} Pl;

typedef struct {
    u8 pad0[0x184];
    Pl *pl;
    u8 pad188[0x16F8 - 0x188];
    s32 f16F8;
    s32 idx;
    u8 pad1700[0x1738 - 0x1700];
    s32 f1738;
    u8 pad173C[0x1758 - 0x173C];
    s32 f1758;
    s32 f175C;
} Game_3;

typedef struct {
    s16 d[2];
    u8 pad4[0x2C];
    s32 f30;
    s32 f34;
    u8 pad38[8];
} Rec_5;

typedef struct { s16 f0; s16 f2; u8 rest[0x3C]; } Tab64;

typedef struct {
    s16 f0;
    s16 cnt;
    u8 pad[60];
} Trk_3;

extern V3_3 D_800CE234;
extern V3_3 D_800CE228;
extern V3_3 D_800CE21C;
extern V3_3 D_800CE210;
extern Cfg_12 D_800CEB08;
extern Edit *D_800CEA40_11;
extern Ent_4 D_800CE300[];
extern Ent_4 D_800CE400[];
extern Rec_5 D_800CE700[];
extern Scis D_800CE9C0_2;
extern Game_3 *D_800CEA40_2;
extern Rec_5 D_800CE280_2[];
extern Tab64 D_800CE280_3[];
extern Trk_3 D_800CE280_4[];

static inline void sel(void) {
    Node_25 *n;

    if (D_800CEA40_11->f190 == NULL) {
        n = D_800CEA40_11->head;
        while (n != NULL) {
            if (n->pt->fC.h.hi & 2) {
                D_800CEA40_11->f190 = n;
                break;
            }
            n = n->next;
            if (n == D_800CEA40_11->head) {
                break;
            }
        }
    }
}

s32 func_80090560(void)
{
    D_800CE228.x += func_8009EDCC(1, D_800CE234.x, 0x23, D_800F56A8);
    D_800CE228.y += func_8009EDCC(1, D_800CE234.y, 0x23, D_800F56A8);
    D_800CE228.z += func_8009EDCC(2, D_800CE234.z, 0x23, D_800F56A8);
    D_800CE21C.x = D_800CE228.x >> 10;
    D_800CE21C.y = D_800CE228.y >> 10;
    D_800CE21C.z = (u16)(D_800CE228.z >> 10);
    D_800F56A8++;
    if (D_800F56A8 == 0x23) {
        D_800CE240 = 0;
        D_800CE21C.x = D_800CE210.x;
        D_800CE21C.y = D_800CE210.y;
        D_800CE21C.z = ((u32)(D_800CE21C.z + 0x800) >> 13) << 13;
    }
}

typedef struct Obj_68 {
    s32 pad0;
    struct Obj_68 *p4;
    struct Obj_68 *p8;
    s16 hC;
    s16 hE;
    u16 flags;
    s16 h12;
    s16 *p14;
} Obj_68;

typedef struct {
    u8 pad0[0x170];
    s32 mode;
    s32 flags;
    u8 pad178[0xC];
    Obj_68 *obj;
    u8 pad188[0x16AC - 0x188];
    s16 *p16AC;
} Ctx_3;

extern Ctx_3 *D_800CEA40_7;

void func_80090670(void)
{
    Obj_68 *obj;
    Obj_68 *o;
    s32 a;
    V3_3 *p;
    s32 fl;

    obj = D_800CEA40_7->obj;
    if (obj != NULL && (obj->flags & 4) && (obj->p4 != NULL || obj->p8 != NULL) && D_800CEA40_7->mode != 6) {
        o = obj->p4;
        if (o == NULL) {
            o = obj->p8;
        }
    } else {
        o = D_800CEA40_7->obj;
    }
    if (o != NULL && (D_800CEA40_7->mode == 6 || !(o->flags & 4))) {
        a = o->p14[7];
        a -= o->h12 << 1;
        if (fl = o->flags & 2, p = &D_800CE210, fl) {
            a += 4;
        }
        if (D_800CEA40_7->flags & 1) {
            a += 4;
        }
        p->z = a & 7;
    } else {
        D_800CE210.z = (D_800CEA40_7->flags & 1) << 2;
    }
    D_800CE210.z = (D_800CE210.z - (D_800CE21C.z >> 13)) & 7;
    switch (D_800CE210.z) {
    case 5:
        D_800CE210.z = -3;
        break;
    case 6:
        D_800CE210.z = -2;
        break;
    case 7:
        D_800CE210.z = -1;
        break;
    }
    if (D_800CEA40_7->flags & 2) {
        D_800CE210.z -= 4;
        D_800CEA40_7->flags ^= 3;
    }
    if (D_800CEA40_7->flags & 4) {
        D_800CE210.z += 4;
        D_800CEA40_7->flags ^= 5;
    }
    if (D_800CEA40_7->obj != NULL) {
        D_800CE210.x = o->hC * 8000 + *(s16 *)((u8 *)o->p14 + ((o->h12 & 1) << 1)) * 4000 - D_800CEA40_7->p16AC[0] * 4000;
        D_800CE210.y = o->hE * 8000 + *(s16 *)((u8 *)o->p14 + (((o->h12 + 1) & 1) << 1)) * 4000 - D_800CEA40_7->p16AC[1] * 4000;
    } else {
        D_800CE210.x = -4000;
        D_800CE210.y = -4000;
    }
    if (D_800CE210.x != D_800CE21C.x || D_800CE210.y != D_800CE21C.y || D_800CE210.z != 0) {
        D_800CE240 = 1;
        D_800F56A8 = 0;
        D_800CE234.x = D_800CE210.x - D_800CE21C.x;
        D_800CE234.y = D_800CE210.y - D_800CE21C.y;
        D_800CE234.z = D_800CE210.z << 13;
        D_800CE228.x = D_800CE21C.x << 10;
        D_800CE228.y = D_800CE21C.y << 10;
        D_800CE228.z = D_800CE21C.z << 10;
    }
}

typedef struct { s16 a, b; s16 pad4[2]; s16 c, d; s16 padC[2]; } Vw;

typedef struct State_11 {
    u8 pad0[0x16C];
    s32 f16C;
    s32 f170;
    s32 f174;
    s32 f178;
    s32 f17C;
    s32 f180;
    s32 f184;
    s32 f188;
    s32 f18C;
    s32 f190;
    Ent24_2 e[150];
    s16 * f16AC;
    u8 pad16B0[0x6];
    s16 f16B6;
    s16 f16B8;
    char name[7];
    u8 pad16C1[0x1F];
    void * f16E0;
    u8 pad16E4[0x4];
    s32 f16E8;
    s32 f16EC;
    u8 pad16F0[0x4];
    s32 f16F4;
    u8 pad16F8[0x4];
    s32 f16FC;
    u8 pad1700[0xC];
    s32 f170C;
    s32 f1710;
    s32 f1714;
    s32 f1718;
    s32 f171C;
    s32 f1720;
    s32 f1724;
    s32 f1728;
    s32 f172C;
    u8 pad1730[0x8];
    s32 f1738;
    u8 pad173C[0x10];
    void * f174C;
    void * f1750;
    u8 pad1754[0x4C];
    s16 f17A0;
    s16 f17A2;
    s16 f17A4;
    u8 pad17A6[0x2];
    s16 f17A8;
    s16 f17AA;
    s16 f17AC;
    u8 pad17AE[0x2];
    void * f17B0;
    u8 pad17B4[0x4];
    Vw vw[4];
    s32 f17F8;
    u8 pad17FC[0x8];
    s32 f1804;
    s32 f1808;
    u8 pad180C[0x4];
} State_11;

typedef struct { s16 c[8]; } Row_5;
typedef struct { u32 n; Row_5 *v; } VtxList;
typedef struct { VtxList *vl; u8 pad[0x1C]; } MEnt_2;
typedef struct { u32 n; MEnt_2 e[1]; } Model_8;

typedef struct {
    u8 pad0[0xC];
    u16 flags;
    u8 padE[0x12];
    s32 *f20;
    s32 *f24;
    u8 pad28[8];
    Model_8 *f30;
    Model_8 *f34;
    u8 pad38[8];
} Obj_69;

typedef struct { s32 f0; u8 pad4[0x3E4]; void *f3E8; } Glob70;
typedef struct { s32 f0, f4, f8; } Tri3_4;

extern State_11 *D_800CEA40_8;
extern Glob70 D_800EFD70_87;
extern Obj_69 D_800CE280_5[];
extern Tri3_4 D_800CE210_2;
extern Tri3_4 D_800CE21C_2;

void func_80090A04(void)
{
    s32 i, j;
    u32 k, n;
    Obj_69 *o;
    VtxList *vl;
    s32 v;
    s32 c, q, x, y, w, f, d;

    D_800EFD64 = D_800CBF30;
    if (D_800CEA40_8 == NULL || !(D_800CEA40_8->f174 & 0x6000)) {
        D_800CEA40_8 = func_800BC908(0x1810, 0, D_800DAB1C, 0x50A);
        func_8000ED10((u8 *)D_800CEA40_8, 0, 0x1810);
    } else {
        D_800EFD70_87.f0 &= ~2;
        D_800CEA40_8->f174 &= ~0x10;
    }
    func_80087DD8();
    func_80014A80();
    if (func_800BF530(D_800DAB24, &D_800E7728) != 0) {
        func_8000ECB8(1);
    }
    func_800482D0(D_800E7728);
    D_800CEA40_8->f17C = 0;
    D_800CEA40_8->f180 = 0;
    D_800CEA40_8->f184 = 0;
    D_800CEA40_8->f190 = 0;
    D_800CEA40_8->f16B6 = 0;
    D_800CEA40_8->f16B8 = 0;
    for (c = 149; c >= 0; c--) {
        D_800CEA40_8->e[c].f0 = 0;
    }
    strcpy(D_800CEA40_8->name, "<NONE>");
    func_800BFC40(D_800DAB30, D_800248EC);
    func_800BFC40(D_800DAB34, D_80014D20);
    func_800BF530(D_800DAB38, &D_800CEA40_8->f1720);
    D_800CEA40_8->f1710 = func_800BF79C(D_800CEA40_8->f1720, D_800D125C);
    D_800CEA40_8->f1714 = func_800BF79C(D_800CEA40_8->f1720, D_800D1368);
    D_800CEA40_8->f1724 = func_800BF79C(D_800CEA40_8->f1720, D_800D1370);
    D_800CEA40_8->f1728 = func_800BF79C(D_800CEA40_8->f1720, *D_800CE270[0]);
    D_800CEA40_8->f170C = func_800BF79C(D_800CEA40_8->f1720, D_800D1274);
    if (!(D_800CEA40_8->f174 & 0x6000)) {
        if (D_800C9570 != NULL) {
            func_800BCB04(D_800C9570);
            D_800C9570 = NULL;
        }
    }
    for (q = 0; q < 4; q++) {
        D_800CEA40_8->vw[q].a = D_800CE9C0[q].w * 2;
        D_800CEA40_8->vw[q].b = D_800CE9C0[q].h * 2;
        D_800CEA40_8->vw[q].c = (D_800CE9C0[q].x + D_800CE9C0[q].w / 2) * 4;
        D_800CEA40_8->vw[q].d = (D_800CE9C0[q].y + D_800CE9C0[q].h / 2) * 4;
    }
    D_800CEA40_8->f17A0 = 1000;
    D_800CEA40_8->f17A2 = 320;
    D_800CEA40_8->f17A4 = 0x1FF;
    D_800CEA40_8->f17A8 = (D_800F13EC / 2) * 4;
    D_800CEA40_8->f17AA = (D_800F13F0 / 2) * 4;
    D_800CEA40_8->f17AC = 0x1FF;
    if (!(D_800CEA40_8->f174 & 0x6000)) {
        D_800CEA40_8->f16E8 = 1;
        D_800CEA40_8->f16EC = 0;
        D_800CEA40_8->f16E0 = &D_800CEB08;
        D_800CEA40_8->f170 = 0;
    }
    D_800CEA40_8->f16AC = D_800CE26C;
    D_800CEA40_8->f184 = 0;
    D_800CEA40_8->f17B0 = D_800CE280_5;
    D_800CEA40_8->f188 = 0;
    D_800CEA40_8->f18C = 0;
    D_800CEA40_8->f190 = 0;
    D_800CEA40_8->f17C = D_800CEA40_8->f184;
    D_800CEA40_8->f180 = D_800CEA40_8->f184;
    D_800CEA40_8->f1718 = func_800C1E60(0, 0x40, 2, 2, 0x21);
    D_800CEA40_8->f171C = func_800C1E60(0, 0x20, 2, 2, 0x21);
    func_80096B3C(D_800CEA40_8->f184);
    D_800CEA40_8->f16C = 0;
    for (i = 0; i < 29; i++) {
        D_800CE280_5[i].flags |= 0x10;
        if (D_800CE280_5[i].flags & 0x100) {
            D_800CE280_5[i].flags |= 0x20;
        } else {
            D_800CE280_5[i].flags &= ~0x20;
        }
        if (D_800CE280_5[i].f20 == NULL) continue;
        D_800CE280_5[i].f30 = (Model_8 *)func_800BF79C(D_800CEA40_8->f1720, *D_800CE280_5[i].f20);
        if (D_800CE280_5[i].f24 != NULL) {
            D_800CE280_5[i].f34 = (Model_8 *)func_800BF79C(D_800CEA40_8->f1720, *D_800CE280_5[i].f24);
        } else {
            D_800CE280_5[i].f34 = NULL;
        }
        for (k = 0; k < D_800CE280_5[i].f30->n; k++) {
            vl = D_800CE280_5[i].f30->e[k].vl;
            for (n = 0; n < vl->n; n++) {
                for (j = 0; j < 3; j += 2) {
                    v = (s16)vl->v[n].c[j];
                    if (v < 0) { s32 w2 = -v; s32 d2 = w2 / 50; s32 f2 = w2 - d2 * 50 >= 15; v = d2 + f2; v = -(v * 50); }
                    else { d = v / 50; f = v - d * 50 >= 15; v = d + f; v = v * 50; }
                    vl->v[n].c[j] = v;
                }
            }
        }
    }
    func_8008CF94(D_800CEA40_8->f16AC[0], D_800CEA40_8->f16AC[1]);
    for (y = 0; y < D_800CEA40_8->f16AC[1]; y++) {
        for (x = 0; x < D_800CEA40_8->f16AC[0]; x++) {
            func_8008D3B4(x, y, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    func_8008EF84(D_800CE9C0[1].x, D_800CE9C0[1].y, D_800CE9C0[1].w - 1, D_800CE9C0[1].h - 1);
    D_800CEA40_8->f172C = 90000;
    D_800CEA40_8->f1738 = 45000;
    D_800CE210_2.f8 = 0;
    D_800CE21C_2.f8 = 0;
    {
        s32 fl = D_800CEA40_8->f174;
        s32 m6 = fl & 0x6000;
        if (m6 != 0) {
            D_800CEA40_8->f174 = m6;
            if (fl & 0x2000) {
                D_800CEB08.f10 = 2;
                D_800CEB08.f6 = 2;
            } else {
                D_800CEB08.f10 = 3;
                D_800CEB08.f6 = 3;
            }
        } else {
            D_800CEB08.f10 = 1;
            D_800CEB08.f6 = 1;
            D_800CEA40_8->f174 = 0;
        }
    }
    func_8009EBF4();
    func_8009ECE0();
    D_800CEA40_8->f16F4 = 8;
    D_800CEA40_8->f174C = func_800BC908(0xF00, 0, D_800DAB1C, 0x5E1);
    D_800CEA40_8->f1750 = func_800BC908(0xF00, 0, D_800DAB1C, 0x5E2);
    D_800CEA40_8->f16FC = 0;
    func_8009C618();
    func_8009EBA8(0);
    D_800CEA40_8->f16F4 = 8;
    func_8008F0B0();
    D_800CEA40_8->f1804 = func_800BF79C(D_800F47C0, D_800D13E0);
    D_800CEA40_8->f1808 = func_800BF79C(D_800F47C0, D_800D142C);
    if (D_800EFD70_87.f3E8 != NULL) {
        D_800CEA40_8->f174 &= ~0x6000;
        func_800A4700((u8 *)D_800EFD70_87.f3E8 + 4);
        func_800BCB04(D_800EFD70_87.f3E8);
        D_800EFD70_87.f3E8 = NULL;
        func_80094738(D_800CEA40_8->f180);
    }
    func_80090670();
    D_800CE21C_2.f0 = D_800CE210_2.f0;
    D_800CE21C_2.f4 = D_800CE210_2.f4;
    D_800CE21C_2.f8 = ((u32)(D_800CE21C_2.f8 + 0x800) >> 13) << 13;
    D_800CE240 = 0;
    func_8008E76C(D_800CEA40_8->f17F8, D_800CEA40_8->f1724, D_800CEA40_8->f1728);
    osViSetSpecialFeatures(0xAA);
}

typedef struct { u8 pad[0x3E8]; s32 *p; } FB;

extern FB D_800EFD70_88;

void func_80091270(void)
{
    s32 iVar7;
    s32 n;

    if ((*(s32 *)(D_800CEA40 + 0x174) & 0x400) != 0) {
        iVar7 = *D_800C9570_2;
    } else {
        iVar7 = -1;
    }
    func_800BFDD4(D_800E7728);
    func_80014AD0();
    func_8008ED48();
    func_800BCB04((void *)*(s32 *)(D_800CEA40 + 0x174C));
    func_800BCB04((void *)*(s32 *)(D_800CEA40 + 0x1750));
    func_800BFDD4(*(s32 **)(D_800CEA40 + 0x1720));
    func_800BCB04((void *)*(s32 *)(D_800CEA40 + 0x171C));
    func_800BCB04((void *)*(s32 *)(D_800CEA40 + 0x1718));
    func_800A5964(1);
    if ((*(s32 *)(D_800CEA40 + 0x174) & 0x6000) == 0) {
        D_800CA920 = 6;
        func_800BCB04(D_800CEA40);
        D_800CEA40 = 0;
    }
    func_80087E00();
    func_80090484();
    osViSetSpecialFeatures(0xAA);
    if (iVar7 >= 0) {
        __builtin_memcpy(D_800C29E0, D_800F0158, 0x400);
        n = (iVar7 - 1) * 0xc + 0x110;
        memcpy(D_800C29E0 + 0x400, D_800C9570_2, n);
        func_800BCB04(D_800C9570_2);
        func_800BCB04(D_800EFD70_88.p);
        D_800C9570_2 = func_800BC908(n, 0, D_800DAB1C, 0x651);
        D_800EFD70_88.p = func_800BC908(0x400, 0, D_800DAB1C, 0x652);
        memcpy(D_800C9570_2, D_800C29E0 + 0x400, n);
        __builtin_memcpy(D_800EFD70_88.p, D_800C29E0, 0x400);
    }
    func_8004834C();
}

#define PRESSED(b) ((D_800E30B0_3[0] & (b)) && !(D_800E25B0 & (b)))

typedef struct { u8 c[11]; } Name11;

typedef struct {
    u8 pad0[0x170];
    s32 f170;
    u8 pad174[0x16BA - 0x174];
    Name11 name;
    u8 buf[11];
    s32 sel;
    s32 x;
    s32 y;
    s32 len;
    Obj_2 *obj;
} Game_35;

extern Game_35 *D_800CEA40_9;

void func_80091564(Game_36 *arg0, s32 arg1)
{
    s32 i;
    s32 old;
    s32 lo, hi, lo2, hi2;

    for (i = 0; i < 4; i++) {
        if ((((s32)D_800E30B0_3[i] >> 24) & 0xF) < 6) {
            D_800E30B0_3[i] &= ~0xC;
        }
        if ((s32)(D_800E30B0_3[i] >> 28) < 6) {
            D_800E30B0_3[i] &= ~3;
        }
    }

    if (PRESSED(0x10)) {
        if (D_800CEA40_9->sel == 0x26 && D_800CEA40_9->len == 0) {
            func_8001102C(3);
        } else {
            func_8001102C(1);
        }
        {
            Game_35 *g = D_800CEA40_9;
            if (g->sel == 0x26) {
                s32 n = g->len;
                if (n != 0) {
                    g->buf[n] = 0;
                    g->len = n - 1;
                }
            } else if (g->sel == 0x27) {
                g->name = *(Name11 *)g->buf;
                D_800CEA40_9->f170 = 0;
                D_800CEA40_9->obj->flags |= 1;
            } else {
                g->len++;
                if (g->len >= 10) {
                    g->x = 799;
                    g->y = 799;
                    g->sel = 0x27;
                }
            }
        }
        return;
    }
    if (PRESSED(0x20)) {
        if (D_800CEA40_9->len == 0) {
            func_8001102C(3);
            return;
        }
        func_8001102C(1);
        D_800CEA40_9->buf[D_800CEA40_9->len - 1] = D_800CEA40_9->buf[D_800CEA40_9->len];
        {
            Game_35 *g = D_800CEA40_9;
            s32 n = g->len;
            g->buf[n--] = 0;
            g->len = n;
        }
        return;
    }

    {
        s32 y;
        y = (s32)D_800CEA40_9;
        old = ((Game_35 *)y)->sel;
        if (((Game_35 *)y)->len < 10) {
            Game_35 *g;
            u32 v;
            s32 x, nx;
            lo = 0;
            lo2 = lo;
            hi = 800;
            v = D_800E30B0_3[0];
            g = (Game_35 *)y;
            hi2 = hi;
            x = g->x;
            if (v & 0xC) {
                if (v & 8) {
                    nx = x + (((s32)v >> 24) & 0xF);
                } else {
                    nx = x - (((s32)v >> 24) & 0xF);
                }
            } else {
                nx = x;
            }
            g->x = nx;
            if (PRESSED(0x2000)) {
                D_800CEA40_9->x -= 100;
            }
            if (PRESSED(0x4000)) {
                D_800CEA40_9->x += 100;
            }
            {
                Game_35 *h = D_800CEA40_9;
                s32 c = h->x;
                if (c >= hi) {
                    h->x = lo;
                } else if (c < lo) {
                    h->x = hi - 1;
                }
            }
            {
                Game_35 *h = D_800CEA40_9;
                u32 v2 = D_800E30B0_3[0];
                s32 ny;
                y = h->y;
                if (v2 & 3) {
                    if (v2 & 1) {
                        ny = y - ((s32)((v2 >> 28) * 3) >> 1);
                    } else {
                        ny = y - (-(s32)(v2 >> 28) * 3) / 2;
                    }
                } else {
                    ny = y;
                }
                h->y = ny;
            }
            if (PRESSED(0x800)) {
                D_800CEA40_9->y -= 0xA0;
            }
            if (PRESSED(0x1000)) {
                D_800CEA40_9->y += 0xA0;
            }
            {
                Game_35 *h = D_800CEA40_9;
                s32 c = h->y;
                if (c >= hi2) {
                    h->y = lo2;
                } else if (c < lo2) {
                    h->y = hi2 - 1;
                }
            }
            D_800CEA40_9->sel = D_800CEA40_9->x / 100 + (D_800CEA40_9->y / 0xA0) * 8;
        } else if (PRESSED(4) || PRESSED(0x2000) || PRESSED(8) || PRESSED(0x4000)) {
            Game_35 *h = D_800CEA40_9;
            if (h->sel == 0x27) {
                h->x = 600;
                h->y = 799;
                h->sel = 0x26;
            } else {
                h->x = 799;
                h->y = 799;
                h->sel = 0x27;
            }
        }
    }

    if (D_800CEA40_9->sel < 0x26) {
        if (strlen(D_800CEA40_9->buf) == 0) {
            D_800CEA40_9->buf[1] = 0;
        }
        D_800CEA40_9->buf[D_800CEA40_9->len] = *D_800CD1AC[D_800CEA40_9->sel];
    } else {
        D_800CEA40_9->buf[D_800CEA40_9->len] = 0;
    }
    if (old != D_800CEA40_9->sel) {
        func_8001102C(0);
    }
}

typedef struct {
    s32 f0;
    u8 pad4[0x38];
    s32 f3C;
    u8 pad40[0xC];
    s32 f4C;
    u8 pad50[0x398];
    u8 *f3E8;
    u8 pad3EC[0x24];
    s32 f410;
} Sys;

typedef struct {
    u8 pad0[0x2C4];
    u8 f2C4;
    u8 pad2C5[0x22F];
    u8 f4F4[0x310];
} Save_7;

typedef struct {
    Pair p;
    u32 f8;
} Cam_6;

extern Save_7 D_800F1900_16;
extern Sys D_800EFD70_89;
extern Menu_6 D_800CEB9C;
extern Cam_6 D_800CE21C_3;
extern Cam_6 D_800CE210_3;

void func_80091AFC(void)
{
    Game_36 *p = D_800CEA40_10;
    s32 flags = p->f174;
    s32 i, j, k, n;

    if (flags & 0x800) {
        if ((D_800E30B0_2[0] & 0x10) && !(D_800E25B0_2[0] & 0x10)) {
            D_800E25B0_2[0] |= 0x10;
            D_800CEA40_10->f174 &= ~0x800;
            D_800CEA40_10->f16E0->fC |= 1;
            func_8001102C(1);
        }
    } else if (flags & 0x1000) {
        if ((D_800E30B0_2[0] & 0x20) && !(D_800E25B0_2[0] & 0x20)) {
            D_800E25B0_2[0] |= 0x20;
            D_800CEA40_10->f174 &= ~0x1000;
            D_800CEA40_10->f16E0->fC |= 1;
            func_8001102C(2);
        } else if ((D_800E30B0_2[0] & 0x10) && !(D_800E25B0_2[0] & 0x10)) {
            D_800E25B0_2[0] |= 0x10;
            func_8001102C(1);
            if (D_800CEA40_10->f17FE) {
                switch (D_800CEA40_10->f17FC) {
                case 7:
                    D_800CEA40_10->f17FC = 6;
                    if (D_800F1900_16.f2C4 & 8) {
                        D_800CEA40_10->f17FE = 1;
                    } else {
                        D_800CEA40_10->f17FE = 0;
                    }
                    break;
                case 6:
                    if (D_800F1900_16.f2C4 & 8) {
                        for (j = 0; j < D_800CEA40_10->f16AC->h; j++) {
                            for (i = 0; i < D_800CEA40_10->f16AC->w; i++) {
                                func_8008D3B4(i, j, 0, 0, 0, 0, 0, 0, 0);
                            }
                        }
                        D_800CEA40_10->f16F4 = 8;
                        func_800A4700(D_800F1DF4);
                        func_80094738(D_800CEA40_10->f180);
                        D_800CEA40_10->f170 = 0;
                        D_800CEA40_10->f16E0->fC |= 1;
                        func_8009EB4C();
                        func_8008E76C(D_800CEA40_10->f17F8, D_800CEA40_10->f1724, D_800CEA40_10->f1728);
                        D_800CEA40_10->f16E0->fC |= 1;
                        D_800CEA40_10->f174 &= ~0x1000;
                    } else {
                        D_800CEA40_10->f1800 = 0;
                        D_800CEA40_10->f174 |= 0x800;
                    }
                    break;
                case 8:
                    D_800EFD70_89.f3E8 = func_800BC908(0x400, 0, (s32)D_800DAB1C, 0x6FE);
                    func_800A42D0_2(D_800EFD70_89.f3E8);
                    __builtin_memcpy(D_800F1DF4, D_800EFD70_89.f3E8, 0x310);
                    func_8006F5F8(4, 0);
                    func_800BCB04(D_800EFD70_89.f3E8);
                    D_800EFD70_89.f3E8 = 0;
                    D_800CEA40_10->f16E0->fC |= 1;
                    D_800CEA40_10->f174 &= ~0x1000;
                    break;
                case 5:
                    if (D_800F1900_16.f2C4 & 8) {
                        D_800CEA40_10->f17FC = 8;
                        D_800CEA40_10->f17FE = 0;
                    } else {
                        D_800EFD70_89.f3E8 = func_800BC908(0x400, 0, (s32)D_800DAB1C, 0x6FE);
                        func_800A42D0_2(D_800EFD70_89.f3E8);
                        __builtin_memcpy(D_800F1900_16.f4F4, D_800EFD70_89.f3E8, 0x310);
                        func_8006F5F8(4, 0);
                        func_800BCB04(D_800EFD70_89.f3E8);
                        D_800EFD70_89.f3E8 = 0;
                        D_800CEA40_10->f16E0->fC |= 1;
                        D_800CEA40_10->f174 &= ~0x1000;
                    }
                    break;
                case 3:
                    D_800EFD70_89.f0 |= 2;
                    D_800CEA40_10->f178 = 0;
                    D_800CEA40_10->f174 |= 0x10;
                    D_800CEA40_10->f16E0->fC |= 1;
                    D_800CEA40_10->f174 &= ~0x1000;
                    break;
                case 4:
                    D_800CEA40_10->f17C = 0;
                    D_800CEA40_10->f180 = 0;
                    D_800CEA40_10->f184 = 0;
                    D_800CEA40_10->f190 = 0;
                    D_800CEA40_10->f16B6 = 0;
                    D_800CEA40_10->f16B8 = 0;
                    for (k = 0; k < 150; k++) {
                        D_800CEA40_10->slots[k].f0 = 0;
                    }
                    D_800CEA40_10->f16F4 = 8;
                    __builtin_strcpy(D_800CEA40_10->name, "<NONE>");
                    for (j = 0; j < D_800CEA40_10->f16AC->h; j++) {
                        for (i = 0; i < D_800CEA40_10->f16AC->w; i++) {
                            func_8008D3B4(i, j, 0, 0, 0, 0, 0, 0, 0);
                        }
                    }
                    D_800CEA40_10->f184 = 0;
                    D_800CEA40_10->f17C = 0;
                    D_800CEA40_10->f180 = D_800CEA40_10->f184;
                    func_80096B3C(D_800CEA40_10->f184);
                    D_800CEA40_10->f16E0->fC |= 1;
                    D_800CEA40_10->f17F8 = 0;
                    D_800CEA40_10->f174 &= ~0x1000;
                    func_8008E76C(0, D_800CEA40_10->f1724, D_800CEA40_10->f1728);
                    func_80090670();
                    D_800CE240 = 0;
                    D_800CE21C_3.p.a = D_800CE210_3.p.a;
                    D_800CE21C_3.p.b = D_800CE210_3.p.b;
                    D_800CE21C_3.f8 = ((D_800CE21C_3.f8 + 0x800) >> 13) << 13;
                    break;
                }
            } else {
                switch (D_800CEA40_10->f17FC) {
                case 8:
                    D_800CEA40_10->f1800 = 1;
                    D_800CEA40_10->f174 = (D_800CEA40_10->f174 & ~0x1000) | 0x800;
                    break;
                case 6:
                    D_800EFD70_89.f0 |= 2;
                    D_800CEA40_10->f178 = 0;
                    D_800CEA40_10->f174 = (D_800CEA40_10->f174 | 0x2010) & ~0x4000;
                    D_800CEA40_10->f16E0->fC |= 1;
                    D_800CEA40_10->f174 &= ~0x1000;
                    break;
                case 5:
                    D_800EFD70_89.f0 |= 2;
                    D_800CEA40_10->f178 = 0;
                    D_800CEA40_10->f174 = (D_800CEA40_10->f174 | 0x4010) & ~0x2000;
                    D_800EFD70_89.f3E8 = func_800BC908(0x400, 0, (s32)D_800DAB1C, 0x7B3);
                    for (n = func_800A42D0_2(D_800EFD70_89.f3E8 + 4) + 4; n < 0x400; n++) {
                        D_800EFD70_89.f3E8[n] = 0;
                    }
                    *(u32 *)D_800EFD70_89.f3E8 = func_80060170(D_800EFD70_89.f3E8 + 4, 0x3FC);
                default:
                    D_800CEA40_10->f16E0->fC |= 1;
                    D_800CEA40_10->f174 &= ~0x1000;
                    break;
                }
            }
        } else if (((D_800E30B0_2[0] & 0x800) && !(D_800E25B0_2[0] & 0x800)) ||
                   ((D_800E30B0_2[0] & 0x1000) && !(D_800E25B0_2[0] & 0x1000))) {
            if ((D_800CEA40_10->f17FC != 6 || (D_800F1900_16.f2C4 & 8)) && D_800CEA40_10->f17FC != 5) {
                D_800CEA40_10->f17FE ^= 1;
                func_8001102C(0);
            }
        }
    } else {
        if (p->f184 != NULL && p->f170 != 7) {
            if (((D_800E30B0_2[0] & 0x2000) && !(D_800E25B0_2[0] & 0x2000) && !(D_800CEA40_10->f174 & 1)) ||
                ((D_800E30B0_2[0] & 0x4000) && !(D_800E25B0_2[0] & 0x4000) && (D_800CEA40_10->f174 & 1))) {
                if (D_800CEA40_10->f184->next) {
                    D_800CEA40_10->f184 = D_800CEA40_10->f184->next;
                }
            } else if (((D_800E30B0_2[0] & 0x4000) && !(D_800E25B0_2[0] & 0x4000) && !(D_800CEA40_10->f174 & 1)) ||
                       ((D_800E30B0_2[0] & 0x2000) && !(D_800E25B0_2[0] & 0x2000) && (D_800CEA40_10->f174 & 1))) {
                if (D_800CEA40_10->f184->prev) {
                    D_800CEA40_10->f184 = D_800CEA40_10->f184->prev;
                }
            }
        }
        if (D_800CEA40_10->f170 == 7 || D_800CEA40_10->f170 == 8) {
            func_80091564(p, flags);
        } else if (func_8008F878(D_800CEA40_10->f16E0, flags)) {
            switch (D_800CEB18) {
            case 1:
                D_800CEA40_10->f170 = 0;
                if (D_800CEA40_10->f16E0 != &D_800CEB9C) {
                    switch (D_800CEA40_10->f16E8) {
                    case 1:
                        D_800CEA40_10->f16E0 = &D_800CEB9C;
                        break;
                    case 0:
                        D_800CEA40_10->f16E8 = 1;
                        D_800CEA40_10->f16E4 = &D_800CEB9C;
                        break;
                    case 2:
                        D_800CEA40_10->f16E8 = 3;
                        D_800CEA40_10->f16E4 = &D_800CEB9C;
                        break;
                    case 3:
                        D_800CEA40_10->f16E8 = 1;
                        D_800CEA40_10->f16E0 = &D_800CEB9C;
                        break;
                    }
                    D_800CEA40_10->f174 &= ~0x10000;
                }
                D_800CEB9C.f10 = 0;
                D_800CEB9C.f6 = 0;
                D_800CEA40_10->f16C = 1;
                break;
            case 0:
                D_800CEA40_10->f17FC = 4;
                D_800CEA40_10->f174 |= 0x1000;
                D_800CEA40_10->f16E0->fC &= ~1;
                D_800CEA40_10->f17FE = 0;
                break;
            case 2:
                D_800CEA40_10->f174 |= 0x1000;
                if (D_800CEA40_10->f184 != NULL) {
                    D_800CEA40_10->f17FC = 7;
                    D_800CEA40_10->f17FE = 0;
                } else {
                    D_800CEA40_10->f17FC = 6;
                    if (D_800F1900_16.f2C4 & 8) {
                        D_800CEA40_10->f17FE = 1;
                    } else {
                        D_800CEA40_10->f17FE = 0;
                    }
                }
                D_800CEA40_10->f16E0->fC &= ~1;
                break;
            case 3:
                if (D_800CEA40_10->name[0] == '<') {
                    D_800CEA40_10->f174 |= 0x800;
                    D_800CEA40_10->f16E0->fC &= ~1;
                    D_800CEA40_10->f1800 = 4;
                    D_800CEB08.f10 = 6;
                    D_800CEB08.f6 = 6;
                } else {
                    D_800CEA40_10->f17FC = 5;
                    D_800CEA40_10->f174 |= 0x1000;
                    D_800CEA40_10->f16E0->fC &= ~1;
                    D_800CEA40_10->f17FE = 0;
                }
                break;
            case 5:
                if (++D_800CEA40_10->f17F8 == 3) {
                    D_800CEA40_10->f17F8 = 0;
                }
                func_8008E76C(D_800CEA40_10->f17F8, D_800CEA40_10->f1724, D_800CEA40_10->f1728);
                break;
            case 7:
                D_800CEA40_10->f17FC = 3;
                D_800CEA40_10->f17FE = 0;
                D_800CEA40_10->f174 = (D_800CEA40_10->f174 & ~0x6000) | 0x1000;
                D_800CEA40_10->f16E0->fC &= ~1;
                break;
            case 6:
                D_800CEA40_10->f16DC = 0;
                D_800CEA40_10->f16D0 = 0;
                D_800CEA40_10->f16D4 = 0;
                D_800CEA40_10->f16D8 = 0;
                func_8000ED10(D_800CEA40_10->f16C5, 0, 0xB);
                D_800CEA40_10->f170 = 7;
                D_800CEA40_10->f16E0->fC &= ~1;
                break;
            case 4:
                if (D_800EFD70_89.f410 == 0) {
                    D_800EFD70_89.f410 = 1;
                }
                if (D_800CEA40_10->f17C->next == D_800CEA40_10->f180) {
                    D_800EFD70_89.f4C = 0xD;
                } else {
                    D_800EFD70_89.f4C = 0xE;
                }
                D_800EFD70_89.f3C = 1;
                D_800EFD70_89.f0 &= ~2;
                D_800CEA40_10->f174 &= ~0x6000;
                D_800EFD70_89.f0 |= 0x4000000;
                D_800CEA40_10->f174 |= 0x410;
                D_800CEA40_10->f178 = 0;
                D_800CEA40_10->f174 &= ~0x1000;
                if (D_800EFD70_89.f3E8 == NULL) {
                    D_800EFD70_89.f3E8 = func_800BC908(0x400, 0, (s32)D_800DAB1C, 0x88C);
                    for (n = func_800A42D0_2(D_800EFD70_89.f3E8 + 4) + 4; n < 0x400; n++) {
                        D_800EFD70_89.f3E8[n] = 0;
                    }
                }
                break;
            }
        }
    }
    if (D_800CEA40_10->f184 != NULL) {
        if (D_800CEA40_10->f184->f10 & 2) {
            D_800CEA40_10->f174 |= 8;
        } else {
            D_800CEA40_10->f174 &= ~8;
        }
    }
}

#define PRESSED_2(m) ((D_800E30B0 & (m)) && !(D_800E25B0_3[0] & (m)))
#define DEL()                                                                          if (D_800CEA40_11->cur != NULL) {                                                         if (D_800CEA40_11->cur->prev == NULL) {                                                   n = D_800CEA40_11->cur;                                                               D_800CEA40_11->cur = n->next;                                                         func_8009AF18(n);                                                                  sel();                                                                         } else if (D_800CEA40_11->cur->next == NULL || D_800CEA40_11->head->prev == D_800CEA40_11->tail) {             n = D_800CEA40_11->cur;                                                               D_800CEA40_11->cur = n->prev;                                                         func_8009AF18(n);                                                                  sel();                                                                         }                                                                              }

typedef struct Item_15 {
    s32 f0;
    u32 flags;
    u8 pad8[8];
} Item_15;

extern Item_15 D_800CEB1C[];
extern Ctl_23 D_800CEB9C_2;

s32 func_8008F878_2(Ctl_23 *c);

static inline s32 chk(Edit *e, Node_25 *c) {
    Node_25 *n;
    s32 r;

    if (c == NULL || c != e->f190 || (c->prev == NULL && c->next == NULL)) {
        return 1;
    }
    r = 0;
    n = e->head;
    while (n != NULL) {
        if (n != c && (n->pt->fC.h.hi & 2)) {
            r = 1;
            break;
        }
        n = n->next;
        if (n == e->head) {
            break;
        }
    }
    return r;
}

static inline s32 chkg(void) {
    Node_25 *n;
    s32 r;

    if (D_800CEA40_11->cur == NULL || D_800CEA40_11->cur != D_800CEA40_11->f190) {
        return 1;
    }
    if (D_800CEA40_11->cur->prev != NULL || D_800CEA40_11->cur->next != NULL) {
        r = 0;
        n = D_800CEA40_11->head;
        while (n != NULL) {
            if (n != D_800CEA40_11->cur && (n->pt->fC.h.hi & 2)) {
                r = 1;
                break;
            }
            n = n->next;
            if (n == D_800CEA40_11->head) {
                break;
            }
        }
        return r;
    }
    return 1;
}

void func_80092908(void) {
    s16 minx, miny, maxx, maxy;
    s16 d[2];
    Node_25 *n;
    s32 ok;
    s32 i, j;
    Edit *e;
    Node_25 *c;
    Node_25 *s;

    minx = 1000;
    maxx = -1000;
    miny = 1000;
    maxy = -1000;
    D_800CEA40_11->flags &= ~0x1E0;
    if (D_800CEA40_11->cur != NULL) {
        n = D_800CEA40_11->head;
        do {
            if (n->x < minx) {
                minx = n->x;
            }
            if (n->y < miny) {
                miny = n->y;
            }
            if (n->x + n->w > maxx) {
                maxx = n->x + n->w;
            }
            if (n->y + n->h > maxy) {
                maxy = n->y + n->h;
            }
            n = n->next;
        } while (n != NULL && n != D_800CEA40_11->head);
        if (minx > 0) {
            D_800CEA40_11->flags |= 0x20;
        }
        if (miny > 0) {
            D_800CEA40_11->flags |= 0x100;
        }
        if (maxx < D_800CEA40_11->size[0]) {
            D_800CEA40_11->flags |= 0x40;
        }
        if (maxy < D_800CEA40_11->size[1]) {
            D_800CEA40_11->flags |= 0x80;
        }
    }
    if (D_800CEA40_11->cur != NULL && (D_800CEA40_11->cur->st & 4)) {
        D_800CEB1C[6].flags &= ~0x10;
        D_800CEB1C[4].flags &= ~0x10;
        D_800CEB1C[7].flags &= ~0x10;
        D_800CEB1C[5].flags &= ~0x10;
        D_800CEA40_11->flags &= ~0x8000;
    } else {
        D_800CEB1C[7].flags |= 0x10;
        if (D_800CEA40_11->cur == NULL ||
            (D_800CEA40_11->cur->next != NULL && D_800CEA40_11->cur->prev != NULL && D_800CEA40_11->head->prev == NULL) ||
            (e = D_800CEA40_11, c = e->cur, !chk(e, c))) {
            D_800CEB1C[6].flags &= ~0x10;
            D_800CEA40_11->flags &= ~0x8000;
        } else {
            D_800CEA40_11->flags |= 0x8000;
            D_800CEB1C[6].flags |= 0x10;
        }
        if (D_800CEA40_11->flags & 0x1E0) {
            D_800CEB1C[4].flags |= 0x10;
        } else {
            D_800CEB1C[4].flags &= ~0x10;
        }
    }
    if (D_800CEA40_11->f16E0 != NULL) {
        func_80090398(D_800CEA40_11->f16E0);
    }
    if (D_800CEA40_11->flags & 0x800) {
        if (PRESSED_2(0x10)) {
            D_800E25B0_3[0] |= 0x10;
            D_800CEA40_11->flags &= ~0x800;
            D_800CEA40_11->f16E0->fC |= 1;
        }
    } else {
        if (D_800CEA40_11->mode == 10 || D_800CEA40_11->mode == 11) {
            if (PRESSED_2(0x10)) {
                D_800CEA40_11->mode = 6;
            } else if (PRESSED_2(0x20)) {
                D_800CEA40_11->f16E0->fC |= 1;
                D_800CEA40_11->mode = 9;
                D_800CEA40_11->flags &= ~0x1000;
            }
        } else if (D_800CEA40_11->mode == 9 || D_800CEA40_11->mode == 6) {
            if (PRESSED_2(0x20)) {
                if (D_800CEA40_11->mode == 9) {
                    func_80094638(D_800CEA40_11->cur);
                    D_800CEA40_11->mode = 0;
                    D_800CEA40_11->flags &= ~0x200;
                }
            } else {
                func_800938BC();
            }
        } else if (D_800CEA40_11->mode == 4) {
            ok = 0;
            if (PRESSED_2(0x20)) {
                d[0] = D_800CEA40_11->head->x - D_800CEA40_11->f16B0;
                d[1] = D_800CEA40_11->head->y - D_800CEA40_11->f16B2;
                n = D_800CEA40_11->head;
                do {
                    n->x -= d[0];
                    n->y -= d[1];
                    n = n->next;
                } while (n != NULL && n != D_800CEA40_11->head);
                D_800CEA40_11->mode = 0;
                D_800CEA40_11->f16E0->fC |= 1;
                ok = 1;
                func_8001102C(2);
            } else if (PRESSED_2(0x10)) {
                D_800CEA40_11->mode = 0;
                D_800CEA40_11->f16E0->fC |= 1;
                func_8001102C(1);
            } else if ((D_800CEA40_11->flags & 0x20) && PRESSED_2(0x2000)) {
                n = D_800CEA40_11->head;
                do {
                    n->x--;
                    n = n->next;
                } while (n != NULL && n != D_800CEA40_11->head);
                func_8001102C(0);
                ok = 1;
            } else if ((D_800CEA40_11->flags & 0x40) && PRESSED_2(0x4000)) {
                n = D_800CEA40_11->head;
                do {
                    n->x++;
                    n = n->next;
                } while (n != NULL && n != D_800CEA40_11->head);
                func_8001102C(0);
                ok = 1;
            } else if ((D_800CEA40_11->flags & 0x80) && PRESSED_2(0x800)) {
                n = D_800CEA40_11->head;
                do {
                    n->y++;
                    n = n->next;
                } while (n != NULL && n != D_800CEA40_11->head);
                func_8001102C(0);
                ok = 1;
            } else if ((D_800CEA40_11->flags & 0x100) && PRESSED_2(0x1000)) {
                n = D_800CEA40_11->head;
                do {
                    n->y--;
                    n = n->next;
                } while (n != NULL && n != D_800CEA40_11->head);
                func_8001102C(0);
                ok = 1;
            }
            if (ok) {
                n = D_800CEA40_11->head;
                do {
                    func_80096B3C(D_800CEA40_11->head);
                    n = n->next;
                } while (n != NULL && n != D_800CEA40_11->head);
                for (i = 0; i < D_800CEA40_11->size[1]; i++) {
                    for (j = 0; j < D_800CEA40_11->size[0]; j++) {
                        func_8008D3B4(j, i, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        } else if (func_8008F878_2(D_800CEA40_11->f16E0)) {
            switch (D_800CEB9C_2.h10) {
            case 0:
            case 1:
            case 2:
            case 3:
                if (D_800CEA40_11->cur == NULL) {
                    D_800CEA40_11->cur = func_8009AB24(NULL, NULL, D_800CEA40_11->f17B0);
                    D_800CEA40_11->cur->st |= 4;
                    D_800CEA40_11->mode = 9;
                    D_800CEA40_11->head = D_800CEA40_11->cur;
                    D_800CEA40_11->tail = D_800CEA40_11->cur;
                } else if (D_800CEA40_11->cur->next == NULL) {
                    if (D_800CEA40_11->cur->prev == NULL) {
                        D_800CEA40_11->cur = func_8009AB24(D_800CEA40_11->cur, NULL, D_800CEA40_11->f17B0);
                        D_800CEA40_11->cur->st |= 4;
                        D_800CEA40_11->mode = 0;
                        D_800CEA40_11->tail = D_800CEA40_11->cur;
                        D_800CEA40_11->f16E0->fC |= 1;
                        D_800CEA40_11->mode = 9;
                        D_800CEA40_11->flags |= 0x200;
                    } else {
                        D_800CEA40_11->cur = func_8009AB24(D_800CEA40_11->cur, NULL, D_800CEA40_11->f17B0);
                        D_800CEA40_11->cur->st |= 4;
                        D_800CEA40_11->mode = 9;
                        D_800CEA40_11->tail = D_800CEA40_11->cur;
                    }
                } else if (D_800CEA40_11->cur->prev == NULL) {
                    D_800CEA40_11->cur = func_8009AB24(NULL, D_800CEA40_11->cur, D_800CEA40_11->f17B0);
                    D_800CEA40_11->cur->st |= 4;
                    D_800CEA40_11->mode = 9;
                    D_800CEA40_11->head = D_800CEA40_11->cur;
                }
                break;
            case 5:
                if (D_800CEA40_11->cur != NULL) {
                    D_800CEA40_11->f190 = D_800CEA40_11->cur;
                }
                break;
            case 4:
                if (D_800CEA40_11->cur != NULL) {
                    D_800CEA40_11->f16B0 = D_800CEA40_11->head->x;
                    D_800CEA40_11->f16B2 = D_800CEA40_11->head->y;
                    D_800CEA40_11->mode = 4;
                    D_800CEA40_11->f16E0->fC &= ~1;
                }
                break;
            case 7:
                func_8001102C(2);
                if (D_800CEA40_11->f16E0 != &D_800CEB08) {
                    switch (D_800CEA40_11->f16E8) {
                    case 0:
                        D_800CEA40_11->f16E8 = 1;
                        D_800CEA40_11->f16E4 = &D_800CEB08;
                        break;
                    case 1:
                        D_800CEA40_11->f16E0 = &D_800CEB08;
                        break;
                    case 2:
                        D_800CEA40_11->f16E8 = 3;
                        D_800CEA40_11->f16E4 = &D_800CEB08;
                        break;
                    case 3:
                        D_800CEA40_11->f16E8 = 1;
                        D_800CEA40_11->f16E0 = &D_800CEB08;
                        break;
                    }
                    D_800CEA40_11->flags &= ~0x10000;
                }
                D_800CEA40_11->f16C = 0;
                break;
            case 6:
                if (D_800CEA40_11->cur != NULL && chkg()) {
                    DEL();
                }
                break;
            }
        } else if (PRESSED_2(0x20)) {
            e = D_800CEA40_11;
            if (e->flags & 0x8000) {
                c = e->cur;
                if (chk(e, c)) {
                    DEL();
                }
            }
        }
        if (D_800CEA40_11->cur != NULL) {
            s = D_800CEA40_11->cur;
            if (s->st & 4) {
                if (s->next == NULL) {
                    s = s->prev;
                } else {
                    s = s->next;
                }
            }
            if (s != NULL) {
                func_8009B9A8(s);
            } else {
                func_8009B820();
            }
        }
        if (D_800CEA40_11->mode == 0 && D_800CEA40_11->cur != NULL) {
            if ((PRESSED_2(0x2000) && !(D_800CEA40_11->flags & 1)) || (PRESSED_2(0x4000) && (D_800CEA40_11->flags & 1))) {
                if (D_800CEA40_11->cur->prev != NULL) {
                    D_800CEA40_11->cur = D_800CEA40_11->cur->prev;
                }
            } else if ((PRESSED_2(0x4000) && !(D_800CEA40_11->flags & 1)) ||
                       (PRESSED_2(0x2000) && (D_800CEA40_11->flags & 1))) {
                if (D_800CEA40_11->cur->next != NULL) {
                    D_800CEA40_11->cur = D_800CEA40_11->cur->next;
                }
            }
        }
    }
    if (D_800CEA40_11->cur != NULL) {
        if (D_800CEA40_11->cur->st & 2) {
            D_800CEA40_11->flags |= 8;
        } else {
            D_800CEA40_11->flags &= ~8;
        }
    }
}

#define PRESSED_3(m) ((D_800E30B0 & (m)) && !(D_800E25B0 & (m)))
#define IDX (D_800CEB9C_3.p8[D_800CEB9C_3.h10].hA)

#define INSERT(T)                                                                          \
    if (D_800CEA40_12->cur == NULL) {                                                         \
        D_800CEA40_12->cur = func_8009AB24(NULL, NULL, &T[IDX]);                              \
        D_800CEA40_12->head = D_800CEA40_12->cur;                                                \
        D_800CEA40_12->tail = D_800CEA40_12->cur;                                                \
        D_800CEA40_12->f190 = D_800CEA40_12->cur;                                                \
    } else if (D_800CEA40_12->cur->prev != NULL ||                                            \
               ((D_800CEA40_12->flags & 0x200) && D_800CEA40_12->cur->next == NULL)) {          \
        D_800CEA40_12->cur = func_8009AB24(D_800CEA40_12->cur, D_800CEA40_12->cur->next, &T[IDX]);  \
        D_800CEA40_12->tail = D_800CEA40_12->cur;                                                \
    } else {                                                                               \
        D_800CEA40_12->cur = func_8009AB24(NULL, D_800CEA40_12->cur, &T[IDX]);                   \
        D_800CEA40_12->head = D_800CEA40_12->cur;                                                \
    }

typedef struct Ctl_24 {
    u8 pad0[0xC];
    u32 fC;
} Ctl_24;

typedef struct Node_26 {
    s16 used;
    s16 pad2;
    struct Node_26 *next;
    struct Node_26 *prev;
    s16 x;
    s16 y;
    u16 st;
    s16 idx;
    Ent_4 *pt;
} Node_26;

typedef struct Edit_2 {
    u8 pad0[0x170];
    s32 mode;
    u32 flags;
    u32 pad178;
    Node_26 *head;
    Node_26 *tail;
    Node_26 *cur;
    Node_26 *f188;
    Node_26 *f18C;
    Node_26 *f190;
    u8 pad194[0x16AC - 0x194];
    s16 *size;
    u8 pad16B0[6];
    s16 scrollx;
    s16 scrolly;
    u8 pad16BA[0x16E0 - 0x16BA];
    Ctl_24 *f16E0;
    u8 pad16E4[0x16FC - 0x16E4];
    s32 f16FC;
    u8 pad1700[0x17B0 - 0x1700];
    Ent_4 *f17B0;
    u8 pad17B4[0x1800 - 0x17B4];
    s16 f1800;
} Edit_2;

typedef struct Sel_4 {
    u8 pad0[0xA];
    s16 hA;
    u8 padC[4];
} Sel_4;

typedef struct Menu_7 {
    u8 pad0[6];
    s16 h6;
    Sel_4 *p8;
    u8 padC[4];
    s16 h10;
} Menu_7;

extern Edit_2 *D_800CEA40_12;
extern Menu_7 D_800CEB9C_3;

s32 func_8008F878_3(Ctl_24 *c);

void func_800938BC(void) {
    Node_26 *cur;
    Node_26 *c;
    Node_26 *p;
    Node_26 *n;
    s32 i;
    s32 j;
    s32 ok;

    c = D_800CEA40_12->cur;
    p = D_800CEA40_12->f188;
    if (c != NULL && c->prev == NULL && c->next == NULL) {
        if ((PRESSED_3(0x2000) && !(D_800CEA40_12->flags & 1)) || (PRESSED_3(0x4000) && (D_800CEA40_12->flags & 1))) {
            if (D_800CEA40_12->scrollx >= -30) {
                D_800CEA40_12->scrollx--;
            }
        }
        if ((PRESSED_3(0x4000) && !(D_800CEA40_12->flags & 1)) || (PRESSED_3(0x2000) && (D_800CEA40_12->flags & 1))) {
            if (D_800CEA40_12->scrollx < 31) {
                D_800CEA40_12->scrollx++;
            }
        }
        if ((PRESSED_3(0x1000) && !(D_800CEA40_12->flags & 1)) || (PRESSED_3(0x800) && (D_800CEA40_12->flags & 1))) {
            if (D_800CEA40_12->scrolly >= -30) {
                D_800CEA40_12->scrolly--;
            }
        }
        if ((PRESSED_3(0x800) && !(D_800CEA40_12->flags & 1)) || (PRESSED_3(0x1000) && (D_800CEA40_12->flags & 1))) {
            if (D_800CEA40_12->scrolly < 31) {
                D_800CEA40_12->scrolly++;
            }
        }
    }
    if (p != NULL) {
        do {
            p = p->next;
        } while (p->prev != D_800CEA40_12->f18C);
    }
    if (PRESSED_3(0x10000)) {
        if ((D_800CEA40_12->cur->pt->fC.w & 0x300000) == 0x300000) {
            D_800E30B0 &= 0x10000;
            func_8001102C(0);
            for (i = 0; i < D_800CEA40_12->cur->pt->dim[(D_800CEA40_12->cur->idx + 1) & 1]; i++) {
                for (j = 0; j < D_800CEA40_12->cur->pt->dim[D_800CEA40_12->cur->idx & 1]; j++) {
                    if (D_800CEA40_12->cur->x + j >= 0 && D_800CEA40_12->cur->x + j < D_800CEA40_12->size[0] &&
                        D_800CEA40_12->cur->y + i >= 0 && D_800CEA40_12->cur->y + i < D_800CEA40_12->size[1]) {
                        func_8008D3B4(D_800CEA40_12->cur->x + j, D_800CEA40_12->cur->y + i, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
            D_800CEA40_12->flags ^= 8;
            if (D_800CEA40_12->flags & 8) {
                D_800CEA40_12->cur->st |= 2;
            } else {
                D_800CEA40_12->cur->st &= ~2;
            }
        }
        p = D_800CEA40_12->head;
        if (p != NULL) {
            do {
                func_80096B3C(p);
                p = p->next;
            } while (p != NULL && p != D_800CEA40_12->head);
        }
    } else if (PRESSED_3(0x20000)) {
        if (D_800CEA40_12->cur != NULL && (D_800CEA40_12->cur->pt->fC.h.hi & 0x40)) {
            D_800CEA40_12->f16E0->fC &= ~1;
            func_8009F260();
            func_8009EBA8(0);
            if (D_800CEA40_12->cur->pt->fC.h.hi & 0x400) {
                D_800CEA40_12->mode = 10;
            } else if (D_800CEA40_12->cur->pt->fC.h.hi & 0x200) {
                D_800CEA40_12->mode = 11;
            } else {
                D_800CEA40_12->mode = 6;
            }
        }
    }
    n = D_800CEA40_12->cur;
    if (n->st & 4) {
        if (n->next == NULL) {
            n = n->prev;
        } else {
            n = n->next;
        }
    }
    if (n != NULL) {
        func_8009B9A8(n);
    } else {
        func_8009B820();
    }
    if (D_800CEA40_12->mode == 6) {
        func_8009E584();
        D_800CEA40_12->flags |= 0x200;
    } else if (func_8008F878_3(D_800CEA40_12->f16E0)) {
        ok = 0;
        switch (D_800CEB9C_3.h10) {
        case 3:
            func_80094638_2();
            D_800CEA40_12->f16FC = IDX;
            INSERT(D_800CE700);
            ok = 1;
            break;
        case 0:
            func_80094638_2();
            INSERT(D_800CE280);
            ok = 1;
            break;
        case 1:
            func_80094638_2();
            INSERT(D_800CE300);
            ok = 1;
            break;
        case 2:
            func_80094638_2();
            INSERT(D_800CE400);
            ok = 1;
            break;
        }
        D_800CEA40_12->flags &= ~0x200;
        if (ok) {
            D_800CEA40_12->cur->st &= ~4;
            if (func_8009B9A8(D_800CEA40_12->cur)) {
                cur = D_800CEA40_12->cur;
                if (cur->prev == NULL) {
                    if (cur->next == NULL) {
                        D_800CEA40_12->cur = func_8009AB24(cur, NULL, D_800CEA40_12->f17B0);
                        D_800CEA40_12->cur->st |= 4;
                        D_800CEA40_12->tail = D_800CEA40_12->cur;
                        D_800CEA40_12->flags |= 0x200;
                    } else {
                        D_800CEA40_12->cur = func_8009AB24(NULL, cur, D_800CEA40_12->f17B0);
                        D_800CEA40_12->cur->st |= 4;
                        D_800CEA40_12->head = D_800CEA40_12->cur;
                    }
                } else {
                    D_800CEA40_12->cur = func_8009AB24(cur, NULL, D_800CEA40_12->f17B0);
                    D_800CEA40_12->cur->st |= 4;
                    D_800CEA40_12->tail = D_800CEA40_12->cur;
                }
            } else {
                func_80094738(D_800CEA40_12->cur);
                D_800CEA40_12->mode = 0;
                D_800CEA40_12->flags |= 0x800;
                if (D_800CEA40_12->head->prev != NULL) {
                    D_800CEA40_12->f1800 = 2;
                    D_800CEB9C_3.h10 = 7;
                    D_800CEB9C_3.h6 = 7;
                } else {
                    D_800CEA40_12->f1800 = 3;
                }
                D_800CEA40_12->f16E0->fC &= ~1;
            }
        }
    } else {
        switch (D_800CEB9C_3.h10) {
        case 3:
            func_80094638_2();
            D_800CEA40_12->f16FC = IDX;
            INSERT(D_800CE700);
            D_800CEA40_12->cur->st |= 4;
            break;
        case 0:
            func_80094638_2();
            INSERT(D_800CE280);
            D_800CEA40_12->cur->st |= 4;
            break;
        case 1:
            func_80094638_2();
            INSERT(D_800CE300);
            D_800CEA40_12->cur->st |= 4;
            break;
        case 2:
            func_80094638_2();
            INSERT(D_800CE400);
            D_800CEA40_12->cur->st |= 4;
            break;
        }
    }
    func_80096B3C(D_800CEA40_12->cur);
}

typedef struct Node_27 {
    s32 f0;
    struct Node_27 *n4;
    struct Node_27 *n8;
    s32 f0c;
    u16 flags;
} Node_27;

typedef struct Gs_2 {
    u8 pad[0x17C];
    Node_27 *f17c;
    Node_27 *f180;
    Node_27 *f184;
} Gs_2;

extern Gs_2 *D_800CEA40_13;

void func_80094638(s32 arg0) {
    Node_27 *n;
    Node_27 *cur;
    s32 f;

    n = D_800CEA40_13->f184;
    while (n != 0 && (n->flags & 4) != 0) {
        n = n->n8;
    }
    f = 1;
    if (n == 0) {
        n = D_800CEA40_13->f184;
        while (n != 0) {
            if ((n->flags & 4) == 0) {
                break;
            }
            n = n->n4;
        }
        f = 0;
    }
    D_800CEA40_13->f184 = n;
    if (n == 0) {
        n = D_800CEA40_13->f17c;
        f = 1;
        D_800CEA40_13->f17c = 0;
        D_800CEA40_13->f180 = 0;
    } else if (f != 0) {
        D_800CEA40_13->f180 = n;
    } else {
        D_800CEA40_13->f17c = n;
    }
    while (n != 0) {
        cur = n;
        if (f != 0) {
            n = cur->n4;
        } else {
            n = cur->n8;
        }
        if ((cur->flags & 4) != 0) {
            func_8009AF18(cur);
        }
    }
}

typedef struct Game_37 {
    u8 pad0[0x17C];
    Node_28 *first;        /* 0x17C */
    Node_28 *last;         /* 0x180 */
} Game_37;

extern Game_37 *D_800CEA40_14;

void func_80094738(Node_28 *n)
{
    Node_28 *c;

    for (c = D_800CEA40_14->first; c != NULL; c = c->a) {
        if (c->b == D_800CEA40_14->last) {
            return;
        }
        if (c->a != NULL && c->b != NULL) {
            continue;
        }
        if (c == n) {
            continue;
        }
        if (c->b == NULL) {
            if (n->a == NULL && ((n->dirB - c->dirA) & 3) == 2) {
                switch (n->dirB) {
                case 0:
                    if (n->y + n->h == c->y && n->x + n->f1E == c->x + c->f1C) {
                        n->a = c;
                        c->b = n;
                        return;
                    }
                    break;
                case 2:
                    if (c->y + c->h == n->y && n->x + n->f1E == c->x + c->f1C) {
                        n->a = c;
                        c->b = n;
                        return;
                    }
                    break;
                case 1:
                    if (n->x + n->w == c->x && n->y + n->f1E == c->y + c->f1C) {
                        n->a = c;
                        c->b = n;
                        return;
                    }
                    break;
                case 3:
                    if (n->x == c->x + c->w && n->y + n->f1E == c->y + c->f1C) {
                        n->a = c;
                        c->b = n;
                        return;
                    }
                    break;
                }
            }
        } else {
            if (n->b == NULL && ((n->dirA - c->dirB) & 3) == 2) {
                switch (n->dirA) {
                case 0:
                    if (n->y + n->h == c->y && n->x + n->f1C == c->x + c->f1E) {
                        n->b = c;
                        c->a = n;
                        return;
                    }
                    break;
                case 2:
                    if (c->y + c->h == n->y && n->x + n->f1C == c->x + c->f1E) {
                        n->b = c;
                        c->a = n;
                        return;
                    }
                    break;
                case 1:
                    if (n->x + n->w == c->x && n->y + n->f1C == c->y + c->f1E) {
                        n->b = c;
                        c->a = n;
                        return;
                    }
                    break;
                case 3:
                    if (n->x == c->x + c->w && n->y + n->f1C == c->y + c->f1E) {
                        n->b = c;
                        c->a = n;
                        return;
                    }
                    break;
                }
            }
        }
    }
}

#define STKY(v) (((v) & 0xC) ? (((v) & 8) ? (((s32)(v) >> 24) & 0xF) : -(((s32)(v) >> 24) & 0xF)) : 0)
#define STKX(v) (((v) & 3) ? (((v) & 1) ? (s32)((v) >> 28) : -(s32)((v) >> 28)) : 0)
#define ABS(x) ((x) > 0 ? (x) : -(x))

#define CALC1(F, R, A, B, C, D)                                                          \
    {                                                                                    \
        f32 x = D_800CEA40_15->F;                                                           \
        s32 t = (s32)((x - A) * B / C + D);                                              \
        D_800CEA40_15->R = t;                                                               \
        fs = (f32)t * (f32)t + x * x;                                                    \
    }

#define CALC2(R, O)                                                                          {                                                                                            s32 s = (s32)__builtin_sqrtf(fs);                                                        D_800CEA40_15->O = (u16)func_80013010(((D_800CEA40_15->R << 14) / s) & 0x3FFF);            }

typedef struct Ctl_27 Ctl_27;

typedef struct Rec_48 {
    s32 f0;
    struct Rec_48 *next;
    struct Rec_48 *p8;
} Rec_48;

typedef struct {
    u8 pad0[0xC];
    u16 hC;
} Info_4;

typedef struct {
    u8 pad0[0x14];
    Info_4 *p14;
} Track_18;

typedef struct {
    u8 pad0[0x16C];
    s32 f16C;
    s32 f170;
    u32 f174;
    s32 f178;
    Rec_48 *f17C;
    Rec_48 *f180;
    Track_18 *f184;
    u8 pad188[0x190 - 0x188];
    s32 f190;
    u8 pad194[0x16E0 - 0x194];
    Ctl_27 *f16E0;
    Ctl_27 *f16E4;
    s32 f16E8;
    u8 pad16EC[0x16F6 - 0x16EC];
    u16 f16F6;
    u8 pad16F8[0x172C - 0x16F8];
    s32 f172C;
    s32 f1730;
    s32 f1734;
    s32 f1738;
    s32 f173C;
    s32 f1740;
    Track_18 *f1744;
    Track_18 *f1748;
    u8 pad174C[0x1758 - 0x174C];
    s32 f1758;
    s32 f175C;
    u8 pad1760[0x17B0 - 0x1760];
    void *f17B0;
} Edit_3;

typedef struct {
    s32 f0;
    u32 f4;
    s32 f8;
    s32 fC;
} Btn;

typedef struct {
    u8 pad0[0x54];
    u32 f54;
} Lbl;

typedef struct {
    u8 pad0[0xA];
    s16 hA;
    u8 padC[4];
} Sel_5;

typedef struct {
    u8 pad0[8];
    Sel_5 *p8;
    u8 padC[4];
    s16 h10;
} Menu_8;

extern Edit_3 *D_800CEA40_15;
extern Btn D_800CEA88[];
extern Lbl D_800CEB1C_2;
extern Menu_8 D_800CEB9C_4;

void func_800949BC(void) {
    Edit_3 *g;
    Rec_48 *p;
    f32 fs;

    g = D_800CEA40_15;
    if (g->f1744 != g->f184) {
        g->f1748 = g->f1744;
        g->f1744 = g->f184;
    }
    if ((D_800E30B0 & 4) && ABS(STKY(D_800E30B0)) >= 3) {
        D_800E30B0 |= 0x2000;
    }
    if ((D_800E30B0 & 8) && ABS(STKY(D_800E30B0)) >= 3) {
        D_800E30B0 |= 0x4000;
    }
    if ((D_800E30B0 & 1) && ABS(STKX(D_800E30B0)) >= 3) {
        D_800E30B0 |= 0x800;
    }
    if ((D_800E30B0 & 2) && ABS(STKX(D_800E30B0)) >= 3) {
        D_800E30B0 |= 0x1000;
    }
    if ((D_800E30B0 & 0x10) && !(D_800E25B0 & 0x10)) {
        D_800CEA40_15->f174 |= 0x10000;
    } else if (D_800CEA40_15->f174 & 0x10000) {
        if (((D_800E30B0 & 0x20) && !(D_800E25B0 & 0x20)) || !(D_800E30B0 & 0x10)) {
            D_800CEA40_15->f174 &= ~0x10000;
        }
    }
    if (D_800CEA40_15->f1758 != 0) {
        D_800CEA40_15->f1738 += D_800CEA40_15->f175C / 50;
        D_800CEA40_15->f1758--;
        CALC1(f1738, f1740, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
        CALC2(f1740, f173C);
    } else if (D_800CEA40_15->f184 != 0) {
        if (D_800E30B0 & 0x80) {
            if (D_800CEA40_15->f170 == 6) {
                D_800CEA40_15->f1738 -= 600;
                if (D_800CEA40_15->f1738 < 20000) {
                    D_800CEA40_15->f1738 = 20000;
                }
                CALC1(f1738, f1740, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
                CALC2(f1740, f173C);
            } else {
                D_800CEA40_15->f172C -= 600;
                if (D_800CEA40_15->f172C < 20000) {
                    D_800CEA40_15->f172C = 20000;
                }
                CALC1(f172C, f1734, 20000.0f, 38000.0f, 165000.0f, 40000.0f);
                CALC2(f1734, f1730);
            }
        }
        if (D_800E30B0 & 0x100) {
            if (D_800CEA40_15->f170 == 6) {
                D_800CEA40_15->f1738 += 600;
                if (D_800CEA40_15->f1738 > 120000) {
                    D_800CEA40_15->f1738 = 120000;
                }
                CALC1(f1738, f1740, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
                CALC2(f1740, f173C);
            } else {
                D_800CEA40_15->f172C += 600;
                if (D_800CEA40_15->f172C > 185000) {
                    D_800CEA40_15->f172C = 185000;
                }
                CALC1(f172C, f1734, 20000.0f, 38000.0f, 165000.0f, 40000.0f);
                CALC2(f1734, f1730);
            }
        }
    }
    if (D_800CEA40_15->f184 != 0) {
        if ((D_800E30B0 & 0x200) && !(D_800E25B0 & 0x200)) {
            if (D_800CEA40_15->f174 & 2) {
                D_800CEA40_15->f174 &= ~2;
            } else if (D_800CEA40_15->f174 & 4) {
                D_800CEA40_15->f174 &= ~4;
            } else {
                D_800CEA40_15->f174 |= 2;
            }
        }
        if ((D_800E30B0 & 0x400) && !(D_800E25B0 & 0x400)) {
            if (D_800CEA40_15->f174 & 4) {
                D_800CEA40_15->f174 &= ~4;
            } else if (D_800CEA40_15->f174 & 2) {
                D_800CEA40_15->f174 &= ~2;
            } else {
                D_800CEA40_15->f174 |= 4;
            }
        }
    }
    D_800CEB54[0] = D_800CEA40_15->f16F6;
    if (D_800CEA40_15->f16E0 != NULL) {
        func_80090398(D_800CEA40_15->f16E0);
    }
    if (D_800CE240 == 0) {
        func_80090670();
    }
    if (D_800CE240 != 0) {
        func_80090560();
    }
    if (D_800CEA40_15->f184 == NULL) {
        func_8009B820();
    }
    if (D_800CEA40_15->f184 == NULL) {
        D_800CEA88[0].f4 &= ~0x10;
        D_800CEA88[6].f4 &= ~0x10;
    } else {
        D_800CEA88[0].f4 |= 0x10;
        D_800CEA88[6].f4 |= 0x10;
    }
    if (D_800CEA40_15->f184 != NULL && (D_800CEA40_15->f184->p14->hC & 2) && D_800CEA40_15->f190 != 0) {
        D_800CEB1C_2.f54 |= 0x10;
    } else {
        D_800CEB1C_2.f54 &= ~0x10;
    }
    if (D_800CEA40_15->f190 != 0) {
        D_800CEA88[4].f4 |= 0x10;
        D_800CEA88[5].f4 |= 0x10;
    } else {
        D_800CEA88[4].f4 &= ~0x10;
        D_800CEA88[5].f4 &= ~0x10;
    }
    if (D_800CEA40_15->f190 != 0 && D_800CEA40_15->f17C->p8 == D_800CEA40_15->f180) {
        D_800CEA88[3].f4 |= 0x10;
    } else {
        D_800CEA88[3].f4 &= ~0x10;
    }
    if (D_800CEA40_15->f16E0 != NULL) {
        func_80090398(D_800CEA40_15->f16E0);
    }
    if (D_800CEA40_15->f16E8 == 3 && D_800CEA40_15->f16E4 != NULL) {
        D_800CEA40_15->f16E8 = 1;
        D_800CEA40_15->f16E0 = D_800CEA40_15->f16E4;
        D_800CEA40_15->f16E4 = NULL;
    }
    switch (D_800CEB9C_4.h10) {
    case 0:
        D_800CEA40_15->f17B0 = &D_800CE280[D_800CEB9C_4.p8[0].hA];
        break;
    case 1:
        D_800CEA40_15->f17B0 = &D_800CE300[D_800CEB9C_4.p8[1].hA];
        break;
    case 2:
        D_800CEA40_15->f17B0 = &D_800CE400[D_800CEB9C_4.p8[2].hA];
        break;
    case 3:
        D_800CEA40_15->f17B0 = &D_800CE700[D_800CEB9C_4.p8[3].hA];
        break;
    }
    if (D_800CEA40_15->f174 & 0x10) {
        if (++D_800CEA40_15->f178 >= 0) {
            if (D_800CEA40_15->f174 & 0x400) {
                func_800A4A20();
            }
            D_800C29CC->f10 = 3;
        }
    } else {
        switch (D_800CEA40_15->f16C) {
        case 0:
            func_80091AFC();
            break;
        case 1:
            func_80092908();
            break;
        }
    }
    p = D_800CEA40_15->f17C;
    if (p != NULL) {
        do {
            func_80096B3C(p);
            p = p->next;
        } while (p != NULL && p != D_800CEA40_15->f17C);
    }
}

#define RAW(g, a, b) { Gfx *_g = (Gfx *)(g); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct {
    char pad0[0x16C5];
    u8 name[11];
    s32 sel;
    char pad1[0x8];
    s32 mode;
    char pad2[0x38];
    s32 *font;
} Menu_9;

typedef struct {
    char pad0[4];
    u8 *img;
} Res_8;

extern Res_8 *D_800CE200_5;
extern Menu_9 *D_800CEA40_16;

Gfx *func_8009559C(Gfx *gdl, s32 arg1)
{
    s32 u, j, m, i, k, y0, xo, t, yoff, yy, idx, x, c, s, f;
    u8 *p;
    Menu_9 *mp;
    u8 buf[8];

    RAW(gdl++, 0xE3000C00, 0);
    RAW(gdl++, 0xFA000000, 0xFAFAFAFF);
    RAW(gdl++, 0xFC119623, 0xFF2FFFFF);
    RAW(gdl++, 0xE3001001, 0);
    RAW(gdl++, 0xE200001C, 0x00404240);
    RAW(gdl++, 0xD7000002, 0x80008000);
    p = D_800CE200_5->img + 0x48;
    gdl = func_8000A690(0xBC, 0x78 - (p[1] >> 1), p, gdl, arg1, 1, 1, 6, 0);
    func_800C1E60(D_800CEA40_16->font, 0x40, 2, 2, 0x20);
    func_800C1F2C(D_800CEA40_16->font, D_800DABE8);
    func_800C0ACC(D_800CEA40_16->font, 0xCC, 0xA7);
    c = -0xF2;
    for (i = 0; i < 5; i++) {
        for (k = 0, idx = i << 3; k < 8; k++) {
            mp = D_800CEA40_16;
            s = 1;
            if (idx >= 0x26) s = 2;
            f = 2;
            if (idx == mp->sel) f = 7;
            if (mp->mode == 10 && idx < 0x26) func_800C1E60(mp->font, 0x20, s, f, 0x20000021);
            else func_800C1E60(mp->font, 0x20, s, f, 0x21);
            func_800C1F2C(D_800CEA40_16->font, D_800CD1AC[idx]);
            x = k * 26 - (i * 5 + c);
            func_800C0ACC(D_800CEA40_16->font, x, i * 17 + 0x48);
            if (idx == D_800CEA40_16->sel) {
                p = D_800CE200_5->img + 0x50;
                gdl = func_8000A690(x - p[0] * 4 + 4, i * 17 - (p[1] >> 1) + 0x49, p, gdl, arg1, 1, 1, 6, 0);
                do { } while (0);
                idx++;
            } else {
                idx++;
            }
        }
    }
    for (j = 0; j < 10; j++) {
        if (D_800CEA40_16->name[j] == 0) break;
        f = 2;
        if (j == D_800CEA40_16->mode) f = 7;
        func_800C1E60(D_800CEA40_16->font, 0x20, 2, f, 0x21);
        buf[0] = D_800CEA40_16->name[j];
        buf[1] = 0;
        func_800C1F2C(D_800CEA40_16->font, buf);
        func_800C0ACC(D_800CEA40_16->font, j * 13 + 0x121, 0xA5);
    }
    for (j = 0; j < 10; j++) {
        f = 2;
        if (j == D_800CEA40_16->mode) f = 7;
        func_800C1E60(D_800CEA40_16->font, 0x20, 1, f, 0x21);
        func_800C1F2C(D_800CEA40_16->font, D_800DABF8);
        func_800C0ACC(D_800CEA40_16->font, j * 13 + 0x120, 0xAB);
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    s16 f0;
    s16 n;              /* 0x02 */
    s32 f4;
    s32 f8;
    u16 fC;             /* 0x0C */
    u16 fE;
    u8 pad10[0xC];
    char *name;         /* 0x1C */
    u8 pad20[0x20];
} Trk_15;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    Trk_15 *trk;           /* 0x14 */
} Sel_6;

typedef struct {
    u8 pad0[0x170];
    u32 mode;           /* 0x170 */
    u8 pad174[0x10];
    Sel_6 *sel;           /* 0x184 */
    u8 pad188[0x16BA - 0x188];
    char name[0x26];    /* 0x16BA */
    s32 f16E0;
    s32 f16E4;
    u32 f16E8;
    s32 f16EC;
    s32 f16F0;
    s32 f16F4;
    s32 f16F8;
    s32 f16FC;
    u8 pad1700[0x18];
    s32 *font2;         /* 0x1718 */
    s32 *font;          /* 0x171C */
    u8 pad1720[0x80];
    Vp vp0;             /* 0x17A0 */
    u8 pad17B0[0x18];
    Vp vp[3];           /* 0x17C8 */
    u8 pad17F8[0xC];
    Pic *pic1804;       /* 0x1804 */
    Pic *pic1808;       /* 0x1808 */
} Menu_10;

typedef struct {
    s32 f0;
    u8 *img;
} Res_9;

extern Menu_10 *D_800CEA40_17;
extern Res_9 *D_800CE200_6;
extern Trk_15 D_800CE280_6[];

void func_800959CC(s32 arg0, s32 arg1, Gfx **pgdl)
{
    char buf[0x68];
    char *s;
    Gfx *gdl;
    s32 i;
    u8 *p;
    u8 *img;
    char *str;
    Menu_10 *m;
    Trk_15 *t;
    Trk_15 *e;

    D_800F55B0 = arg0;
    switch (D_800CEA40_17->f16E8) {
    case 1:
        D_800CEA40_17->f16F0 = 0;
        break;
    case 2:
        D_800CEA40_17->f16F0 = 30 - D_800CEA40_17->f16EC;
        break;
    case 0:
        D_800CEA40_17->f16F0 = D_800CEA40_17->f16EC;
        break;
    case 3:
        D_800CEA40_17->f16F0 = 30;
        break;
    }
    D_800CEA40_17->f16F0 *= 4;
    D_800CE9C0[1].x = D_800CEA00[1].x - D_800CEA40_17->f16F0;
    D_800CE9C0[1].w = D_800CEA00[1].w + D_800CEA40_17->f16F0;
    D_800CE9C0[2].x = D_800CEA00[2].x - D_800CEA40_17->f16F0;
    D_800CE9C0[3].x = D_800CEA00[3].x - D_800CEA40_17->f16F0;
    func_8008EFFC(D_800CE9C0[1].x, D_800CE9C0[1].y, D_800CE9C0[1].w - 1, D_800CE9C0[1].h - 1, D_800CEA40_17->f16F0);
    D_800CEA40_17->vp[0].vp.vscale[0] = D_800CEA00[1].w * 2;
    D_800CEA40_17->vp[0].vp.vscale[1] = D_800CEA00[1].h * 2;
    D_800CEA40_17->vp[0].vp.vtrans[0] = (D_800CEA00[1].x + D_800CEA00[1].w / 2) * 4;
    D_800CEA40_17->vp[0].vp.vtrans[1] = (D_800CEA00[1].y + D_800CEA00[1].h / 2) * 4;
    D_800CEA40_17->vp[1].vp.vscale[0] = D_800CE9C0[2].w * 2;
    D_800CEA40_17->vp[1].vp.vscale[1] = D_800CE9C0[2].h * 2;
    D_800CEA40_17->vp[1].vp.vtrans[0] = (D_800CE9C0[2].x + D_800CE9C0[2].w / 2) * 4;
    D_800CEA40_17->vp[1].vp.vtrans[1] = (D_800CE9C0[2].y + D_800CE9C0[2].h / 2) * 4;
    D_800CEA40_17->vp[2].vp.vscale[0] = D_800CE9C0[3].w * 2;
    D_800CEA40_17->vp[2].vp.vscale[1] = D_800CE9C0[3].h * 2;
    D_800CEA40_17->vp[2].vp.vtrans[0] = (D_800CE9C0[3].x + D_800CE9C0[3].w / 2) * 4;
    D_800CEA40_17->vp[2].vp.vtrans[1] = (D_800CE9C0[3].y + D_800CE9C0[3].h / 2) * 4;

    gdl = *pgdl;
    gdl = func_8000B9CC(gdl, arg1, arg0, D_800A9D00, D_800D15C0);
    gdl = func_80008B1C(gdl, arg1, arg0);
    gDPSetTextureFilter(gdl++, G_TF_AVERAGE);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gSPClipRatio(gdl++, FRUSTRATIO_2);
    gSPViewport(gdl++, &D_800CEA40_17->vp0);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, D_800C29E0);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29E0);
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[arg1]);
    gDPSetFillColor(gdl++, 0);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[arg1]);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800CE9C0[0].x, D_800CE9C0[0].y,
                  D_800CE9C0[0].x + D_800CE9C0[0].w - 1, D_800CE9C0[0].y + D_800CE9C0[0].h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPPipeSync(gdl++);
    gDPSetFillColor(gdl++, 0x00010001);
    gDPFillRectangle(gdl++, D_800CE9C0[2].x, D_800CE9C0[2].y,
                     D_800CE9C0[2].x + D_800CE9C0[2].w - 1, D_800CE9C0[2].y + D_800CE9C0[2].h - 1);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gdl = func_8008FB08(arg1, gdl, D_800CEA40_17->f16E0, 0x1A, 0x11);
    gdl = func_800978A8(arg0, arg1, gdl);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xB4, 0xB4, 0xB4, 0xFF);

    switch (D_800CEA40_17->mode) {
    case 10:
    case 11:
        func_800C1E60(D_800CEA40_17->font, 0x20, 4, 2, 0x21);
        func_800C1F2C(D_800CEA40_17->font, "Warning");
        func_800C0ACC(D_800CEA40_17->font, 0x14C, 0x48);
        for (i = 0;; i++) {
            if (*(D_800CEA40_17->mode == 10 ? &D_800CEBB0[i] : &D_800CEBCC[i]) == NULL) break;
            func_800C1E60(D_800CEA40_17->font, 0x20, 5, 2, 0x21);
            func_800C1F2C(D_800CEA40_17->font, *(D_800CEA40_17->mode == 10 ? &D_800CEBB0[i] : &D_800CEBCC[i]));
            func_800C0ACC(D_800CEA40_17->font, 0x144 - i * 4, i * 12 + 0x61);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        img = D_800CEA40_17->pic1808->img;
        gDPLoadTLUT_pal256(gdl++, D_800CEA40_17->pic1808->pal);
        gdl = func_8000A690(0xD0, 0x3D, img, gdl, arg1, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        gdl = func_8000A690(0xB6, 0x3A, D_800CEA40_17->pic1804->img, gdl, arg1, 1, 1, 6, 0);
        break;
    }

    if (D_800CEA40_17->sel != NULL) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = D_800CE200_6->img + 0x58;
        func_800C1E60(D_800CEA40_17->font, 0x20, 5, 2, 0x21);
        func_800C1F2C(D_800CEA40_17->font, D_800DABE8);
        func_800C1F2C(D_800CEA40_17->font, D_800CEA40_17->name);
        func_800C0ACC(D_800CEA40_17->font, 0x140, 0x19);
        for (i = 0; i < (0x207 - D_800CE9C0[1].x) / (p[0] * 8); i++) {
            gdl = func_8000A690(D_800CE9C0[1].x + p[0] * (i << 3), 0x11, p, gdl, arg1, 1, 1, 6, 0);
        }
        if (D_800CEA40_17->mode == 9 && (D_800E2038 & 0x20)) {
            gDPSetPrimColor(gdl++, 0, 0, 0xB4, 0xB4, 0x50, 0xFF);
        }
        for (i = 0; i < (0x207 - D_800CE9C0[1].x) / (p[0] * 8); i++) {
            gdl = func_8000A690(D_800CE9C0[1].x + p[0] * (i << 3), 0xCD, p, gdl, arg1, 1, 1, 6, 0);
        }
        gDPPipeSync(gdl++);
        func_800C1E60(D_800CEA40_17->font2, 0x40, 5, 2, 0x21);
        i = D_800CEA40_17->sel->trk - D_800CE280_6;
        if (D_800CEA40_17->mode == 9 && D_800CEA40_17->sel != NULL &&
            (D_800CEA40_17->sel->f4 != 0 || D_800CEA40_17->sel->f8 != 0)) {
            func_800C1F2C(D_800CEA40_17->font2, "< ");
        }
        if (i < 2) {
            func_800C1F2C(D_800CEA40_17->font2, "Straight ");
            i += 1;
        } else if (i < 6) {
            func_800C1F2C(D_800CEA40_17->font2, "Turn ");
            i -= 1;
        } else if (i < 18) {
            func_800C1F2C(D_800CEA40_17->font2, "Jump ");
            i -= 5;
        } else {
            i -= 17;
        }
        if (D_800CEA40_17->sel->trk - D_800CE280_6 < 18) {
            s = buf + 0x48;
            func_8000EE30(i, s);
            func_800C1F2C(D_800CEA40_17->font2, s);
            func_800C1F2C(D_800CEA40_17->font2, " : ");
        }
        func_800C1F2C(D_800CEA40_17->font2, D_800CEA40_17->sel->trk->name);
        if (D_800CEA40_17->sel->trk - D_800CE280_6 >= 18) {
            if (D_800CEA40_17->mode == 9) {
                func_800C1F2C(D_800CEA40_17->font2, " - Press Z Button To Edit");
            } else if (D_800CEA40_17->mode == 6 && D_800CE280_6[D_800CEA40_17->f16FC + 18].n >= 5 &&
                       !(D_800CE280_6[D_800CEA40_17->f16FC + 18].fC & 0x200) &&
                       (D_800CEA40_17->f16F8 == 1 || D_800CEA40_17->f16F8 == D_800CE280_6[D_800CEA40_17->f16FC + 18].n * 2 - 1)) {
                func_800C1F2C(D_800CEA40_17->font2, " - R Button Deletes Segment");
            }
        } else if (D_800CEA40_17->mode == 9 && (*(u32 *)&D_800CEA40_17->sel->trk->fC & 0x300000) == 0x300000) {
            func_800C1F2C(D_800CEA40_17->font2, " - R Button Flips");
        }
        if (D_800CEA40_17->mode == 9 && D_800CEA40_17->sel != NULL &&
            (D_800CEA40_17->sel->f4 != 0 || D_800CEA40_17->sel->f8 != 0)) {
            func_800C1F2C(D_800CEA40_17->font2, " >");
        }
        func_800C0ACC(D_800CEA40_17->font2, 0x142, 0xD5);
    }
    if (D_800CEA40_17->mode == 7 || D_800CEA40_17->mode == 8) {
        gdl = func_8009559C(gdl, arg1);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800CE9C0[0].x, D_800CE9C0[0].y,
                  D_800CE9C0[0].x + D_800CE9C0[0].w - 1, D_800CE9C0[0].y + D_800CE9C0[0].h - 1);
    gdl = func_800C1394(gdl);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

typedef struct { s16 p[2]; s16 d[2]; s16 l[2]; } Seg_18;

struct Rec_49 {
    u8 pad[0x10];
    u16 flags;
    s16 off;
    Seg_18 *tab;
    s16 n0;
    s16 n1;
    s16 o1c;
    s16 o1e;
    s16 o20;
    s16 o22;
};

void func_80096B3C(Rec_49 *r)
{
    s32 a, b;

    if (r == NULL) return;
    if (r->flags & 2) {
        b = 0;
        a = 1;
    } else {
        a = 0;
        b = 1;
    }
    r->n0 = (r->tab->d[a] + r->off) & 3;
    r->n1 = (r->tab->d[b] + r->off) & 3;

    switch (r->n0) {
    case 0:
        switch (r->tab->d[a]) {
        case 0: case 3: r->o1c = r->tab->l[a]; break;
        case 2: r->o1c = r->tab->p[0] - r->tab->l[a] - 2; break;
        case 1: r->o1c = r->tab->p[1] - r->tab->l[a] - 2; break;
        }
        break;
    case 2:
        switch (r->tab->d[a]) {
        case 1: case 2: r->o1c = r->tab->l[a]; break;
        case 0: r->o1c = r->tab->p[0] - r->tab->l[a] - 2; break;
        case 3: r->o1c = r->tab->p[1] - r->tab->l[a] - 2; break;
        }
        break;
    case 3:
        switch (r->tab->d[a]) {
        case 0: case 3: r->o1c = r->tab->l[a]; break;
        case 2: r->o1c = r->tab->p[0] - r->tab->l[a] - 2; break;
        case 1: r->o1c = r->tab->p[1] - r->tab->l[a] - 2; break;
        }
        break;
    case 1:
        switch (r->tab->d[a]) {
        case 1: case 2: r->o1c = r->tab->l[a]; break;
        case 0: r->o1c = r->tab->p[0] - r->tab->l[a] - 2; break;
        case 3: r->o1c = r->tab->p[1] - r->tab->l[a] - 2; break;
        }
        break;
    }

    switch (r->n1) {
    case 0:
        switch (r->tab->d[b]) {
        case 0: case 3: r->o1e = r->tab->l[b]; break;
        case 2: r->o1e = r->tab->p[0] - r->tab->l[b] - 2; break;
        case 1: r->o1e = r->tab->p[1] - r->tab->l[b] - 2; break;
        }
        break;
    case 2:
        switch (r->tab->d[b]) {
        case 1: case 2: r->o1e = r->tab->l[b]; break;
        case 0: r->o1e = r->tab->p[0] - r->tab->l[b] - 2; break;
        case 3: r->o1e = r->tab->p[1] - r->tab->l[b] - 2; break;
        }
        break;
    case 3:
        switch (r->tab->d[b]) {
        case 0: case 3: r->o1e = r->tab->l[b]; break;
        case 2: r->o1e = r->tab->p[0] - r->tab->l[b] - 2; break;
        case 1: r->o1e = r->tab->p[1] - r->tab->l[b] - 2; break;
        }
        break;
    case 1:
        switch (r->tab->d[b]) {
        case 1: case 2: r->o1e = r->tab->l[b]; break;
        case 0: r->o1e = r->tab->p[0] - r->tab->l[b] - 2; break;
        case 3: r->o1e = r->tab->p[1] - r->tab->l[b] - 2; break;
        }
        break;
    }

    if (r->off & 1) {
        r->o20 = r->tab->p[1];
        r->o22 = r->tab->p[0];
    } else {
        r->o20 = r->tab->p[0];
        r->o22 = r->tab->p[1];
    }
}

typedef struct { s16 x, y, z; u16 f; s16 s, t; u8 r, g, b, a; } V_3;
typedef struct { u8 pad[0x174C]; V_3 *v[2]; s32 count; } G_5;

extern G_5 *D_800CEA40_18;

void func_80096FB0(s32 idx, s32 x0, s32 y0, s16 z0, s32 x1, s32 y1, s32 z1, u8 r, u8 g, u8 b)
{
    s32 ax, ay, ox, oy;

    x0 = x0 - D_800F13EC / 2;
    x1 = x1 - D_800F13EC / 2;
    y0 = D_800F13F0 / 2 - y0;
    y1 = -y1;
    y1 += D_800F13F0 / 2;
    if (D_800CEA40_18->count >= 0x3C) {
        return;
    }
    ax = x1 - x0;
    if (ax <= 0) ax = x0 - x1;
    ay = y1 - y0;
    if ((ay <= 0 ? y0 - y1 : ay) < ax) {
        ox = 0;
        oy = 2;
    } else {
        ox = 3;
        oy = 0;
    }
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 0].x = x0;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 0].y = y0;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 0].z = z0;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 1].x = x0 + ox;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 1].y = y0 + oy;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 1].z = z0;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 2].x = x1;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 2].y = y1;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 2].z = z1;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 3].x = x1 + ox;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 3].y = y1 + oy;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 3].z = z1;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 0].r = r;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 0].g = g;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 0].b = b;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 1].r = r;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 1].g = g;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 1].b = b;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 2].r = r;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 2].g = g;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 2].b = b;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 3].r = r;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 3].g = g;
    D_800CEA40_18->v[idx][D_800CEA40_18->count * 4 + 3].b = b;
    D_800CEA40_18->count++;
}

typedef struct { u8 pad[0x174C]; s32 vtx[2]; s32 n; } Obj_70;

extern Obj_70 *D_800CEA40_19;

Gfx *func_80097308(u8 *a0, s32 arg1, Gfx *gdl)
{
    Mtx *m;
    s32 i;

    m = (Mtx *)(a0 + 0x300);
    guOrtho(m, -D_800F13EC / 2, D_800F13EC / 2, -D_800F13F0 / 2, D_800F13F0 / 2, 50.0f,
                  D_800C23F0, 1.0f);
    gSPMatrix(gdl++, &D_800C2678, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, m, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPViewport(gdl++, &D_800C25E8);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800CE9C0_2.x, D_800CE9C0_2.y,
                  D_800CE9C0_2.x + D_800CE9C0_2.w - 1, D_800CE9C0_2.y + D_800CE9C0_2.h - 1);
    gSPClearGeometryMode(gdl++, 0x3F0604);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gDPSetCombineMode(gdl++, G_CC_SHADE, G_CC_SHADE);
    gDPSetRenderMode(gdl++, 0x00442078, 0);
    for (i = 0; i < D_800CEA40_19->n; i++) {
        gSPVertex(gdl++, D_800CEA40_19->vtx[arg1] + i * 0x40, 4, 0);
        gDPPipeSync(gdl++);
        gSP2Triangles(gdl++, 0, 1, 3, 0, 0, 3, 2, 0);
        gDPPipeSync(gdl++);
    }
    return gdl;
}

typedef struct {
    u8 unk0[0x1760];
    f32 mtx[4][4];
} Camera;

extern Camera *D_800CEA40_20;

s32 func_80097658(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, s32 *sz, s32 *sw, s32 arg7, s32 arg8) {
    f32 hx;
    f32 hy;
    f32 tx;
    f32 ty;
    f32 px;
    f32 py;
    f32 pz;
    f32 pw;

    hx = D_800F4FD8_2.vscale[0] * 0.25f;
    hy = D_800F4FD8_2.vscale[1] * 0.25f;
    x *= 0.0625f;
    y *= 0.0625f;
    z *= 0.0625f;
    px = D_800CEA40_20->mtx[0][0] * x + D_800CEA40_20->mtx[1][0] * y + D_800CEA40_20->mtx[2][0] * z + D_800CEA40_20->mtx[3][0];
    py = D_800CEA40_20->mtx[0][1] * x + D_800CEA40_20->mtx[1][1] * y + D_800CEA40_20->mtx[2][1] * z + D_800CEA40_20->mtx[3][1];
    pz = D_800CEA40_20->mtx[0][2] * x + D_800CEA40_20->mtx[1][2] * y + D_800CEA40_20->mtx[2][2] * z + D_800CEA40_20->mtx[3][2];
    pw = D_800CEA40_20->mtx[0][3] * x + D_800CEA40_20->mtx[1][3] * y + D_800CEA40_20->mtx[2][3] * z + D_800CEA40_20->mtx[3][3];
    tx = D_800F4FD8_2.vtrans[0] * 0.25f;
    ty = D_800F4FD8_2.vtrans[1] * 0.25f;
    if (pw == 0.0f) {
        return 1;
    }
    px /= pw;
    py /= pw;
    pz /= pw;
    if (pz <= -1.0f || pz >= 1.0f) {
        return 1;
    }
    {
        f32 rx, ry, rz;
        rx = px * hx + tx;
        ry = py * -hy + ty;
        rz = pz * (D_800C23F0 - 50.0f) + 50.0f;
        *sx = rx;
        *sy = ry;
        *sz = rz;
    }
    if (sw != NULL) {
        *sw = pw;
    }
    return 0;
}

#define gDPRaw(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }
#define SINT(i) ((s16)D_800C2C60[(i) & 0xFFF] >> 4)
#define CellAt(d, k) ((Cell_9 *)((u8 *)(d)->cells + ((k) << 5)))
#define EntAt(d, k) ((Ent_85 *)((u8 *)(d)->ents + ((k) << 5)))
#define W (o->def->dim[0])
#define H (o->def->dim[1])
#define K0 (j * W + i)
#define K1 (i * W + W - 1 - j)
#define K2 ((H - 1 - j) * W + W - 1 - i)
#define K3 ((H - 1 - i) * W + j)
#define E D_800CEA40_21
#define XB(a, b) (E->sel->x * 8000 - E->map->a * 4000)
#define ZB(a) (E->sel->y * 8000 - E->map->a * 4000)

typedef struct Cell_9 {
    s32 f0;
    s32 f4;
    s32 pad8[6];
} Cell_9;

typedef struct Ent_85 {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
    s32 pad18[2];
} Ent_85;

typedef struct Def {
    s16 dim[2];
    u8 pad4[8];
    u16 flags;
    s16 fE;
    u8 pad10[0x20];
    Cell_9 *cells;
    Ent_85 *ents;
} Def;

typedef struct Obj_71 {
    s32 f0;
    struct Obj_71 *next;
    struct Obj_71 *prev;
    s16 x;
    s16 y;
    u16 flags;
    s16 rot;
    Def *def;
    u8 pad18[8];
    s16 w;
    s16 h;
} Obj_71;

typedef struct {
    s32 dist;
    s32 ry;
    s32 y;
} Cam_7;

typedef struct {
    u8 pad0[0x170];
    s32 mode;
    s32 flags;
    s32 pad178;
    Obj_71 *head;
    s32 pad180;
    Obj_71 *sel;
    Obj_71 *mark0;
    Obj_71 *mark1;
    Obj_71 *cur;
    u8 pad194[0x16AC - 0x194];
    Dim *map;
    u8 pad16B0[0x16F8 - 0x16B0];
    s32 f16F8;
    s32 f16FC;
    s16 f1700;
    u8 pad1702[0x172C - 0x1702];
    Cam_7 cam[2];
    u8 pad1744[0x174C - 0x1744];
    void *buf[2];
    s32 n;
    u8 pad1758[8];
    f32 mtx[4][4];
    u8 pad17A0[0x17C8 - 0x17A0];
    Vp vp;
    u8 pad17D8[0x17FC - 0x17D8];
    s16 f17FC;
    s16 pad17FE;
    s16 f1800;
} Edit_4;

typedef struct {
    s32 x;
    s32 z;
    u32 ang;
} CamPos_2;

extern Edit_4 *D_800CEA40_21;
extern CamPos_2 D_800CE21C_4;

Gfx *func_800978A8(s32 arg0, s32 arg1, Gfx *gdl) {
    s32 p0[3];
    s32 p1[3];
    s32 p2[3];
    s32 p3[3];
    Obj_71 *o;
    s32 flag;
    s32 sel;
    s32 i;
    s32 j;
    s32 k;
    s32 n;
    s32 x;
    s32 y;
    s16 rot;
    s32 fl;
    u32 a;
    s32 r;

    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPPipeSync(gdl++);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800CE9C0_2.x, D_800CE9C0_2.y, D_800CE9C0_2.x + D_800CE9C0_2.w - 1,
                  D_800CE9C0_2.y + D_800CE9C0_2.h - 1);
    gSPViewport(gdl++, &E->vp);
    gSPClearGeometryMode(gdl, G_ZBUFFER);
    gDPRaw(gdl++, 0xE200001C, 0x0C084000);
    gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 1);
    gDPFillRectangle(gdl++, D_800CE9C0_2.x, D_800CE9C0_2.y, D_800CE9C0_2.x + D_800CE9C0_2.w - 1,
                     D_800CE9C0_2.y + D_800CE9C0_2.h - 1);
    gDPPipeSync(gdl++);
    gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
    gDPRaw(gdl++, 0xE200001C, 0x004041C8);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);

    if (E->mode == 4) {
        func_8008D4C4_2(arg1, 0, 250000, 0, 0, 0x4000);
    } else if (E->mode == 6) {
        a = D_800CE21C_4.ang;
        func_8008D4C4_2(arg1, D_800CE21C_4.x + ((SINT((a >> 4) - 0x400) * E->cam[1].dist) >> 10), E->cam[1].y,
                      D_800CE21C_4.z - ((SINT(a >> 4) * E->cam[1].dist) >> 10), a, E->cam[1].ry);
    } else {
        a = D_800CE21C_4.ang;
        func_8008D4C4_2(arg1, D_800CE21C_4.x + ((SINT((a >> 4) - 0x400) * E->cam[0].dist) >> 10), E->cam[0].y,
                      D_800CE21C_4.z - ((SINT(a >> 4) * E->cam[0].dist) >> 10), a, E->cam[0].ry);
    }

    o = E->head;
    if (o != NULL) {
        do {
            if (E->sel != NULL && E->mode == 6 && (E->sel->def->flags & 0x40)) {
                fl = o->flags & ~8;
            } else {
                fl = o->flags | 8;
            }
            o->flags = fl;
            o = o->next;
        } while (o != NULL && o != E->head);
    }
    if (E->sel != NULL && E->mode == 6 && (E->sel->def->flags & 0x40)) {
        for (o = E->sel->next; o != NULL; o = o->next) {
            o->flags |= 8;
            if (o->def->flags & 0x800) {
                break;
            }
        }
        for (o = E->sel; o != NULL; o = o->prev) {
            o->flags |= 8;
            if (o->def->flags & 0x800) {
                break;
            }
        }
    }

    o = E->head;
    flag = 0;
    if (o != NULL) {
        do {
            if (E->mark0 != NULL && E->mark1 != NULL) {
                if (flag != 0 && o->prev == E->mark1) {
                    flag = 0;
                }
                if (o == E->mark0) {
                    flag = 1;
                }
            }
            sel = 0;
            if ((o->flags & 4) || o == E->sel) {
                sel = 1;
            }
            sel |= 2;
            for (j = 0; j < o->def->dim[(o->rot + 1) & 1]; j++) {
                for (i = 0; i < o->def->dim[(rot = o->rot) & 1]; i++) {
                    if (o->x + i < 0 || o->x + i >= E->map->w) {
                        continue;
                    }
                    if (o->y + j < 0 || o->y + j >= E->map->h) {
                        continue;
                    }
                    if (o->flags & 8) {
                        switch (rot) {
                        case 0:
                            func_8008D3B4(o->x + i, o->y + j, CellAt(o->def, K0)->f4, rot,
                                          o->def->ents ? EntAt(o->def, K0)->f4 : 0, o->rot, sel,
                                          o->def->ents ? (EntAt(o->def, K0)->f8 + EntAt(o->def, K0)->f10) / 2 - 250 - i * 500 : 0,
                                          o->def->ents ? (EntAt(o->def, K0)->fC + EntAt(o->def, K0)->f14) / 2 - 250 - j * 500 : 0);
                            break;
                        case 1:
                            func_8008D3B4(o->x + i, o->y + j, CellAt(o->def, K1)->f4, rot,
                                          o->def->ents ? EntAt(o->def, K1)->f4 : 0, o->rot, sel,
                                          o->def->ents ? (EntAt(o->def, K1)->fC + EntAt(o->def, K1)->f14) / 2 - 250 - i * 500 : 0,
                                          o->def->ents ? (o->h - j - 1) * 500 + (250 - (EntAt(o->def, K1)->f8 + EntAt(o->def, K1)->f10) / 2) : 0);
                            break;
                        case 2:
                            func_8008D3B4(o->x + i, o->y + j, CellAt(o->def, K2)->f4, rot,
                                          o->def->ents ? EntAt(o->def, K2)->f4 : 0, o->rot, sel,
                                          o->def->ents ? (o->w - i - 1) * 500 + (250 - (EntAt(o->def, K2)->f8 + EntAt(o->def, K2)->f10) / 2) : 0,
                                          o->def->ents ? (o->h - j - 1) * 500 + (250 - (EntAt(o->def, K2)->fC + EntAt(o->def, K2)->f14) / 2) : 0);
                            break;
                        case 3:
                            func_8008D3B4(o->x + i, o->y + j, CellAt(o->def, K3)->f4, rot,
                                          o->def->ents ? EntAt(o->def, K3)->f4 : 0, o->rot, sel,
                                          o->def->ents ? (o->w - i - 1) * 500 + (250 - (EntAt(o->def, K3)->fC + EntAt(o->def, K3)->f14) / 2) : 0,
                                          o->def->ents ? (EntAt(o->def, K3)->f8 + EntAt(o->def, K3)->f10) / 2 - 250 - j * 500 : 0);
                            break;
                        }
                    } else {
                        func_8008D3B4(o->x + i, o->y + j, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
            o = o->next;
        } while (o != NULL && o != E->head);
    }

    func_8008F07C(0, 0, D_800F13EC, D_800F13F0);
    if (E->cur != NULL) {
        r = ((-E->cur->rot * 2 - E->cur->def->fE) & 7) << 13;
        gdl = func_8008D728(arg1, gdl, E->sel->x, E->sel->y, E->sel->w, E->sel->h,
                            (E->cur->x * 2 + E->cur->w) * 4000 - 60000, (E->cur->y * 2 + E->cur->h) * 4000 - 60000,
                            (E->cur->def->flags & 2) ? r : -1, 1);
        if (E->mode == 6) {
            E->n = 0;
            guMtxL2F(E->mtx, &D_800F4F08[arg1]);
            func_8000ED10(E->buf[arg1], 0, 0xF00);
            if (E->sel != NULL && (E->sel->def->flags & 0x40)) {
                switch (E->sel->rot) {
                case 0:
                    for (k = 0; k < E->sel->def->dim[1] * 2 + 1; k++) {
                        func_80097658(XB(w, 0) + 2000, k != 0 ? D_800F55B8[E->f16FC][k - 1] * 16 : 0,
                                      ZB(h) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        func_80097658(XB(w, 0) + 14000, k != 0 ? D_800F55B8[E->f16FC][k - 1] * 16 : 0,
                                      ZB(h) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (E->f16F8 == k) {
                            switch (E->f1700) {
                            case 0:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (k > 0) {
                            func_80097658(XB(w, 0) + 2000, k != 1 ? D_800F55B8[E->f16FC][k - 2] * 16 : 0,
                                          ZB(h) + (k - 1) * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 2000, D_800F55B8[E->f16FC][k - 1] * 16,
                                          ZB(h) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 14000, k != 1 ? D_800F55B8[E->f16FC][k - 2] * 16 : 0,
                                          ZB(h) + (k - 1) * 4000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 14000, D_800F55B8[E->f16FC][k - 1] * 16,
                                          ZB(h) + k * 4000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (E->f16F8 == k || E->f16F8 == k - 1) {
                                switch (E->f1700) {
                                case 0:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 1:
                    for (k = 0; k < E->sel->def->dim[1] * 2 + 1; k++) {
                        func_80097658(XB(h, 0) + k * 4000, k != 0 ? D_800F55B8[E->f16FC][k - 1] * 16 : 0,
                                      ZB(w) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        func_80097658(XB(h, 0) + k * 4000, k != 0 ? D_800F55B8[E->f16FC][k - 1] * 16 : 0,
                                      ZB(w) + 14000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (E->f16F8 == k) {
                            switch (E->f1700) {
                            case 0:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (k > 0) {
                            func_80097658(XB(h, 0) + (k - 1) * 4000, k != 1 ? D_800F55B8[E->f16FC][k - 2] * 16 : 0,
                                          ZB(w) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + k * 4000, D_800F55B8[E->f16FC][k - 1] * 16,
                                          ZB(w) + 2000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + (k - 1) * 4000, k != 1 ? D_800F55B8[E->f16FC][k - 2] * 16 : 0,
                                          ZB(w) + 14000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + k * 4000, D_800F55B8[E->f16FC][k - 1] * 16,
                                          ZB(w) + 14000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (E->f16F8 == k || E->f16F8 == k - 1) {
                                switch (E->f1700) {
                                case 0:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 2:
                    for (k = 0; k < E->sel->def->dim[1] * 2 + 1; k++) {
                        n = E->sel->def->dim[1] * 2 - k;
                        func_80097658(XB(w, 0) + 2000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                      ZB(h) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        func_80097658(XB(w, 0) + 14000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                      ZB(h) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (E->f16F8 == n) {
                            switch (E->f1700) {
                            case 0:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (n < E->sel->def->dim[1] * 2) {
                            func_80097658(XB(w, 0) + 2000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                          ZB(h) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 2000, D_800F55B8[E->f16FC][n] * 16,
                                          ZB(h) + (k - 1) * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 14000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                          ZB(h) + k * 4000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            func_80097658(XB(w, 0) + 14000, D_800F55B8[E->f16FC][n] * 16,
                                          ZB(h) + (k - 1) * 4000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (E->f16F8 == n || E->f16F8 == n + 1) {
                                switch (E->f1700) {
                                case 0:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 3:
                    for (k = 0; k < E->sel->def->dim[1] * 2 + 1; k++) {
                        n = E->sel->def->dim[1] * 2 - k;
                        func_80097658(XB(h, 0) + k * 4000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                      ZB(w) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        func_80097658(XB(h, 0) + k * 4000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                      ZB(w) + 14000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (E->f16F8 == n) {
                            switch (E->f1700) {
                            case 0:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (n < E->sel->def->dim[1] * 2) {
                            func_80097658(XB(h, 0) + k * 4000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                          ZB(w) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + (k - 1) * 4000, D_800F55B8[E->f16FC][n] * 16,
                                          ZB(w) + 2000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + k * 4000, n != 0 ? D_800F55B8[E->f16FC][n - 1] * 16 : 0,
                                          ZB(w) + 14000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            func_80097658(XB(h, 0) + (k - 1) * 4000, D_800F55B8[E->f16FC][n] * 16,
                                          ZB(w) + 14000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (E->f16F8 == n || E->f16F8 == n + 1) {
                                switch (E->f1700) {
                                case 0:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                func_80096FB0_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                func_80096FB0_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                }
            }
            gdl = func_80097308(arg0, arg1, gdl);
        }
    } else {
        gdl = func_8008D728(arg1, gdl, 0, 0, 0, 0, 0, 0, -1, 1);
    }

    if (E->flags & 0x800) {
        switch (E->f1800) {
        case 0:
            gdl = func_8008F230(gdl, arg0, arg1, "No Track Found On Game Pak");
            break;
        case 1:
            gdl = func_8008F230(gdl, arg0, arg1, "Save Canceled");
            break;
        case 2:
            gdl = func_8008F230(gdl, arg0, arg1, "Track Complete!");
            break;
        case 3:
            gdl = func_8008F230(gdl, arg0, arg1, "You Ran Out Of Room!");
            break;
        case 4:
            gdl = func_8008F230(gdl, arg0, arg1, "Name Your Track Before Saving!");
            break;
        }
    } else if (E->flags & 0x1000) {
        switch (E->f17FC) {
        case 0:
            gdl = func_8008F458(gdl, arg0, arg1, "Add Segment?", "OK", "Cancel", 1, 1);
            break;
        case 1:
            gdl = func_8008F458(gdl, arg0, arg1, "Delete Will Reset The Piece", "OK", "Cancel", 1, 1);
            break;
        case 3:
        case 7:
            gdl = func_8008F458(gdl, arg0, arg1, "Lose Changes?", "OK", "Cancel", 1, 1);
            break;
        case 4:
            gdl = func_8008F458(gdl, arg0, arg1, "Really Clear Track?", "OK", "Cancel", 1, 1);
            break;
        case 5:
            gdl = func_8008F458(gdl, arg0, arg1, "Save The Track To...", "Game Pak", "Controller Pak", 0, 1);
            break;
        case 6:
            gdl = func_8008F458(gdl, arg0, arg1, "Load A Track From...", "Game Pak", "Controller Pak",
                                (D_800F1BC4 >> 3) & 1, 1);
            break;
        case 8:
            gdl = func_8008F458(gdl, arg0, arg1, "Overwrite Track On Game Pak?", "OK", "Cancel", 1, 1);
            break;
        }
    }
    return gdl;
}

#define AT(p, i) (*(s16 *)((u8 *)(p) + ((i) << 1)))

typedef struct Img_7 {
    s16 dim[2];
    u8 pad4[8];
    u16 flags;
    u16 padE;
    s16 idx;
} Img_7;

typedef struct Game_38 {
    u8 pad0[0x174];
    u32 flags;
    u32 pad178;
    Node_29 *head;
    Node_29 *tail;
    u8 pad184[0x10];
    Node_29 nodes[150];
    s16 *size;
    u8 pad16B0[6];
    s16 scrollx;
    s16 scrolly;
} Game_38;

extern Game_38 *D_800CEA40_22;

Node_29 *func_8009AB24(Node_29 *a, Node_29 *b, s16 *pt)
{
    Node_29 *n;
    s32 i;
    s32 idx;
    Game_38 *g;

    n = D_800CEA40_22->nodes;
    for (i = 0; i < 150; i++, n++) {
        if (n->used == 0) break;
    }
    g = D_800CEA40_22;
    n->used = 1;
    if (g->flags & 8) {
        if (((Img_7 *)pt)->flags & 0x20) {
            n->st = 2;
        } else {
            n->st = 0;
            g->flags &= ~8;
        }
    } else if (((Img_7 *)pt)->flags & 0x10) {
        n->st = 0;
    } else {
        n->st = 2;
        g->flags |= 8;
    }
    n->next = b;
    n->prev = a;
    n->pt = pt;
    if (b == NULL && a == NULL) {
        D_800CEA40_22->head = n;
        D_800CEA40_22->tail = n;
        idx = ((Img_7 *)pt)->idx;
        if (D_800CEA40_22->flags & 8) idx += 2;
        n->idx = idx;
        n->x = D_800CEA40_22->size[0] / 2 - AT(pt, idx & 1) / 2;
        n->y = D_800CEA40_22->size[1] / 2 - AT(pt, (n->idx + 1) & 1) / 2;
        n->x += D_800CEA40_22->scrollx;
        n->y += D_800CEA40_22->scrolly;
        if (n->x < 0) {
            n->x = 0;
            D_800CEA40_22->scrollx = AT(pt, n->idx & 1) / 2 - D_800CEA40_22->size[0] / 2;
        }
        if (n->x + AT(n->pt, n->idx & 1) > D_800CEA40_22->size[0]) {
            n->x = D_800CEA40_22->size[0] - AT(n->pt, n->idx & 1);
            D_800CEA40_22->scrollx = n->x - (D_800CEA40_22->size[0] / 2 - AT(pt, n->idx & 1) / 2);
        }
        if (n->y < 0) {
            n->y = 0;
            D_800CEA40_22->scrolly = AT(pt, ~n->idx & 1) / 2 - D_800CEA40_22->size[1] / 2;
        }
        if (n->y + AT(n->pt, ~n->idx & 1) > D_800CEA40_22->size[1]) {
            n->y = D_800CEA40_22->size[1] - AT(n->pt, ~n->idx & 1);
            D_800CEA40_22->scrolly = n->y - (D_800CEA40_22->size[1] / 2 - AT(pt, ~n->idx & 1) / 2);
        }
    } else {
        if (a != NULL) {
            D_800CEA40_22->tail = n;
            a->next = n;
            func_8009B3A4(a, n);
        }
        if (b != NULL) {
            D_800CEA40_22->head = n;
            b->prev = n;
            func_8009B624(b, n);
        }
    }
    return n;
}

#define PTRF(b, off) (*(u8 **)((u8 *)(b) + (off)))
#define S16F(b, off) (*(s16 *)((u8 *)(b) + (off)))
#define U16F(b, off) (*(u16 *)((u8 *)(b) + (off)))
#define S32F(b, off) (*(s32 *)((u8 *)(b) + (off)))

void func_8009AF18(u8 *p)
{
    s16 *tbl2;
    s32 ix;
    s32 iy;
    s32 x;
    s32 y;

    if (PTRF(p, 4) != 0) {
        if (PTRF(p, 8) != 0) {
            S32F(PTRF(p, 8), 4) = 0;
            S32F(PTRF(p, 4), 8) = 0;
            if ((U16F(p, 0x10) & 4) == 0) {
                S32F(D_800CEA40, 0x17C) = (s32)PTRF(p, 4);
                S32F(D_800CEA40, 0x180) = (s32)PTRF(p, 8);
            }
            goto L_d4;
        }
        goto L_c;
    }
    if (PTRF(p, 8) != 0) {
        S32F(PTRF(p, 8), 4) = 0;
        goto L_8c;
    }
L_c:
    if (PTRF(p, 4) != 0) {
        S32F(PTRF(p, 4), 8) = (s32)PTRF(p, 8);
    }
L_8c:
    if ((U16F(p, 0x10) & 4) == 0) {
        if (S32F(D_800CEA40, 0x17C) == (s32)p) {
            S32F(D_800CEA40, 0x17C) = (s32)PTRF(p, 4);
        }
        if (S32F(D_800CEA40, 0x180) == (s32)p) {
            S32F(D_800CEA40, 0x180) = (s32)PTRF(p, 8);
        }
    }
L_d4:
    {
        u8 *gg = D_800CEA40;
        *(u16 *)p = 0;
        if (S32F(gg, 0x190) == (s32)p) {
            S32F(gg, 0x190) = 0;
        }
    }
    for (iy = 0; iy < *(s16 *)((u8 *)PTRF(p, 0x14) + (((S16F(p, 0x12) + 1) & 1) << 1)); iy++) {
        for (ix = 0; ix < *(s16 *)((u8 *)PTRF(p, 0x14) + ((U16F(p, 0x12) & 1) << 1)); ix++) {
            x = S16F(p, 0xC) + ix;
            if (x >= 0) {
                tbl2 = (s16 *)PTRF(D_800CEA40, 0x16AC);
                if (x < tbl2[0]) {
                    y = S16F(p, 0xE) + iy;
                    if (y >= 0 && y < tbl2[1]) {
                        func_8008D3B4(x, y, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        }
    }
}

s32 func_8009B11C(Blk_22 *b, s32 s)
{
    s16 *P = b->P;
    s16 *Q = &P[s];
    s32 t = Q[2];

    switch ((t + b->k12) & 3) {
    case 1:
    case 3:
        return 0;
    case 0:
        switch (t) {
        case 0:
        case 3:
            return Q[4];
        case 2:
            return P[0] - Q[4] - 2;
        case 1:
            return P[1] - Q[4] - 2;
        }
    case 2: {
        s16 *R = b->P;
        s16 *Q2 = &R[s];

        switch (Q2[2]) {
        case 0:
            return R[0] - Q2[4] - 2;
        case 1:
            return Q2[4];
        case 2:
            return Q2[4];
        case 3:
            return R[1] - Q2[4] - 2;
        }
    }
    }
}

s32 func_8009B258(Spr_4 *p, s32 k)
{
    s16 *P = p->P;
    s16 *Q = P + k;
    s32 a = Q[2];

    switch ((a + p->rot) & 3) {
    case 0:
        return 0;
    case 2:
        return 0;
    case 3:
        switch (a) {
        case 2:
            return P[0] - Q[4] - 2;
        case 0:
        case 3:
            return Q[4];
        case 1:
            return P[1] - Q[4] - 2;
        }
    case 1: {
        s16 *R = p->P;
        s16 *S = R + k;

        switch (S[2]) {
        case 2:
            return S[4];
        case 0:
            return R[0] - S[4] - 2;
        case 3:
            return R[1] - S[4] - 2;
        case 1:
            return S[4];
        }
    }
    }
}

#define TB(p, i) (*(u16 *)((u8 *)(p) + ((i) << 1)))
#define SX16(v) ((s32)((u32)(v) << 16))
#define HALF(v, s) ((s32)((SX16(v) >> 16) + (s32)((u32)SX16(v) >> 31)) >> (s))

void func_8009B3A4(Node_30 *p, Node_30 *c)
{
    Node_30 *q = p;
    s32 k = 1;
    s32 m = 0;
    s32 t, t2, r, v, ka, ma;

    if (q == 0) {
        u16 *tg = *(u16 **)(D_800CEA40 + 0x16AC);
        c->px = HALF(tg[0], k) - HALF(c->tab[0], k);
        tg = *(u16 **)(D_800CEA40 + 0x16AC);
        c->py = HALF(tg[1], k) - HALF(c->tab[1], k);
        return;
    }
    if (q->flags & 2) k = 0;
    if (c->flags & 2) m = 1;
    c->px = q->px;
    *(volatile s16 *)&c->py = q->py;
    t = q->cnt + (*(u16 *)((u8 *)q->tab + (k << 1) + 4) - *(u16 *)((u8 *)*(u16 *volatile *)&c->tab + (m << 1) + 4)) + 2;
    t2 = t & 3;
    c->cnt = t2;
    r = (*(s16 *)((u8 *)q->tab + (k << 1) + 4) + (s32)(s16)q->cnt) & 3;
    switch (r) {
    case 0:
        c->py += TB(q->tab, ((s32)(s16)q->cnt + 1) & 1);
        v = func_8009B11C(q, k);
        c->px += v;
        v = func_8009B11C(c, m);
        c->px -= v;
        break;
    case 2:
        c->py -= TB(c->tab, (t2 + 1) & 1);
        v = func_8009B11C(q, k);
        c->px += v;
        v = func_8009B11C(c, m);
        c->px -= v;
        break;
    case 3:
        c->px -= TB(c->tab, t & 1);
        v = func_8009B258(q, k);
        c->py += v;
        v = func_8009B258(c, m);
        c->py -= v;
        break;
    case 1:
        c->px += TB(q->tab, q->cnt & 1);
        v = func_8009B258(q, k);
        c->py += v;
        v = func_8009B258(c, m);
        c->py -= v;
        break;
    }
}

typedef struct { s16 size[2]; s16 rot[2]; } Tab_8;
struct Spr_5 { u8 pad[0xc]; s16 x; s16 y; u16 fl; s16 n; Tab_8 *tab; };

void func_8009B624(Spr_5 *a, Spr_5 *b)
{
    s32 fa, fb;
    u16 n;
    s32 f; u32 g;

    f = a->fl & 2; fa = f != 0;
    b->x = a->x;
    b->y = a->y;
    g = b->fl & 2; fb = g < 1;
    n = a->n + (a->tab->rot[fa] - b->tab->rot[fb]) + 2;
    b->n = n & 3;
    switch ((a->tab->rot[fa] + a->n) & 3) {
    case 0:
        b->y += a->tab->size[(a->n + 1) & 1];
        b->x += func_8009B11C(a, fa);
        b->x -= func_8009B11C(b, fb);
        break;
    case 2:
        b->y -= b->tab->size[(b->n + 1) & 1];
        b->x += func_8009B11C(a, fa);
        b->x -= func_8009B11C(b, fb);
        break;
    case 3:
        b->x -= b->tab->size[b->n & 1];
        b->y += func_8009B258(a, fa);
        b->y -= func_8009B258(b, fb);
        break;
    case 1:
        b->x += a->tab->size[a->n & 1];
        b->y += func_8009B258(a, fa);
        b->y -= func_8009B258(b, fb);
        break;
    }
}

typedef struct { u8 pad[0xC]; u16 f; u8 rest[0x32]; } Blk_23;
typedef struct { s32 f0; u32 flags; s16 n; s16 pad; u32 mask; } Rec_50;
typedef struct { s32 f0; s32 f4; Rec_50 *recs; } Grp;

extern Blk_23 D_800CE280_7[];
extern Grp D_800CEB9C_5;

void func_8009B820(void)
{
    s32 i;
    s32 k;
    s32 t0;
    s32 v;
    Blk_23 *p;
    s32 w;
    s32 r;

    for (k = 0; k < 0x1D; k++) {
        p = &D_800CE280_7[k];
        v = p->f;
        if (v & 2) {
            w = v | 0x10;
            p->f = w;
            if (w & 0x100) {
                r = w | 0x20;
            } else {
                r = w & ~0x20;
            }
        } else {
            r = v & ~0x10;
            r &= ~0x20;
        }
        p->f = r;
    }
    for (i = 0; i < 4; i++) {
        switch (i) {
        case 0:
            t0 = 0;
            break;
        case 1:
            t0 = 2;
            break;
        case 2:
            t0 = 6;
            break;
        case 3:
            t0 = 0x12;
            break;
        }
        D_800CEB9C_5.recs[i].mask = 0;
        for (k = 0; k < D_800CEB9C_5.recs[i].n; k++) {
            if (D_800CE280_7[t0 + k].f & 0x30) {
                D_800CEB9C_5.recs[i].mask |= 1 << k;
            }
        }
        if (D_800CEB9C_5.recs[i].mask == 0) {
            D_800CEB9C_5.recs[i].flags &= ~0x10;
        } else {
            D_800CEB9C_5.recs[i].flags |= 0x10;
        }
    }
    func_80090398(&D_800CEB9C_5);
}

typedef struct Size {
    s16 w;
    s16 h;
} Size;

typedef struct Game_39 {
    u8 pad0[0x170];
    s32 f170;
    u8 pad174[0x17C - 0x174];
    Obj_72 *list;          /* 0x17C */
    u8 pad180[0x16AC - 0x180];
    Size *map;          /* 0x16AC */
    u8 pad16B0[0x16F4 - 0x16B0];
    s32 f16F4;
    u8 pad16F8[0x1702 - 0x16F8];
    s16 f1702;
} Game_39;

typedef struct Ent_86 {
    s16 dim[2];         /* 0x00 */
    s16 dir[2];         /* 0x04 */
    s16 off[2];         /* 0x08 */
    s16 flags;
    u8 pad[0x40 - 0xE];
} Ent_86;

typedef struct Grid_3 {
    u8 g[30][30];
} Grid_3;

typedef struct Rect_4 {
    s32 x0, y0, x1, y1;
} Rect_4;

extern Game_39 *D_800CEA40_23;
extern Ent_86 D_800CE280_8[];
extern Grid_3 D_800F56B0;

s32 func_8009B9A8(Obj_72 *o)
{
    Grid_3 grid;
    Rect_4 r;
    s16 k;
    s16 other;
    s32 i, j;
    s32 n, m, lo, hi, best;
    s16 s;
    s32 ret;
    Obj_72 *p;
    Ent_86 *e;

    k = 1;
    if (o->next == NULL || (o->next->flags & 4)) {
        other = 0;
    } else {
        k = 0;
        other = 1;
    }
    switch (o->dir[k]) {
    case 0:
        {
            s32 t, u;
            t = o->x + o->len[k];
            r.x0 = t - 14;
            u = o->y + o->h;
            r.x1 = t + 16;
            r.y0 = u - 15;
            r.y1 = u + 15;
        }
        break;
    case 1:
        {
            s32 t, u;
            t = o->x + o->w;
            r.x0 = t - 15;
            u = o->y + o->len[k];
            r.x1 = t + 15;
            r.y0 = u - 14;
            r.y1 = u + 16;
        }
        break;
    case 2:
        {
            s32 t, u;
            t = o->x + o->len[k];
            r.x0 = t - 14;
            u = o->y;
            r.x1 = t + 16;
            r.y0 = u - 15;
            r.y1 = u + 15;
        }
        break;
    case 3:
        {
            s32 t, u;
            t = o->x;
            r.x0 = t - 15;
            u = o->y + o->len[k];
            r.x1 = t + 15;
            r.y0 = u - 14;
            r.y1 = u + 16;
        }
        break;
    }
    func_8000ED10((u8 *)&grid, 0, 0x384);
    for (j = 0; j < r.y1 - r.y0; j++) {
        for (i = 0; i < r.x1 - r.x0; i++) {
            if (i + r.x0 < 0 || i + r.x0 >= D_800CEA40_23->map->w
                || j + r.y0 < 0 || j + r.y0 >= D_800CEA40_23->map->h) {
                switch (o->dir[k]) {
                case 0:
                    grid.g[i][j] = 1;
                    break;
                case 2:
                    grid.g[29 - i][29 - j] = 1;
                    break;
                case 3:
                    grid.g[j][29 - i] = 1;
                    break;
                case 1:
                    grid.g[29 - j][i] = 1;
                    break;
                }
            }
        }
    }
    p = D_800CEA40_23->list;
    if (p != NULL) {
        do {
            if (!(p->flags & 4) && p->x < r.x1 && r.x0 < p->x + p->w
                && p->y < r.y1 && r.y0 < p->y + p->h) {
                switch (o->dir[k]) {
                case 0:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[i][j] = 1;
                            }
                        }
                    }
                    break;
                case 2:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[29 - i][29 - j] = 1;
                            }
                        }
                    }
                    break;
                case 3:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[j][29 - i] = 1;
                            }
                        }
                    }
                    break;
                case 1:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[29 - j][i] = 1;
                            }
                        }
                    }
                    break;
                }
            }
            p = p->next;
        } while (p != NULL && p != D_800CEA40_23->list);
    }
    D_800F56B0 = grid;

    for (n = 0; n < 29; n++) {
        e = &D_800CE280_8[n];
        if (!(e->flags & 8)) {
            continue;
        }
        best = 0;
        if (e->flags & 0x80) {
            lo = 1;
            hi = 8;
        } else {
            hi = e->dim[(e->dir[k] + 1) & 1];
            lo = hi;
        }
        for (m = lo; m <= hi; m++) {
            if ((u32)(n - 18) < 8 && n - 18 >= D_800CEA40_23->f16F4) {
                e->flags &= ~0x30;
            } else {
                switch (e->dir[other]) {
                case 0:
                    s = e->dim[0] - e->off[other] - 2;
                    break;
                case 3:
                    s = e->dim[1] - e->off[other] - 2;
                    break;
                case 1:
                    s = e->off[other];
                    break;
                case 2:
                    s = e->off[other];
                    break;
                }
                e->flags |= 0x10;
                for (j = 0; j < m; j++) {
                    for (i = 0; i < e->dim[e->dir[k] & 1]; i++) {
                        if (grid.g[i + (14 - s)][j + 15]) {
                            e->flags &= ~0x10;
                            break;
                        }
                    }
                    if (!(e->flags & 0x10)) {
                        break;
                    }
                }
                if (e->flags & 0x100) {
                    switch (e->dir[k]) {
                    case 0:
                        s = e->dim[0] - e->off[k] - 2;
                        break;
                    case 3:
                        s = e->dim[1] - e->off[k] - 2;
                        break;
                    case 1:
                        s = e->off[k];
                        break;
                    case 2:
                        s = e->off[k];
                        break;
                    }
                    e->flags |= 0x20;
                    j = 0;
                    if (m > 0) {
                        do {
                            for (i = 0; i < e->dim[e->dir[k] & 1]; i++) {
                                if (grid.g[i + (14 - s)][j + 15]) {
                                    e->flags &= ~0x20;
                                    break;
                                }
                            }
                            j++;
                        } while ((e->flags & 0x20) && j < m);
                    }
                } else {
                    e->flags &= ~0x20;
                }
            }
            if (e->flags & 0x10) {
                best = m;
            } else if (e->flags & 0x80) {
                if (m != lo) {
                    e->flags |= 0x10;
                }
                break;
            }
        }
        if (e->flags & 0x80) {
            if (D_800CEA40_23->f170 != 6) {
                e->dim[1] = (best < e->dim[1]) ? best : e->dim[1];
            }
            D_800CEA40_23->f1702 = best;
        }
    }

    ret = 0;
    for (i = 0; i < 6; i++) {
        switch (i) {
        case 0:
            j = 0;
            break;
        case 1:
            j = 2;
            break;
        case 2:
            j = 6;
            break;
        case 3:
            j = 0x12;
            break;
        default:
            continue;
        }
        D_800CEB9C_6.slots[i].mask = 0;
        for (n = 0; n < D_800CEB9C_6.slots[i].n; n++) {
            if (D_800CE280_8[j + n].flags & 0x30) {
                D_800CEB9C_6.slots[i].mask |= 1 << n;
            }
        }
        if (D_800CEB9C_6.slots[i].mask == 0) {
            D_800CEB9C_6.slots[i].flags &= ~0x10;
        } else {
            ret = 1;
            D_800CEB9C_6.slots[i].flags |= 0x10;
        }
    }
    func_80090398(&D_800CEB9C_6);
    return ret;
}

typedef struct {
    u16 x0;
    u16 x2;
    u8 pad4[0x2C];
    void *x30;
    void *x34;
    u8 pad38[8];
} Rec_51;

typedef struct {
    u8 pad[0x16F8];
    s32 x16F8;
    s32 x16FC;
    u16 x1700;
    u16 x1702;
} Ctl_25;

extern Rec_51 D_800CE280_9[];
extern Ctl_25 *D_800CEA40_24;

void func_8009C618(void)
{
    s32 i, j;
    s32 *p1 = D_800CE280_9[0x1C].x30;
    s32 *p2 = D_800CE280_9[0x1C].x34;

    D_800CEA40_24->x16F8 = 1;
    D_800CEA40_24->x16FC = 0;
    D_800CEA40_24->x1702 = 8;
    for (i = 0; i < 8; i++) {
        D_800CE280_9[i + 0x12].x2 = 4;
        D_800CE280_9[i + 0x12].x30 = func_800BC908((*p1 << 8) | 4, 0, D_800DAB1C, 0x1D73);
        D_800CE280_9[i + 0x12].x34 = func_800BC908((*p2 << 8) | 4, 0, D_800DAB1C, 0x1D74);
        func_800A5484(D_800CE280_9[i + 0x12].x30, D_800CE280_9[i + 0x12].x34, 8);
        for (j = 0; j < 15; j++) {
            D_800F55B8[i][j] = 0;
        }
        func_800A4FF0(D_800CE280_9[i + 0x12].x30, D_800CE280_9[i + 0x12].x34, D_800F55B8[i], 8);
    }
    {
        Ctl_25 *ea;
        u8 *rb;
        volatile Rec_51 *r;
        u8 *tb;
        i = 0;
        tb = (u8 *)D_800F55B8;
        if (tb) j = 3; else j = 4;
        ea = D_800CEA40_24;
        rb = (u8 *)D_800CE280_9;
        r = (volatile Rec_51 *)(rb + ((ea->x16FC + 0x12) << 6));
        r->x2;
        if (r) j = 1; else j = 2;
        for (; i < 15; i++) {
            *(u16 *)(tb + (ea->x16FC * 15 + i) * 2) = 0;
        }
    }
}

typedef struct Game_40 {
    u8 pad0[0x184];
    Sub_2 *sub;
    u8 pad1[0x16FC - 0x188];
    s32 idx;
    u8 pad2[0x1738 - 0x1700];
    s32 f1738;
    u8 pad3[0x1758 - 0x173C];
    s32 t1758;
    s32 t175c;
} Game_40;

typedef struct T2 {
    s16 cnt[2];
    u8 pad[0x2C];
    u8 *p30;
    u8 *p34;
    u8 pad2[0x8];
} T2;

extern Game_40 *D_800CEA40_25;
extern T2 D_800CE280_10[];

void func_8009C7D4(void)
{
    T2 *t;
    s32 x;
    s32 y;
    s32 i;
    s16 sv;
    Sub_2 *sub;
    Game_40 *g;

    t = &D_800CE280_10[D_800CEA40_25->idx + 18];
    for (y = 0; y < t->cnt[(D_800CEA40_25->sub->cur + 1) & 1]; y++) {
        for (x = 0; x < t->cnt[D_800CEA40_25->sub->cur & 1]; x++) {
            func_8008D3B4(D_800CEA40_25->sub->c + x, D_800CEA40_25->sub->e + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }

    for (i = 0; i < D_800CE280_10[D_800CEA40_25->idx + 18].cnt[1]; i++) {
        __builtin_memcpy(D_800F55B8_2 + D_800CEA40_25->idx * 30 + i * 4, D_800F55B8_2 + D_800CEA40_25->idx * 30 + i * 4 + 4, 4);
    }
    g = D_800CEA40_25;
    D_800CE280_10[g->idx + 18].cnt[1]--;
    {
        s32 k = D_800CE280_10[g->idx + 18].cnt[1] * 4 - 2;
        k += g->idx * 30;
        *(s16 *)(D_800F55B8_2 + k) = 0;
    }
    func_800A4FF0(D_800CE280_10[g->idx + 18].p30, D_800CE280_10[g->idx + 18].p34,
                  D_800F55B8_2 + g->idx * 30, D_800CE280_10[g->idx + 18].cnt[1]);

    sub = D_800CEA40_25->sub;
    sv = sub->arr[sub->f8 == 0];
    if (sv != 0) {
        if (sv == 1) {
            sub->c++;
        }
    } else {
        sub->e++;
    }
    if (D_800CEA40_25->f1738 > 45000) {
        D_800CEA40_25->t175c = -10000;
        D_800CEA40_25->t1758 = 50;
    }
}

void func_8009CAA4(void)
{
    s32 x, y, i, j;
    Rec_5 *tb;
    Pl *pl;
    s16 s;

    tb = &D_800CE700[D_800CEA40_2->idx];
    for (y = 0; y < tb->d[(D_800CEA40_2->pl->f12 + 1) & 1]; y++) {
        for (x = 0; x < tb->d[D_800CEA40_2->pl->f12 & 1]; x++) {
            func_8008D3B4(D_800CEA40_2->pl->x + x, D_800CEA40_2->pl->y + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    D_800CE280_2[D_800CEA40_2->idx + 18].d[1]--;
    for (i = 0; i < 2; i++) {
        j = i - 1;
        D_800F55B8[D_800CEA40_2->idx][D_800CE280_2[D_800CEA40_2->idx + 18].d[1] * 2 + j] = 0;
    }
    func_800A4FF0(D_800CE280_2[D_800CEA40_2->idx + 18].f30, D_800CE280_2[D_800CEA40_2->idx + 18].f34,
                  D_800F55B8[D_800CEA40_2->idx], D_800CE280_2[D_800CEA40_2->idx + 18].d[1]);
    D_800CEA40_2->f16F8 -= 2;
    pl = D_800CEA40_2->pl;
    s = pl->sel[pl->f8 == 0];
    if (s != 0) {
        if (s == 1) {
            pl->x++;
        }
    } else {
        pl->y++;
    }
    if (D_800CEA40_2->f1738 > 45000) {
        D_800CEA40_2->f175C = -10000;
        D_800CEA40_2->f1758 = 50;
    }
}

typedef struct U4 {
    u32 w;
} __attribute__((packed)) U4;

typedef struct Game_41 {
    u8 pad0[0x184];
    Sub_2 *sub;
    u8 pad1[0x16f8 - 0x188];
    s32 n16f8;
    s32 idx;
    u8 pad2[0x1758 - 0x1700];
    s32 t1758;
    s32 t175c;
} Game_41;

typedef struct T1 {
    s16 cnt[2];
    u8 pad[60];
} T1;

typedef struct T2_2 {
    s16 w0;
    s16 cnt;
    u8 pad[0x2c];
    u8 *p30;
    u8 *p34;
    u8 pad2[0x8];
} T2_2;

extern Game_41 *D_800CEA40_26;

void func_8009CD24(void) {
    T1 *t1;
    T2_2 *t2;
    T2_2 *b2;
    T2_2 *bp;
    T2_2 *bq;
    T2_2 *tq;
    T2_2 *tr;
    T2_2 *ts;
    Game_41 *g;
    u8 *hb;
    Game_41 *gt;
    u8 *ub;
    u8 *sb;
    Sub_2 *sub;
    s32 x;
    s32 y;
    s32 i;
    s32 off;
    s16 sv;

    t1 = &D_800CE700[D_800CEA40_26->idx];
    for (y = 0; y < t1->cnt[(D_800CEA40_26->sub->cur + 1) & 1]; y++) {
        for (x = 0; x < t1->cnt[D_800CEA40_26->sub->cur & 1]; x++) {
            func_8008D3B4(D_800CEA40_26->sub->c + x, D_800CEA40_26->sub->e + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }

    bp = D_800CE280;
    tq = (T2_2 *)((u8 *)bp + ((D_800CEA40_26->idx + 0x12) << 6));
    i = tq->cnt - 1;
    if (i >= 0) {
        do {
            off = D_800CEA40_26->idx * 30 + i * 4;
            ub = D_800F55BC;
            sb = ub - 4;
            *(U4 *)(ub + off) = *(U4 *)(sb + off);
            i--;
        } while (i >= 0);
    }

    func_8000ED10(D_800F55B8_2 + D_800CEA40_26->idx * 30, 0, 4);

    g = D_800CEA40_26;
    bq = D_800CE280;
    tr = (T2_2 *)((u8 *)bq + ((g->idx + 0x12) << 6));
    tr->cnt = tr->cnt + 1;
    bq = D_800CE280;
    ts = (T2_2 *)((u8 *)bq + ((g->idx + 0x12) << 6));
    func_800A4FF0(ts->p30, ts->p34, D_800F55B8_2 + g->idx * 30, ts->cnt);

    D_800CEA40_26->n16f8 = D_800CEA40_26->n16f8 + 1;
    sub = D_800CEA40_26->sub;
    sv = sub->arr[sub->f8 == 0];
    if (sv != 0) {
        if (sv == 1) {
            sub->c = sub->c - 1;
        }
    } else {
        sub->e = sub->e - 1;
    }
    D_800CEA40_26->t175c = 10000;
    D_800CEA40_26->t1758 = 50;
}

void func_8009CFC4(void)
{
    s32 x, y, i;
    Rec_5 *tb;
    Pl *pl;
    s16 s;

    tb = &D_800CE700[D_800CEA40_2->idx];
    for (y = 0; y < tb->d[(D_800CEA40_2->pl->f12 + 1) & 1]; y++) {
        for (x = 0; x < tb->d[D_800CEA40_2->pl->f12 & 1]; x++) {
            func_8008D3B4(D_800CEA40_2->pl->x + x, D_800CEA40_2->pl->y + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    for (i = 0; i < 2; i++) {
        D_800F55B8[D_800CEA40_2->idx][D_800CE280_2[D_800CEA40_2->idx + 18].d[1] * 2 + i] = 0;
    }
    D_800CE280_2[D_800CEA40_2->idx + 18].d[1]++;
    func_800A4FF0(D_800CE280_2[D_800CEA40_2->idx + 18].f30, D_800CE280_2[D_800CEA40_2->idx + 18].f34,
                  D_800F55B8[D_800CEA40_2->idx], D_800CE280_2[D_800CEA40_2->idx + 18].d[1]);
    D_800CEA40_2->f16F8 += 1;
    pl = D_800CEA40_2->pl;
    s = pl->sel[pl->f8 == 0];
    if (s != 0) {
        if (s == 1) {
            pl->x--;
        }
    } else {
        pl->y--;
    }
    D_800CEA40_2->f175C = 10000;
    D_800CEA40_2->f1758 = 50;
}

#define CEA_IDX (*(s32 *)(D_800CEA40 + 0x16FC))
#define LIM ((s32)D_800CE280_3[CEA_IDX + 0x12].f2 << 1)

int func_8009D224(int param_1, u16 *param_2)
{
    int t0;
    int q;
    int quo;
    int res;
    int n;
    u8 *tb;
    u8 *tb2;
    s16 sv;
    u16 uv;

    if (param_1 - 1 > 0 && param_1 - 1 < LIM) {
        s16 *gb = D_800F55B8;
        s32 k = param_1 - 2;
        t0 = gb[CEA_IDX * 15 + k] + 0xFA;
    } else {
        t0 = 0xFA;
    }
    tb = D_800C2C60;
    if (param_1 - 1 > 0) {
        q = (((u32)param_2[param_1 - 2] + 0x1555) >> 3) & 0x1ffe;
    } else {
        q = 0x2AA;
    }
    sv = *(s16 *)(tb + q);
    uv = *(u16 *)(tb + q);
    if (sv == 0) {
        uv = 1;
    }
    tb2 = D_800C2C60;
    if (param_1 - 1 > 0) {
        q = ((((param_2[param_1 - 2] + 0x1555) >> 4) - 0x400) & 0xfff) << 1;
    } else {
        q = 0x1AAA;
    }
    quo = (*(s16 *)(tb2 + q) * 0xFA) / (s16)uv;
    if (param_1 - 1 > 0 && param_1 - 1 < LIM) {
        s16 *gb = D_800F55B8;
        s32 k = param_1 - 2;
        quo += gb[CEA_IDX * 15 + k];
    }
    res = quo;
    if (t0 < res) {
        res = t0;
    }
    return res;
}

#define CE_ROW() (*(s32 *)(D_800CEA40 + 0x16FC))
#define CE_LIM() ((D_800CE280_3[CE_ROW() + 0x12].f2) << 1)
#define GETA(i) (((i) + 1 > 0 && (i) + 1 < CE_LIM()) ? D_800F55B8[CE_ROW()][i] : 0)
#define ANG(p, i) (((i) + 1 < CE_LIM()) ? 0x2000 - (p)[(i) + 1] : 0x2000)
#define SINS(x) D_800C2C60[((x) >> 4) & 0xFFF]
#define COSS(x) D_800C2C60[(((x) >> 4) - 0x400) & 0xFFF]

s32 func_8009D3D4(s32 p1, s16 *p2)
{
    s32 t0;
    s32 q;
    s16 s;

    t0 = GETA(p1) + 500;
    s = SINS(ANG(p2, p1));
    if (s == 0) {
        s = 1;
    }
    q = COSS(ANG(p2, p1)) * 250 / s;
    q += GETA(p1);
    return (t0 < q) ? t0 : q;
}

#define OFS(n) (((n) - 1 > 0 && (n) - 1 < D_800CE280_4[CUR + 0x12].cnt * 2) ? D_800F55B8[CUR][(n) - 2] : 0)
#define ANG_2(n, p) ((n) - 1 > 0 ? (p)[(n) - 2] : 0)

int func_8009D5D8(int n, u16 *p)
{
    s32 lo;
    s16 sv;
    s32 res;

    lo = OFS(n) - 500;
    sv = D_800C2C60[((ANG_2(n, p) - 0x2000) >> 4) & 0xFFF];
    if (sv == 0) {
        sv = 1;
    }
    res = D_800C2C60[(((ANG_2(n, p) - 0x2000) >> 4) - 0x400) & 0xFFF] * 250 / sv;
    res += OFS(n);
    return res < lo ? lo : res;
}

#define OFS_2(i) (((i) > 0 && (i) < D_800CE280_4[CUR + 0x12].cnt * 2) ? D_800F55B8[CUR][(i) - 1] : 0)
#define ANG_3(i, p) ((i) < D_800CE280_4[CUR + 0x12].cnt * 2 ? (p)[i] : 0)

int func_8009D788(int n, s16 *p)
{
    s32 lo;
    s16 sv;
    s32 res;

    lo = OFS_2(n + 1) - 250;
    sv = D_800C2C60[((-0x2000 - ANG_3(n + 1, p)) >> 4) & 0xFFF];
    if (sv == 0) {
        sv = 1;
    }
    res = D_800C2C60[(((-0x2000 - ANG_3(n + 1, p)) >> 4) - 0x400) & 0xFFF] * 250 / sv;
    res += OFS_2(n + 1);
    return res < lo ? lo : res;
}

#define LIM_2 (D_800CE280_11[D_800CEA40_27->cur + 0x12].lim * 2)
#define T1_2 ((s16 *)D_800F55B8)

typedef struct {
    u8 pad[0x16FC];
    s32 cur;
} Ctx9D;

typedef struct { s16 n0; s16 lim; s16 rest[30]; } Row9D;

extern Ctx9D *D_800CEA40_27;
extern Row9D D_800CE280_11[];

static inline s16 *At(s16 *b, s32 c, s32 i)
{
    return &b[c * 15 + i];
}

s32 func_8009D98C(s32 n)
{
    u16 r1;
    s32 dummy;
    u16 r2;
    s32 a, cnt, v;

    if (n > 0 && n < LIM_2)
        a = *At(T1_2, D_800CEA40_27->cur, n - 1);
    else
        a = 0;
    cnt = a;
    for (;;) {
        D_800F55B8[D_800CEA40_27->cur][n - 1]++;
        if (n > 0 && n < LIM_2)
            v = D_800F55B8[D_800CEA40_27->cur][n - 1];
        else
            v = 0;
        if (n - 1 > 0 && n - 1 < LIM_2)
            v -= D_800F55B8[D_800CEA40_27->cur][n - 2];
        func_8001254C(4000, v << 4, &r1, &dummy);
        r1 = 0x4000 - r1;
        if (n + 1 > 0 && n + 1 < LIM_2)
            v = T1_2[D_800CEA40_27->cur * 15 + n];
        else
            v = 0;
        if (n > 0 && n < LIM_2)
            v -= D_800F55B8[D_800CEA40_27->cur][n - 1];
        func_8001254C(4000, v << 4, &r2, &dummy);
        r2 = 0x4000 - r2;
        if ((s16)(r2 - r1) < -0x2000)
            break;
        cnt++;
    }
    {
        s16 *b = T1_2;
        *At(b, D_800CEA40_27->cur, n - 1) = a;
    }
    return cnt;
}

#define LV() (*(s32 *)(D_800CEA40 + 0x16FC))
#define CE2() (D_800CE280[LV() + 0x12].h1 << 1)

s32 func_8009DC6C(s32 param_1)
{
    s32 iVar2;
    s32 iVar3;
    s32 iVar4;
    s32 iVar5;
    s32 w38;
    s32 iStack_34;
    s32 w30;
    u32 uVar1;

    if (param_1 > 0 && param_1 < CE2()) {
        iVar5 = D_800F55B8[LV()][param_1 - 1];
    } else {
        iVar5 = 0;
    }
    iVar4 = iVar5;
    iVar3 = param_1 - 1;
    while (1) {
        D_800F55B8[LV()][iVar3] = (s16)((u16)D_800F55B8[LV()][iVar3] - 1);
        if (param_1 > 0 && param_1 < CE2()) {
            iVar2 = D_800F55B8[LV()][iVar3];
        } else {
            iVar2 = 0;
        }
        if (iVar3 > 0 && iVar3 < CE2()) {
            iVar2 = iVar2 - D_800F55B8[LV()][param_1 - 2];
        }
        func_8001254C(4000, iVar2 << 4, (u16 *)&w38, &iStack_34);
        ((u16 *)&w38)[0] = 0x4000 - ((u16 *)&w38)[0];
        if (param_1 + 1 > 0 && param_1 + 1 < CE2()) {
            iVar2 = D_800F55B8[LV()][param_1];
        } else {
            iVar2 = 0;
        }
        if (param_1 > 0 && param_1 < CE2()) {
            iVar2 = iVar2 - D_800F55B8[LV()][iVar3];
        }
        func_8001254C(4000, iVar2 << 4, (u16 *)&w30, &iStack_34);
        uVar1 = ((u16 *)&w30)[0];
        ((u16 *)&w30)[0] = 0x4000 - uVar1;
        if (0x1555 < (s16)((0x4000 - uVar1) - ((u16 *)&w38)[0])) {
            break;
        }
        iVar4 = iVar4 - 1;
    }
    D_800F55B8[LV()][param_1 - 1] = (s16)iVar5;
    return iVar4;
}

#define A_2 (D_800CEA40_28->a)
#define B_2 (D_800CEA40_28->b)
#define LIM_3 (D_800CE280_12[B_2 + 0x12].lim * 2)
#define H_2(c) ((c) > 0 && (c) < LIM_3 ? D_800F55B8[B_2][(c) - 1] : 0)
#define MIN_2(x, y) ((x) < (y) ? (x) : (y))
#define MAX_2(x, y) ((x) > (y) ? (x) : (y))
#define CELL (({ s16 *_tb = (s16 *)D_800F55B8; _tb; })[({ s32 _c = A_2 - 1; B_2 * 15 + _c; })])

typedef struct { u8 pad0[2]; s16 lim; u8 rest[0x3C]; } Tb_2;
typedef struct { u8 pad[0x16F8]; s32 a; s32 b; s16 st; } Ctl_26;

extern Tb_2 D_800CE280_12[];
extern Ctl_26 *D_800CEA40_28;

void func_8009DF4C(void)
{
    volatile s32 pad[8];
    s32 t1;
    s32 hi, lo;
    s32 flag;
    s32 v, x;
    u32 F;

    t1 = D_800E30B0 >> 17;
    t1 &= 1;
    hi = MIN_2(H_2(A_2 + 1) + 0x8C, H_2(A_2 - 1) + 0x8C);
    lo = MAX_2(H_2(A_2 - 1) - 0x8C, H_2(A_2 + 1) - 0x8C);
    if (hi > 0x1C2) {
        hi = 0x1C2;
    }
    if (lo < -0x1C2) {
        lo = -0x1C2;
    }
    *(volatile s16 *)&D_800CEA40_28->st = 2;
    if (D_800E30B0 & 1) {
        goto body;
    }
    if (D_800E30B0 & 2) {
        goto body;
    }
    if ((D_800E30B0 & 0x800) && !(D_800E25B0 & 0x800)) {
        goto body;
    }
    if ((D_800E30B0 & 0x1000) && !(D_800E25B0 & 0x1000)) {
    body:
        if ((D_800E30B0 & 0x800) && !(D_800E25B0 & 0x800)) {
            *(u16 *)&CELL += 0x14;
        } else if ((D_800E30B0 & 0x1000) && !(D_800E25B0 & 0x1000)) {
            *(u16 *)&CELL -= 0x14;
        } else {
            s16 *p = &CELL;
            s32 w = *p;
            if (D_800E30B0 & 3) {
                if (D_800E30B0 & 1) {
                    x = w + (D_800E30B0 >> 28);
                } else {
                    x = w - (D_800E30B0 >> 28);
                }
            } else {
                x = w;
            }
            *p = x;
        }
        if (t1 != 0) {
            v = CELL;
            if (hi < v) {
                if (flag != 0) {
                    D_800CEA40_28->st = 1;
                }
                CELL = hi;
            } else if (v < lo) {
                CELL = lo;
                D_800CEA40_28->st = 1;
            }
        } else {
            v = CELL;
            if (hi < v) {
                D_800CEA40_28->st = 0;
                CELL = hi;
            } else if (v < lo) {
                CELL = lo;
                D_800CEA40_28->st = 0;
            }
        }
    }
}

#define PRESSED_4(b) ((D_800E30B0 & (b)) && !(D_800E25B0_3[0] & (b)))

typedef struct {
    u8 pad0[0x170];
    s32 f170;
    u32 f174;
    u8 pad178[0x16E0 - 0x178];
    Obj_2 *sub;
    u8 pad16E4[0x16F4 - 0x16E4];
    s32 f16F4;
    s32 f16F8;
    s32 f16FC;
    u8 pad1700[2];
    s16 f1702;
    u8 pad1704[0x17FC - 0x1704];
    s16 f17FC;
    s16 f17FE;
} Game_42;

typedef struct {
    u8 pad0[0x34];
    u32 f34;
    u8 pad38[4];
    u32 f3C;
} Obj_73;

typedef struct {
    u8 pad0[6];
    s16 f6;
    Obj_73 *obj;
    u8 padC[4];
    s16 f10;
} Menu_11;

typedef struct {
    u8 pad0[2];
    s16 n;
    u8 pad4[8];
    u16 fC;
    u8 padE[0x30 - 0xE];
    void *a;
    void *b;
    u8 pad38[8];
} Tab_9;

extern Game_42 *D_800CEA40_29;
extern Menu_11 D_800CEB9C_7;

void func_8009E584(void)
{
    s32 i;

    if (D_800CEA40_29->f174 & 0x1000) {
        if (PRESSED_4(0x20)) {
            D_800E25B0_3[0] |= 0x20;
            D_800CEA40_29->sub->flags |= 1;
            D_800CEA40_29->f174 &= ~0x1000;
        } else if (PRESSED_4(0x10)) {
            D_800E25B0_3[0] |= 0x10;
            if (D_800CEA40_29->f17FE != 0) {
                switch (D_800CEA40_29->f17FC) {
                case 0:
                    if (D_800CEA40_29->f16F8 == 1) {
                        func_8009CD24();
                    } else {
                        func_8009CFC4();
                    }
                    break;
                case 1:
                    for (i = 0; i < 15; i++) {
                        D_800F55B8[D_800CEA40_29->f16FC][i] = 0;
                    }
                    if (D_800CEA40_29->f16F8 == 1) {
                        func_8009C7D4();
                    } else {
                        func_8009CAA4();
                    }
                    break;
                }
            }
            D_800CEA40_29->sub->flags |= 1;
            D_800CEA40_29->f174 &= ~0x1000;
        } else if (PRESSED_4(0x800) || PRESSED_4(0x1000)) {
            D_800CEA40_29->f17FE ^= 1;
            func_8001102C(0);
        }
    } else if (PRESSED_4(0x10)) {
        D_800CEA40_29->f170 = 9;
        D_800CEA40_29->sub->flags |= 1;
        D_800CEB9C_7.f10 = 3;
        D_800CEB9C_7.f6 = 3;
        D_800CEB9C_7.obj->f34 |= 0x10;
        D_800CEB9C_7.obj->f3C |= 1 << (D_800CEA40_29->f16F4 - 1);
    } else {
        if (PRESSED_4(0x10000)) {
            Tab_9 *b = D_800CE280;
            s32 k = D_800CEA40_29->f16FC + 0x12;
            Tab_9 *t;
            t = &b[k];
            if (t->n >= 5 && !(t->fC & 0x200)) {
                if (D_800CEA40_29->f16F8 == 1 || D_800CEA40_29->f16F8 == t->n * 2 - 1) {
                    D_800CEA40_29->f17FC = 1;
                    D_800CEA40_29->f174 |= 0x1000;
                    D_800CEA40_29->sub->flags &= ~1;
                    D_800CEA40_29->f17FE = 0;
                    goto end;
                }
            }
        }
        if ((PRESSED_4(0x4000) && (D_800CEA40_29->f174 & 1)) || (PRESSED_4(0x2000) && !(D_800CEA40_29->f174 & 1))) {
            if (D_800CEA40_29->f16F8 >= 2) {
                D_800CEA40_29->f16F8--;
                func_8001102C(0);
            } else {
                Tab_9 *b = D_800CE280;
            s32 k = D_800CEA40_29->f16FC + 0x12;
                Tab_9 *t;
            t = &b[k];
                if (t->n < D_800CEA40_29->f1702 && !(t->fC & 0x200)) {
                    D_800CEA40_29->f17FC = 0;
                    D_800CEA40_29->f174 |= 0x1000;
                    D_800CEA40_29->sub->flags &= ~1;
                    D_800CEA40_29->f17FE = 0;
                }
            }
        }
        if ((PRESSED_4(0x4000) && !(D_800CEA40_29->f174 & 1)) || (PRESSED_4(0x2000) && (D_800CEA40_29->f174 & 1))) {
            Tab_9 *b = D_800CE280;
            s32 k = D_800CEA40_29->f16FC + 0x12;
            Tab_9 *t;
            t = &b[k];
            if (D_800CEA40_29->f16F8 < t->n * 2 - 1) {
                D_800CEA40_29->f16F8++;
                func_8001102C(0);
            } else if (t->n < D_800CEA40_29->f1702 && !(t->fC & 0x200)) {
                D_800CEA40_29->f17FC = 0;
                D_800CEA40_29->f174 |= 0x1000;
                D_800CEA40_29->sub->flags &= ~1;
                D_800CEA40_29->f17FE = 0;
            }
        }
        func_8009DF4C();
    }
end:
    {
        Tab_9 *b = D_800CE280;
            s32 k = D_800CEA40_29->f16FC + 0x12;
        Tab_9 *t;
            t = &b[k];
        func_800A4FF0(t->a, t->b, D_800F55B8[D_800CEA40_29->f16FC], t->n);
    }
}

s32 func_8009EB00(s32 a, s32 b)
{
    return b;
}

s32 func_8009EB08(Ctx_4 *c)
{
    Node_31 *n = c->head;
    if (n != NULL && c->end != NULL) {
        do {
            if (n == c->end) {
                return 1;
            }
            n = n->next;
        } while (n != NULL);
        return 0;
    }
    return 1;
}

void func_8009EB4C(void) {
    u8 *p = D_800CEA40;
    u8 *n;
    if (*(u8 **)(p + 0x190) != 0) {
        return;
    }
    n = *(u8 **)(p + 0x17C);
    if (n == 0) {
        return;
    }
    do {
        if (*(u16 *)(*(u8 **)(n + 0x14) + 0xC) & 2) {
            *(u8 **)(p + 0x190) = n;
            return;
        }
        n = *(u8 **)(n + 4);
        if (n == *(u8 **)(p + 0x17C)) {
            return;
        }
    } while (n != 0);
}

void func_8009EBA8(s32 arg0)
{
    u8 *p = D_800CEA40;
    if (*(u32 *)(p + 0x174) & 1) {
        s32 idx = *(s32 *)(p + 0x16FC);
        *(s32 *)(p + 0x16F8) = D_800CE280[idx + 0x12].h1 * 2 - 1;
        return;
    }
    *(s32 *)(p + 0x16F8) = 1;
}

typedef struct {
    u8 pad[0x172C];
    s32 a;
    s32 b;
    s32 c;
} Obj_74;

extern Obj_74 *D_800CEA40_30;

void func_8009EBF4(void)
{
    f32 x, y;
    s32 d;

    x = D_800CEA40_30->a;
    D_800CEA40_30->c = ((x - 20000.0f) * 38000.0f) / 165000.0f + 40000.0f;
    y = D_800CEA40_30->c;
    d = __builtin_sqrtf(y * y + x * x);
    D_800CEA40_30->b = func_80013010((D_800CEA40_30->c << 14) / d & 0x3FFF) & 0xFFFF;
}

typedef struct Gs_3 {
    u8 pad[0x1738];
    s32 f1738;
    s32 f173c;
    s32 f1740;
} Gs_3;

extern Gs_3 *D_800CEA40_31;

void func_8009ECE0(void) {
    Gs_3 *g;
    f32 x;
    s32 n;
    f32 hyp;

    g = D_800CEA40_31;
    x = (f32)g->f1738;
    n = (s32)((x - 20000.0f) * 32000.0f / 100000.0f + 8000.0f);
    g->f1740 = n;
    hyp = __builtin_sqrtf((f32)n * (f32)n + x * x);
    D_800CEA40_31->f173c = func_80013010((D_800CEA40_31->f1740 << 14) / (s32)hyp & 0x3FFF) & 0xFFFF;
}

s32 func_8009EDCC(s32 mode, s32 v, s32 d, s32 f)
{
    s32 iv1;
    s32 iv2;
    s32 q;
    s32 num;
    s32 r;
    s32 res;
    s32 ta;
    s32 tb;
    volatile s32 pad[2];

    res = 0;
    if (v != 0) {
        if (mode == 1) {
            goto m1;
        }
        if (mode == 0) {
            goto m0;
        }
        if (mode == 2) {
            goto m2;
        }
        if (mode == 3) {
            goto m3;
        }
        iv1 = 0;
        iv2 = 0;
        goto dv;
    m0:
        iv1 = 0x4000;
        iv2 = 0x4000;
        v = v << 1;
        goto common;
    m1:
        iv1 = 0x4000;
        iv2 = 0x8000;
        v = v << 1;
        goto common;
    m2:
        iv1 = 0x8000;
        iv2 = 0x4000;
        goto common;
    m3:
        num = v << 10;
        q = num / d;
        r = num % d;
        if (f == 0) {
            q = q + r;
        }
        return q;
    dv:
        v = iv1;
    common:
        q = (iv1 << 10) / d;
        ta = (s32)D_800C2C60[(((q * f) >> 10) + iv2 >> 4) - 0x400 & 0xFFF];
        f = f + 1;
        tb = (s32)D_800C2C60[(((q * f) >> 10) + iv2 >> 4) - 0x400 & 0xFFF];
        r = ta - tb;
        res = (r * v) >> 5;
    }
    return res;
}

typedef struct {
    u8 pad0[0x174];
    u32 f174;
    u8 pad1[0x16E0 - 0x178];
    s32 cur;
    s32 e4;
    s32 state;
} Ctx_5;

extern Ctx_5 *D_800CEA40_32;

void func_8009EF20(s32 a0)
{
    Ctx_5 *st = D_800CEA40_32;
    if (st->cur != a0) {
        s32 s = st->state;
        if (s == 1) {
            st->cur = a0;
            goto tail;
        }
        if (s == 0) goto c0;
        if (s == 2) goto c2;
        if (s == 3) goto c3;
        goto tail;
c0:
        st->state = 1;
        st->e4 = a0;
        goto tail;
c2:
        st->state = 3;
        st->e4 = a0;
        goto tail;
c3:
        st->state = 1;
        st->cur = a0;
tail:
        D_800CEA40_32->f174 &= 0xFFFEFFFF;
    }
}

void func_8009EFA4(void)
{
    u32 *dst = &D_800CE21C;
    u32 *src = &D_800CE210;
    u32 a, b, c;

    a = src[0];
    b = src[1];
    c = dst[2];
    D_800CE240 = 0;
    dst[0] = a;
    dst[1] = b;
    dst[2] = ((c + 0x800) >> 13) << 13;
}

#define G_6 ((u8 *)D_800CEA40_33)

void func_8009EFE4(s32 a0) {
    u8 *p;
    s32 i;

    *(s32 *)(G_6 + 0x17C) = 0;
    *(s32 *)(G_6 + 0x180) = 0;
    *(s32 *)(G_6 + 0x184) = 0;
    *(s32 *)(G_6 + 0x190) = 0;
    *(s16 *)(G_6 + 0x16B6) = 0;
    *(s16 *)(G_6 + 0x16B8) = 0;

    p = G_6 + 0x14F4;
    i = 0x95;
    do {
        *(s16 *)(p + 0x194) = 0;
        i--;
        p -= 0x24;
    } while (i >= 0);

    if (a0 != 0) {
        *(s32 *)(G_6 + 0x16F4) = 8;
    }
    strcpy((char *)D_800CEA40_33 + 0x16BA, "<NONE>");
}

typedef struct { u8 pad[0x3E8]; void *ptr; } GS_2;
typedef struct { u8 b[0x310]; } Blk_24;

extern GS_2 D_800EFD70_90;
extern Blk_24 D_800F1DF4_2;

void func_8009F068(void)
{
    D_800EFD70_90.ptr = func_800BC908(0x400, 0, D_800DAB1C, 0x6FE);
    func_800A42D0(D_800EFD70_90.ptr);
    D_800F1DF4_2 = *(Blk_24 *)D_800EFD70_90.ptr;
    func_8006F5F8(4, 0);
    func_800BCB04(D_800EFD70_90.ptr);
    D_800EFD70_90.ptr = 0;
}

typedef struct Sub_12 {
    u8 pad[0xC];
    u16 flags;
} Sub_12;

typedef struct Node_32 {
    s32 w0;
    struct Node_32 *next;
    s32 w8;
    s32 w12;
    s32 w16;
    Sub_12 *sub;
} Node_32;

typedef struct Obj_75 {
    u8 p0[0x17C];
    Node_32 *list;
    u8 p1[4];
    Node_32 *cur;
    u8 p2[8];
    Node_32 *other;
} Obj_75;

s32 func_8009F170(void) {
    Obj_75 *o = D_800CEA40;
    Node_32 *a = o->cur;
    Node_32 *n;
    s32 r;
    if (a == NULL || a != o->other || (a->w8 == 0 && a->next == NULL)) {
        goto ret1;
    }
    n = o->list;
    r = 0;
    while (n != NULL) {
        if (n != a && (n->sub->flags & 2) != 0) {
            r = 1;
            break;
        }
        n = n->next;
        if (n == o->list) {
            break;
        }
    }
    return r;
ret1:
    return 1;
}

void func_8009F1F8(s32 param_1) {
    s32 p = param_1;
    *(s32 *)(D_800CEA40 + 0x1754) = 0;
    guMtxL2F(D_800CEA40 + 0x1760, D_800F4F08_3 + (p << 6));
    func_8000ED10(*(u8 **)((s32)D_800CEA40 + (p << 2) + 0x174C), 0, 0xF00);
}

typedef struct {
    u8 pad0[0xC];
    u16 f0C;
    u8 rest[0x40 - 0xE];
} Slot_14;

typedef struct Node_33 {
    u8 pad0[4];
    struct Node_33 *next;
    u32 f8;
    u8 pad1[0x14 - 0xC];
    Slot_14 *owner;
} Node_33;

typedef struct {
    u8 pad0[0x17C];
    Node_33 *head;
} Obj_76;

void func_8009F260(void)
{
    Obj_76 *obj;
    Slot_14 *blk;
    Node_33 *n;
    s32 i;
    s32 cnt, flag;
    s32 m;
    i = 0;
    obj = D_800CEA40;
    m = -0x601;
    for (; i < 8; i++) {
        u32 c;
        blk = &D_800CE700[i];
        c = blk->f0C;
        n = obj->head;
        blk->f0C = c & m;
        cnt = 0;
        flag = cnt;
        if (n != 0) {
            do {
                if (n->owner == blk) {
                    cnt++;
                    if (n->f8 != 0 && n->next != 0) {
                        flag = 1;
                    }
                }
                n = n->next;
            } while (n != obj->head && n != 0);
        }
        if (flag) {
            blk->f0C = blk->f0C | 0x200;
        }
        if (cnt >= 2) {
            blk->f0C = blk->f0C | 0x600;
        }
    }
}

typedef struct {
    s32 unk0;
    s32 p;
    u8 pad[0x18];
} Cell_10;

typedef struct {
    s16 unk0;
    s16 rows;
    u8 pad4[0x2C];
    Cell_10 *a;
    Cell_10 *b;
    u8 pad38[8];
} Rec_52;

typedef struct {
    Rec_52 rec[28];
    u8 pad[0x30];
    u32 *w;
} Big_3;

extern Big_3 D_800CE280_13;

void func_8009F310(s32 n)
{
    u32 *wp;
    s32 i, j;
    u32 k;
    u32 o;
    Cell_10 *a;
    Cell_10 *b;

    wp = D_800CE280_13.w;
    for (i = 0; i < n; i++) {
        a = D_800CE280_13.rec[i + 18].a;
        b = D_800CE280_13.rec[i + 18].b;
        for (j = 0; j < D_800CE280_13.rec[i + 18].rows; j++) {
            for (k = 0; k < *wp; k++) {
                o = (j * *wp + k) * 32;
                func_800BCB04(*(s32 *)((u8 *)a + o + 4));
                o = (j * *wp + k) * 32;
                func_800BCB04(*(s32 *)((u8 *)b + o + 4));
            }
        }
        func_800BCB04(a);
        func_800BCB04(b);
    }
}
