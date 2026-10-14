/* Particles of mud2.c (dust, mud, sparks, splashes, smoke) and the rain streaks. */
#ifndef STRUCTS_PARTICLE_H
#define STRUCTS_PARTICLE_H

#include "common.h"

/* Particle types; each is drawn in its own pass of particles_draw. */
#define PARTICLE_DUST    0 /* puff behind the rear wheel, coloured by the surface */
#define PARTICLE_MUD     1 /* clod thrown up, falls under gravity */
#define PARTICLE_SPARK   2 /* drawn as a point */
#define PARTICLE_SPLASH  3 /* water and rain splashes */
#define PARTICLE_SPARKLE 5 /* camera flashes in the stands (flash.c) */
#define PARTICLE_SMOKE   6 /* desert campfire smoke (fractal.c) */

/* One entry of the ring of 96 (sParticles); particle_spawn takes the next free one after sParticleLast. */
typedef struct Particle {
    /* 0x00 */ s16 life;    /* frames left; < 0 free */
    /* 0x02 */ s16 maxLife;
    /* 0x04 */ u8 type;     /* PARTICLE_* */
    /* 0x05 */ u8 surface;  /* gSurfaceTable entry whose colours tint dust and mud */
    /* 0x06 */ u8 r;        /* tint, 255 = the surface colour as is */
    /* 0x07 */ u8 g;
    /* 0x08 */ u8 b;
    /* 0x09 */ u8 pad09[3];
    /* 0x0C */ Vec3f pos;   /* metres */
    /* 0x18 */ Vec3f vel;   /* metres per frame */
} Particle; /* size 0x24 */

#endif
