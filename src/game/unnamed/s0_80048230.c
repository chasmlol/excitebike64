#include "game.h"

void font_load_race_set(void *arc)
{
    if (sFontSetLoaded != 2) {
        if (sFontSetLoaded != 0 && font_unload() != 0) {
            fatal_halt(1);
        }
        while (sched_get_current_task() != 0) {
        }
        gFontLoadBuffer = gZBuffer;
        if (font_load(arc, sRaceFontIds, sFontColors) != 0) {
            fatal_halt(1);
        }
        gFontLoadBuffer = 0;
        sFontSetLoaded = 2;
    }
}

void font_load_menu_set(u32 *param_1)
{
    if (sFontSetLoaded != 1) {
        if (sFontSetLoaded != 0 && font_unload() != 0) {
            fatal_halt(1);
        }
        if (font_load(param_1, sMenuFontIds, sFontColors) != 0) {
            fatal_halt(1);
        }
        sFontSetLoaded = 1;
    }
}

void font_unload_set(void) {
    font_unload();
    sFontSetLoaded = 0;
}

Gfx *font_gfx_setup(Gfx *g)
{
    Gfx *p0, *p1, *p2, *p3, *p4, *p5;
    p0 = g++;
    p0->words.w0 = 0xE7000000;
    p0->words.w1 = 0;
    p1 = g++;
    p1->words.w0 = 0xE3001001;
    p1->words.w1 = 0;
    p2 = g++;
    p2->words.w0 = 0xF9000000;
    p2->words.w1 = 0x10;
    p3 = g++;
    p3->words.w0 = 0xE3000C00;
    p3->words.w1 = 0;
    p4 = g++;
    p4->words.w0 = 0xE3001201;
    p4->words.w1 = 0;
    p5 = g++;
    p5->words.w0 = 0xD7000002;
    p5->words.w1 = 0x80008000;
    return g;
}
