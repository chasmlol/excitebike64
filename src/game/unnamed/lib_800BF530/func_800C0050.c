typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

extern void func_8000EF8C(char *a0, char *a1);
extern void func_8000ED30(char *dst, char *src);
extern s32 func_8000EDA0(u8 *param_1, u8 *param_2);
extern void func_800BE770(s32 *q);
extern s32 *func_800BE694(u8 *param_1, char *name);
extern s32 func_800BE848(u8 *param_1, s32 param_2, s32 param_3, s32 *param_4);
extern char D_800DB494[];
extern char D_800DB43C[];
extern s32 D_800D5734;

s32 func_800C0050(u8 *a0, u8 *a1)
{
    u8 buf[0x50];
    s32 cnt;
    s32 *q;
    u32 k;
    s32 i;

    func_8000EF8C((char *)buf, (char *)(a0 + 0x40c));
    func_8000ED30((char *)buf, D_800DB494);
    q = func_800BE694(buf, D_800DB43C);
    if (q == 0) {
        return -1;
    }
    func_800BE848((u8 *)&cnt, 1, 4, q);
    k = 0;
    if (D_800D5734 != 0) {
        do {
            u8 t1 = ((u8 *)&cnt)[k];
            u8 t2 = ((u8 *)&cnt)[3 - k];
            ((u8 *)&cnt)[k] = t2;
            ((u8 *)&cnt)[3 - k] = t1;
            k++;
        } while (k < 2);
    }
    i = 0;
    if (cnt > 0) {
        do {
            func_800BE848(buf, 1, 0x10, q);
            if (func_8000EDA0(a1, buf) == 0) {
                func_800BE770(q);
                return i;
            }
            i++;
        } while (i < cnt);
    }
    func_800BE770(q);
    return -1;
}
