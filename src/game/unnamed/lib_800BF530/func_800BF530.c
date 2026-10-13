#include "ctx.h"

typedef struct {
    u32 count;
    u32 *data;
    u8 pad8[0x400];
    u8 *bytes;
    char name[0x80];
    u8 sub[0x88];
    u32 f514;
    u32 buf[1];
} Obj;

extern char D_800DB420[];
extern char D_800DB434[];
extern char D_800DB43C[];
extern char D_800DB440[];
extern char D_800DB44C[];
extern s32 D_800D5734;
extern Obj *D_80100AB0;

void func_8000ECB8();
void func_8000EB00();
void func_8000ED10(void *a0, u8 a1, s32 a2);
void func_8000EF8C(char *a0, char *a1);
void func_800BFDA4(Obj *p);
void func_8000ED30(char *dst, char *src);
void func_800BE7F4(s32 *o, s32 v, s32 mode);
s32 func_800BE83C(s32 *p);
s32 *func_800BE694(char *a, char *b);
Obj *func_800BC908(u32 size, s32 a, char *name, s32 tag);
void func_800BE848(void *dst, s32 a, u32 n, s32 *f);
void func_800BE770(s32 *f);
void func_800AEC60(void *a, char *b, char *c);

s32 func_800BF530(char *path, Obj **out)
{
    char buf[128];
    s32 flag;
    s32 *f;
    u32 n;
    u32 cnt;
    Obj *o;
    u32 j;
    u8 *p;
    u8 t;

    flag = 0;
    *(u8 *)&flag = 0xFF;
    if (flag == 0xFF) {
        D_800D5734 = 0;
    } else if (flag == (s32)0xFF000000) {
        D_800D5734 = 1;
    } else {
        func_8000EB00(D_800DB420);
        func_8000ECB8(1);
    }

    func_8000EF8C(buf, path);
    func_8000ED30(buf, D_800DB434);
    f = func_800BE694(buf, D_800DB43C);
    if (f == NULL) {
        return 1;
    }
    func_800BE7F4(f, 0, 2);
    n = func_800BE83C(f);
    func_800BE7F4(f, 0, 0);
    if (D_80100AB0 == NULL) {
        cnt = n >> 2;
        o = func_800BC908(cnt * 12 + 0x518, 0, D_800DB440, 0xB0);
        if (o == NULL) {
            return 2;
        }
        o->f514 = 0;
    } else {
        cnt = n >> 2;
        o = D_80100AB0;
        o->f514 = 1;
        D_80100AB0 = NULL;
    }
    func_800BFDA4(o);
    o->count = n >> 2;
    if (n >> 2 == 0) {
        return 2;
    }
    o->data = o->buf;
    func_8000ED10(o->buf, 0, n);
    o->bytes = (u8 *)(o->buf + cnt * 2);
    func_800BE848(o->buf + cnt * 2, 1, n, f);

    if (D_800D5734 != 0) {
        flag = 0;
        if ((s32)o->count > 0) {
            do {
                j = 0;
                do {
                    p = (u8 *)(flag * 4 + (s32)o->bytes);
                    t = p[j];
                    p[j] = p[3 - j];
                    ((u8 *)((s32)o->bytes + flag * 4))[3 - j] = t;
                    j++;
                } while (j < 2);
                flag++;
            } while (flag < (s32)o->count);
        }
    }
    func_800BE770(f);
    func_8000EF8C(o->name, path);
    func_800AEC60(o->sub, D_800DB44C, path);
    *out = o;
    return 0;
}
