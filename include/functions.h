/* A prototype for every function of the game. */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

/* src/game/render.c */
void func_80000450(ResFile *r, s32 n, s32 **ids, s32 arg3, s32 arg4, s32 arg5);
void func_80000F0C(void);
void func_8000115C(void);
void func_800020CC(void);
Gfx *func_80002418(Gfx *dl, Mtx *m, s32 n, s32 flag);
Gfx *func_80002418_2(Gfx *dl, s32 p2, s32 p3);
u32 *func_80002418_3();
u32 *func_80002418_4(u32 *dl, s32 p2, s32 p3);
s32 func_80002554(s32 x, s32 y, s32 z, s32 k, s32 c, s32 q, s32 n);
Gfx *func_800028D8(Gfx *gdl, s32 fb, u8 *mtx);
Gfx *func_800028D8_2();
Gfx *func_80002D20(Gfx *gdl, s32 fb, Dyn *arg2);
Gfx *func_800035AC(Gfx *gdl, s32 fb, u8 *mtx);
Gfx *func_80003AC4(Gfx *gdl);
void func_80003CC0(Dyn *dyn, s32 fb, Gfx **pgdl);
Gfx *func_80007010(Gfx *gdl, s32 vp, s32 p3, s32 xlu);
void func_80007D90(s32 pl, s32 k);
Gfx *func_80008B1C(Gfx *gdl, s32 a1, s32 a2);
Gfx *func_80008DE0(Gfx *gdl, s32 arg1);
Gfx *func_80009754(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_8000A690(s32 x, s32 y, Img *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw);
void func_8000B028(void);
Gfx *func_8000B3C8(Gfx *gdl);
void func_8000B938(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
Gfx *func_8000B9CC(Gfx *gdl, s32 a1, s32 a2, s32 a3, s32 a4);
void func_8000BA58(void);
void func_8000BA84(void);
void func_8000BB34(u8 *src, s32 x, s32 y, s32 w, s32 h);
void func_8000BBC8(u8 *param_1, u8 *param_2, u8 *param_3, s32 param_4, s32 param_5);
void func_8000BC30(u16 *p1, u16 *p2, u16 *p3, s32 a, s32 n);

/* src/game/podium.c */
void func_8000BD10(void);
void func_8000BEA0(void);
Gfx *func_8000C0E0(Gfx *gdl, s32 x, s32 arg2);
void func_8000C450(void);
void func_8000CC48(void);
void func_8000CC74(s32 a0, s32 a1, s32 a2, s32 *p4, s32 *p5);

/* src/game/unnamed/s0_8000CD00.c */
void func_8000CD00(void);
s32 func_8000CD00_2(void);
void func_8000CEAC(void);
int func_8000CFE8(void);
void start_controller_read(void);
void func_8000D0A0(u32 a0, s32 a1);
void func_8000D0D8(void);
void func_8000D1A8(s32 p1, u8 *p2, s32 p3);

/* src/game/main.c */
void func_8000D2E0(void);
void func_8000E31C(OSContPad *pad, u32 *out);
void func_8000E5C0(void);
void func_8000E6E8(void);
void func_8000E8B0(s32 param_1);
void func_8000E8F4(void);
void func_8000E980(void);
void func_8000E9B8(void);
void func_8000E9C0(void);
void func_8000EA24(void);
void func_8000EA2C(void);
void func_8000EA54(void);
s32 func_8000EA5C(void);

/* src/game/unnamed/s0_8000EB00.c */
void func_8000EB00(s32 a, ...);
u16 func_8000EB18(u32 r);
s32 func_8000EB18_2(u32 r);
u32 func_8000EB18_3(u32 r);
s32 func_8000EB18_4(s32 r);
void func_8000ECB8(s32 arg0) __attribute__((noreturn));
void *func_8000ECB8_2(s32);
void func_8000ECC4(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_8000ECC4_2();
s32 func_8000ECE0(s32 a0);
s32 func_8000ECF8(s32 a0);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8000ED10_2(void *a0, s32 a1, s32 a2);
void func_8000ED10_3();
void func_8000ED10_4(u8 *a0, s32 a1, s32 a2);
void func_8000ED30(char *dst, char *src);
void func_8000ED70(u8 *d, u8 *s, s32 n);
s32 func_8000EDA0(u8 *param_1, u8 *param_2);
void func_8000EE20(void);
void func_8000EE28(void);
void func_8000EE30(s32 v, char *out);
void func_8000EF0C(u32 param_1, char *param_2);
void func_8000EF50(u8 *s);
void func_8000EF8C(char *a0, char *a1);
void func_8000EFB0();
void func_8000EFB0_2(char *buf);
void func_8000EFB0_3(char *fmt, ...);
void func_8000EFCC(void);
u32 func_8000EFE8(void);
s32 func_8000EFE8_2(void);

/* src/game/unnamed/s0_8000F060.c */
void func_8000F060(void);
void func_8000F164(void);
void func_8000F240(Slot_2 *param_1);
void func_8000F600(void);
void func_8000F7A4(void);
void func_8000FB44(s32 arg0);
void func_80010994(s32 k, s32 n, s32 vol);
void func_80010AFC(void);
void func_80010CFC(void);
void func_80010D44(void);
void func_80010D84(s32 a0);
void func_80010DF8(u32 a);
s32 func_80010E18(void);
void func_80010E28(s32 a0, s32 a1, s32 a2, s32 a3);
void func_80010E28_2(s32 a0, s32 a1);
void func_80010E5C(s32 mode, s32 v);
s32 func_80010ED4(s32 param_1);
void func_80010F40(void);
void func_80010FC8(void);
void func_8001102C(s32 param_1);
void func_8001106C(void);
void func_80011098(s32 *p);
void func_800110DC(void);
void func_800111A0(void);
void func_800112A4(void);
void func_80011300(s32 a0);
void func_80011378(s32 i, s32 j, s32 vol, s32 pan, s32 arg4);

/* src/game/unnamed/s0_80011490.c */
void func_80011490(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
void func_800118DC(MtxS_2 *m, f32 c, f32 sn, s32 s);
void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
void func_80011C1C(s32 *a, s32 *b, s32 *c);
void func_80011EF0(f32 *r, f32 *a, f32 *b);
void func_800120A4(s16 *a, s16 *b, s16 *o);
void func_80012250(f32 *p, s32 *o1, s32 *o2, s32 *o3);
void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio);
void func_80012958(f32 *m, s16 a, s16 b, s16 c);
void func_80012B14(void);
void func_80012C00(void);
void func_80012D30(u8 *p);
void func_80012D94(s32 *p);
void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3);
void func_80012EBC(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4);
s32 func_80012FE4(s32 a);
s32 func_80013010(s32 x);
f32 func_80013098(f32 p1, f32 p2);
f32 func_8001318C(f32 p);
void func_80013248(f32 m[3][3]);
void func_8001327C(f32 *v, f32 *m, f32 *o);
void func_80013310(f32 *a, f32 *b, f32 *o);
void func_800133A4(Vec3f *a, Vec3f *b, Vec3f *r);
f32 func_80013408(f32 *a, f32 *b);
void func_80013438(f32 *out, f32 x, f32 y, f32 z);
void func_80013568(f32 *m, f32 *q);
void func_80013650(s32 *m, f32 *q);
f32 func_80013774(f32 x);
u8 func_800138A4(void);
u32 func_800138FC(f32 *p);
void func_800139A8(s32 *src, u16 *aux, s16 *dst);
void func_80013ACC(Obj_5 *src, MtxS_2 *dst, s32 n);
void func_80013C20(f32 *o, f32 *p, f32 *q);
f32 func_80013D3C(f32 x);

/* src/game/unnamed/s0_80013D50.c */
s32 func_80013D50(void);

/* src/game/prim.c */
Gfx *func_80013D70(s32 flags, Model *mdl, Seg_3 **segs, Gfx *gdl);
s32 func_80013D70_2(s32, void *, s32, s32);
void func_800147C4(u32 *param_1, s32 param_2);
void func_80014948(ImgSet *s);
void func_80014A68(s32 a0);
void func_80014A74(s32 a0);
void func_80014A80(void);
void func_80014AD0(void);
void func_80014AD8(s32 x);
void func_80014AE4(void **param_1, u32 *param_2, s32 param_3, u32 param_4);
void func_80014B84(u32 *p1, u32 a1, u32 a2, u32 a3);
void func_80014C14(u8 *p, s32 stride, s32 rows);
void func_80014C80(Hdr **pp);
void func_80014CCC(Hdr **pp);
void func_80014D20(u32 *p);
s32 func_80014D48(u16 *p, s32 r, s32 g, s32 b, s32 flag, s32 n);

/* src/game/min.c */
void func_80014E10(void);
void func_80015124(s32 param_1);
void func_80015254(void);
void func_800154FC(s32 arg);
void func_80015CD8(void);
void func_80015E60(void);
void func_8001618C(void);
void func_800166CC(void);
void func_8001683C(void);
void func_800169BC(void);
void func_80017214(void);
void func_80017338(u32 *arg);
void func_800195B8(Res_3 *r, s32 idx);
void func_800197B4(s32 x);
void func_80019CCC(s32 p);
void func_80019DCC(void);
void func_80019E78(void);
void func_80019EC0(s32 param_1);
void func_80019F7C(void);
void func_8001A044(void);
void func_8001A0D4(void);
void func_8001A14C(void);

/* src/game/bikenew.c */
void func_8001A190(void);
void func_8001AEB0(u8 *a0, s32 a1);
f32 func_8001B05C(Obj_7 *o, f32 *p, f32 p3);
void func_8001B354(Bike_4 *b, Wheel *w, f32 dt, u8 flag);
void func_8001B808(Bike_5 *b);
void func_8001BA5C(Rec_9 *p, f32 x);
void func_8001BC98(u8 *s);
void func_8001BEDC(Bike_6 *a, Cam *b);
f32 func_8001C550(V3 *a, V3 *b, f32 t, f32 d, f32 *e);
void func_8001C780(Bike_7 *b);
void func_8001CB80(Obj_8 *a0, f32 *a1);
void func_8001CDE0(u8 *p);
void func_8001D008(Bike_8 *p);
void func_8001D130(Obj_9 *o, s32 a1);
void func_8001D640(Bike_42 *b);
void func_8001DD10(Bike_9 *b);
f32 func_8001E89C(u8 *p);
void func_8001EC90(Bike_10 *b);
void func_8001FB40(u8 *param_1);
void func_8001FC60(Bike_11 *b);
void func_8001FE1C(Bike_42 *b);
void func_80020504(void *a0);
f32 func_80020744(Body *p, f32 g, f32 t, f32 *out);
void func_80020B18(Bike_12 *b);
void func_80020D38(Ent_12 *e);
void func_80021068(Bike_13 *b);
void func_80021CF8(u8 *a2);
void func_80021EC8(Bike_14 *b, s32 n);
u32 func_80022324(u32 a0, s32 a1);
void func_80022448(s32 n);
float func_800228A0(float *p);
void func_80022908(Obj_10 *p);
void func_800229B8(void);
void func_80022A24(u8 *p);
void func_80022A68(u8 *p);
void func_80022B50(Obj_11 *o);

/* src/game/tubegfx.c */
void func_80022BD0(void);
Gfx *func_80022FD8(Gfx *gdl, s32 a1, s32 a2, s32 view);
Gfx *func_800238F0(Gfx *gdl, s32 p2, s32 p3, u32 sh);
Gfx *func_80023C0C(Gfx *gdl, s32 a1, s32 a2, s32 mode);
void func_8002400C(u32 *list, s32 p, s32 strict, s16 *slots, s32 nslots);
void func_80024490(Model_2 *m);
void func_80024708(void);
void func_8002482C(s32 param_1);
void func_800248EC(u32 *param);
void func_80024980(void);
void func_80024DA4(void);
void func_80024F24(s32 n);
Gfx *func_80025060(Gfx *gdl, s32 mtx, s32 a2, s32 view, s32 fog);
Gfx *func_800258F4(Gfx *gdl, s32 mtx, s32 a2, s32 view);
Gfx *func_80026120(Gfx *gdl, s32 mtx, s32 a2, s32 view);
void func_80026924(List_4 *l, s32 n);
void func_800270F8(Lst *a, Lst *b);
s32 func_8002733C(s32 x, s32 y, s32 *out);
s32 **func_80027574(Ent_19 *h, s32 p2, u32 p3, s32 p4);
void func_80027934(u32 *param_1);
void func_80027AB4(void);
void func_80027B7C(void);
void func_80027DE0(s32 a, s32 *b, s32 *c);
void func_8002803C(Gfx *gdl, s32 a, s32 b, s32 c);
Gfx *func_8002803C_2(Gfx *, Dyn *, s32, s32);
short *func_8002811C(s32 p1, s32 p2, s32 p3, s32 p4);
Gfx *func_800281F4(Gfx *gdl, s32 idx);
void func_80028278(void);
void func_800282C8(void);
void func_80028330(Ent_22 *p);
void func_800283C0(void);
void func_8002845C(s32 param_1, s32 param_2, s32 param_3, s32 param_4);
void func_80028590(void);
Gfx *func_800285C4(Gfx *gdl, s32 flag);

/* src/game/height.c */
void func_80028600(s32 arg0);
void func_80028790(void);
void func_80028864(void);
f32 func_800292E4(Obj_13 *self);
f32 func_800295E0(u8 *p);
void func_80029830(Obj_14 *p, s32 *out, s32 arg2);
int func_80029B1C(u8 *s);
s32 func_80029CE0(Bike_16 *bike, Rec6 *out);
f32 func_8002A4EC(u8 *param_1);
void func_8002A680(Obj_15 *p, s32 *out);
void func_8002A8C0(Bike_17 *p, s32 *out, s32 z);
s32 func_8002A8C0_2(Bike_16 *p, Rec6 *out, s32 z);
void func_8002AE5C(Ent_23 *e, Rec_4 *r);
void func_8002AF64(Ent_24 *p);
void func_8002B210(Bike_18 *p, Rec6 *out);
void func_8002B348(Ent_2 *p);
void func_8002B5F8(Ent_2 *src);
s32 func_8002B5F8_2(Rec6 *src);
void func_8002B6F4(S *p, f32 *v);
s32 func_8002B870(void);
f32 func_8002BBD4(Obj_17 *p, s32 a, s32 b);
void func_8002BD40(Obj_18 *param_1, s32 *param_2);
void func_8002BFE0(Actor *act, Rec6 *out);
void func_8002C5A4(s16 p1, f32 p2, u16 *p3);
void func_8002C6B4(s16 param_1, f32 param_2, f32 *param_3, f32 *param_4, f32 *param_5);
void func_8002C7C0(s32 a, s32 b, s32 c);
void func_8002CA70(s32 p1, s32 p2, s32 p3, s32 p4);
void func_8002CC30(s32 m, s32 n);
void func_8002CC30_2(s32 a);
void func_8002D1F0(s32 param_1, s32 param_2);
void func_8002D384(s32 p, s32 q);
void func_8002D464(int param_1, int *param_2);
void func_8002D480(s32 a, s32 *b);
void func_8002D49C(Obj_20 *param_1, s32 *param_2);
void func_8002D528(Obj_21 *o, Out_2 *out);
void func_8002D5A0(u8 *p, s32 *p2);
void func_8002D604(s32 param_1, s32 *param_2);
void func_8002D620(s32 a, s32 *b);
void func_8002D63C(Obj_22 *o, s32 *out);
void func_8002D788(s32 *p1, s32 *p2);
void func_8002D888(Player_10 *arg0, s32 *arg1);
void func_8002D890(void);
void func_8002D898(void);
void func_8002D8A0(void);
void func_8002D8C4(s16 p1, f32 f, u16 *p3);
void func_8002D964(s32 a0, f32 f, s16 *out);
void func_8002DA48(s32 i, s32 v);
void func_8002DA60(Area_2 *area, s32 b, s32 c, u32 *polys);
void func_8002E190(void);
s32 func_8002E29C(s32 id, s32 x, s32 y, s32 z);
s32 func_8002E920(s32 x, s32 y, s32 z);
s32 func_8002EB5C(s32 x, s32 z);
float func_8002ED38(float x, float z);
void func_8002EF1C(s32 x, s32 z, s32 *out);
s32 func_8002F1D0(s32 p1, s32 p2, Vec3i *v);
void func_8002F4B0(f32 *param_1);
void func_8002F62C(s32 x0, s32 z0, s32 x1, s32 z1, s32 hy);
void func_8002FF44(f32 *pos);
s32 func_800300B0(f32 *p1, f32 *p2, f32 *p3, f32 *p4);
void func_80030478(f32 *p);
u8 func_80030658(f32 *start, f32 *end, f32 *out, f32 *tmax, s32 wantFlag);
void func_80030ECC(s32 x0, s32 z0, s32 x1, s32 z1);
void func_80031A78(Hdr_5 **param_1, s32 param_2);
void func_80031D08(int *param_1);
void func_80031D24(void);
s32 func_80031D54(s32 *p1, s32 *p2, s32 x, s32 y);
int func_80031DF8(int x, int y);
s32 func_80031EC4(Poly_2 *poly, s32 px, s32 pz);
s32 func_80031FA0(s32 p1, s32 p2, s32 p3);
s32 func_8003207C(s32 p1, s32 p2, s32 p3);
void func_80032150(void);
void func_8003240C(s32 param_1, s32 param_2, s32 arg2);
void func_800325FC(s32 a0);
Gfx *func_80032854(Gfx *gdl, u8 *mtx, s32 idx);
Gfx *func_8003371C(Gfx *gdl, s32 arg);
s32 func_80033AC8(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9);
void func_80033D40(void);
s32 func_80033DA0(s32 x);
s32 func_80033DE0(s32 a, s32 idx);

/* src/game/unnamed/s0_80033E30.c */
void func_80033E30(s32 arg0);
void func_80033FF0(void);
Gfx *func_80034100(s32 a, s32 b, s32 *tex, Gfx *gdl);
Gfx *func_80035178(Gfx *gdl, s32 unused, u8 *s);
s32 func_800353EC(s32 *outx, s32 *outy, s32 p3, s32 p4);
void func_80035630(void);
void func_80035638(void);
Gfx *func_80035660(Gfx *gdl, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, s32 du, s32 dv, s32 x0, s32 y0, s32 x1, s32 y1, u32 *flag);

/* src/game/unnamed/s0_800358C0.c */
void func_800358C0(void);
void func_80035A84(Obj_27 *o);
void func_80035BAC(s32 idx, s32 dt);
f32 func_80036108(f32 x);
void func_800361EC(Vec3f *p);

/* src/game/mud2.c */
void func_80036230(void);
void func_80036374(s32 type, u8 b, V3 *pos, V3 *vel, u8 *col);
void func_80036374_2(s32 type, s32 b, V3 *pos, V3 *vel, u8 *col);
void func_80036374_3(s32 a, s32 b, f32 *c, f32 *d, u8 *e);
void func_80036374_4(s32 a, s32 b, f32 *c, s32 *d, s32 *e);
void func_80036544(Obj_29 *param_1);
void func_800366AC(void *param_1);
void func_800368A4(void);
void func_80036DF4(u8 *p);
void func_80037290(void);
Gfx *func_80037414(Gfx *gdl, s32 view, s32 arg2);
Gfx *func_80039E04(Gfx *gdl, s32 mtx, s32 buf);
void func_8003A694(s32 p);
void func_8003A720(void);
void func_8003A744(Vec3f *src, s32 val);
void func_8003A780(void);
void func_8003A84C(void);
void func_8003A8A8(u8 *p);
void func_8003A928(f32 *v, s32 *a1, s32 *a2, s32 *a3, s32 *a4, s32 a5, s32 idx);
void func_8003AA8C(s32 param_1);

/* src/game/skel.c */
Blk_2 *func_8003AAE0(Blk_2 *gdl, s32 unused, s32 n);
Gfx *func_8003AC18(s32 idx, Gfx *gdl, Mtx *m);
Gfx *func_8003B0AC(s32 id, s32 unused, Gfx *gdl, s32 arg3);
Gfx *func_8003B558(s32 param_1, s32 param_2, Gfx *gdl);
void func_8003B7E0(s32 n);
void func_8003BB1C(Facet_2 *a, s32 lim);
void func_8003C21C(void);
void func_8003C2AC(void);
void *func_8003C2E0(u32 *param_1, s32 param_2);
void func_8003C4A4(Node_6 *n, s32 count);
s32 func_8003C588(Rec_20 *recs, s32 n, s32 *out, s32 *mat, s32 *pos, s32 idx);
void func_8003C720(Blk_10 *tbl, List_6 *list);
void func_8003C7A8(s32 *param_1, s32 param_2);
void func_8003C834(Ent_35 *base, s32 val, s32 n);

/* src/game/unnamed/s0_8003C8E0.c */
void func_8003C8E0(Joint *j, s32 idx, Vec3i *tgt, Obj_30 *o);
void func_8003CD80(Joint_2 *j, s32 idx, Vec3i *tgt, s32 *arg3);
void func_8003D180(s32 *param_1);
void func_8003D4BC(Model_4 *m, s32 i);
void func_8003E1FC(u8 *param_1);
void func_8003E330(Bone_2 *bones, s32 param_2, s32 param_3);
void func_8003E944(u8 *param_1);
void func_8003EAC4(s32 *p1, s32 p2);
void func_8003ED1C(void);
void func_8003F004(Obj_31 *p, s32 n);
void func_8003F128(void);
void func_8003F204(void);
void func_8003F274(Row_4 *p1, s32 param_2, s32 p3, s32 p4, s32 arg4);
s32 func_8003F534(s32 a0, s32 *a1);
void func_8003F564(Ent_39 *p, s32 n);
s32 func_8003F600(s32 a, s32 b, s32 c);
void func_8003F658(void);

/* src/game/desgfx.c */
void func_8003F6A0(void);
void func_8003F8FC(s32 arg0);
Gfx *func_8003FD4C(Gfx *gdl, Mtx *m, s32 arg2, s32 arg3);
Gfx *func_80040190(Gfx *gdl, s32 a1, s32 a2, s32 a3);
Gfx *func_800408A4(Gfx *gdl, s32 p2, s32 idx, s32 arg3);
Blk_2 *func_80040B64(Blk_2 *gdl, s32 p2, s32 arg2, s32 arg3);
void func_80040DBC(void);
void func_800417B8(void);
void func_80041E90(s32 n, s32 x, s32 y, s32 mode);
s32 func_800422AC(s32 mask);
void func_80042584(void);
void func_80042620(s32 a0);
void func_8004262C(s32 a, s32 b, s32 *q, s32 *r);
void func_80042694(s32 param_1, s32 param_2);
void func_800426EC(void);
Gfx *func_80042718(Gfx *gdl, s32 x);
void func_8004281C(s32 x);

/* src/game/fractal.c */
void func_80042830(s32 px, s32 pz);
void func_80042A5C(Field *f, s32 n);
s32 func_8004367C(s32 a0, s32 a1);
void func_80043768(u8 *buf);
void func_80043864(Gen *p, s32 x, s32 y);
void func_80043B60(volatile Field_2 *f);
s32 func_80043F24(s32 x, s32 z, s32 ang, s32 range, Elem_2 *out, s32 max);
void func_80044174(void);
void func_80044314(s32 param_1);
void func_80044358(void);
void func_800443F0(s32 n);
u8 func_800444C4(void);
s32 func_80044530(s32 param_1, s32 param_2);

/* src/game/unnamed/s0_800445B0.c */
void func_800445B0(Bike_23 *b, s32 n);
void func_80044B60(Bike_24 *b);
s32 func_800453D8(Body_3 *b, Pt_4 *t, s32 n, u8 m);
void func_800467C4(Body_4 *b, Vec3f *n, Vec3f *p, f32 maxr, Vec3f *c, f32 fr);
f32 func_80046B28(f32 *a, f32 *b, f32 *h, Vec3f *v, s32 n, f32 thr);
void func_80046CC0(Bike_26 *b);
void func_800477B4(f32 x);
void func_800477F0(u8 *p, u8 v);
void func_800477F0_2(Bike_5 *b, s32 arg1);
void func_800477F0_3(Obj_57 *p, s32 v);
f32 func_80047814(f32 *p);
void func_80047910(Obj_35 *o, s32 b);

/* src/game/unnamed/s0_80047940.c */
void func_80047940(void);
void func_80047A1C(void);
void func_80047A90(void);
void func_80047F7C(u8 *s);

/* src/game/unnamed/s0_80048230.c */
void func_80048230(void *arc);
void func_800482D0(u32 *param_1);
void func_8004834C(void);
Gfx *func_80048370(Gfx *g);

/* src/game/path.c */
f32 func_80048410(s32 a, s32 b);
void func_800486B0(void);
void func_80048814(s32 idx, s32 stop);
void func_8004896C(s32 i, s32 end);
void func_80048ADC(s32 start, s32 stop);
u8 func_80048D30(f32 x, s32 idx);
void func_80048E08(void);
void func_80048F58(void);
void func_800492D0(u32 *p);
void func_800498A0(s32 idx);
void func_80049AA4(s32 end, s32 i);
void func_80049D24(void);
void func_8004A334(void);
void func_8004A5FC(u32 *param_1);
s32 func_8004A788(Vec3f *pos, s32 start, s32 count, u8 mode);
f32 func_8004A944(Vec3f *pos, s32 idx);
void func_8004AB24(Vec3f *pos, s32 *area, f32 *t, s32 p4);
void func_8004AC98(Bike_28 *b);
void func_8004B494(Ent_47 *p, s32 a1);
s32 func_8004B5F0(Vec3f *pos, s32 start, s32 count, u8 mode);
void func_8004B684(s32 param_1, s32 param_2);
void func_8004B6CC(void);

/* src/game/tunes.c */
void func_8004B710(s32 param_1);
s32 func_8004B890(void);
void func_8004C014(void);
void func_8004C22C(s32 param_1);
void func_8004C3C4(void);
void func_8004C4E4(void);
void func_8004C5A0(s32 param_1, s32 param_2, s32 arg2);
void func_8004C644(void);
void func_8004C854(u32 a, u32 b);
void func_8004CD78(s32 param_1);
void func_8004CE80(s32 param_1, s32 param_2);
void func_8004D1F4(u32 a, u32 b);
void func_8004D7DC(u32 param_1);
void func_8004DBF8(u32 param_1);
void func_8004DBF8_2(s32 a0);
void func_8004DDD4(u32 param_1);
void func_8004DFB4(u32 a, u32 b);
void func_8004E2E0(u32 a, s32 b);
void func_8004E3EC(u32 a, u32 b, s32 c);
void func_8004E6D8(u32 param_1, s32 param_2, u32 param_3);
void func_8004E960(u32 p);
void func_8004EADC(u32 param_1);
void func_8004ECB4(u32 param_1);
void func_8004EE2C(u32 param_1);
void func_8004EFA4(void);
void func_8004F064(u32 param_1);
s32 func_8004F180(void);
void func_8004F190(void);
int func_8004F270(s32 param_1, s32 param_2, s32 param_3);
s32 func_8004F328(void);
u32 func_8004F3D0(u32 key);
void func_8004F410(s32 p);
void func_8004F47C(void);
void func_8004F4DC(s32 a0);
void func_8004F534(void);
void func_8004F5C8(s32 on);
void func_8004F670(s32 *p, s32 n);
void func_8004F70C(s32 *p);
void func_8004F788(u8 *param_1);
void func_8004F7E8(void);
void func_8004F818(s16 *p, s32 n);
void func_8004F8A4(void);
s32 func_8004F8FC(u32 *p);

/* src/game/event.c */
void func_8004F9B0(s32 a0);
void func_8004FBE8(void);
void func_8004FCBC(void);
void func_80050838(s32 a0, s32 a1);
s32 func_80050CF4(s32 *p);
u32 func_80050F7C(s32 p0);
void func_800511B8(s32 p);
void func_800515A8(void);
void func_800516DC(void);
void func_8005183C(void);
void func_80051B6C(void);
void func_800521F4(void);
void func_80052A6C(s32 arg0);
void func_80052C90(void);
void func_80052F30(s32 param_1, char *param_2);
void func_8005313C(s32 param_1, char *param_2);
void func_800532D4(u8 *p, s32 n, s32 arg2);
Gfx *func_800533D0(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_800535B8(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_80053D58(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_800549E4(Gfx *gdl, s32 a1, s32 a2, s32 idx);
Gfx *func_80054ED0(Gfx *gdl, s32 a);
void *func_80055658(void *param_1, s32 param_2, void *param_3);
Gfx *func_8005581C(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *func_80056C94(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_80058294(Gfx *gdl, s32 arg1, u8 *str, s32 x, s32 y);
Gfx *func_80058438(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3);
s32 func_800595A4(void);
Gfx *func_80059614(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *func_8005A4C8(Gfx *gdl, s32 a1, s32 a2, s32 pl);
s32 func_8005AE38(void);
void func_8005AE4C(void);
u32 func_8005AED8(s32 key);
s32 func_8005AF4C(s32 *p);
u32 func_8005AF90(void);
u8 func_8005AF90_2(void);
s32 func_8005AFFC(void);
void func_8005B030(void);
void func_8005B05C(void);
void func_8005B080(void);
void func_8005B114(void);
void func_8005B1AC(void);
void func_8005B1E0(void);
void func_8005B218(void);
void func_8005B28C(void);
void func_8005B2E8(void);
void func_8005B33C(void);
Gfx *func_8005B404(s32 p1, s32 p2, s32 p3, Gfx *dl, s32 p5);
s32 func_8005B55C(void);
s32 func_8005B58C(s32 key);

/* src/game/unnamed/s0_8005B5E0.c */
void func_8005B5E0(void);

/* src/game/unnamed/s0_8005B5F0.c */
void func_8005B5F0(u8 *p);
void func_8005B7F8(Bike_33 *b, f32 maxv);
void func_8005B928(Bike_34 *b);
void func_8005C5BC(u8 *p, s32 m);
void func_8005C8F4(void);
void func_8005C8FC(void);

/* src/game/decal.c */
void func_8005C910(void);
void func_8005CB20(void);
void func_8005CC84(Obj_40 *o);
void func_8005CD30(char *p, f32 x, s32 n);
f32 func_8005CEFC(f32 x);
f32 func_8005CF20(f32 x);
void func_8005D004(char *param_1, f32 param_2);
void func_8005D050(char *param_1, f32 param_2, f32 param_3);
void func_8005D118(s32 arg0, f32 *v);
void func_8005D1D0(char *param_1, s32 param_2);
void func_8005D220(char *param_1, u32 param_2);
void func_8005D270(void);
void func_8005D444(void);
Rec_33 *func_8005DCFC(s32 p1, s32 *p2, s32 *p3, s32 *p4, s32 *p5, s32 p6);
Gfx *func_8005E090(Gfx *gdl, s32 arg1, Mtx *arg2, s32 arg3);
void func_8005E960(void);
void func_8005E9A4(s32 a0);
void func_8005E9BC(void);

/* src/game/unnamed/s0_8005EA20.c */
void func_8005EA20(u8 *param_1, s32 param_2);
void func_8005EB68(s32 v, s32 n);
void func_8005EC14(u32 *p);
s32 func_8005F188(void);
void func_8005F2D8(void);
void func_8005F924(void);
void func_8005FA18(void);
void func_8005FC68(void);
void func_8005FE10(s32 param_1);
s32 func_8005FE10_2(s32 x);
void func_8005FEE0(s32 param_1);
s32 func_8005FFB0(s32 *dst);
s32 func_80060078(u8 *s, u32 *a1, u32 *a2, u32 *a3);
s32 func_800600D4(u8 *p, s32 n);
u32 func_80060170(u8 *p, s32 n);
s32 func_800601C4(void);
s32 func_80060204(s32 n);

/* src/game/collide.c */
Node_16 *func_80060270(Param *p);
s32 func_80060538(Bv *a, Bv *b, Xf *x1, Xf *x2, Vec3f *o1, Vec3f *o2);
s32 func_800607BC(Box_3 *p1, Box_3 *p2, Xf_2 *m1, Xf_2 *m2, Vec3f *v1, Vec3f *v2);
s32 func_80060AA0(void *p1, void *p2);
s32 func_800610B0(Obj_42 *a, Obj_42 *b);
s32 func_80061B90(u8 *p1, u8 *p2);
void func_80061D54(Node_17 *p);
void func_80061DA8(void);
void func_80061DCC(void);
void func_80061E20(u8 *p);
void func_80061E8C(u8 *p);
void func_80061F1C(Node_18 *p);

/* src/game/unnamed/s0_80061F80.c */
s32 func_80061F80(s8 *p, s16 v);
s32 func_80061F98(u8 *a0, s16 a1);
u8 func_80061FB0(void);
s32 func_80061FC8(u8 *a0);
u8 func_80061FD4(s8 *p, s16 v);
s32 func_80062008(void);
s32 func_80062018(Obj_43 *p, s16 s);
s32 func_80062090(void);

/* src/game/hacks.c */
void func_800620A0(void);
void func_80062244(void);
void func_80062588(void);
Gfx *func_800626CC(Gfx *gdl, s32 unused, s32 p3, s32 p4);
void func_80062998(void);
void func_80062D0C(void);
void func_80062D38(void);
s32 func_80062DA0(s32 a0, Dyn *arg1, s32 arg2, s32 arg3);
s32 func_80062DD4(s32 x, Dyn *arg1, s32 arg2, s32 arg3);
void func_80062E08(void);
void func_80062E9C(void);
s32 func_80062EC0(s32 x);
void *func_80062EC8(void *p);
void func_80062ED0(void);
s32 func_80062F40(s32 x);

/* src/game/unnamed/s0_80062F50.c */
void func_80062F50(Spawn *sp);
s32 func_8006333C(Player_21 *p, Player_21 *q);
void func_80063488(Player_22 *p);
void func_800635A4(void);
void func_80063768(void);
void func_800638E8(Ent_65 *p, Ent_65 *p2, s32 p3);
void func_80063A24(u8 *a, u8 *b, s32 n);
void func_80063B60(u8 *p1, s16 *p2, f32 p3, s32 *p4);
void func_80063D30(void);
void func_800644E0(void);
void func_80064A8C(s32 arg0, s32 count);
void func_80064B9C(s32 p1, s32 n);
void func_80064CA8(s32 arg0);
void func_80064D10(void);
void func_80064D28(s32 a, s32 b);
void func_80064DB8(void);
void func_80064DF4(s32 arg0, s32 count);
void func_80064EB8(s32 arg0);
void func_80064F30(u8 *p);
s32 func_80064FE0(Obj_47 **p, f32 x);
void func_80065048(void);
void func_80065080(void);
void func_800650E4(void);

/* src/game/unnamed/s0_800651A0.c */
void func_800651A0(u8 *param_1, int arg1);
void func_800652DC(void);
void func_800656B4(s32 p);
void func_800657FC(s32 a0, s32 a1);
void sched_submit_task(s32 a0);
s32 func_80065858(void);
void func_80065868(void *arg);
void func_800658CC(u8 *param_1);
void func_80065904(s32 p);
void func_800659B4(s32 x, s32 y, u32 bits);
void func_80065A5C(void);
void *func_80065A64(s32 a0);

/* src/game/unnamed/s0_80065AF0.c */
s32 func_80065AF0(void);

/* src/game/instance.c */
s32 func_80065B10(u32 *a, s32 b, s32 c, Box_4 *d, s32 e);
Gfx *func_80066070(Gfx *gdl, Mtx *m, s32 unused, s32 view, s32 vis);
void func_80066268(void);

/* src/game/shatter.c */
s32 func_80066320(Tri3_2 *t, s32 *outI, s32 *outN, s32 *ox, s32 *oy, s32 *oz);
void func_80066494(Mdl_2 *p);
void func_800671A8(void);
void func_800674A4(void);
void func_80067818(s32 param_1);
void func_800679C0(void);
void func_80067AC4(void);
void func_80067B54(void);

/* src/game/palmod.c */
void func_80067B80(void);
Gfx *func_80067E4C(Gfx *gdl);
Gfx *func_80068484(Gfx *gdl, s32 player, s32 big);
Gfx *func_800685D8(Gfx *gdl, s32 idx, s32 half);
Gfx *func_80068910(Gfx *gdl, Dyn *arg1, s32 arg2);
void func_80068F38(s32 player, s32 *px, s32 *py);
void func_80069158(void);
void func_80069A84(f32 *a, f32 *b, f32 *c);
Gfx *func_80069AC8(Gfx *gdl, s32 idx, s32 flag, s32 x, s32 y);
void func_80069BC8(u16 *dst, u16 *src, s32 x, s32 y);
void func_80069C6C(void);
void func_80069CA0(Obj_51 *a, s32 b);
void func_8006A0AC(void);
void func_8006A8B0(s32 p);
void func_8006A944(void);
void func_8006A9DC(s32 a0, s32 *a1, s32 *a2, s32 *a3);
void func_8006AA9C(u16 *dst, u8 *src);
void func_8006AB64(u16 *dst, u16 *src);
void func_8006AC40(s32 arg0);
void func_8006AC40_2(void);
void func_8006ADF0(void);
void func_8006B160(void);
void func_8006B190(s32 x);

/* src/game/unnamed/s0_8006B1A0.c */
void func_8006B1A0(void);
void func_8006B2AC(u8 *p);
void func_8006B4EC(void);
void func_8006BC14(Obj_52 *p);
s32 func_8006BD4C(Obj_53 *o, s32 n);
u8 func_8006BD4C_2(Obj_52 *p, s32 a1);
void func_8006BF04(Obj_54 *o);
void func_8006BF78(Obj_55 *o);

/* src/game/unnamed/s0_8006BFC0.c */
void func_8006BFC0(void);
void func_8006C28C(s32 a0);
void func_8006C340(void);

/* src/game/unnamed/s0_8006C510.c */
Gfx *func_8006C510(Gfx *param_1, s32 param_2, s32 param_3);
Gfx *func_8006C794(Gfx *gdl, Mtx *proj, s32 i, s32 j);
Gfx *func_8006CD3C(Gfx *gdl, Mtx *proj, s32 n);
Gfx *func_8006D664(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3);
void func_8006EB34(u32 *param_1, Dyn *arg1, s32 arg2);
Gfx *func_8006EB34_2(Gfx *, Dyn *, s32);
void func_8006EE58(void);
void func_8006EEC4(void);
void func_8006EF68(void);
void func_8006EF8C(void);
void func_8006EFB0(void);
s32 func_8006F128(void);
void func_8006F3E4(s32 arg0);
void func_8006F5F8(s32 x, s32 arg1);

/* src/game/unnamed/s0_8006F620.c */
void func_8006F620(void);
void func_8006F8D4(Ent_78 *e);
void func_8006FB3C(Obj_56 *o);
void func_8006FE84(Ent_79 *o, s32 n);
s32 func_800701B4(Obj_57 *p, s32 k);
void func_8007047C(Obj_58 *o);
s32 func_80070514(u8 *a, u8 *b);
void func_80070560(s32 arg0);
s32 func_80070568(Obj_59 *p);

/* src/game/ponytail.c */
void func_80070600(s32 idx, Obj_60 *o);
void func_800712E4(s32 n);
void func_800715DC(s32 a0);
void func_80071678(s32 a0, s32 a1, s32 a2, void *a3);

/* src/game/unnamed/s0_800716C0.c */
void func_800716C0(void);
void func_800718F0(void);
void func_80071978(void);

/* src/game/snow.c */
void func_80071990(void);
Gfx *func_80071A7C(Gfx *gdl, s32 mtx, s32 arg2);
void func_80072114(void);
void func_800729D0(s32 *param_1);
void func_80072A9C(void);
void func_80072AD0(Mdl2 *m, Obj2 *o);
void func_80072D64(s32 id);
void func_80072E7C(void);
void func_80072EF0(void);
Gfx *func_80072F70(Gfx *gdl, Dyn *arg1, s32 arg2);

/* src/game/unnamed/s1_80073200.c */
void func_80073200(void);
void func_80073260(s32 param_1);

/* src/game/shell.c */
s32 func_800732D0(u8 *src, char *dst, s32 flag);
void func_80073664(s32 alpha, u16 *src);
void func_800737A4(void);
void func_80073F84(s32 param_1);
void func_8007405C(s32 arg0);
void func_8007417C(Item_8 *a, Pal *b, u32 *arg2, Pad16 *arg3);
void func_80074298(s32 param_1, s32 param_2, u32 *param_3, s8 *param_4);
void func_80074444(s32 param_1, s32 param_2, u32 *param_3, s32 param_4);
void func_80074684(Obj_61 *o, s32 unused, s32 *btn, Pad16 *arg3);
void func_80074EF4(Obj_62 *p1, s32 p2, u32 *p3, Pad16 *arg3);
void func_800752A0(Info *arg0, s32 arg1, u32 *arg2, Pad16 *arg3);
void func_80075764(Info *arg0, s32 arg1, s32 *arg2, Pad16 *arg3);
void func_80075F3C(Obj_63 *p1, s32 p2, u32 *p3, Pad16 *arg3);
void func_8007658C(Arg0 *a0, Arg1 *a1, s32 *pad, Pad16 *arg3);
s32 func_80077000(s32 n);
void func_800770F0(Item_9 *item, Ctl_19 *ctl, s32 *btn, Pad16 *arg3);
void func_80077BA0(u32 *p);
void func_80077C3C(Pad16 *pad);
void func_8007896C(void);
void func_80078E54(Scr *s);
void func_80079AA4(void);
void func_80079B64(void);
void func_80079E00(Item_11 *p);
void func_8007AF38(Item_12 *p1, s32 p2, s32 p3, s32 arg3);
void func_8007AF38_2(s32 a, s32 b, s32 c);
void func_8007AF38_3(void *menu, s32 a1, s32 a2);
void func_8007B134(s32 x, s32 y, s32 p3, s32 p4);
Gfx *func_8007B39C(Gfx *gdl, s32 p2, s32 sel, char **strs);
Gfx *func_8007B9B4(Gfx *gdl, s32 p2, s32 p3);
Gfx *func_8007C640(Gfx *gdl, s32 p2, char *str);
void func_8007CB14(s32 p, s32 q);
Gfx *func_8007CEDC(Mtx *m, s32 fb, Gfx *gdl, Ctl_21 *pad, s32 e);
void func_80087310(Mtx *m, s32 fb, Gfx **pgdl);
void func_80087DD8(void);
void func_80087E00(void);
void func_80087E30(s32 p);
void func_80087E84(s32 a);
void func_80087EC8(s32 a0);
void func_80087EF8(s32 arg);
void func_80087F9C(s32 x);
void func_80087FB0(s32 p);
void func_8008802C(s32 p);
void func_800880B8(s32 x);
void func_800880DC(s32 a0);
void func_80088170(void);
void func_80088198(int param_1);
s32 func_800881D4(u32 *p, s32 n);
void func_8008821C(void);
void func_800882A0(void);
s32 func_80088324(Obj_67 *o);
void func_800883BC(u8 *p, s32 x, u32 *f, void *s);
s32 func_80088424(s32 p);
s32 func_800884E0(void);

/* src/game/pak.c */
void func_80088500(s32 p, s32 z);
Gfx *func_80088734(Gfx *gdl, u8 *s, s32 p3, s32 p4, s32 x, s32 y);
void func_80088AE0(void);
void func_80088CF8(s32 arg0, s32 arg1, Gfx **pgdl);
void func_80089E30(s32 p);
void func_80089F2C(Pad16 *arg0);
void func_8008B6DC(void);
s32 func_8008B7B4(u32 p);
void func_8008B9BC(s32 chan);
void func_8008BC30(void);
void func_8008BE9C(void);
void func_8008BF60(void);
int func_8008C068(u32 param_1);
s32 func_8008C0E4(u32 param_1, char *param_2, char *param_3, s32 param_4, s32 param_5, s32 param_6);
s32 func_8008C1FC(s32 channel, s32 file_no);
s32 func_8008C2BC(u32 param_1, s32 param_2, u32 param_3, u32 param_4, u8 *param_5);
s32 func_8008C370(u32 p1, s32 p2, u32 p3, u32 p4, u8 *p5);
void func_8008C424(s32 c, u8 *buf, s32 n);
s32 func_8008C55C(u32 param_1);
void func_8008C5E8(u8 *p, s32 n);
void func_8008C670(u8 *buf, s32 n);
void func_8008C6D0(void);
void func_8008C744(void);
void func_8008C790(s32 idx);
void func_8008C830(s32 *p1, char *p2, s32 p3, s32 p4, s32 p5, s32 p6);
void func_8008C8D8(char *param_1, s32 param_2, s32 param_3);
void func_8008C92C(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6);
void func_8008C9B8(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6);
void func_8008CA40(s32 a0, s32 a1);
void func_8008CA68(u8 *param_1);
s32 func_8008CA68_2(u8 *param_1);

/* src/game/arnedgfx.c */
void func_8008CB10(s32 x, s32 z, s32 w, s32 h);
Gfx *func_8008CC80(Gfx *gdl);
void func_8008CF94(s32 w, s32 h);
void func_8008D3B4(s32 x, s32 y, s32 w0, s32 b0a, s32 w1, s32 b0b, s32 b0c, s32 h0, s32 h1);
void func_8008D4C4(s32 idx, s32 x, s32 y, s32 z, s16 rx, s16 ry);
void func_8008D4C4_2(s32 idx, s32 x, s32 y, s32 z, s32 rx, s32 ry);
Gfx *func_8008D728(s32 idx, Gfx *gdl, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
void func_8008E76C(s32 param_1, s32 param_2, s32 param_3);
void func_8008E8A4(s32 n, s32 id, s32 x, s32 y, s32 w, s32 h, s32 p7, s32 p8, s32 p9, s32 p10, s32 p11, s32 p12);
void func_8008EB74(s32 param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
s16 *func_8008EC3C(s32 param_1, s32 param_2);
void func_8008ED48(void);
void func_8008EDE4(Vtx_t *in, s32 n, s32 rot, Vtx_t *out, s32 dx, s32 dz, s32 flag);
void func_8008EF84(s32 a0, s32 a1, s32 a2, s32 a3);
void func_8008EFFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
void func_8008F07C(s32 a0, s32 a1, s32 a2, s32 a3);

/* src/game/gui.c */
void func_8008F0B0(void);
s32 *func_8008F230(Gfx *gdl, s32 a1, s32 a2, s32 a3);
Gfx *func_8008F458(Gfx *gdl, s32 arg1, s32 arg2, char *s0, char *s1, char *s2, s32 f1, s32 f2);
s32 func_8008F878(Ctl_22 *c, s32 arg1);
Gfx *func_8008FB08(s32 arg0, Gfx *gdl, Menu_5 *m, s32 x, s32 y);
s32 func_80090398(Ctl_2 *c);
void func_80090484(void);

/* src/game/edit.c */
s32 func_80090560(void);
void func_80090670(void);
void func_80090A04(void);
void func_80091270(void);
void func_80091564(Game_36 *arg0, s32 arg1);
void func_80091AFC(void);
void func_80092908(void);
void func_800938BC(void);
void func_80094638(s32 arg0);
void func_80094638_2(void);
void func_80094738(Node_28 *n);
void func_800949BC(void);
Gfx *func_8009559C(Gfx *gdl, s32 arg1);
void func_800959CC(s32 arg0, s32 arg1, Gfx **pgdl);
void func_80096B3C(Rec_49 *r);
void func_80096FB0(s32 idx, s32 x0, s32 y0, s16 z0, s32 x1, s32 y1, s32 z1, u8 r, u8 g, u8 b);
void func_80096FB0_2(s32 idx, s32 x0, s32 y0, s32 z0, s32 x1, s32 y1, s32 z1, s32 r, s32 g, s32 b);
Gfx *func_80097308(u8 *a0, s32 arg1, Gfx *gdl);
s32 func_80097658(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, s32 *sz, s32 *sw, s32 arg7, s32 arg8);
Gfx *func_800978A8(s32 arg0, s32 arg1, Gfx *gdl);
Node_29 *func_8009AB24(Node_29 *a, Node_29 *b, s16 *pt);
void func_8009AF18(u8 *p);
s32 func_8009B11C(Blk_22 *b, s32 s);
s32 func_8009B258(Spr_4 *p, s32 k);
void func_8009B3A4(Node_30 *p, Node_30 *c);
void func_8009B624(Spr_5 *a, Spr_5 *b);
void func_8009B820(void);
s32 func_8009B9A8(Obj_72 *o);
void func_8009C618(void);
void func_8009C7D4(void);
void func_8009CAA4(void);
void func_8009CD24(void);
void func_8009CFC4(void);
int func_8009D224(int param_1, u16 *param_2);
s32 func_8009D3D4(s32 p1, s16 *p2);
int func_8009D5D8(int n, u16 *p);
int func_8009D788(int n, s16 *p);
s32 func_8009D98C(s32 n);
s32 func_8009DC6C(s32 param_1);
void func_8009DF4C(void);
void func_8009E584(void);
s32 func_8009EB00(s32 a, s32 b);
s32 func_8009EB08(Ctx_4 *c);
void func_8009EB4C(void);
void func_8009EBA8(s32 arg0);
void func_8009EBF4(void);
void func_8009ECE0(void);
s32 func_8009EDCC(s32 mode, s32 v, s32 d, s32 f);
void func_8009EF20(s32 a0);
void func_8009EFA4(void);
void func_8009EFE4(s32 a0);
void func_8009F068(void);
s32 func_8009F170(void);
void func_8009F1F8(s32 param_1);
void func_8009F260(void);
void func_8009F310(s32 n);

/* src/game/nesn64.c */
void func_8009F470(void);
void func_8009FA04(volatile s32 *p);
Gfx *func_8009FDF4(s32 a0, s32 a1, Gfx *gdl);
void func_800A020C(s32 a0, s32 a1, Gfx **pgdl);
void func_800A070C(s32 row);
void func_800A0D60(s32 arg0);
void func_800A11BC(s32 param);
void func_800A1484(void);
void func_800A1570(s16 *p);
void func_800A1898(void);
void func_800A19BC(void);
void func_800A1A68(void);
s32 func_800A1AA0(void);
void func_800A1ADC(void);
void func_800A1B0C(u8 *p);
void func_800A1BA0(s32 idx, s32 mode);
void func_800A1DCC(s32 n, s32 m);
void func_800A1F8C(s32 n, s32 m);
void func_800A2178(s32 addr, s32 value);
u32 func_800A2444(s32 code, s32 unused, u32 val);
u32 func_800A2444_2();
u32 func_800A2444_3(u32 a);
s32 func_800A2444_4();
void func_800A2588(void);
void func_800A265C(void);
void func_800A2750(void);
void func_800A283C(void);
void func_800A2928(void);
void func_800A29F0(void);
void func_800A2B14(void);
void func_800A2BB8(void);
void func_800A2C50(void);
void func_800A2D64(void);
s32 func_800A2F60(s32 param_1, s32 param_2);
void func_800A2FB4(void);
void func_800A3014(void);
void func_800A3040(void);
void func_800A3070(void);
void func_800A30E8(void);
void func_800A3118(void);
void func_800A3150(void);
void func_800A3180(void);
void func_800A31B0(void);
void func_800A31E0(void);
void func_800A3210(void);
void func_800A3240(void);
void func_800A3250(void);
void func_800A3260(void);
void func_800A3270(void);
void func_800A3280(void);
void func_800A32D4(void);
void func_800A3328(void);
void func_800A337C(void);
void func_800A3408(void);
void func_800A3430(void);
void func_800A3458(void);
void func_800A3484(void);
void func_800A3510(void);
void func_800A3538(void);
void func_800A3560(void);
void func_800A3578(void);
void func_800A35D4(void);
void func_800A3608(void);
void func_800A363C(void);
void func_800A3670(void);
void func_800A3678(void);
void func_800A36A4(void);
void func_800A36D0(void);
void func_800A36FC(void);
void func_800A375C(void);
void func_800A37A8(void);
void func_800A3804(void);
void func_800A3818(void);
void func_800A3834(void);
void func_800A3848(void);
void func_800A38A0(void);
void func_800A38F8(void);
void func_800A3950(void);
void func_800A3974(void);
void func_800A3998(void);
void func_800A39BC(void);
void func_800A39E0(void);
void func_800A39F0(void);
void func_800A3A14(void);
void func_800A3A1C(void);
void func_800A3A24(void);
void func_800A3A2C(void);
void func_800A3A4C(void);
void func_800A3A54(void);
void func_800A3ADC(void);
void func_800A3AE4(void);
void func_800A3B58(void);
void func_800A3B60(void);
void func_800A3B8C(void);
void func_800A3B94(void);
void func_800A3C28(void);
void func_800A3C30(void);
void func_800A3CC4(void);
void func_800A3CCC(void);
void func_800A3D60(void);
void func_800A3D68(void);
void func_800A3E00(void);
void func_800A3E08(void);
void func_800A3E84(void);
void func_800A3E8C(void);
void func_800A3F08(void);
void func_800A3F10(void);
void func_800A3F3C(void);
void func_800A3F44(void);
void func_800A3F7C(void);
void func_800A3F84(void);
void func_800A3FE0(void);
void func_800A4018(void);
void func_800A4038(void);
void func_800A4078(void);
void func_800A40B8(void);
void func_800A40FC(void);
void func_800A4140(void);
void func_800A4168(void);
void func_800A4190(s32 p, u8 v);
s32 func_800A4204(s32 addr);
void func_800A4254(s32 addr, s32 val);

/* src/game/unnamed/s1_800A42A0.c */
s32 func_800A42A0(void);

/* src/game/UsrTrk.c */
s32 func_800A42D0(u8 *buf);
s16 func_800A42D0_2(u8 *p);
void func_800A44EC(u8 *p);
void func_800A4700(u8 *p);
void func_800A4A20(void);
void func_800A4BEC(void);
void func_800A4FF0(Mesh_8 **dst, Mesh_8 **src, s16 *h, s32 n);
void func_800A5484(Set *out1, Set *out2, s32 count);
void func_800A5840(void);
void func_800A5964(s32 param_1);
void func_800A5A74(void);
s16 func_800A9B80(void);

/* src/game/unnamed/lib_800B7680.c */
s32 func_800B7680(u8 *name);
s32 func_800B781C(s32 arg0);
void func_800B78F0(s32 param_1);
s32 func_800B7990(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 arg5);
void func_800B7BE0(void);
void func_800B8164(s32 param_1, s32 param_2);
void func_800B82FC(s32 *param_1);
void func_800B8444(Chan_6 *p, s32 a1, s32 a2, s32 a3, s32 a4);
void func_800B8760(Ring *r);
void func_800B8B0C(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_800B8B34(s32 p1, s32 p2, s32 p3, s32 p4, s32 *p5, s32 *p6);
void func_800B8C8C(void);
s32 func_800B8CBC(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5);
void func_800B8D98(void);
void func_800B8E08(s32 arg0, s32 *arg1);
void func_800B8E3C(void);
void func_800B8E70(s32 arg0, u8 *arg1, u32 arg2);
void func_800B8E98(s32 id);
void func_800B8F2C(void);
void func_800B8F78(void);
void func_800B905C(void);
void func_800B9138(void);
void func_800B9160(void);
void func_800B9188(s32 key, s32 val);
void func_800B9244(s32 a, s32 b);
void func_800B92F8(s32 key, s32 v);
void func_800B93E8(s32 p1, u32 p2);
void func_800B94A0(s32 param_1, s32 param_2, s32 param_3);
void func_800B9548(s32 id, s32 v);
s32 func_800B9618(s32 param_1);
s32 func_800B96AC(s32 p1);
s32 func_800B9740(s32 param_1);
s32 func_800B97E0(s32 param_1);
s32 func_800B9874(void);
void func_800B98B4(void);
void func_800B99B0(s32 a0);
s32 func_800B99CC(void);
void func_800B99F0(s32 arg0);
void func_800B9A18(s32 arg0);
void func_800B9A40(void);
void func_800B9A68(void);
s32 *func_800B9A90(s32 p1);
void func_800B9B4C(Ring_2 *r, u32 n, u8 *src);
void func_800B9C64(Obj_80 *o, s32 a1);
void func_800B9CA0(Handle *h, s32 v);
void func_800B9D7C(s32 *o, s32 v);
void func_800B9E50(s32 *o, s32 v);
void func_800B9F24(void);
void func_800B9F4C(void);
void func_800B9F74(void);
void func_800B9FE0(u8 *d, u8 *s, s32 n);
s32 func_800BA008(u8 *a, u8 *b);
void func_800BA074(s32 *out);
Ent_99 *func_800BA0E0(void);
void func_800BA160(s32 *p);
void func_800BA16C(Node_45 *n);
void func_800BA254(Node_46 *n);
s32 func_800BA2C0(s32 param_1);
u32 func_800BA330(s32 p);
void func_800BA3F4(s32 p1, s32 p2);
void func_800BA4A8(s32 arg0);
u32 func_800BA4D0(Ring_3 *r, u32 n, u8 *dst);
void func_800BA5CC(s32 i);
void func_800BA5F0(s32 a0);
s32 func_800BA614(s32 param_1);

/* src/game/unnamed/lib_800BA6B0.c */
s32 func_800BA6B0(s32 param_1, u32 param_2, s32 param_3, s32 arg3);
s32 func_800BA934(s32 param_1);
s32 func_800BAB1C(s32 param_1, s32 arg1, s32 arg2, s32 arg3);
s32 func_800BAC58(s32 param);
s32 func_800BAD40(u8 *p1, s32 p2, s32 p3, s32 p4, s32 p5);
void func_800BAE54(s32 param_1, s32 param_2);
void func_800BAE54_2(s32 a);
void func_800BAF14(s32 i, s32 val);
void func_800BAF14_2(s32 a);
void func_800BAFD8(s32 i, s32 val);
void func_800BAFD8_2(s32 a);
void func_800BB09C(s32 *param_1, s32 param_2);
void func_800BB350(Hdr_13 **param_1);
s32 func_800BB47C(s32 *a, s32 *b, s32 *c);
void func_800BB49C(void);
void func_800BB4EC(u32 p);
void func_800BB590(s32 idx, s32 *out);
void func_800BB5CC(s32 i, u32 *src);
void func_800BB5CC_2(s32 a);
void func_800BB608(s32 param_1, s32 param_2);
void func_800BB608_2(s32 a);
void func_800BB6CC(s32 i, s32 v);
void func_800BB6CC_2(s32 a);
s32 func_800BB6EC(s32 a0);
void func_800BB710(s32 n, u32 flags);
void func_800BB710_2(s32 a);
s32 func_800BB780(s32 i);
s32 func_800BB7A4(s32 arg0);
s32 func_800BB7A4_2(void);
void func_800BB7B4(s32 a, s32 b, s32 c);
s32 func_800BB85C(s32 size);
void func_800BB920(s32 p);
void func_800BB978(s32 k);
void func_800BB9C0(s32 arg0);
void func_800BB9FC(u32 idx, u8 *src, s32 n);
void func_800BB9FC_2(s32 a);
void func_800BBAB0(s32 a0);
void func_800BBABC(s32 a, s32 b);
void func_800BBAD0(s32 i);
s32 func_800BBB88(s32 a0);
void func_800BBBAC(s32 x);

/* src/game/n64snd.c */
s32 func_800BBBC0(s32 param_1, s32 param_2, s32 param_3, s32 param_4, u32 *param_5, s32 *param_6);
void func_800BBDE0(void);
s32 func_800BBFC0(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3);
void func_800BC024(void);
void func_800BC064(s32 a, s32 b, s32 c, s32 d);
void func_800BC080(s32 a);
void func_800BC09C(s32 a, s32 b, s32 c, s32 d, s32 e);
void func_800BC0BC(u32 a);
void func_800BC0D8(s32 param_1, s32 *param_2);
void func_800BC0F4(s32 a);
void func_800BC110(s32 a);
void func_800BC12C(s32 a);
void func_800BC148(s32 a);
void func_800BC164(s32 a);
void func_800BC180(s32 a);
void func_800BC19C(s32 a);
void func_800BC1B8(s32 a);
void func_800BC1D4(s32 a);
void func_800BC1F0(s32 a);
void func_800BC20C(s32 a);
u32 func_800BC228(void);
void func_800BC264(s32 v);
void func_800BC270(s32 a);
void func_800BC28C(s32 a);
void func_800BC2A8(s32 a);
void func_800BC2C4(s32 a);
void func_800BC2E0(void);
void func_800BC2E8(void);
void func_800BC2F0(void);
void func_800BC2F8(s32 a0);
void func_800BC370(void);

/* src/game/unnamed/lib_800BC3D0.c */
void func_800BC3D0(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_800BC3D0_2(void);
void func_800BC66C(void);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
s32 *func_800BC908_2(s32 a, s32 b, void *c, s32 d);
void func_800BCB04(void *ptr);
s32 func_800BCB04_2(void *ptr);
void *func_800BCCF0(void *ptr, s32 size, s32 tag, s32 id);
s32 func_800BCEB8(s32 a, u32 b, s32 c);
void func_800BD678(void);
void func_800BD684(void);
s32 func_800BD698(s32 p0, s32 p1, s32 a, s32 b);
s32 *func_800BD730(s32 a0, s32 a1, s32 a2, s32 a3);
s32 *func_800BD798(s32 *p, u32 size, s32 a, s32 b, s32 c);
s32 func_800BD8E0(void);
s32 func_800BD924(s32 *p);
void func_800BD9D4(s32 a, s32 b);
void func_800BD9F4(u32 a, u32 b);
void func_800BDA14(s32 *a, s32 b, s32 c);
void func_800BDA34(s32 a, s32 b);
Block *func_800BDA54(Block **start, Block **end);
s32 func_800BDB44(Block *start, Block *end, Block *list);
void func_800BDC2C(char *a0, u32 a1);

/* src/game/unnamed/lib_800BDCB0.c */
s32 func_800BDCB0(s32 *p1, u32 *p2);
s32 func_800BDE38(void);
void func_800BDF08(void);
u32 func_800BDF18(s32 param_1);

/* src/game/unnamed/lib_800BDF80.c */
s32 func_800BDF80(u8 *src);
Ent_106 *func_800BE2D0(Obj_81 *so, s32 si, s8 sf);
s32 func_800BE5D8(void);
Slot_17 *func_800BE694(u8 *param_1, char *arg1);
void func_800BE770(Q_2 *q);
void func_800BE7F4(s32 *o, s32 v, s32 mode);
s32 func_800BE83C(s32 *p);
s32 func_800BE848(u8 *buf, s32 size, s32 count, Stream_2 *f);
s32 func_800BE998(s32 *a0);
u8 *func_800BE9B0(u8 *param_1);
Node_51 *func_800BEA40(s32 *k, s32 id, s8 flag);
void func_800BEB68(void *param_1, void *param_2, s32 param_3);

/* src/game/unnamed/lib_800BEC20.c */
s32 func_800BEC20(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
void func_800BED20(void);
s32 func_800BEE78(void);
s32 func_800BEF2C(s32 a0);
void func_800BEF8C(s32 a0);
void func_800BEFB4(s32 x);

/* src/game/unnamed/lib_800BF040.c */
s32 func_800BF040(s32 a0, u16 *a1, s32 a2);

/* src/game/unnamed/lib_800BF0E0.c */
s32 func_800BF0E0(void);
u32 *func_800BF100(void);

/* src/game/unnamed/lib_800BF280.c */
s32 func_800BF280(Q_3 *q, s32 v, s32 mode);

/* src/game/unnamed/lib_800BF3C0.c */
Blk_29 *func_800BF3C0(void);
int func_800BF460(u32 param_1);
s32 func_800BF4C4(s32 p);

/* src/game/unnamed/lib_800BF530.c */
s32 func_800BF530(char *path, Obj_82 **out);
u32 func_800BF79C(u32 *pa0, s32 pa1);
void func_800BFC40(u8 *name, s32 val);
void func_800BFDA4(s32 *p);
void func_800BFDD4(s32 *param_1);
void func_800BFEA4(u32 *p);
s32 func_800BFEA4_2(u32 *p);
void func_800BFEE4(ListHdr *h);
void func_800BFF60(u32 *param_1, u8 *param_2, u32 *param_3);
void *func_800BFF60_2(void *a0, void *a1, void *a2);
s32 func_800C0050(u8 *a0, u8 *a1);
s32 func_800C0174(ResFile_2 *r, s32 idx);
void func_800C0258(s32 a0);
void func_800C0264(s32 *param_1, u32 param_2);
u32 func_800C02E4(u8 *p);
Hdr_14 *func_800C0350(Src_4 *s);
u32 *func_800C04EC(u32 *param_1);
Out_6 *func_800C055C(Src_5 *p);
s32 func_800C06D0(ResFile *arc, u32 **ids, char *name);
u8 *func_800C0ACC(s32 *p, s32 x, s32 y);
s32 func_800C1118(Obj_83 *p, s32 x, s32 unused, s32 n);
u8 *func_800C1228(u8 *s, Font_6 *f, u32 *outMax, s32 *outW);
Gfx *func_800C1394(Gfx *gdl);
Gfx *func_800C14F4(Gfx *gdl, Spr **list);
Gfx *func_800C1838(Gfx *gdl, Spr **lists);
s32 func_800C1A6C(u8 *hdr);
s32 func_800C1C20(u32 *map);
void func_800C1D54(s32 a, s32 b, s32 c, s32 d);
u32 func_800C1DEC(void);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
u8 *func_800C1F04(u8 *p);
char *func_800C1F2C(Text *p, char *s);
void func_800C204C(s32 arg0);
void func_800C2054(char *dst, char *src);
void func_800C20F0(u8 *s);
void func_800C2124(u8 *s);
void func_800C21BC(u8 *s);
void func_800C21F0(u8 *s);
s32 func_800C2224(void);

/* libultra functions its headers don't declare, and other views of libultra functions */
s32 __osDisableInt(void);
void __osRestoreInt(s32);
void __osPiGetAccess(void);
void __osPiRelAccess(void);
s32 osAiGetStatus_2(void);
s32 strlen(char *s);
u32 strlen_2(char *s);
void *memcpy(void *, const void *, u32);
void memcpy_2(void *dst, void *src, s32 n);
void __osEnqueueAndYield(s32 *p);
s32 *__osPopThread(Q_3 *q);
s32 __osSpGetStatus(void);
void __osMotorAccess(void *a, s32 c);
void __osInitialize_autodetect(void);

/* other code */
void func_800DB628(s32, s16 *, s32);
void func_800DB8E0(void);
void func_800DBAA0(void);
void func_800DBCA4(void);
void func_800DBD00(void);
void func_800DBE94(u8 *src, u16 *dst, s32 count);
void func_800DBF94(u8 *a, u16 *b, s32 c, u8 *d);
int abs(int);
char *strcpy(char *, const char *);

#endif
