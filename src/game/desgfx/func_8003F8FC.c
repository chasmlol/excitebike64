#include "ctx.h"

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    s32 x, y, z;
    u8 pad[0x1C - 12];
} Elem;

typedef struct {
    u8 pad[0x19C];
    s16 x;
    s16 y;
    u8 slot;
    s8 active;
    u8 pad2[0x1A8 - 0x1A2];
} Obj;

extern s32 D_800E76D0;
extern s32 D_800CA3B8;
extern s32 D_800E8000;
extern Vec3i D_800E8090;
extern s32 D_800ED698;
extern s32 D_800ED69C;
extern s32 D_800ED6A0;
extern s32 D_800EDFD4;
extern s32 D_800EBDF0;
extern s32 D_800EDFD8;
extern s32 D_800EBDF4;
extern s32 D_800EB9EC;
extern Obj *D_800ED7AC;
extern s32 D_800E30A8;
extern s32 D_800ED6A8[];
extern s32 D_800ED728[];
extern Mtx D_800EA9E8[][32];
extern s32 D_800ED608;
extern Mtx *D_800EBDF8[];
extern Elem D_800EA7C8[];
extern s32 D_800CA3B0;
extern Mtx *D_800EDBB4;
extern Mtx D_800ED610[];

void func_800443F0(s32 n);
void func_8004262C(s32 a, s32 b, s32 *q, s32 *r);
void func_80042830(s32 px, s32 pz);
void func_80040DBC(void);
void func_800417B8(void);
void func_80011490(Mtx *m, s32 a, s32 b, s32 c, s32 x, s32 y, s32 z, s32 s);
s32 func_80043F24(s32 x, s32 z, s32 ang, s32 range, Elem *out, s32 max);
void func_800120A4(Mtx *a, Mtx *b, Mtx *o);

void func_8003F8FC(void)
{
    s32 i;
    s32 j;
    Elem *e;
    Mtx m1;
    Mtx m2;

    D_800CA3B8 = D_800E76D0 / 4;
    func_800443F0(100);
    D_800ED698 = D_800E8090.x;
    D_800ED69C = D_800E8090.y;
    D_800ED6A0 = D_800E8090.z;
    func_8004262C(D_800ED698, 0x1F400, &D_800EDFD4, &D_800EBDF0);
    func_8004262C(D_800ED6A0, 0x1F400, &D_800EDFD8, &D_800EBDF4);
    func_80042830(D_800ED698, D_800ED6A0);
    func_80040DBC();
    func_800417B8();
    D_800EB9EC = 0;
    for (i = 0; i < 250; i++) {
        if (D_800ED7AC[i].active != 0) {
            for (j = 0; j < D_800EB9EC; j++) {
                if (D_800ED6A8[j] == D_800ED7AC[i].x && D_800ED728[j] == D_800ED7AC[i].y) {
                    D_800ED7AC[i].slot = j;
                    break;
                }
            }
            if (j == D_800EB9EC) {
                D_800ED6A8[j] = D_800ED7AC[i].x;
                D_800ED728[j] = D_800ED7AC[i].y;
                func_80011490(&D_800EA9E8[D_800E30A8][j], 0, 0, 0,
                              (D_800ED7AC[i].x - D_800EDFD4) * 0x1F400 - D_800EBDF0 + 64000,
                              -D_800ED69C,
                              (D_800ED7AC[i].y - D_800EDFD8) * 0x1F400 - D_800EBDF4 + 64000,
                              0x100);
                D_800ED7AC[i].slot = D_800EB9EC;
                D_800EB9EC++;
            }
        }
    }
    D_800ED608 = func_80043F24(D_800ED698, D_800ED6A0, D_800E8000, 0x1F400, D_800EA7C8, 0x10);
    for (i = 0; i < D_800ED608; i++) {
        func_80011490(&D_800EBDF8[D_800E30A8][i], 0, 0, 0,
                      D_800EA7C8[i].x - D_800ED698, D_800EA7C8[i].y - D_800ED69C, D_800EA7C8[i].z - D_800ED6A0, 0x10);
    }
    if (D_800CA3B0 >= 0) {
        func_80011490(&m1, 0, 0, D_800CA3B0, 0, 0, 0, 0x10);
        func_80011490(&m2, -0x1000, 0, 0, -70, -55, 300, 10);
        func_800120A4(&m1, &m2, &D_800EDBB4[D_800E30A8]);
    }
    func_80011490(&D_800ED610[D_800E30A8], 0, 0, D_800E8000, 0, 0, 1000, 0x100);
}
