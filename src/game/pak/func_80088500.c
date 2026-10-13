#include "ctx.h"

typedef struct { char c[16]; } Name16;
typedef struct { char c[4]; } Ext4;
typedef struct {
    s32 size;
    s32 magic;
    u16 company;
    Ext4 ext;
    Name16 name;
    s16 used;
    u8 data[0x20];
    char buf[0x30];
    u8 status;
    u8 pad[3];
} Rec;

extern s32 D_800F4AE0[];
extern Rec *D_800F48E0[];
extern Rec *D_800F4A10[];
extern Rec D_800CE168;
extern char D_800F47D0[];
extern char D_800DA708[];
extern char D_800DA70C[];
extern struct G4A28 {
    s32 v;
    u8 pad[0x28];
    s32 (*fn)(u8 *, char *, s32);
} D_800F4A28;

s32 func_800AEC10(char *p);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_8000ED30(char *dst, char *src);

void func_80088500(s32 p, s32 z)
{
    s32 *cnt;
    s32 i;
    s32 r;
    s32 j;
    Rec *rec;

    D_800F4AE0[p] = 0;
    if (z != 0) {
        D_800F48E0[p * 17] = &D_800CE168;
        D_800F4AE0[p] = 1;
    }
    for (i = 0; i < 16; i++) {
        rec = &D_800F4A10[p][i];
        if (rec->used != -1) {
            if (rec->magic != 0x4E4D5847) {
                r = 1;
            } else {
                r = D_800F4A28.fn(rec->data, D_800F47D0, D_800F4A28.v == 0);
                if (r == 2) {
                    if (D_800F4A28.v == 1) {
                        continue;
                    }
                    r = 1;
                }
            }
            if (r != 1 || D_800F4A28.v != 1) {
                func_8000ED10((u8 *)rec->buf, 0, 0x30);
                *(Name16 *)rec->buf = rec->name;
                if (rec->ext.c[0] != 0) {
                    func_8000ED30(rec->buf, D_800DA708);
                    *(Ext4 *)&rec->buf[func_800AEC10(rec->buf)] = rec->ext;
                }
                if (r != 1) {
                    func_8000ED30(rec->buf, D_800DA70C);
                    func_8000ED30(rec->buf, D_800F47D0);
                }
                rec->status = r;
                j = p * 17; j += D_800F4AE0[p];
                D_800F48E0[j] = rec;
                D_800F4AE0[p]++;
            }
        }
    }
}
