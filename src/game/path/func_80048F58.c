#include "ctx.h"

typedef struct {
    Vec3f pos;
    f32 t;
    f32 f10;
    f32 f14;
    u8 pad18[0x46 - 0x18];
    s16 next;
    s16 f48;
    s16 prev;
    s16 f4c;
    s16 f4e;
    u8 pad50[0x58 - 0x50];
    f32 f58;
    f32 f5c;
    u8 pad60[0x65 - 0x60];
    u8 bits;
    u8 pad66[0x6C - 0x66];
} Node;

extern u8 D_800D7250[];
extern Node *D_800EE498;
extern s32 D_800E8508;
extern s32 D_800E84F8;
extern s32 D_800EE490;
extern s32 D_800EE494;
extern s32 D_800EE488;

void *func_800BC908(s32 size, u32 flags, u8 *owner, s32 tag);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
float func_800228A0(float *p);
void func_8002F4B0(f32 *param_1);
void func_800486B0(void);
void func_80048814(s32 idx, s32 stop);
void func_8004896C(s32 i, s32 end);
void func_80048E08(void);

void func_80048F58(void)
{
    s32 i, s, a1;
    f32 R, t;
    Vec3f v, w, m;

    D_800EE498 = func_800BC908(540000, 0, D_800D7250, 0x178);
    func_8000ED10((u8 *)D_800EE498, 0, 540000);
    R = 30.0f;
    m.x = 0.0f;
    m.y = 0.0f;
    m.z = 0.0f;
    for (i = 0; i < 1000; i++) {
        D_800EE498[i].prev = i - 1;
        D_800EE498[i].next = i + 1;
        D_800EE498[i].f4e = -1;
        D_800EE498[i].f4c = -1;
        D_800EE498[i].f48 = -1;
        t = 3.0f;
        do {
            t -= 1.0f;
            v.x = 0.0f;
            v.y = 40.0f;
            v.z = R - t;
            v.y = func_800228A0((float *)&v);
            if (D_800E8508 == 0) {
                break;
            }
            func_8002F4B0((float *)&w);
        } while (w.z * m.z < -0.01f && 1.1f < t);
        m = w;
        D_800EE498[i].pos = v;
        D_800EE498[i].t = t;
        D_800EE498[i].bits = 0;
        for (s = 3; s >= 0; s--) {
            v.x = s * 2.178f;
            func_800228A0((float *)&v);
            switch ((D_800E84F8 >> 4) & 0xF) {
            case 2:
            case 3:
                a1 = 1;
                break;
            case 8:
                a1 = 2;
                break;
            case 7:
                a1 = 3;
                break;
            default:
                a1 = 0;
                break;
            }
            D_800EE498[i].bits |= a1 << (s * 2);
        }
        R -= t;
        D_800EE498[i].f10 = 50.0f;
        D_800EE498[i].f58 = 7.284f;
        D_800EE498[i].f5c = 0.75f;
        if (D_800E8508 == 0) {
            break;
        }
    }
    D_800EE498[0].prev = i;
    D_800EE498[i].next = 0;
    D_800EE490 = i + 1;
    D_800EE494 = 1;
    D_800EE488 = 0;
    func_800486B0();
    func_80048814(0, 0);
    func_8004896C(0, 0);
    func_80048E08();
    for (i = 0; i < D_800EE490; i++) {
        D_800EE498[i].f10 = 50.0f;
        D_800EE498[i].f14 = -1.0f;
    }
}
