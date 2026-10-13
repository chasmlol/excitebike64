#include "ctx.h"

typedef struct { u8 pad[0xE]; s8 asc; } Font;
typedef struct {
    s32 font;
    s32 f4;
    u32 pad8 : 24;
    u32 valign : 4;
    u32 halign : 4;
    s32 fC;
    s32 x;
    s32 y;
    u32 w;
    u32 h;
    char text[1];
} Text;

extern Font *D_80105380[];
void func_8000ED30(char *dst, char *src);
void func_800C1228(char *a, Font *b, u32 *c, u32 *d);

char *func_800C1F2C(Text *p, char *s)
{
    Font *font;

    if (p != NULL) {
        func_8000ED30(p->text, s);
        font = D_80105380[p->font];
        func_800C1228(p->text, font, &p->w, &p->h);
        if (p->halign == 1) {
            p->x = -((p->w + 1) / 2);
        } else if (p->halign == 2) {
            p->x = -p->w;
        } else {
            p->x = 0;
        }
        if (p->valign == 1) {
            p->y = 0;
        } else if (p->valign == 0) {
            p->y = font->asc - 1;
        } else if (p->valign == 2) {
            p->y = font->asc - 1 - (p->h + 1) / 2;
        } else if (p->valign == 3) {
            p->y = font->asc - 1 - p->h;
        }
    }
    return s;
}
