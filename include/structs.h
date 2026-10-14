/* Types several source files of the game use. */
#ifndef STRUCTS_H
#define STRUCTS_H

#include "common.h"

/* Declared here for the prototypes and globals; most are defined in the source file that uses them. */
typedef struct Dyn Dyn;
typedef struct Slot_2 Slot_2;
typedef struct MtxS_2 MtxS_2;
typedef struct Seg_3 Seg_3;
typedef struct Model Model;
typedef struct ImgSet ImgSet;
typedef struct Hdr Hdr;
typedef struct Res_3 Res_3;
typedef struct Model_2 Model_2;
typedef struct List_4 List_4;
typedef struct Lst Lst;
typedef struct Ent_19 Ent_19;
typedef struct Ent_22 Ent_22;
typedef struct Rec6 Rec6;
typedef struct Rec_4 Rec_4;
typedef struct Ent_2 Ent_2;
typedef struct S S;
typedef struct Out_2 Out_2;
typedef struct Area_2 Area_2;
typedef struct Hdr_5 Hdr_5;
typedef struct Poly_2 Poly_2;
typedef struct List_6 List_6;
typedef struct Ent_35 Ent_35;
typedef struct Obj_30 Obj_30;
typedef struct Gen Gen;
typedef struct Elem_2 Elem_2;
typedef struct Pt_4 Pt_4;
typedef struct Obj_40 Obj_40;
typedef struct Rec_33 Rec_33;
typedef struct Obj_43 Obj_43;
typedef struct Spawn Spawn;
typedef struct Box_4 Box_4;
typedef struct Tri3_2 Tri3_2;
typedef struct Mdl_2 Mdl_2;
typedef struct Obj_53 Obj_53;
typedef struct Obj_55 Obj_55;
typedef struct Obj_58 Obj_58;
typedef struct Mdl2 Mdl2;
typedef struct Obj2 Obj2;
typedef struct Item_8 Item_8;
typedef struct Obj_61 Obj_61;
typedef struct Obj_62 Obj_62;
typedef struct Obj_63 Obj_63;
typedef struct Arg0 Arg0;
typedef struct Arg1 Arg1;
typedef struct Ctl_19 Ctl_19;
typedef struct Scr Scr;
typedef struct Ctl_21 Ctl_21;
typedef struct Obj_67 Obj_67;
typedef struct Menu_5 Menu_5;
typedef struct Game_36 Game_36;
typedef struct Rec_49 Rec_49;
typedef struct Spr_5 Spr_5;
typedef struct Set Set;
typedef struct Mesh_8 Mesh_8;
typedef struct Chan_6 Chan_6;
typedef struct Ring_2 Ring_2;
typedef struct Obj_80 Obj_80;
typedef struct Handle Handle;
typedef struct Ent_99 Ent_99;
typedef struct Ring_3 Ring_3;
typedef struct Hdr_13 Hdr_13;
typedef struct Slot_17 Slot_17;
typedef struct Stream_2 Stream_2;
typedef struct Blk_29 Blk_29;
typedef struct Obj_82 Obj_82;
typedef struct ListHdr ListHdr;
typedef struct ResFile_2 ResFile_2;
typedef struct Hdr_14 Hdr_14;
typedef struct Src_4 Src_4;
typedef struct Src_5 Src_5;
typedef struct Out_6 Out_6;
typedef struct Obj_83 Obj_83;
typedef struct Font_6 Font_6;
typedef struct Text Text;

typedef struct { u8 r, g, b; } Rgb;
typedef struct { s32 pad0; u32 *tbl; } ResFile;
typedef struct { s32 w[4]; } Blk;
typedef struct { u8 pad[0xA0]; } Rec;
typedef struct { s32 a; s32 b; } Pair;

typedef struct {
    s32 w[16];
} Blk64;

typedef struct {
    u8 pad[0x68];
} Rec68;

typedef struct { s32 x; s32 y; } Pair_2;

typedef struct {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} Rect;

typedef struct { s16 x; s16 y; } Pos;

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    s32 f0;
    s32 f4;
} Pal;

typedef struct {
    u8 pad[0x2C];
    s32 f2C;
} Cfg;

typedef struct { u8 w, h, pad, pal; u8 *addr; } Img;
typedef struct { s32 x, y, z, w; } V4;

typedef struct Vec {
    s32 x, y, z;
} Vec;

typedef struct { u8 pad[0x22]; s16 n; } Cnt;
typedef struct { s32 m[9]; } Mtx3;
typedef struct { f32 x, y, z; u8 pad[0x60]; } Spot;

typedef struct Rec_2 {
    u8 pad[0xA0];
} Rec_2;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 f14;
} Gear;

typedef struct MtxS {
    s16 i[4][4];
    u16 f[4][4];
} MtxS;

typedef struct {
    u32 a;
    u32 b;
} Entry;

typedef struct {
    u8 pad[0x1C];
} Ent1C;

typedef struct { u8 pad[0x64]; s16 f64; } Obj64;

typedef struct {
    u8 pad[0x10];
    s32 f10;
} S29CC;

typedef struct {
    u8 pad0[0x2C4];
    u8 f2C4;
    u8 f2C5;
    u8 f2C6;
    u8 f2C7;
} Opt;

typedef struct { u32 w[4]; } Pad16;

typedef struct {
    s32 key;
    s32 val;
} Pair_4;

typedef struct {
    f32 x, z;
} Start;

typedef struct {
    u8 unk0[0x28];
    u32 flags;
    u8 unk2C[0x40 - 0x2C];
} Ent64;

typedef struct { f32 x, y, z; } V3;
typedef struct {
    u8 pad[0x28];
    s32 f28;
} Info;

typedef struct { s32 a, b, c; } T3;

typedef struct {
    u8 pad0[4];
    s32 f4;
} Trk_2;

typedef struct {
    f32 x, y, z, w;
} Quat;

typedef struct { s32 a, b, c, d; } Quad;
typedef struct { u8 pad[0x28]; u32 flags; } Trk;

typedef struct {
    s16 x, y, z;
    s16 pad[5];
} Vtx16;

typedef struct { u8 pad[0x2C]; s32 mode; } Game_2;

typedef struct {
    u8 w;
    u8 h;
    u8 pad[2];
    u8 *img;
} TexInfo;

typedef struct {
    u32 w0, w1;
} Blk_2;

typedef struct Facet_2 {
    u8 p00[8];
    Vec v[3];
} Facet_2;

typedef struct { s32 w[3]; } V3_2;

typedef struct Field {
    s32 x;
    s32 z;
    s32 f8;
    s32 seed;           /* 0x0C */
    s32 sp;             /* 0x10 */
    u32 stack[64];      /* 0x14 */
    u8 rowE[129];       /* 0x114 */
    u8 colE[129];       /* 0x195 */
    u8 buf[2][128];     /* 0x216 */
    s32 toggle;         /* 0x318 */
    u8 *map;            /* 0x31C */
} Field;

typedef struct Obj_34 {
    s32 x;      /* 0x324 */
    s32 val;    /* 0x328 */
    s32 z;      /* 0x32C */
    s32 f330;
    s32 f334;
    s32 type;   /* 0x338 */
    s32 idx;    /* 0x33C */
} Obj_34;

typedef struct Field_2 {
    s32 x;
    s32 z;
    u8 pad[0x314];
    s8 *map;    /* 0x31C */
    s32 count;  /* 0x320 */
    Obj_34 obj[8]; /* 0x324 */
} Field_2;


typedef struct {
    u8 pad0[0x34];
    s32 x34, x38, x3C;
    u8 rot[1];
} Model_5;

typedef struct {
    f32 x, y, z;      /* 0x00 */
    f32 vx, vy, vz;   /* 0x0C */
    s32 i18;          /* 0x18 */
    f32 f1C;          /* 0x1C */
    union { f32 f; s32 i; } f20;          /* 0x20 */
    u8 pad24[0x40 - 0x24];
    f32 r40, r44, r48; /* 0x40 */
} Body_2;

typedef struct { f32 q[4]; } Quat_2;

typedef struct { s32 w[7]; } V7;
typedef struct { u8 pad[0x24]; } Ent24;
typedef struct { u8 b0, b1, b2, b3; } Pts;
typedef struct { s32 f0; u8 pad[0x48]; s32 f4C; } Cfg_2;
typedef struct { u8 w; u8 h; u8 pad[2]; void *img; } Img_2;

typedef struct {
    u8 pad0[0x50];
    s32 w50;
} Track;

typedef struct {
    u8 pad0[0x3C];
    s32 count;
} Ctr;

typedef struct { u32 mask; u8 pad[0x1C]; } Obj;

typedef struct Node_16 {
    s32 w0;
    f32 x;
    f32 y;
    f32 z;
    s32 w10;
    s32 w14;
    s32 w18;
    f32 r;
    struct Node_16 *c1;
    struct Node_16 *c2;
} Node_16;

typedef struct Param {
    s32 kid;
    s32 kid2;
    f32 x;
    f32 y;
    f32 z;
    f32 r;
} Param;

typedef struct Bv {
    u8 flag;
    u8 pad[7];
    f32 f8;
    f32 fc;
    f32 f10;
    f32 f14;
    f32 f18;
    f32 f1c;
    struct Bv *next;
    struct Bv *child;
} Bv;

typedef struct Xf {
    u8 pad[0xc];
    f32 m0, m1, m2, m3, m4, m5;
} Xf;

typedef struct Xf_2 {
    u8 pad0[0xc];
    f32 f0c;
    f32 f10;
    f32 f14;
    f32 f18;
    f32 f1c;
    f32 f20;
} Xf_2;

typedef struct Box_3 {
    u8 flag;
    u8 pad1[7];
    f32 f08;
    f32 f0c;
    f32 x;
    f32 y;
    f32 z;
    f32 r;
    struct Box_3 *next;
    struct Box_3 *child;
} Box_3;

typedef struct Node_17 {
    u8 flag;
    u8 pad1[0x1F];
    struct Node_17 *next;
    struct Node_17 *child;
} Node_17;

typedef struct Node_18 {
    u8 flag;
    u8 pad0[0x1F];
    struct Node_18 *sib;
    struct Node_18 *first;
} Node_18;

typedef struct {
    u8 pad[0x1C];
    u32 flags;
} ObjInfo;

typedef struct { u16 a, b; } Pal_2;

typedef struct Obj_51 {
    s32 pad0;
    u16 *src;
} Obj_51;

typedef struct {
    s32 f0;
    u32 pal;            /* 0x04 */
    u8 img[1];          /* 0x08 */
} Pic;

typedef struct {
    u32 w[7];
} Rec_3;

typedef struct Item_9 {
    u8 pad0[4];
    struct Item_9 *p4;
    struct Item_9 *p8;
    u8 padC[0x14 - 0xC];
    s16 h14;
    u8 pad16[2];
    s16 h18;
    u8 pad1A[0x20 - 0x1A];
    s32 *p20;
    u8 pad24[4];
    s32 p28;
} Item_9;

typedef struct Item_11 {
    u8 pad0[4];
    s32 w4;
    u8 pad8[0x10 - 0x8];
    u16 h10;
    s16 h12;
    s16 h14;
    s16 h16;
    s16 h18;
    u8 pad1A[2];
    s32 **p1C;
    s32 *p20;
    s32 w24;
    u8 pad28[4];
} Item_11;

typedef struct MenuItem {
    s32 f0;
    s32 f4;
    s32 f8;
    void (*func)(s32);
} MenuItem;

typedef struct Item_12 {
    s32 f0;
    struct Item_12 *child;   /* 0x04 */
    struct Item_12 *next;    /* 0x08 */
    MenuItem *items;      /* 0x0C */
    u16 flags;            /* 0x10 */
    u8 pad12[0x20 - 0x12];
    s32 *sel;             /* 0x20 */
    s32 f24;
    struct Item_12 *parent;  /* 0x28 */
} Item_12;

typedef struct Slot {
    s32 w0;
    u32 flags;
    s16 n;
    s16 cur;
    u32 mask;
} Slot;

typedef struct Ctl_22 {
    s32 w0;
    s16 n;
    s16 cur;
    Slot *ents;
    u32 flags;
    s16 last;
} Ctl_22;

typedef struct Ctl_2 {
    s32 w0;
    s16 count;
    s16 x;
    Slot *slots;
    s32 w3;
    s16 sel;
} Ctl_2;

typedef struct Node_28 {
    s32 f0;
    struct Node_28 *a;     /* 0x04 */
    struct Node_28 *b;     /* 0x08 */
    s16 x;              /* 0x0C */
    s16 y;              /* 0x0E */
    u8 pad10[8];
    s16 dirA;           /* 0x18 */
    s16 dirB;           /* 0x1A */
    s16 f1C;            /* 0x1C */
    s16 f1E;            /* 0x1E */
    s16 w;              /* 0x20 */
    s16 h;              /* 0x22 */
} Node_28;

typedef struct Node_29 {
    s16 used;
    s16 pad2;
    struct Node_29 *next;
    struct Node_29 *prev;
    s16 x;
    s16 y;
    s16 st;
    s16 idx;
    s16 *pt;
    u8 pad18[0xC];
} Node_29;

typedef struct Blk_22 {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s16 h10;
    s16 k12;
    s16 *P;
} Blk_22;

typedef struct Spr_4 {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s16 h10;
    s16 rot;
    s16 *P;
} Spr_4;

typedef struct Node_30 {
    u8 pad[0xC];
    s16 px;
    s16 py;
    u16 flags;
    u16 cnt;
    u16 *tab;
} Node_30;

typedef struct Obj_72 {
    s32 f0;
    struct Obj_72 *next;   /* 0x04 */
    s32 f8;
    s16 x;              /* 0x0C */
    s16 y;              /* 0x0E */
    u16 flags;          /* 0x10 */
    s16 f12;
    s32 f14;
    s16 dir[2];         /* 0x18 */
    s16 len[2];         /* 0x1C */
    s16 w;              /* 0x20 */
    s16 h;              /* 0x22 */
} Obj_72;

typedef struct {
    s16 h0;
    s16 h1;
    u8 rest[60];
} Row;

typedef struct Node_31 {
    s32 f0;
    struct Node_31 *next;
} Node_31;

typedef struct Ctx_4 {
    s32 f0;
    Node_31 *head;
    Node_31 *end;
} Ctx_4;

typedef struct Ring {
    s32 f00;
    s32 cur;        /* 0x04 */
    s32 slot[2];    /* 0x08 */
    u8 flag[2];     /* 0x10 */
    u8 f12[2];      /* 0x12 */
    u32 size;       /* 0x14 */
    u8 *buf;        /* 0x18 */
    u32 pos;        /* 0x1c */
    u32 f20;        /* 0x20 */
    u32 avail;      /* 0x24 */
    s32 arg28;      /* 0x28 */
    u32 len;        /* 0x2c */
    u8 b30;
    u8 b31;
    u8 b32;
    u8 sel;         /* 0x33 */
} Ring;

typedef struct Node_45 {
    struct Node_45 *prev;
    struct Node_45 *next;
    s32 pad[9];
    s32 key;
    s32 flags;
} Node_45;

typedef struct Node_46 {
    struct Node_46 *prev;
    struct Node_46 *next;
} Node_46;

typedef struct Block {
    struct Block *next;
    s32 unk4;
    s32 size;
    s32 check;
} Block;

typedef struct Ent_106 {
    struct Ent_106 *next;
    struct Ent_106 *prev;
    u8 *buf;
    s32 owner;
    s32 idx;
} Ent_106;

typedef struct X {
    u8 pad[0x10];
    s32 flags;
    s32 pad2;
    u8 *base;
    u32 size;
} X;

typedef struct Obj_81 {
    X *x;
    Ent_106 *ent;
} Obj_81;

typedef struct Node_50 {
    struct Node_50 *next;
    struct Node_50 *prev;
    s32 pad[2];
    s32 cnt;
} Node_50;

typedef struct Q_2 {
    Node_50 *a;
    Node_50 *b;
    s32 c;
    s32 d;
} Q_2;

typedef struct Node_51 {
    struct Node_51 *next;
    struct Node_51 *prev;
    s32 f8;
    s32 key;
    s32 cnt;
} Node_51;

typedef struct Q_3 {
    s32 *p0;
    s32 f4;
    s32 f8;
    s32 f0c;
    s32 f10;
    s32 *f14;
} Q_3;

typedef struct Spr {
    struct Spr *next;
    s16 x;
    s16 y;
    u8 w, h, s, t;
    u8 r, g, b, a;
} Spr;

#include "structs/rider.h"

#endif
