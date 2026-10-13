#include "ctx.h"

typedef struct { u8 p0[0x22]; u16 f22; u8 p1[0x6C - 0x24]; f32 f6c; f32 f70; f32 f74; u8 p2[0x3FF - 0x78]; u8 f3ff; u16 f400; } Obj;
typedef struct { u8 p[0x2C]; s32 f2c; } GS;
typedef struct { s32 l60[4]; s32 l50; s32 l4c; s32 l48; s32 pad[1]; s32 arr[10]; } Stk;

extern GS *D_800EFD64;

void func_80011A58(s32 *a, s32 b, s32 c, s32 d);
void func_80012DC8(s32 *a, s32 *b, s32 *c);
void func_8002B5F8(s32 *p);

void func_8002D63C(Obj *o, s32 *out)
{
    Stk st;
    s16 it;
    s16 sv;
    s32 a;

    it = 2000;
    if (D_800EFD64->f2c == 1) {
        it = 6000;
    }
    a = o->f22 + 0x7000;
    sv = a;
    if (o->f3ff > 1) {
        sv = a - o->f400 * 0x40;
        o->f400 = o->f400 + 1;
    }
    st.l60[0] = 0;
    st.l60[1] = 0;
    st.l60[2] = -5000;
    func_80011A58(st.arr, it, 0, sv);
    func_80012DC8(st.l60, st.arr, &st.l50);
    out[0] = (s32)((f32)st.l50 + o->f6c * 1000.0f);
    out[1] = (s32)((f32)st.l4c + o->f70 * 1000.0f + 750.0f);
    out[2] = (s32)((f32)st.l48 + o->f74 * 1000.0f);
    out[3] = it;
    out[5] = sv;
    out[4] = 0;
    func_8002B5F8(out);
}
