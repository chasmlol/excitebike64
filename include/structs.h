/* Types several source files of the game use. */
#ifndef STRUCTS_H
#define STRUCTS_H

#include "common.h"

/* Declared here for the prototypes and globals; most are defined in the source file that uses them. */
typedef struct Dyn Dyn;
typedef struct Slot_2 Slot_2;
typedef struct MtxS_2 MtxS_2;
typedef struct Obj_5 Obj_5;
typedef struct Seg_3 Seg_3;
typedef struct Model Model;
typedef struct ImgSet ImgSet;
typedef struct Hdr Hdr;
typedef struct Res_3 Res_3;
typedef struct Obj_7 Obj_7;
typedef struct Bike_4 Bike_4;
typedef struct Wheel Wheel;
typedef struct Bike_5 Bike_5;
typedef struct Rec_9 Rec_9;
typedef struct Cam Cam;
typedef struct Bike_6 Bike_6;
typedef struct Bike_7 Bike_7;
typedef struct Bike_8 Bike_8;
typedef struct Obj_9 Obj_9;
typedef struct Bike_42 Bike_42;
typedef struct Bike_10 Bike_10;
typedef struct Body Body;
typedef struct Bike_12 Bike_12;
typedef struct Ent_12 Ent_12;
typedef struct Obj_10 Obj_10;
typedef struct Obj_11 Obj_11;
typedef struct Model_2 Model_2;
typedef struct List_4 List_4;
typedef struct Lst Lst;
typedef struct Ent_19 Ent_19;
typedef struct Ent_22 Ent_22;
typedef struct Player_10 Player_10;
typedef struct Obj_13 Obj_13;
typedef struct Obj_14 Obj_14;
typedef struct Bike_16 Bike_16;
typedef struct Rec6 Rec6;
typedef struct Obj_15 Obj_15;
typedef struct Bike_17 Bike_17;
typedef struct Rec_4 Rec_4;
typedef struct Ent_23 Ent_23;
typedef struct Bike_18 Bike_18;
typedef struct Ent_2 Ent_2;
typedef struct S S;
typedef struct Obj_17 Obj_17;
typedef struct Obj_18 Obj_18;
typedef struct Actor Actor;
typedef struct Obj_20 Obj_20;
typedef struct Obj_21 Obj_21;
typedef struct Out_2 Out_2;
typedef struct Obj_22 Obj_22;
typedef struct Area_2 Area_2;
typedef struct Hdr_5 Hdr_5;
typedef struct Poly_2 Poly_2;
typedef struct Obj_27 Obj_27;
typedef struct Obj_29 Obj_29;
typedef struct Node_6 Node_6;
typedef struct Rec_20 Rec_20;
typedef struct Blk_10 Blk_10;
typedef struct List_6 List_6;
typedef struct Ent_35 Ent_35;
typedef struct Joint Joint;
typedef struct Obj_30 Obj_30;
typedef struct Joint_2 Joint_2;
typedef struct Obj_31 Obj_31;
typedef struct Row_4 Row_4;
typedef struct Gen Gen;
typedef struct Elem_2 Elem_2;
typedef struct Pt_4 Pt_4;
typedef struct Body_4 Body_4;
typedef struct Obj_35 Obj_35;
typedef struct Bike_28 Bike_28;
typedef struct Ent_47 Ent_47;
typedef struct Bike_33 Bike_33;
typedef struct Bike_34 Bike_34;
typedef struct Obj_40 Obj_40;
typedef struct Rec_33 Rec_33;
typedef struct Obj_42 Obj_42;
typedef struct Obj_43 Obj_43;
typedef struct Spawn Spawn;
typedef struct Player_22 Player_22;
typedef struct Ent_65 Ent_65;
typedef struct Obj_47 Obj_47;
typedef struct Box_4 Box_4;
typedef struct Tri3_2 Tri3_2;
typedef struct Mdl_2 Mdl_2;
typedef struct Obj_53 Obj_53;
typedef struct Obj_55 Obj_55;
typedef struct Ent_78 Ent_78;
typedef struct Obj_56 Obj_56;
typedef struct Ent_79 Ent_79;
typedef struct Obj_58 Obj_58;
typedef struct Obj_59 Obj_59;
typedef struct Obj_60 Obj_60;
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
#include "structs/nes.h"
#include "structs/softmix.h"
#include "structs/sfx.h"
typedef struct Slot_17 Slot_17;
typedef struct Stream_2 Stream_2;
typedef struct Blk_29 Blk_29;
typedef struct Obj_82 Obj_82;
typedef struct ListHdr ListHdr;
typedef struct ResFile_2 ResFile_2;
typedef struct Obj_83 Obj_83;
typedef struct Font_6 Font_6;
typedef struct Text Text;
typedef struct Bike_25 Bike_25;

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
typedef struct Sub_3 { u8 p0[0x1c]; f32 s1c; f32 s20; f32 s24; } Sub_3;

typedef struct Obj_8 {
    u8 p0[0x28]; f32 f28; f32 f2c; f32 f30; u8 p34[4]; f32 f38;
    u8 p3c[0x1c]; f32 f58; f32 f5c; f32 f60; u8 p64[0x2c]; Sub_3 *sub;
} Obj_8;

typedef struct Bike_9 {
    /* 0x000 */ u8 pad0[0x54];
    /* 0x054 */ Vec3f pos;
    /* 0x060 */ u8 pad60[0x88 - 0x60];
    /* 0x088 */ Vec3f v88;
    /* 0x094 */ u8 pad94[0xA0 - 0x94];
    /* 0x0A0 */ Vec3f mA0;
    /* 0x0AC */ Vec3f mAC;
    /* 0x0B8 */ Vec3f mB8;
    /* 0x0C4 */ u8 padC4[0x1BC - 0xC4];
    /* 0x1BC */ f32 f1BC;
    /* 0x1C0 */ u8 padC0[0x416 - 0x1C0];
    /* 0x416 */ s16 s416;
    /* 0x418 */ u8 pad418[0x448 - 0x418];
    /* 0x448 */ f32 f448;
    /* 0x44C */ u8 pad44C[0x458 - 0x44C];
    /* 0x458 */ s32 n458;
    /* 0x45C */ Vec3f v45C;
    /* 0x468 */ f32 f468;
    /* 0x46C */ u8 pad46C[0x4A0 - 0x46C];
    /* 0x4A0 */ Vec3f v4A0;
    /* 0x4AC */ u8 pad4AC[0x4BC - 0x4AC];
    /* 0x4BC */ f32 f4BC;
    /* 0x4C0 */ Vec3f v4C0;
    /* 0x4CC */ Vec3f v4CC;
    /* 0x4D8 */ Vec3f v4D8;
    /* 0x4E4 */ u8 pad4E4[0x4FC - 0x4E4];
    /* 0x4FC */ s32 n4FC;
    /* 0x500 */ Vec3f v500;
    /* 0x50C */ f32 f50C;
    /* 0x510 */ u8 pad510[0x544 - 0x510];
    /* 0x544 */ Vec3f v544;
    /* 0x550 */ u8 pad550[0x560 - 0x550];
    /* 0x560 */ f32 f560;
    /* 0x564 */ Vec3f v564;
    /* 0x570 */ Vec3f v570;
    /* 0x57C */ Vec3f v57C;
    /* 0x588 */ u8 pad588[0x5A0 - 0x588];
    /* 0x5A0 */ f32 f5A0;
    /* 0x5A4 */ u8 pad5A4[0x5D0 - 0x5A4];
    /* 0x5D0 */ s8 c5D0;
    /* 0x5D1 */ u8 pad5D1[0x7D0 - 0x5D1];
    /* 0x7D0 */ f32 f7D0;
} Bike_9;

typedef struct Bike_11 {
    u8 pad0[0x48];
    f32 f48[3];
    f32 pos[3];          /* 54 */
    u8 pad1[0xA0 - 0x60];
    f32 dir[3];          /* A0 */
    u8 pad2[0xC8 - 0xAC];
    f32 pts[8][3];       /* C8 */
    u8 pad3[0x1C5 - 0x128];
    u8 flags;            /* 1C5 */
    u8 pad4[0x438 - 0x1C6];
    f32 f438;
    u8 pad5[0x458 - 0x43C];
    s32 f458;
    u8 pad6[0x4FC - 0x45C];
    s32 f4FC;
    u8 pad7[0x7E0 - 0x500];
} Bike_11;

typedef struct {
    u8 pad[0x28];
    s32 f28;
} Info;

typedef struct { s32 a, b, c; } T3;

typedef struct Bike_13 {
    u8 pad0[0x18];
    s32 f18;
    u8 pad1C[0x8];
    s32 f24;
    u8 pad28[0x1C];
    u8 f44;
    u8 pad45[0x3];
    Vec3f pos;      /* 0x48 */
    Vec3f vel;      /* 0x54 */
    u8 pad60[0x28];
    Vec3f f88;
    u8 pad94[0xC];
    Vec3f fA0;
    Vec3f fAC;
    u8 padB8[0x2F8];
    s32 f3B0;
    u8 pad3B4[0x1C];
    s8 f3D0;
    u8 pad3D1[0x45];
    s16 f416;
    u8 pad418[0x8];
    f32 f420;
    u8 pad424[0x34];
    s32 f458;
    u8 pad45C[0x38];
    Vec3f f494;
    u8 pad4A0[0x5C];
    s32 f4FC;
    u8 pad500[0xB0];
    s32 f5B0;
    u8 pad5B4[0x4];
    s32 f5B8;
    u8 pad5BC[0x10];
    u8 f5CC;
    u8 f5CD;
    u8 pad5CE[0x2];
    s8 f5D0;
    u8 pad5D1;
    s8 f5D2;
    u8 pad5D3[0x4F];
    s16 f622;
    u8 pad624[0x4];
    u16 f628;
    u8 pad62A[0x2A];
    u8 f654;
    u8 pad655[0x93];
    u8 f6E8;
    u8 pad6E9[0x2];
    u8 f6EB;
    u8 f6EC;
    u8 pad6ED[0x3];
    f32 f6F0;
    f32 f6F4;
    f32 f6F8;
    Vec3f f6FC;
    Vec3f f708;
    s16 f714;
    u8 pad716[0x2];
    f32 f718;
    Vec3f f71C;
    Vec3f f728;
    s16 f734;
    u8 pad736[0x2];
    f32 f738;
    s32 f73C;
    s16 f740;
    u8 pad742[0x2];
    s32 f744;
    s32 f748;
    Vec3f f74C;
    f32 f758;
    u8 pad75C[0x5C];
    f32 f7B8;
    u8 pad7BC[0xC];
    f32 f7C8;
    u8 pad7CC[0x14];
} Bike_13;

typedef struct {
    u8 pad0[4];
    s32 f4;
} Trk_2;

typedef struct Bike_14 {
    u8 pad0[0x44];
    u8 f44;
    s8 f45;
    u8 pad46[0x342];
    u8 f388[4];
    u8 pad38C[0x44];
    s8 f3D0;
    u8 pad3D1[0x2E];
    u8 f3FF;
    u8 pad400[0x12];
    s16 f412;
    u8 pad414[0x5];
    u8 f419;
    u8 f41A;
    u8 f41B;
    u8 pad41C[0x4];
    f32 f420;
    f32 f424;
    u8 pad428[0x24];
    f32 f44C;
    u8 pad450[0x4];
    u8 f454[4];
    s32 f458;
    u8 pad45C[0x9C];
    u8 f4F8[4];
    s32 f4FC;
    u8 pad500[0xA8];
    u16 f5A8;
    u16 f5AA;
    u8 pad5AC[0x4];
    s32 f5B0;
    u8 pad5B4[0x4];
    s32 f5B8;
    u8 pad5BC[0x13];
    u8 f5CF;
    u8 f5D0;
    u8 pad5D1[0x97];
    s16 f668;
    u8 pad66A[0x26];
    s32 f690;
    u8 pad694[0x54];
    u8 f6E8;
    u8 pad6E9[0x5B];
    s32 f744;
    u8 pad748[0x98];
} Bike_14;

typedef struct {
    f32 x, y, z, w;
} Quat;

typedef struct { s32 a, b, c, d; } Quad;
typedef struct { u8 pad[0x28]; u32 flags; } Trk;

typedef struct Ent_24 {
    s32 f0;
    s32 f4;
    s32 f8;
    u8 pad0C[0x34 - 0xC];
    u16 f34;
    u8 pad36[0x48 - 0x36];
    f32 x, y, z;
    u8 pad54[0x5AA - 0x54];
    s16 f5AA;
    u8 pad5AC[0x638 - 0x5AC];
    f32 f638;
    u8 pad63C[0x7E0 - 0x63C];
} Ent_24;

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

typedef struct Model_4 {
    s32 f0;
    Vec3i pos;    /* 0x04 */
    Mtx3 rot;    /* 0x10 */
    Vec3i pos2;   /* 0x34 */
    Mtx3 rot2;   /* 0x40 */
    u8 pad64[0x2C];
    Mtx3 rot3;   /* 0x90 */
} Model_4;

typedef struct Iv3 { s32 w[3]; } Iv3;
typedef struct Blk9 { s32 w[9]; } Blk9;

typedef struct Bone_2 {
    s32 f00;
    Iv3 pos;
    Blk9 mtx;
    Iv3 ppos;
    Blk9 pmtx;
    u16 rot[3];
    s16 base[3];
    u8 p70[0x10];
} Bone_2;

typedef struct { s32 w[3]; } V3_2;

typedef struct Ent_39 {
    u8 p0[6];
    u16 f06;
    u8 p1[2];
    u16 f0A;
    u8 p2[2];
    u16 f0E;
    s32 f10;
    u8 p3[0x50];
    u16 f64;
    u16 f66;
    u16 f68;
    u16 f6A;
    u16 f6C;
    u16 f6E;
    u8 p4[0x10];
} Ent_39;

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

typedef struct Node_53 Node_53;

typedef struct {
    u8 pad0[0x34];
    s32 x34, x38, x3C;
    u8 rot[1];
} Model_5;

typedef struct Bike_23 {
    s32 p0[3];                 /* 0x000 */
    u8 pad0C[0x44 - 0x0C];
    u8 active;                 /* 0x044 */
    s8 x45;                    /* 0x045 */
    u8 pad46[2];
    Vec3f pos;                 /* 0x048 */
    Vec3f vel;                 /* 0x054 */
    u8 pad60[0x88 - 0x60];
    Vec3f p88;                 /* 0x088 */
    u8 pad94[0xA0 - 0x94];
    Vec3f mtx[3];              /* 0x0A0 */
    f32 xC4;
    Vec3f pts[11];           /* 0x0C8 */
    u8 pad14C[0x1B8 - 0x14C];
    f32 x1B8;                  /* 0x1B8 */
    u8 pad1BC[0x1C4 - 0x1BC];
    u8 x1C4;                   /* 0x1C4 */
    u8 x1C5;                   /* 0x1C5 */
    u8 pad1C6[0x1CC - 0x1C6];
    Vec3f p1CC;                /* 0x1CC */
    Vec3f p1D8;                /* 0x1D8 */
    u8 pad1E4[0x1FC - 0x1E4];
    f32 x1FC[4];               /* 0x1FC */
    Vec3f p20C;                /* 0x20C */
    u8 pad218[0x248 - 0x218];
    Node_53 *x248;                /* 0x248 */
    Vec3f pts2[8];             /* 0x24C */
    u8 pad2AC[0x33C - 0x2AC];
    f32 x33C;                  /* 0x33C */
    u8 pad340[0x349 - 0x340];
    u8 x349;                   /* 0x349 */
    u8 pad34A[0x36C - 0x34A];
    u8 x36C;                   /* 0x36C */
    u8 x36D;                   /* 0x36D */
    u8 pad36E[0x3AC - 0x36E];
    s32 x3AC;                  /* 0x3AC */
    u8 pad3B0[0x3CC - 0x3B0];
    s32 x3CC;                  /* 0x3CC */
    u8 x3D0;                   /* 0x3D0 */
    u8 x3D1;                   /* 0x3D1 */
    u8 pad3D2[0x3FB - 0x3D2];
    u8 x3FB;                   /* 0x3FB */
    u8 pad3FC[0x416 - 0x3FC];
    s16 x416;                  /* 0x416 */
    u8 pad418[0x420 - 0x418];
    f32 x420;                  /* 0x420 */
    u8 pad424[4];
    f32 x428;                  /* 0x428 */
    u8 pad42C[0x43C - 0x42C];
    Vec3f p43C;                /* 0x43C */
    u8 pad448[0x5A8 - 0x448];
    u16 x5A8;                  /* 0x5A8 */
    u16 x5AA;                  /* 0x5AA */
    u8 pad5AC[0x624 - 0x5AC];
    s16 x624;                  /* 0x624 */
    s16 x626;                  /* 0x626 */
    s16 x628;                  /* 0x628 */
    u8 pad62A[0x644 - 0x62A];
    s32 x644;                  /* 0x644 */
    u8 pad648[0x655 - 0x648];
    u8 x655;                   /* 0x655 */
    u8 pad656[0x658 - 0x656];
    s32 x658;                  /* 0x658 */
    Model_5 *x65C;               /* 0x65C */
    u8 pad660[0x6E8 - 0x660];
    u8 x6E8;                   /* 0x6E8 */
    u8 pad6E9[0x744 - 0x6E9];
    s32 x744;                  /* 0x744 */
    u8 pad748[0x7A8 - 0x748];
    u8 x7A8;                   /* 0x7A8 */
    u8 pad7A9[0x7E0 - 0x7A9];
} Bike_23;

typedef struct {
    f32 x, y, z;      /* 0x00 */
    f32 vx, vy, vz;   /* 0x0C */
    s32 i18;          /* 0x18 */
    f32 f1C;          /* 0x1C */
    union { f32 f; s32 i; } f20;          /* 0x20 */
    u8 pad24[0x40 - 0x24];
    f32 r40, r44, r48; /* 0x40 */
} Body_2;

typedef struct Bike_24 {
    u8 pad0[0x48];
    Body_2 b0;                 /* 0x48 */
    u8 pad94[0x1C5 - 0x94];
    u8 f1C5;
    u8 pad1C6[0x1CC - 0x1C6];
    Body_2 b1;                 /* 0x1CC */
    u8 pad218[0x349 - 0x218];
    u8 f349;
    u8 pad34A[0x36C - 0x34A];
    u8 f36C;
    u8 f36D;
    u8 pad36E[0x380 - 0x36E];
    f32 f380;
    f32 f384;
    u8 pad388[0x3AC - 0x388];
    s32 f3AC;
    u8 pad3B0[0x3CC - 0x3B0];
    s32 f3CC;
    s8 f3D0;
    u8 f3D1;
    u8 pad3D2[0x428 - 0x3D2];
    f32 f428;
    u8 pad42C[0x478 - 0x42C];
    f32 f478;
    u8 pad47C[0x51C - 0x47C];
    f32 f51C;
    u8 pad520[0x5A0 - 0x520];
    f32 f5A0;
    u8 pad5A4[0x624 - 0x5A4];
    u16 f624;
    u8 pad626[0x6E8 - 0x626];
    u8 f6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Bike_24;

typedef struct { f32 q[4]; } Quat_2;

typedef struct Body_3 {
    Vec3f pos;          /* 0x00 */
    Vec3f vel;          /* 0x0C */
    Vec3f acc;          /* 0x18 */
    u8 pad24[0xC];
    Quat_2 q;             /* 0x30 */
    Vec3f angvel;       /* 0x40 */
    Vec3f angacc;       /* 0x4C */
    f32 m[9];           /* 0x58 */
    s32 f7C;            /* 0x7C */
    Vec3f pts[20];      /* 0x80 */
    f32 f170;           /* 0x170 */
    u8 pad174[8];
    u8 type;            /* 0x17C */
    u8 flags;           /* 0x17D */
    s8 timer;           /* 0x17E */
    u8 pad17F;
    Bike_25 *owner;        /* 0x180 */
} Body_3;

struct Bike_25 {
    u8 pad0[0x44];
    u8 f44;
    s8 f45;
    u8 pad46[2];
    Body_3 b0;            /* 0x48 */
    Body_3 b1;            /* 0x1CC */
    s32 f350;           /* 0x350 */
    Vec3f f354;         /* 0x354 */
    u8 pad360[0x380 - 0x360];
    f32 f380;
    f32 f384;
    u8 pad388[0x3D0 - 0x388];
    u8 f3D0;
    u8 pad3D1[0x416 - 0x3D1];
    s16 f416;
    u8 pad418[0x41D - 0x418];
    u8 f41D;
    u8 pad41E[0x420 - 0x41E];
    f32 f420;
    u8 pad424[0x43C - 0x424];
    Vec3f f43C;
    u8 pad448[0x478 - 0x448];
    f32 f478;
    f32 f47C;
    u8 pad480[0x494 - 0x480];
    Vec3f f494;
    u8 pad4A0[0x4C0 - 0x4A0];
    Vec3f f4C0;
    u8 pad4CC[0x51C - 0x4CC];
    f32 f51C;
    f32 f520;
    u8 pad524[0x538 - 0x524];
    Vec3f f538;
    u8 pad544[0x564 - 0x544];
    Vec3f f564;
    u8 pad570[0x6E8 - 0x570];
    u8 f6E8;
    u8 pad6E9[0x744 - 0x6E9];
    s32 f744;
    u8 pad748[0x764 - 0x748];
    u8 f764;
    u8 pad765[0x768 - 0x765];
    s32 f768;
    u8 pad76C[0x774 - 0x76C];
    s32 f774;
    u8 pad778[0x7A5 - 0x778];
    u8 f7A5;
    u8 pad7A6[0x7B4 - 0x7A6];
    f32 f7B4;
    u8 pad7B8[0x7D4 - 0x7B8];
    f32 f7D4;
    u8 pad7D8[0x7E0 - 0x7D8];
};

typedef struct Bike_26 {
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
} Bike_26;

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

typedef struct Player_21 {
    u8 pad0[0x34];
    s16 ang;
    u8 pad36[0x48 - 0x36];
    f32 x;
    f32 y;
    f32 z;
    u8 pad54[0x62C - 0x54];
    f32 f62C;
    u8 pad630[0x7A5 - 0x630];
    u8 f7A5;
    u8 pad7A6[0x7B0 - 0x7A6];
    s32 f7B0;
    u8 pad7B4[0x7D8 - 0x7B4];
    f32 f7D8;
} Player_21;

typedef struct {
    u8 pad[0x1C];
    u32 flags;
} ObjInfo;

typedef struct { u16 a, b; } Pal_2;

typedef struct Obj_51 {
    s32 pad0;
    u16 *src;
} Obj_51;

typedef struct Obj_52 {
    u8 pad0[0x48];
    f32 f48;
    u8 pad1[0x54 - 0x4C];
    s32 w54;
    u8 pad2[0x5D0 - 0x58];
    s8 b5d0;
    u8 pad3[0x744 - 0x5D1];
    s32 w744;
    u8 pad4[0x7A9 - 0x748];
    u8 u7a9;
    u8 u7aa;
} Obj_52;

typedef struct Obj_54 {
    u8 p0[0x48];
    f32 f48;
    u8 p1[4];
    f32 f50;
    u8 p2[0x628 - 0x54];
    s16 s628;
    u8 p3[0x630 - 0x62A];
    f32 f630;
    u8 p4[4];
    f32 f638;
    u8 p5[0x660 - 0x63C];
    s16 s660;
} Obj_54;

typedef struct {
    s32 f0;
    u32 pal;            /* 0x04 */
    u8 img[1];          /* 0x08 */
} Pic;

typedef struct {
    u32 w[7];
} Rec_3;

typedef struct Obj_57 {
    u8 pad0[0x22];
    u16 f22;
    u8 pad24[0x10];
    u16 f34;
    u8 pad36[0x3AC - 0x36];
    s32 f3AC;
    union {
        s32 w;
        struct { u16 hi; u16 lo; } h;
    } f3B0;
    s32 f3B4;
    s16 f3B8;
    s16 f3BA;
    u8 pad3BC[0x3D0 - 0x3BC];
    s8 f3D0;
    u8 pad3D1[2];
    s8 f3D3;
    u8 pad3D4[0x6F8 - 0x3D4];
    f32 f6F8;
    u8 pad6FC[0x744 - 0x6FC];
    s32 f744;
} Obj_57;

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

#endif
