#include "ctx.h"

typedef struct {
    s32 f0;
    s32 f4;
} Ent;

extern u32 D_800C2B64;
extern u32 D_800C2B58;
extern Ent D_800C2C10[];

void func_800B8E08();
void func_800B7990();

void func_80010D84(s32 a0) {
    Ent *e;
    Ent *b;
    s32 buf[4];
    s32 n;

    if (D_800C2B64 != 0) {
        b = D_800C2C10;
        e = b + a0;
        func_800B8E08(e->f4, buf);
        n = D_800C2B58;
        if (n > 0xFF) {
            n = 0xFF;
        }
        func_800B7990(e->f4, buf[3], n, 7, 1, 0);
    }
}
