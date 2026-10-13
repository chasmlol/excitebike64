#include "ctx.h"

typedef struct {
    u8 pad0[8];
    s16 f8;
    u8 pad1[0xA];
} Cell;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u8 pad[0xA];
} GridPt;

extern Cell *D_800CE1FC;
extern u8 *D_800F4FF0;
extern s32 D_800F4F8C;
extern s32 D_800F4F90;
extern u8 **D_800F55A0;
extern s16 D_800F4B80[];
extern GridPt *D_800F4F9C;
extern void *D_800F557C;
extern s32 D_800F4FD4;
extern s32 D_800F55A4;
extern char D_800DA830[];

void *func_8000ECB8(s32);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8008ED48(void);
void func_8008CB10(s32 p1, s32 p2, s32 p3, s32 p4);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);

void func_8008CF94(s32 w, s32 h)
{
    s32 size;
    s32 n;
    s32 i;
    s32 j;
    s32 k;
    s32 nx;
    s32 ny;
    s32 hh;
    u8 *p;
    GridPt *g;

    if (D_800CE1FC != NULL) {
        func_8008ED48();
    }
    D_800F4FF0 = func_800BC908(0x3880, 0, (s32)D_800DA830, 0xD4);
    size = w * ((h << 2) + h << 2);
    D_800CE1FC = p = func_800BC908(size, 0, (s32)D_800DA830, 0xD7);
    if (p == NULL) {
        p = func_8000ECB8(1);
    }
    D_800F4F8C = w;
    D_800F4F90 = h;
    func_8000ED10(p, 0, size);
    n = D_800F4F8C * D_800F4F90;
    for (i = 0; i < n; i++) {
        D_800CE1FC[i].f8 = -1;
    }
    n = -1;
    for (i = 0x1C1; i >= 0; i--) {
        D_800F4B80[i] = n;
    }
    D_800F55A0 = func_800BC908(0x708, 0, (s32)D_800DA830, 0xE4);
    for (i = 0; i < 0x1C2; i++) {
        p = func_800BC908(0x750, 0, (s32)D_800DA830, 0xE7);
        D_800F55A0[i] = p;
        func_8000ED10(p, 0, 0x750);
    }
    hh = h;
    do {
        GridPt **pp = &D_800F4F9C;
        nx = w + 1;
        ny = hh + 1;
        *pp = func_800BC908(nx * (ny << 4), 0, (s32)D_800DA830, 0xEF);
        i = 0;
    } while (0);
    k = 0;
    if (ny > 0) {
        do {
            for (j = 0; j < w + 1; j++) {
                D_800F4F9C[k].x = j * 500 - w * 250;
                D_800F4F9C[k].y = 0;
                D_800F4F9C[k].z = i * 500 - hh * 250;
                k++;
            }
        } while (++i < hh + 1);
    }
    i = 0;
    func_8000ED10(D_800F4FF0, 0, 0x3880);
    D_800F557C = func_800BC908((D_800F4F8C + D_800F4F90) * 0x80, 0, (s32)D_800DA830, 0x100);
    D_800F4FD4 = 0;
    for (i = 0; i < D_800F4F8C; i++) {
        func_8008CB10(i * 500 - D_800F4F8C * 250, D_800F4F90 * 250, 500, 0);
        func_8008CB10(D_800F4F8C * 250 - i * 500, -(D_800F4F90 * 250), -500, 0);
    }
    for (i = 0; i < D_800F4F90; i++) {
        func_8008CB10(-(D_800F4F8C * 250), i * 500 - D_800F4F90 * 250, 0, 500);
        func_8008CB10(D_800F4F8C * 250, D_800F4F90 * 250 - i * 500, 0, -500);
    }
    D_800F55A4 = -1;
}
