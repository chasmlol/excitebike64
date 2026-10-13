#include "ctx.h"

extern char D_800DB2F4[];
extern char D_800DB340[];
extern char D_800DB364[];
void func_8000EFB0(char *fmt, ...);
void func_800BC3D0(void);

s32 func_800BD924(s32 *p)
{
    s32 *h = p - 4;
    s32 *t;
    s32 sum;
    char *msg;

    sum = h[0] + h[1] + h[2] + h[3];
    if (sum != 0xB562439F) {
        goto bad_head;
    }
    t = h + h[2] * 4;
    if (t[0] + t[1] + t[2] + t[3] != 0xB562439F) {
        goto bad_tail;
    }
    if (h[1] & 1) {
        return (h[2] - 1) * 16;
    }
    goto bad_free;
bad_head:
    msg = D_800DB2F4;
    goto fail;
bad_tail:
    msg = D_800DB340;
    goto fail;
bad_free:
    msg = D_800DB364;
fail:
    func_8000EFB0(msg);
    func_800BC3D0();
    return -1;
}
