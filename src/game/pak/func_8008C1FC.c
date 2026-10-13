#include "ctx.h"

extern u32 D_800E2004;
extern OSPfs D_800E1C78[];

void func_8000CD00(void);
s32 func_800B44D0(OSPfs *pfs, s32 file_no, OSPfsState *state);
s32 func_800B3E70(OSPfs *pfs, u16 company_code, u32 game_code, char *game_name, char *ext_name);

s32 func_8008C1FC(s32 channel, s32 file_no) {
    volatile s32 pad[6];
    OSPfsState state;
    s32 ret;
    s32 r2;

    if (!(D_800E2004 & (1 << channel))) {
        return -1;
    }
    func_8000CD00();
    ret = func_800B44D0(&D_800E1C78[channel], file_no, &state);
    if (ret != 0) {
        return 100 - ret;
    }
    r2 = func_800B3E70(&D_800E1C78[channel], state.company_code, state.game_code, state.game_name, state.ext_name);
    if (r2 != 0) {
        return -r2;
    }
    return 0;
}
