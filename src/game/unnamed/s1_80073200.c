#include "game.h"

void game_boot(void)
{
    __osInitialize_common();
    __osInitialize_autodetect();
    D_800F4780 = 0;
    osCreateThread(&sIdleThread, 1, (void *)idle_thread, 0, &sIdleThreadStackEnd, 8);
    osStartThread(&sIdleThread);
    for (;;) {}
}

void idle_thread(s32 param_1) {
    osCreateThread(&gMainThread, 3, (void *)game_main, param_1, sMainThreadStackEnd, 10);
    osStartThread(&gMainThread);
    osSetThreadPri(NULL, 0);
    main_hang();
}
