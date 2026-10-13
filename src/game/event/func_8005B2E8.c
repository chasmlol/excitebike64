#include "ctx.h"

typedef struct {
    s32 state;
    s32 pad[10];
    s32 cnt;
} Blk;

extern Blk D_800F05A8;
extern s16 D_800E3E2A;
extern void func_800515A8(void);

void func_8005B2E8(void)
{
    func_800515A8();
    if (D_800F05A8.state == 5) {
        if (D_800F05A8.cnt != 0) {
            s32 r = D_800F05A8.cnt - 1;
            D_800F05A8.cnt = r;
            if (r == 0) {
                D_800E3E2A = 0x78;
            }
        }
    }
}
