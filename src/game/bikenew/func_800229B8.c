#include "ctx.h"

typedef struct {
    u8 pad[0x658];
    void *p658;
    void *p65C;
    u8 pad2[0x7E0 - 0x660];
} Rider;

extern Rider D_800E37C0[];
void func_800BCB04(void *ptr);
void func_8005C8FC(void);
void func_80064D10(void);
void func_80061DA8(void);

void func_800229B8(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        func_800BCB04(D_800E37C0[i].p658);
        func_800BCB04(D_800E37C0[i].p65C);
    }
    func_8005C8FC();
    func_80061DA8();
    func_80064D10();
}
