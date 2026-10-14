#include "game.h"

/* The game image's entry (from asm/game_entry.s): initializes libultra and starts the idle thread, which starts
   the main thread. */
void game_boot(void)
{
    __osInitialize_common();
    __osInitialize_autodetect();
    D_800F4780 = 0;
    osCreateThread(&sIdleThread, THREAD_ID_IDLE, (void *)idle_thread, 0, &sIdleThreadStackEnd,
                   THREAD_PRI_IDLE_START);
    osStartThread(&sIdleThread);
    for (;;) {}
}

/* Starts the main thread (game_main) with arg, then drops to the lowest priority and idles. */
void idle_thread(s32 arg)
{
    osCreateThread(&gMainThread, THREAD_ID_MAIN, (void *)game_main, arg, sMainThreadStackEnd, THREAD_PRI_MAIN);
    osStartThread(&gMainThread);
    osSetThreadPri(NULL, OS_PRIORITY_IDLE);
    main_hang();
}
