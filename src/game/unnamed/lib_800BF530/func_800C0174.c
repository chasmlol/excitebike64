#include "ctx.h"

typedef struct {
    u8 unk0[0x408];
    s32 *offsets;
    u8 unk40C[0x48C - 0x40C];
    char path[1];
} ResFile;

extern s32 D_800D5734;

void *func_800BE694(char *path, char *mode);
void func_800BE7F4(void *f, s32 offset, s32 whence);
void func_800BE848(void *buf, s32 size, s32 count, void *f);
void func_800BE770(void *f);

s32 func_800C0174(ResFile *r, s32 idx) {
    void *f;
    u8 tmp[8];
    s32 val;
    u32 i;

    f = func_800BE694(r->path, "rb");
    if (f == NULL) {
        return -1;
    }
    func_800BE7F4(f, r->offsets[idx], 0);
    func_800BE848(tmp, 1, 4, f);
    func_800BE848(&val, 1, 4, f);
    if (D_800D5734 != 0) {
        for (i = 0; i < 2; i++) {
            u8 t = ((u8 *)&val)[i];

            ((u8 *)&val)[i] = ((u8 *)&val)[3 - i];
            ((u8 *)&val)[3 - i] = t;
        }
    }
    func_800BE770(f);
    return val;
}
