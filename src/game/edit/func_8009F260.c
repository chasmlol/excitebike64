#include "ctx.h"

typedef struct {
    u8 pad0[0xC];
    u16 f0C;
    u8 rest[0x40 - 0xE];
} Slot;

typedef struct Node {
    u8 pad0[4];
    struct Node *next;
    u32 f8;
    u8 pad1[0x14 - 0xC];
    Slot *owner;
} Node;

typedef struct {
    u8 pad0[0x17C];
    Node *head;
} Obj;

extern Obj *D_800CEA40;
extern Slot D_800CE700[];

void func_8009F260(void)
{
    Obj *obj;
    Slot *blk;
    Node *n;
    s32 i;
    s32 cnt, flag;
    s32 m;
    i = 0;
    obj = D_800CEA40;
    m = -0x601;
    for (; i < 8; i++) {
        u32 c;
        blk = &D_800CE700[i];
        c = blk->f0C;
        n = obj->head;
        blk->f0C = c & m;
        cnt = 0;
        flag = cnt;
        if (n != 0) {
            do {
                if (n->owner == blk) {
                    cnt++;
                    if (n->f8 != 0 && n->next != 0) {
                        flag = 1;
                    }
                }
                n = n->next;
            } while (n != obj->head && n != 0);
        }
        if (flag) {
            blk->f0C = blk->f0C | 0x200;
        }
        if (cnt >= 2) {
            blk->f0C = blk->f0C | 0x600;
        }
    }
}
