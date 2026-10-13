#include "ctx.h"

typedef struct {
    f32 x, y, z;      /* 0x00 */
    f32 dist;         /* 0x0C */
    f32 f10;          /* 0x10 */
    u8 pad14[0x30];
    u8 f44;           /* 0x44 */
    u8 pad45;
    s16 next;         /* 0x46 */
    s16 f48;
    s16 f4A;
    s16 f4C;
    s16 f4E;
    s16 pad50;
    s16 f52;
    s16 f54;
    s16 f56;
    f32 f58;
    f32 f5C;
    s32 f60;
    u8 f64;
    u8 f65;
    u16 f66;
    f32 f68;
} Node;

typedef struct {
    s32 x;      /* 0x00 */
    s32 z;      /* 0x04 */
    u16 f10;    /* 0x08 */
    u16 f68;    /* 0x0A */
    s16 next;   /* 0x0C */
    s16 f48;    /* 0x0E */
    s16 f4A;    /* 0x10 */
    s16 f4C;    /* 0x12 */
    s16 f4E;    /* 0x14 */
    s16 pad16;
    u8 f44;     /* 0x18 */
    u8 f65;     /* 0x19 */
    s16 f52;    /* 0x1A */
    s16 f54;    /* 0x1C */
    s16 f56;    /* 0x1E */
    s16 f58;    /* 0x20 */
    s16 f5C;    /* 0x22 */
    s32 f60;    /* 0x24 */
} Rec;

extern s32 D_800EE488;
extern f32 D_800EE48C;
extern s32 D_800EE490;
extern s32 D_800EE494;
extern Node *D_800EE498;
extern char *D_800EFD64;
extern char D_800CB3F0[];
extern char D_800D7250[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8004B684(s32 param_1, s32 param_2);
float func_800228A0(float *p);
void func_80048814(s32 idx, s32 stop);
void func_800486B0(void);
void func_80048E08(void);
void func_8004896C(s32 i, s32 end);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);
f32 func_80048410(s32 a, s32 b);
void func_80049AA4(s32 a, s32 b);

void func_800492D0(u32 *p)
{
    u32 w;
    u32 flags;
    s32 stride;
    s32 i;
    Rec *r;
    u8 *e;
    f32 v;
    f32 dx, dz;

    w = *p;
    stride = 0x20;
    if ((w >> 24) == 0xAA) {
        flags = w;
        flags &= 0xFFFFFF;
        p++;
        if (w & 1) stride = 0x28;
        if (w & 2) stride += 1;
        if (w & 4) stride += 2;
        if (w & 8) stride += 2;
        stride = (stride + 3) & ~3;
    }
    D_800EE490 = *p;
    if (flags & 0x20) {
        D_800EE494 = p[1];
        D_800EE488 = p[2];
        p += 2;
    } else {
        D_800EE488 = 0;
        D_800EE494 = 0;
    }
    i = 0;
    D_800EE498 = func_800BC908(D_800EE490 * sizeof(Node), 0, (s32)D_800D7250, 0x20C);
    func_8000ED10((u8 *)D_800EE498, 0, D_800EE490 * sizeof(Node));
    r = (Rec *)(p + 1);
    for (; i < D_800EE490; i++) {
        func_8000ED10((u8 *)&D_800EE498[i], 0, sizeof(Node));
        D_800EE498[i].f4E = r->f4E;
        D_800EE498[i].f4A = r->f4A;
        D_800EE498[i].f4C = r->f4C;
        D_800EE498[i].next = r->next;
        D_800EE498[i].f48 = r->f48;
        D_800EE498[i].x = (f32)r->x / 1000.0f;
        D_800EE498[i].z = (f32)r->z / 1000.0f;
        if (D_800EE498[i].f4A < i) {
            D_800EE498[i].y = D_800EE498[D_800EE498[i].f4A].y + 3.0f;
            if (D_800EFD64 == D_800CB3F0) {
                D_800EE498[i].y = D_800EE498[i].y + 4.0f;
            }
        } else {
            D_800EE498[i].y = 0.0f;
        }
        D_800EE498[i].y = func_800228A0((f32 *)&D_800EE498[i]);
        D_800EE498[i].f10 = r->f10;
        D_800EE498[i].f44 = r->f44;
        D_800EE498[i].f65 = r->f65;
        D_800EE498[i].f56 = r->f56;
        D_800EE498[i].f52 = r->f52;
        D_800EE498[i].f54 = r->f54;
        D_800EE498[i].f68 = r->f68 / 1000.0f;
        if (D_800EE498[i].f10 < D_800EE498[i].f68) {
            D_800EE498[i].f68 = 0.0f;
        }
        e = (u8 *)r + 0x20;
        if (flags & 1) {
            D_800EE498[i].f58 = (f32)r->f58 / 62.5f - 1.0f;
            if (D_800EE498[i].f58 < 0.0f) D_800EE498[i].f58 = 0.0f;
            D_800EE498[i].f5C = (f32)r->f5C / 62.5f - 1.0f;
            if (D_800EE498[i].f5C < 0.0f) D_800EE498[i].f5C = 0.0f;
            e = (u8 *)r + 0x28;
            D_800EE498[i].f60 = r->f60;
        }
        if (flags & 0x10) {
            if (flags & 4) {
                D_800EE498[i].f66 = *e++ << 8;
                D_800EE498[i].f66 |= *e++;
            }
            if (flags & 8) e += 2;
            if (flags & 2) D_800EE498[i].f64 = *e;
        } else {
            if (flags & 2) D_800EE498[i].f64 = *e++;
            if (flags & 4) {
                D_800EE498[i].f66 = *e++ << 8;
                D_800EE498[i].f66 |= *e++;
            }
        }
        r = (Rec *)((u8 *)r + stride);
    }
    for (i = 0; i < D_800EE490; i++) {
        flags = D_800EE498[i].next;
        dx = D_800EE498[flags].x - D_800EE498[i].x;
        dz = D_800EE498[flags].z - D_800EE498[i].z;
        D_800EE498[i].dist = __builtin_sqrtf(dx * dx + dz * dz);
    }
    func_800486B0();
    func_80048814(0, 0);
    func_8004896C(0, 0);
    func_80048E08();
    D_800EE48C = func_80048410(D_800EE488, D_800EE498[D_800EE488].f4A)
               + D_800EE498[D_800EE498[D_800EE488].f4A].dist;
    func_8004B684(0, D_800EE490 - 1);
    func_80049AA4(0, D_800EE498[0].f4A);
    for (i = 0; i < D_800EE490; i++) {
    }
}
