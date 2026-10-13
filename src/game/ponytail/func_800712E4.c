#include "ctx.h"

typedef struct Node {
    s32 f0;
    s32 f4;
    f32 f8;
    u8 pad0C[0x38 - 0xC];
    struct Node *prev;
    struct Node *next;
    f32 f40;
    f32 f44;
} Node;

typedef struct Pl {
    u8 pad0[0xA48];
    Node *nodes;
    s32 *a4c;
    s32 *a50;
    s32 *a54;
    s32 *a58;
    u8 pad5C[0xA80 - 0xA5C];
} Pl;

typedef struct { u16 x, y, z; } V3u;
typedef struct { u8 pad0[0x10]; s32 m[0x1C]; } Obj80;

extern Pl D_800DC150[];
extern char D_800D9178[];
extern void *D_800E7728;
extern s32 D_800D0D88;
extern f32 D_800CCFD0[];
extern s32 *D_800C27A8;
extern s32 D_800D0A68;
extern s32 D_800D0D44;
extern s32 D_800D0C18;
extern V3u D_800F2178[];
extern V3u D_800F2160[];

void func_8000ED10(void *a0, u8 a1, s32 a2);
void func_8003C7A8(s32 *p, s32 n);
void func_80012EBC(s32 *m, u16 *x, u16 *y, u16 *z);
void *func_800BC908(s32 size, s32 b, char *file, s32 line);
void *func_8003C2E0(void *a, s32 b);
s32 *func_800BF79C(void *a, s32 b);
void func_8003C720(s32 *a, s32 *b);
void func_800BCB04(void *p);

void func_800712E4(s32 n)
{
    Pl *pl = &D_800DC150[n];
    s32 i;
    Obj80 *r;

    pl->a4c = func_800BC908(0x180, 0, D_800D9178, 0x16A);
    pl->a58 = func_800BC908(0x10, 0, D_800D9178, 0x16B);
    i = 0;
    pl->a50 = func_8003C2E0(D_800E7728, D_800D0D88);
    func_8003C7A8(pl->a50, 3);
    pl->nodes = func_800BC908(0x120, 0, D_800D9178, 0x170);
    func_8000ED10(pl->nodes, 0, 0x120);
    for (; i < 4; i++) {
        pl->nodes[i].f0 = 0;
        pl->nodes[i].f4 = 0;
        pl->nodes[i].f8 = D_800CCFD0[i];
        if (i != 0) {
            pl->nodes[i].prev = &pl->nodes[i - 1];
        }
        if (i < 3) {
            pl->nodes[i].next = &pl->nodes[i + 1];
        }
    }
    for (i = 0; i < 4; i++) {
        if (pl->nodes[i].prev != NULL) {
            pl->nodes[i].f40 = pl->nodes[i].prev->f8 - pl->nodes[i].f8;
        }
        if (pl->nodes[i].next != NULL) {
            pl->nodes[i].f44 = -(pl->nodes[i].next->f8 - pl->nodes[i].f8);
        }
    }
    pl->a54 = D_800C27A8;
    if (D_800C27A8 == NULL) {
        D_800C27A8 = func_800BF79C(D_800E7728, D_800D0A68);
        func_8003C720(pl->a50, D_800C27A8);
        r = func_8003C2E0(D_800E7728, D_800D0D44);
        for (i = 0; i < 3; i++) {
            func_80012EBC(r[i].m, &D_800F2178[i].x, &D_800F2178[i].y, &D_800F2178[i].z);
        }
        func_800BCB04(r);
        r = func_8003C2E0(D_800E7728, D_800D0C18);
        for (i = 0; i < 3; i++) {
            func_80012EBC(r[i].m, &D_800F2160[i].x, &D_800F2160[i].y, &D_800F2160[i].z);
        }
        func_800BCB04(r);
        pl->a54 = D_800C27A8;
    }
}
