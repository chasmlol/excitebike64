# How the game works

What the decompiled code shows about Excitebike 64 (USA LodgeNet): the threads and game states, the main data
structures, each source file, and the larger subsystems (bike physics, the NES emulator, sound, saves, the track
editor). Names are the ones in symbols.txt and functions.csv; offsets are hex. Where something is inferred from
how the code uses it rather than shown by strings or data, the text says so. Names still of the form
`func_XXXXXXXX` / `D_XXXXXXXX` are not understood well enough yet.

## Startup

**Boot loader** (ROM 0x1000, run at 0x80000400; src/boot, asm/): `boot_entry` sets the stack, clears the BSS and
calls `boot_main`, which starts the idle thread (`boot_idle`) and from it `boot_thread_main`. That thread:

1. sets up the VI (NTSC or MPAL; a PAL console hangs);
2. SWD-unpacks the splash screen (`sBootSplashSwd`) and shows it for 120 frames;
3. DMAs the `BRSOHYES` container (ROM 0x12200) and unpacks its four segments to 0x80300000 with `boot_swd_expand`
   (John Brandwood's SwdExpand, asm/boot_swdexpand.s);
4. copies `boot_game_copy_stub` to 0x802F0000 and jumps to it; the stub moves the game image over the boot loader
   to 0x80000400 and jumps there.

`boot_check_reset` polls for PRE-NMI between the steps. A debug font (`boot_draw_char`, `boot_draw_string`,
`sBootFontBitmap`), a controller probe, a fade (`boot_fade_image`) and a progress bar are linked but never called.

The boot loader also carries **SN Systems' debug runtime** (`sn_*`, src/boot/boot_80001B50.c, boot_80001C2C.c and
asm/boot_sn_stubs.s). It talks to the development board through 0xB1FF0000 (data) and 0xB1FFFFE0-0xB1FFFFFC
(commands: PCinit 0x101, PCopen 0x202, PCcreat 0x303, PCclose 0x404, PCread 0x505, PCwrite 0x606, PClseek 0x707),
hooks the exception vectors (`sn_install_exception_hooks`) and runs `sn_debug_command_loop`; `boot_log_write`
writes to "output.log" on the host.

**Game image**: asm/game_entry.s calls `game_boot` (unnamed/s1_80073200.c), which initializes libultra and starts
`idle_thread`; that starts the main thread, `game_main` (main.c, priority 10), and drops to priority 0. The u16 at
0x80000402 tells `game_main` how it was started: 0xBEEF means the boot loader's entry is still in memory, so code
overlays are unpacked with the header it left (`gBootHeader`, read from 0x803FFFC0); 0xDEAD means a direct load.
`main_init_memory` checks the overlay alignment, clears memory after the BSS, sets up the heap and reserves the
overlay ranges. During a race the front-end code (0x80073200-0x800A42A0: shell, Controller Pak, editor, GUI and
NES emulator) and the UsrTrk.c overlay are given back to the heap and reloaded afterwards
(`race_load_frontend_code`).

At startup the game picks 640x480 or 512x240 framebuffers depending on `gHasExpansionPak`, allocates the two
`gFramebuffers` and `gZBuffer`, and starts the scheduler.

## Threads

- **Main** (`game_main`): everything the game does, audio mixing included. It waits on `sMainQueue`.
- **Scheduler** (`sched_thread`, unnamed/sched.c): sends VI retrace messages to up to four clients
  (`sched_add_client`), runs graphics tasks (`sched_submit_task`, `sched_run_task`), swaps framebuffers when the RDP
  is done, handles PRE-NMI, and on a CPU exception draws the registers in hex on screen (`fault_draw_hex`). It
  switches between three VI modes (320x240, 512x240, 640x480) through `gViModeRequest` / `gViMode`.
- **Scheduler client** (`main_sched_event_thread`, main.c): turns scheduler messages into main-queue messages:
  1 = retrace (it also sets the VI dither filter), 2 = graphics task done.
- libultra's VI and PI manager threads.

There is no audio thread: `snd_update` runs several times per frame from the main loop, and `snd_service` is
installed as the callback that runs while DMA or resource loading waits (`gDmaIdleCallback`), so music keeps
streaming during loads.

## Main loop and game states

`gGameStates` (main.c's table, 4 entries) holds `{init, update(pads), draw(context, framebuffer index, &gdl), exit,
mode, display-list size, wait counter}`:

| state | init / update / draw / exit | what |
|---|---|---|
| 0 | `shell_init` / `shell_update` / `shell_draw` / `shell_exit` | front end (shell.c) |
| 1 | `race_init` / `race_update` / `render_draw_race` / `race_exit` | a race (min.c; drawing in render.c) |
| 2 | `edit_init` / `edit_update` / `edit_draw` / `edit_exit` | track editor (edit.c) |
| 3 | `nes_init` / `nes_update_input` / `nes_draw_frame` / `nes_exit` | NES emulator (nesn64.c) |

`gCurGameState` points at the running entry; its `mode` is 0 (needs init), 2 (running), 3 (exit requested: a state
asks to leave by setting it), 4 (exiting) or 5 (waiting for the GPU to drain). On a retrace message `game_main`:

1. initializes a state whose mode is 0 (display lists sized from the table, the VI mode for that state, cleared
   framebuffers);
2. reads the controllers and turns each `OSContPad` into a game input word (`main_convert_pad_input`, into
   `gPadInput`, last frame in `gPadInputPrev`);
3. runs the state's update, or its exit and the choice of the next state: `gGame.flags` 0x40 selects the NES state,
   0x20 the editor; otherwise the shell and the race alternate (flag 4 runs the same state again). Leaving the
   editor goes to a test race on the user track (track 21) or, with flag 2, back to the shell; `gRaceExitCode` 8
   returns from a race to the editor; after a race with flag 0x800000 the next race is the credits run on track 13
   (`gCreditsMode`);
4. builds the display list with the state's draw function, ends it, and submits an `OSScTask` when fewer than two
   tasks are pending (`sGfxTasksPending`); the two `gGfxContexts` alternate. Every task starts with rspboot
   (`gRspBootText`) and the F3DLX2.Rej xbus microcode (`gF3dlx2RejText`, `gF3dlx2RejData`); display lists switch to
   F3DEX2.NoN xbus (`gF3dex2NonText`, `gF3dex2NonData`) where they need it (`render_load_ucode`: render.c's near
   track and the editor's views).

A graphics-done message decrements `sGfxTasksPending`. Debug leftovers in the loop: a busy-wait lag generator
(`sDebugLagEnable`), a freeze while D-pad up+down are held (it writes a development PI port at 0x0FF70000), and a
screenshot dump to `e:\log\scr*.raw` whose file writes are gone.

## Input

The game input word (`gPadInput[4]`, and per rider in `Rider.input`):

| bits | meaning |
|---|---|
| 0x1 0x2 0x4 0x8 | stick up, down, left, right |
| 0x10 0x20 0x40 | A, B, START |
| 0x80 0x100 0x200 0x400 | C up, down, left, right |
| 0x800-0x4000 | D-pad up, down, left, right |
| 0x8000 0x10000 0x20000 | L, R, Z |
| 24-27, 28-31 | stick x and y magnitude (the direction bits give the signs) |

In a race, B brakes, Z is the turbo (Z with the stick back starts a wheelie, Z at take-off boosts), R is used for
air tricks and, with B, a power slide. `bike_remap_buttons` applies the controller layout chosen in the options (it
swaps B, R and Z). `race_update` maps controllers to players into `gPlayerInputs`.

## Main data

**`gGame`** (`Game`, include/structs/game.h; 0x7B0 bytes, cleared by `shell_init`) is the setup that carries across
states:

- +0x00 flags: 0x2 editor back to shell, 0x4 run the same state again, 0x20 editor, 0x40 NES, 0x80 track chosen,
  0x8000 replay playback, 0x40000 podium ceremony (`gPodiumMode`), 0x100000/0x400000/0x800000 unlock messages,
  0x20000000 debug cheat;
- +0x0C cup, +0x10 track, +0x1C rider per racer, +0x3C number of players, +0x40 laps;
- +0x4C game mode (`mode`, below), +0x54 front-end fade state (0 out, 1 swap, 2 in, 3 active);
- +0x2C8 controller-to-player map, +0x2E8 screen sub-state, +0x410 difficulty, +0x488 Controller Pak screen mode,
  +0x730 HUD glyphs, +0x740 idle frames (the front end times out after 0xE10).

**Game modes** (`gGame.mode`): 0 exhibition, 1 season (`gSeason`, a `Season` of the cups `gCups`; points `gRacePoints`
5/3/2/1), 2 soccer,
5 desert (ten checkpoints shown as campfires), 6 Excite 3D, 7 hill climb (stage time limits), 8 time trial (ghost),
9 tutorial (`gTutorialLessons`, 17 lessons and tests), 10 stunt course, 12 Original Excitebike (the NES game),
13 and 14 the editor's test ride. Modes 3 and 4 are not identified.

**Tracks**: `gTracks` holds 27 records of 0xA0 bytes, the name at +0, the environment type at +0x2C (0 stadium,
drawn by arngfx.c; 1 outdoor, tubegfx.c; 2 desert, desgfx.c) and the replay camera zones at +0x74/+0x78.
`gCurTrack` points at the current one (`gCurTrack - gTracks` is the index the per-track code tests; `enum TrackId`
in include/structs/race.h names the indices).

| # | track | # | track | # | track |
|---|---|---|---|---|---|
| 0 | Kyoto, Japan | 9 | Las Vegas, NV | 18 | Seattle, WA |
| 1 | Mountain Quarry | 10 | Phoenix, AZ | 19 | Blizzard Blitz |
| 2 | Houston, TX | 11 | Rainforest Run | 20 | Excite 3D |
| 3 | Lefty's Mill | 12 | Madrid, Spain | 21 | User Track |
| 4 | Orlando, FL | 13 | The Gravel Pit | 22 | Soccer |
| 5 | Nashville, TN | 14 | Detroit, MI | 23 | Hill Climb |
| 6 | Canyon Chasm | 15 | Goldmine Rush | 24 | Stunt Course |
| 7 | Long Island, NY | 16 | Los Angeles, CA | 25 | Desert |
| 8 | Congo Course | 17 | Construction Yard | 26 | Tutorial |

**Riders**: `gRiders` (8 `Rider`s of 0x7E0 bytes, below) is the simulation state; `gRiderGfx` (8 x 0xA80) the render
side: texture sets, rider and bike palettes, the rider's colour at +0x21C, body display lists skinned on the 15-bone
`gRiderSkelTemplate`, six bike-part display lists on the 10-bone `gBikeSkelTemplate`, matrices, and the ponytail nodes
at +0xA48. `gRiderAlpha` holds per-rider transparency (0x80 for the ghost). `gRiderStandings` is the race order.

**Race**: `gRacePhase` runs 0-3 (intro: track preview, laps), 4 (fade in), 5 (racing), 6 (fade out requested by
`event_request_exit`) and 7 (leave or restart). `gEvent` (`Event`, include/structs/race.h) holds the rules state of
the running event:

- +0x00 state (0-5 start countdown, 6-12 soccer halves and overtime, 13-15 tutorial);
- +0x04 the track's three best times and initials, +0x10 best lap, +0x20 laps, +0x22 riders,
  +0x24 next finishing place, +0x26 countdown;
- +0x2C race time in frames, +0x40 all finished, +0x50 finishing order;
- +0x58 flags (0x10 timer paused, 0x800 message, 0x1000 takedown required, 0x2000 lesson done);
- +0x68/+0x6A soccer scores, +0x70 tutorial action.

Other shared globals: `gFrameCounter` (retraces), `gRaceFrame` (frames of the race), `gPhysDeltaTime` (ticks times
the time scale / 60), `gSurfaceTable` (16 ground surfaces, `Surface`), `gPathNodes` (the
racing line, path.c), the camera output `gCamPos`/`gCamPitch`/`gCamRoll`/`gCamYaw`, `gNumViews` and `gViewRects`
(split screen), `gCheatFlags` and `gDrawFlags`.

**Cheats**: `gCheatFlags` bit n is cheat n of `sCheatTable`: Big Head, Mirror, Debug, Down Hill, Stunt Bonus,
Invisible Rider, Night, Odd Color, Transparent Rider, All Stunts, Invincible, Team Photo, Beat This!, Pin Head.
Entering a code in the front end toggles a bit of `sCheatMask`; Debug Mode sets `gGame` flag 0x20000000 instead.
`shell_exit` copies the mask to `gCheatFlags`, without Stunt Bonus and All Stunts in time trial and tutorial.

**Debug draw switches**: `gDrawFlags` bit n is debug menu item n: Bikes, Sky, HiArn, LoArn, Stand, Bales, Shadows,
Mud, ClrScrn, Collision, Vtx Wind, Hud.

**Structures**: include/structs.h declares the types several source files share; the named structures of the game's
subsystems are in include/structs/ (136 structs in 33 headers, which structs.h includes):

- the riders: rider.h (`Rider` and its parts, the skeletons, the sphere trees, the animations, the ponytail; the
  section below) and surface.h (`Surface`, the ground surfaces);
- the track and what is drawn on it: model.h (`PrimModel`, the compact models prim.c compiles; `TexSet`, texture
  sets), track.h (`TrackModelList` of sections, objects and water; `CollGrid`, a section's ground triangles;
  `WallMesh`, the wall mesh, which UsrTrk.c also builds for a user track; `SurfacePoly`; `Instance`), camera.h
  (`CamOut`, `CamMode`, the camera zones), particle.h, snow.h, decal.h (`TireTrack`), sun.h (`FlareList`), desert.h,
  shatter.h, palmod.h, map.h (`MapImage`), flash.h, arned.h (the editor's 3D view) and resman.h (`ResSet`, the
  resource sets);
- the racing line and the track editor: path.h (`PathNode`) and editor.h (`Editor`, `EditPiece`, `PieceType`,
  `EditArena`, the user track's layout and data);
- the front end: font.h (`Font`, `TextObj`), shell.h (`Screen`, `ScreenItem`, `Cheat`, `RiderBio`), gui.h (`Gui`,
  `GuiMenu`), hud.h (`RaceTexts`) and pak.h (`PakScreen`, `PakNote`, `PakMessage`);
- sound, the NES emulator and files: nes.h (`Nes`), softmix.h, sfx.h, tunes.h, snd.h (`RiderSound`) and file.h
  (`FileDirEntry`, `FileHandle`);
- the game and the race: race.h (`Event`, `TrackRecord`, `Season`, `Cup`, `TutorialLesson`, `PauseScreen`,
  `HudGlyphs`), game.h (`Game`, `GameState`, `SaveData`), hacks.h (`TrackHack`) and podium.h (the ceremony's clips).

## The rider struct

`gRiders`: 8 `Rider`s of 0x7E0 bytes, declared in include/structs/rider.h. Positions are in millimetres (s32) for
drawing and in metres (float) for the physics; s32 and s16 angles are 0x10000 per turn.

| offset | fields |
|---|---|
| 0x000 | `drawPos` bike render position; 0x00C `drawBodyPos` the thrown rider's |
| 0x018 | `drawRot` render angles (from `drawQuat`), 0x024 `rot` angles of the bike body; 0x030 `drawSteer`, 0x034 `heading`; 0x038/0x03C `drawSuspFront`/`drawSuspRear` suspension for rendering |
| 0x044 | `crashState`: 0 riding, 1 crashed, 2 rider clear of the bike; 0x045 `pendingCrash`: > 0 crash on landing, < 0 now (0x19 backwards, 0x1C forwards, 0x1F/0x22 sideways) |
| 0x048 | `bike` (`RigidBody`, 0x184 bytes): `pos`, `vel`, `acc`, `smoothPos` (extrapolated for far CPU riders), `quat` (w, x, y, z), `angVel`, `angAcc`, the matrix rows `right`, `up`, `fwd`, the sphere `tree`, 20 `contacts`, `mass`, `inertia`, `type` (0 human bike, 1 CPU bike, 2 crashed bike, 3 rider, 4 ball), `hitTimer`, `owner` |
| 0x1CC | `body`: the thrown rider, the same `RigidBody` (mass 90, type 3); the soccer ball is one too (`gSoccerBall`) |
| 0x350 | `sparkTimer`, `sparkDir`; 0x370 `drawQuat`; 0x38C `trickQuat`, `trickOffset`, `trickBlend` |
| 0x3AC | animation: `animFrame` (8.8), `animSpeed`, `animHold`, `holdTime`, `headYaw`; 0x3C4 `stuntScore`; 0x3C8 `stuntTotal` (stunt points banked) |
| 0x3D0 | `anim` (current trick or animation in `gRiderAnims`, -1 none), `prevAnim`, `trick`, `queuedTrick`, the stick history, `trickHist`, `trickList` |
| 0x3FE | `pipTimer` (stunt replay inset); 0x3FF `finishState`; 0x400 `celebrationTimer`; 0x402 `checkpoint`, 0x404 `checkpointDist` (desert) |
| 0x410 | `crestCrouch`, `wheelieBalance`, `wheelieTimer`, `crestAhead`, `leftFootOut`/`rightFootOut`, `powerSlide`, `hardTurn`, `noCollide` (ghost, respawn) |
| 0x420 | `speed`, `fwdSpeed`, `crashSpeedCap`, `laneOffset`, `localAngVel`, `crashVel`, `driveForce` (rear wheel), `timeScale`, `aiSkill` |
| 0x454, 0x4F8 | `rearWheel`, `frontWheel` (`Wheel`, 0xA4 bytes): `airTicks`, tire `force`, `springForce`, `suspLength`, `groundDist`, `bump`, `contact`, `brakeTorque`, ground `normal`, `latAxis`/`lonAxis`, `surface` (its `gSurfaceTable` entry), `spin`, `slip`, `grip` |
| 0x5A0 | `steer` (radians); 0x5A8/0x5AA `savedCamMode`/`camMode` (`gCamModes`) |
| 0x5B0 | `input`, `prevInput` (the input word above); `turboPressFrame`/`turboReleaseFrame`, `rHeld` (frames R held, or released if negative) |
| 0x5CC | `landingAssist`, `roughLanding`, `dirtSpray`; 0x5D0/0x5D2 `stickX`/`stickY`; input histories up to 0x61F |
| 0x620 | `blinkCount`, `blinkTimer`; 0x624 `crashTimer` |
| 0x628 | `segment` (path node), `lapDistLeft`, `raceDist`, `progressSpeed`, `lateral` (offset from the racing line), `segmentT` |
| 0x644 | `engine` (`RiderEngine`: rpm, turbo heat, torque, gear, throttle); 0x658/0x65C `bikeSkel`/`riderSkel` |
| 0x660 | `race` (`RiderRace`: lap, place, finishing place, gap, lap times, finish time); 0x690 `flags` (0x8 out of the race, 0x10 finished: brake automatically); 0x69C `timeLimit`; 0x6B2 `bestSegment` |
| 0x6E8 | `isCpu` (humans become CPU riders after the finish), `reversing`, `pathSearch`, `landingPick`, `lapAdjust`, `wrongWay` |
| 0x6F0 | landing prediction: `takeoffVelY`, `landingTime`, and two candidates (time, point, normal, path node) |
| 0x738 | `airPitch`, `airPitchInput`; 0x744 `airFrames` (negative on the ground); 0x748 `jumpState`, `takeoffPos`, `jumpDist` |
| 0x764 | `impactFlags`, `landingImpact`, `landingFrame`; 0x780 `surfaceIdx`; 0x784 `contactFrame[8]` (the last contact with each other rider) |
| 0x7A4 | `frozen`, `inactive`, `laneBlocked`, `standing` (index in `gRiderStandings`), `fistPump`, `gridSlot`, `laneChange`; 0x7AC `offTrackTimer`; 0x7B0 `spawnFrame` |
| 0x7B4 | handling stats from `sRiderStatsTable`: `weight` (collision mass scale), `landing`, `handling`, `turbo`, `airTurn`, `airFloat`, `turnRadius`, `power` |

What hangs off a rider: `bikeSkel` and `riderSkel` are copies of `gBikeSkelTemplate` (`BikeSkeleton`, 10 bones) and
`gRiderSkelTemplate` (`RiderSkeleton`, 15) made of 0x80-byte `Bone`s (parent, local position and rotation, world
position and rotation, angles); `rider_update_pose` fills `sRiderIkTargets` (`RiderIkTargets`: hands, feet, head)
for `rider_apply_ik`. A body's `tree` is a bounding-sphere tree of `CollideNode`s that collide.c builds from
`CollideSphereDesc` tables (`sBikeSphereTreeDesc`, `sRiderSphereDesc`). `gRiderAnims` (`RiderAnim`, indexed by
`anim`) gives each trick and animation its body and root `AnimClip`s, its button glyphs, name, riders, points and
announcer clip. The ponytail is a chain of four `PonytailNode`s per rider, in `gRiderGfx`. The ground surfaces the
wheels point at are `Surface`s (include/structs/surface.h): name, drag, grip, bumps, flags, three colours and a
rolling sound.

## Source files

Each original source file is one contiguous range of code. Files under src/game/unnamed/ have no file-name string;
their names say what they hold or where they start.

### Program and system

- **asm/game_entry.s**: the game image's entry; clears the BSS and calls `game_boot`.
- **unnamed/s1_80073200.c**: `game_boot` and `idle_thread`, which start the main thread.
- **main.c**: the main thread and loop (above), pad conversion, memory setup, the scheduler client thread, the
  development PI handle and debug leftovers.
- **unnamed/sched.c**: the scheduler thread (above) and VI mode switching.
- **unnamed/s0_8000CD00.c**: controllers, Rumble Paks and EEPROM over one SI message queue (`gSiMesgQueue`):
  controller reads into `gContPads`, rumble timers (`gRumbleTimers`), `eeprom_write_verify`.
- **unnamed/s0_8000EB00.c**: small helpers: string functions (`str_*`, named so that GCC does not treat them as its
  built-ins), `mem_fill`, `int_to_str`, `isqrt_u32`, `random_byte`, `fatal_halt` (`break 0x407`) and the
  stubbed-out `debug_printf` / `debug_log`. The boot loader links ten of them again (`boot_*`).
- **unnamed/s0_80011490.c**: maths. Angles are 16-bit, `gCosTable` has 1.0 = 0x4000, with atan and asin tables.
  `mtx3i_*` work on 2.14 fixed-point 3x3 matrices, `mtx3f_*` on float 3x3, `mtx_*` on the N64 `Mtx`; quaternions are
  (w, x, y, z); `math_to_polar` gives headings; `r250_init` seeds the R250 random generator.
- **unnamed/s0_80013D50.c**, **unnamed/s0_80065AF0.c**, **unnamed/s1_800A42A0.c**: one-function files that check
  the CIC or IPL3 (`cic_is_6103`, words of RDRAM the IPL3 leaves); nothing calls them.
- **unnamed/lib_800BC3D0.c**: the heap: first fit in 16-byte units, block headers {next, prev | serial | used,
  size, 0xB562439F - sum}. The `mem_*_dbg` entry points take `__FILE__` and `__LINE__`; those strings are where the
  source file names of this repository come from. It also has realloc, calloc, allocation at a fixed address (main
  reserves the framebuffers with it), a validator and the "Total Memory Used/Free" report (`mem_report`).
- **unnamed/lib_800BDCB0.c**: SWD decompression in C (`gSwdSrc` to `gSwdDst`), the same format as the boot
  loader's assembly version.
- **file.c** (FILE.C): the ROM file system: 32-byte directory entries {name[16], flags, size, offset, unpacked
  size}; files tagged 'sWd' are SWD-compressed per 8 KB page; four handles (`file_open`, `file_read`,
  `file_seek`, `file_tell`, `file_close`) and an eight-page LRU cache.
- **unnamed/lib_800BEC20.c**: PI DMA requests in eight slots (`sDmaRequests`; 0 free, 1 running, 2 queued,
  3 done); `gDmaIdleCallback` runs while `dma_wait` waits. The boot loader links four of these functions again.
- **unnamed/lib_800BF040.c**, **lib_800BF0E0.c**, **lib_800BF280.c**: copies of libultra's `osEPiStartDma`,
  `osPiGetCmdQueue`, a newer `osCartRomInit` and `osJamMesg`, compiled with SN's compiler (`dma_epi_start`,
  `dma_pi_get_cmd_queue`, `dma_cart_rom_init`, `dma_jam_mesg`).
- **unnamed/lib_800BF3C0.c**: an SRAM PI handle with 32 KB reads and writes (`sram_*`); never called.
- **resman.c** (../RESMAN.C): resource sets. A set is a "<name>.dir" of offsets (the byte order is detected), the
  ".BIG" data and ".gid" names. `res_load` relocates a resource, loads what it depends on and calls the handler
  registered for its three-letter type (`res_register_type`). The ROM's file system holds the sets "resource",
  "sfx", "guires", "pieces" and "nesres" with their .DIR, .big, .GID and .GDH files. Two of the headers keep the
  original names: RESOURCE.GDH declares the 56 ids of the "resource" set's groups as ints (`extern int
  GID_ArnGround;` ...): they are the game's `gGid*` variables (`GID_ArnGround` is `gGidArnGround`; `track_res_get`
  gives the current track's resource of a group), and sfx.GDH / sfx.GID name the sound clips (`smp_*`:
  `smp_Overtake`, `smp_Last_Rac`, `smp_Cliff_Fa`, ...), which the announcer and stream functions are named after
  (`announce_overtake`, `announce_race_start`, `tunes_play_cliff_fall`). The other .GDH files are empty.
- **decoder.c** (DECODER.C): decompresses "SAM" samples to companded 8-bit (`decoder_decode_adpcm`,
  `decoder_decode_sample`); the block decoders it calls are hand-written code among the data (asm/game_decoder.s,
  below).
- **fontcode.c** (FONTCODE.C): fonts packed into 40 IA8 pages of 64x64 glyphs. Text objects come from
  `font_text_create` / `font_text_add` {font, colour, flags and alignment, x, y, w, h, text}; `font_draw_text`
  queues glyph sprites per page and `font_render` emits them as texture rectangles.
- **unnamed/s0_80048230.c**: loads the race (3 fonts) and menu (5 fonts) font sets and sets up the RDP for text.

### Race

- **min.c**: the race game state: `race_init` / `race_update` / `race_exit`, the intro, the pause menus
  (`gPauseMenu`: pause, controller setup, options, quit confirmation, tutorial and test-ride menus), rumble,
  volumes, the surface table (`race_load_surface_table`) and replays: recording, playback, the time-trial ghost
  (`gGhostRace`, `gGhostSlot`; with the Mirror cheat, `gMirrorMode`, its recorded steering is flipped) and the
  tutorial demos.
- **event.c**: the rules of each event and the HUD. `event_update` dispatches on the mode (race, soccer, gates,
  hill climb, stunt, tutorial); `event_draw_hud_*` draw the matching HUDs from shared pieces (countdown images,
  position glyphs, the temperature gauge, a checkerboard wipe). The tutorial (`tutorial_*`) runs 17 lessons and
  tests from `gTutorialLessons`. HUD text uses fontcode.c text objects (`gHudTextObjs`).
- **hacks.c**: per-track code, not cheats. `sTrackHacks` lists `{track, init, exit, update, draw pass 1, draw
  pass 2, buffers, resources, resource names}` per track index. Lefty's Mill (3): instances bob on a sine and sink
  where riders cross them, probably floating logs. The Gravel Pit (13): two movers carry one of four random models
  along a line, probably conveyors with rocks; standing on one changes an ambient sound.
- **podium.c**: the end-of-season ceremony, run inside the race state (`gPodiumMode`). The podium is placed at a
  track point (special cases for Las Vegas, Detroit and Blizzard Blitz); bike poses, the camera path and a prop per
  rider are played back from recorded clips; it has its own music, and `gDesaturation` greys the scene at the end.
- **unnamed/s0_80062F50.c**: race positions (`race_sort_positions`, `gRiderStandings`), the announcer's position
  changes, rubber-banding through the CPU riders' time scale, lanes, the starting grid, and two extra riders
  (6 and 7) that appear and leave.
- **unnamed/s0_8005B5F0.c**: CPU riders' throttle and steering along the racing line, and the steering filter of
  human riders (`bike_update_steering`).
- **unnamed/s0_80061F80.c**: predicates that pick a route at branches of the racing line (by lap, at random,
  toggled).
- **unnamed/s0_8005B5E0.c**: one empty function.
- **path.c**: the racing line. `gPathNodes` (`gPathNodeCount` `PathNode`s of 0x6C bytes, include/structs/path.h): position, length 0x0C, maximum
  speed 0x10, lateral vector 0x14/0x18, curvature 0x1C, bank 0x20, slope 0x24, x/z spline coefficients
  0x2C-0x40; links next 0x46, branch 0x48, previous 0x4A, join 0x4C, alternative 0x4E; half-widths 0x58/0x5C,
  flags 0x64, speed tweak 0x65, lap progress 0x66. `path_init` loads it from the track's resource (`path_load`),
  from the user track (`gUserTrackData.path`) or, for Excite 3D, generates a straight line (`path_generate_straight`);
  corner speeds, braking limits and jump speeds are precomputed for the CPU riders. Per bike, `path_update_bike`
  tracks the segment, the lateral offset and progress, the lap (`path_check_lap` at `gPathFinishNode`), triggers and
  the respawn timer. `gPathStartNode` is the starting grid.
- **unnamed/replay.c**: the input recorder and player behind replays and Controller Pak ghosts: a 16-byte header
  (magic 0x7B, 24-bit checksum), then bit-packed blocks of 8 frames of input deltas.
- **unnamed/desert.c**: the desert mode: ten checkpoints (exported to fractal.c, which places a campfire at each),
  the arrow to the next one, laps and order, CPU steering, and moving far-away CPU riders directly.
- **unnamed/s0_8006B1A0.c**: Excite 3D: lanes, the CPU riders' lane changes and progress.
- **unnamed/s0_8006BFC0.c**: the soccer ball (`gSoccerBall`, a rigid body of type 4 with mass 125).
- **unnamed/s0_8006C510.c**: props (the soccer ball model, finish line and banners), the track intro screen and
  the congratulations screen, and the save data (`save_init_defaults`, `save_update_unlocks`, `save_load`,
  `save_write`). Its functions change subject at 0x8006EFB0, which may be the start of another file.
- **unnamed/s0_8006F620.c**: stunts: stick sequences matched against sorted tables, starting and animating a stunt,
  points (fewer for repeats).
- **unnamed/s0_800716C0.c**: ambient sound zones of a track (loaded, updated by position, cleared).

### Bikes and riders

- **bikenew.c**: the bike and rider simulation. `bikes_init` sets up the eight riders (skeleton buffers, sphere trees,
  grid places) and copies each rider's handling profile from `sRiderStatsTable` (8 profiles; debug names lightweight,
  fatboy, english jim, duke of dirt, tomboy, standard, trickster) into the rider's handling stats. `bikes_update` copies
  the input words into the riders and runs `bike_update` once per 60 Hz tick (CPU riders far from any view take one
  double step every other frame). A tick: input (`bike_process_input`), wheelie, jumps and air control, drive and
  brakes, tire constraints and traction, the sum of forces, integration, suspension (`bike_update_suspension`, wheel
  ground search `bike_wheel_find_ground`) and the render pose. Constants: gravity 14.7, spring 15000, damper 2200, bike
  mass 250, rider mass 90. `sBikeTune` is a debug tuning table (float, sldown, lane chg, rmp bst, landing, pitch,
  gravity, rate, damping, power); mode 6 uses its spring and damper.
- **unnamed/s0_800358C0.c**: the engine: rpm, the automatic gearbox, torque (`engine_torque_at_rpm`) and turbo heat
  (`engine_update_heat`; turbo, normal and overheated power, `sTurboTraction`).
- **collide.c**: bounding-sphere trees (40-byte `CollideNode`s from a 16000-byte pool) built from
  `CollideSphereDesc` {child, next, x, y, z, radius} descriptions. `collide_bodies` tests two rigid bodies
  (overlap, then the deepest pair of leaves), applies an impulse (`collide_apply_impulse`) and handles riders
  knocking each other down (`collide_resolve_rider_hit`), noting when each pair last touched (`contactFrame`).
- **unnamed/s0_800445B0.c**: crashes and rigid bodies: `bike_crash_begin` / `bike_crash_update`, `phys_body_step`
  (bikes, thrown riders and the soccer ball, with 11 contact points per bike), contact impulses and `bike_respawn`.
- **unnamed/s0_8003C8E0.c**: the rider's pose: arm and leg IK to the grips and pegs (`rider_ik_*`), head look,
  keyframe animations from `gRiderAnims` (tricks, PUMP, finish poses, crashes CB/CF/CL/CR).
- **skel.c**: skeletons: resources of 0x38-byte entries with child counts become 0x80-byte `Bone`s {parent,
  position, rotation, world position, world rotation, angles}; `skel_update_world_transforms` walks the hierarchy.
- **shadow.c**: each rider and each thrown body is drawn from above into a 32x32 IA8 texture (`sShadowTextures`,
  16 slots); ground triangles within a metre are clipped to a box and drawn with it (`shadow_draw`).
- **ponytail.c**: a four-node spring chain (gravity, wind from the rider's speed, length limits, a sine wobble)
  anchored on a rider bone, writing three bone matrices per frame; probably the tomboy rider's hair.

### Track, camera and effects

- **camera.c**: one camera per view (`gCamView`). The camera mode (`Rider.camMode`) indexes `gCamModes`, 12 entries
  {init, update(rider, s32 out[6]), exit, flags}: 0 soccer (sweeps around the ball), 1-3 chase near/mid/far (the
  only ones the Camera button cycles through; flag 8 excludes the others), 4 side view (Excite 3D), 6 hill climb,
  7 stunt replay in an inset view, 9 fall view, 10 finish celebration, 11 first person. The output {x, y, z,
  pitch, roll, yaw} goes through a 16-sample history and is smoothed (`cam_smooth_to`) or snapped (`cam_snap_to`)
  into `gCamPos`, `gCamPitch`, `gCamRoll` and `gCamYaw`, which rendering, particles, snow and the desert read.
  Pitch follows a spring (`sCamPitchSprings`); C-left (input bit 0x200) turns the chase camera to look back.
  Replays and demos use the track's camera zones (`cam_find_zone`, `gCamZoneState`). Post-passes keep the camera
  inside the track corridor, out of walls and at least 200 mm above the ground; `gCamSegFlags` & 0x40 marks covered
  areas, where rain and snow stop.
- **height.c**: ground and walls. Areas (`sHeightAreas`: mesh, bounding box, an 8x8 grid of triangle bitmasks) sit
  in a 16x16 world grid (`sHeightGrid`). `height_get_ground(x, y, z)` returns the height in mm and sets
  `gGroundFound`, `gGroundSurface` (triangle flags; bits 4-7 index `gSurfaceTable`; 0x400 marks overlapping or
  bridge triangles), `gGroundArea` and `gGroundTri`. Walls (`sWallMesh`) are vertical edges with a height plus
  triangles: `height_gather_walls` collects those in a box and `height_raycast_walls` tests a segment against them;
  `gWallEdgeFlags` per edge: 0x20 removed, 0x40 the camera collides, bits 3-4 removable group. On desert tracks
  the ground is the 512x512 `gDesertHeightMap` (2000 mm per cell, value << 6) and walls are boxes around objects.
- **sun.c**: sun and lens-flare records, projected with `project_to_screen` and hidden by reading back the Z buffer
  (`zbuffer_decode`); hexagonal flare rings and glare sprites, and 64 stars on night tracks.
- **unnamed/s0_80033E30.c**: the 2D sky backdrop (`sky_*`; `gSkyMode` chooses fog-colour fill, sky image or sky
  model) and the sun's screen position for sun.c.
- **fractal.c**: the desert's terrain, generated as you ride: 16 tiles (`gDesertTiles`) form a 4x4 window around
  the camera, each owning 128x128 bytes of the 512-wide height map. Work items run a few per frame: diamond-square
  subdivision (random generator `sFractalRand`), smoothing, object placement (up to eight per tile). Tile edges
  come from coordinate hashes so that neighbours match. The ten campfires (`gDesertFires`) are objects of kind 2;
  the target fire smokes (`fractal_emit_fire_smoke`).
- **instance.c**: trackside object instances from a resource {ids, items {x, y, z, rotation, display list}}, each
  binned to its nearest track section with a precomputed matrix; `instances_draw` draws them per section and view.
- **remove.c**: groups of removable walls (`sRemoveGroups`): lap and segment events of the racing line disable a
  group's wall edges and shatter its model.
- **snow.c**: 128 snowflakes in camera space (Blizzard Blitz, one view) and a flake hitting the lens.
- **mud2.c**: particles: a ring of 96 (0x24 bytes each) of types 0 dust, 1 mud clods, 2 sparks (points), 3 water
  and rain splashes, 5 sparkles, 6 campfire smoke, plus exhaust puffs; colours from the surface table. Rain on
  Rainforest Run (one player) is 64 screen-space streaks that move with the camera (`rain_draw`).
- **decal.c**: tire tracks: a ring of 128 quads laid under skidding bikes (and for 45 frames after leaving mud),
  drawn per area or desert tile with the surface colour and an alpha that fades with age. The first part of the
  file holds debug helpers whose printing is stubbed out (float formatting `debug_ftoa`, a spline recorder).

### Rendering

- **render.c**: `render_draw_race` draws a race frame. Per view: the perspective (fovy 33, far plane `gFarClip`),
  rider levels of detail (`gRiderViewLod`), the background (`gSkyMode`), the environment, the riders
  (`render_draw_riders`, opaque then translucent), then shadows, mud, decals, the HUD and the fade. `render_init`
  and `render_free` load and free everything per race and call the environment's own init and free by track type.
  Views: `gNumViews`, `gNumScreens`, `gViewRects` (scaled from `gViewLayouts`, in 512x240 units); `gSplitVertical`
  picks the two-player split; a small inset view with a black frame (camera mode 7) appears in one-player races
  while a rider's timer runs (`gInsetViewActive`). Rider palettes are rebuilt every frame
  (`render_update_rider_palette`: fog, terrain tint, overheat and invincibility flashes, lightning, the credits'
  sepia). It also draws 2D: images (`render_draw_image`, used by the HUD, menus and editor), the temperature gauge,
  place markers, the sound and controller option screens, and the credits (`gCreditsMode`). A race frame starts
  with the task's F3DLX2.Rej microcode (the opaque riders, then with clip ratio 2 the far track and pass 1 of the
  track hacks); `render_load_ucode` switches to F3DEX2.NoN (clip ratio 1) for the near track and what follows, and
  back to F3DLX2.Rej for translucent riders. The editor (edit.c) draws its views with F3DEX2.NoN.
- **prim.c**: models. `prim_build_dl` compiles the compact model command stream (vertices, triangles, texture and
  TLUT loads, bone matrices) into display lists (the F3DEX2 GBI, which both microcodes read); it also relocates
  "PKG" and "N64" resources, flips images and animates textures.
- **arngfx.c**: stadium tracks (`arn_*`): model lists `gTrackSections`, `gTrackObjects` and `gWaterModels`, entries
  of 0x20 bytes {model, x0, z0, x1, z1, ..., flags, view mask}, culled by distance and angle (`arn_cull_models`).
- **tubegfx.c**: outdoor tracks (`tube_*`): culling by PVS lists, per-object matrices from a pool of 300, animated
  texture banks, water height (`tube_get_water_level`) and splashes.
- **desgfx.c**: the desert (`des_*`): 250 cached 5x5-vertex patches of the height map in three rings of detail
  with 16 edge-stitching display lists, nearby objects, the arrow to the next checkpoint and a marker over it.
- **map.c**: the one-player minimap built from the racing line, or the split-screen progress bar, with rider dots.
- **palmod.c**: palettes registered to be rewritten each frame: lightning on Rainforest Run, the podium's
  desaturation (`gDesaturation`), the credits' sepia, outdoor brightness.
- **flash.c**: camera flashes in the stands: particles on visible stand triangles while a timer runs, started when
  the human rider lands a jump.
- **shatter.c**: breakable outdoor objects burst into triangle debris when a rider rides through them (or remove.c
  breaks them); the object is hidden after 181 frames.
- **unnamed/s0_80072F70.c**: `screen_draw_wipe`, a screen transition; its file name is unknown.

### Front end

- **shell.c**: the front-end game state: splash screens, title, menus, options, rider select, records, cheat and
  initials entry, race results, season standings, and the Controller Pak screen (drawn by pak.c). `sScreens` holds
  24 screens of 0x2C bytes {title, left/right sibling screens, items {text, target, flags (1 locked), callback},
  flags, first picture, first track, type, item count, background, cursor, back screen}; the type selects the input
  handler and drawing (0 title, 2 rider select, 3 list, 4 season round, 6 options, 7 season race result, 8 round
  over, 9 cheat entry, 11 track list, 12 Controller Pak, 13 race result, 14 standings, 18 records, 19 controller
  settings, 20 initials, 21-25 boot splashes and notices). `shell_goto_screen` runs the chosen item's callback
  (which sets the game mode) and changes `sCurScreen`. `snd_play_menu_sound` plays 0 move, 1 select, 2 back,
  3 error.
- **pak.c**: the Controller Pak screen (manager, load, save of ghost and user-track notes) and the Pfs calls behind
  it. The shell fills `gPakScreen` (a `PakScreen`: mode 0 manager / 1 load / 2 save, state, pak, selection, data,
  pages, validator, describe callback) and polls `gPakScreenDone` / `gPakScreenAborted`. Notes are created with game code 'NMXG', company
  "01", name "EB64" and extensions 'A'-'Z', and read back to verify; names go through `sPakCharset`. Probing
  (`pak_probe_all`) also finds Rumble Paks, Transfer Paks and the EEPROM.
- **gui.c**: the track editor's dialog kit (`gGui`): one-line message boxes, two-choice prompts and menus
  (`GuiMenu`: title, items, cursor, choice; `GuiMenuItem`: text, flags and a row of values).

### Track editor

- **edit.c**, **arnedgfx.c**, **UsrTrk.c**: see [Track editor](#track-editor).

### NES emulator and sound

- **nesn64.c**: see [NES emulator](#nes-emulator).
- **unnamed/s0_8000F060.c**, **sfx.c**, **softmix.c**, **n64snd.c**, **tunes.c**: see [Sound](#sound).

## NES emulator

nesn64.c runs the original Excitebike (game state 3, game mode 12). `nes_init` loads the NES ROM from the "nesres"
resource set, puts PRG at $8000 and $C000 and CHR above the CPU space, decodes the CHR into CI4 tiles
(`sNesChrTiles`) and converts the 64-colour palette to RGBA5551. The emulated machine lives in a fixed block at
0x80380000:

| offset | contents |
|---|---|
| +0x00000 | CPU address space |
| +0x10000 | PPU memory |
| +0x14000 | OAM |
| +0x14100 | CPU state: PC (u16), A, X, Y, S, flag bytes N V B D I Z C (+0x14106-+0x1410C), cycle counter (+0x14110), $2006 address (+0x14114), latch and scroll (+0x14118) |
| +0x14120 | joypad byte, then four APU channel records of 0x24 bytes (+0x1412C) |

- **CPU**: `nes_run_frame` runs 248 lines of 120 cycles. `sNesOpcodeTable` (continued by `sNesOpcodeTableTail`) has
  0x18-byte entries {mnemonic, unused stub, addressing-mode function, operation function, cycles << 16 | length}.
  The addressing modes (`nes_am_*`, `nes_ea_*` for stores) leave the effective address in `sNesEffAddr` (-1 = the
  accumulator) and the value in `sNesOperand`; the operations (`nes_op_*`) use them. SED halts: there is no
  decimal mode. The NMI fires at line 240 when $2000 bit 7 is set, and also on the sprite-0 line when $2000 bit 6
  is set, a shortcut for the status-bar split.
- **Video**: every 8 lines `sNesRowCallback` (`nes_draw_bg_row`) emits texture rectangles for one row of the name
  table with the X scroll. Sprites in front of the background are drawn by the RDP (`nes_draw_sprites`); sprites
  behind it are drawn into the framebuffer by the CPU after the frame (`nes_draw_back_sprites`, from main.c).
- **Audio**: writes to the APU registers update `nes_apu_update_square`, `nes_apu_update_triangle` and
  `nes_apu_update_noise`; `nes_apu_render` synthesizes 22050 Hz samples into two audio buffers of its own while
  `gNesActive` keeps the game's sound system off.
- **Design mode SAVE and LOAD**: the original saved designed tracks to the Famicom Data Recorder. When the 6502 PC
  reaches 0xC661/0xC66A (SAVE) or 0xC641/0xC69A (LOAD), `nes_draw_frame` copies the 0x220-byte track at NES RAM
  $05E0 to or from the save block (`gSaveData.nesTrack`); 60 frames later (`sNesRestoreTimer`) the snapshot
  `sNesSnapshot` is restored.
- Holding L and Z leaves the emulator; "HOLD L AND Z BUTTONS TO EXIT" blinks in the name table.

## Sound

The layers, top to bottom:

1. **Game side** (unnamed/s0_8000F060.c, `snd_*`): init and per-frame service, loading and unloading the race
   sounds, music, effects and speech volumes and the master volume (`gSndMasterVolume`), menu sounds, impacts, and
   an engine sound per rider with Doppler (`sRiderSounds`).
2. **sfx.c** (../SFX.C): 256 reference-counted samples and 128 sounds in a priority-sorted list with handles; voice
   allocation, streams and echo. It reaches the mixer only through a driver table, `gSfxDriver`.
3. **n64snd.c**: the N64 driver behind that table (`gN64SndDriver`, 29 slots: capabilities, init, shutdown, sample
   load/create/free/info/write, voice sample/rate/volume/pan/position/flags, clocks, callback, echo, mute).
   `n64snd_update` keeps two audio buffers: for each half buffer it calls the pre-mix callback (`sN64SndCallback`), the
   mixer, the post-mix callback (`gN64SndMixCallback`, set by tunes.c), converts the mix and queues it with
   `osAiSetNextBuffer`. Mode bits: 1 stereo, 2 16-bit; at most 22050 Hz.
4. **softmix.c** (../softmix.c): a software mixer of up to 8 voices at 22050 Hz (voices of 0x30 bytes, 256 sample
   slots of 0x1C bytes, an echo unit), with samples loaded by name from the "SAM" resources of the "SFX" set
   (`gSfxResSet`). The inner loops are hand-written code (asm/game_softmix.s, below).

**tunes.c** streams music, the announcer and the crowd from "BIG.RAW" in a 4-bit ADPCM (IMA ADPCM's step sizes,
four predictors). Four channels of 0x4C bytes (`sStreamChannels`): 0-1 music, 2 announcer, 3 ambience and crowd.
Each loads 8 blocks (0x44 bytes, 128 samples each) per 0x220-byte PI DMA and decodes them with
`tunes_decode_adpcm_block` (hand-written code, below); `tunes_mix_streams` mixes them, optionally at half rate
with interpolation and an echo ring, into a 2048-sample ring (`sStreamMixBuf`) that `tunes_mix_callback` adds to
the n64snd output.

- Music: a song is a table from keys to samples plus events {u16 time, u8 note (0xFF stop, 0xFE loop), u8 channel}
  that start whole sampled phrases on channels 0 and 1 (`tunes_play_song`, `tunes_update_song`).
- Announcer: an 8-entry queue (`sSpeechQueue`) played one clip at a time by `tunes_update_speech`; negative entries
  are "Nth place" clips resolved when played. The `announce_*` functions build phrases from rider names (6 riders x
  4 intonations, `sRiderNameClips`) and event clips (crash, takes the lead, final lap, ...), with per-rider
  cooldowns (`gAnnounceTimes`); there is no announcer in soccer and the tutorial.
- Channel 3: looping positional ambience from the track's emitters (`tunes_set_ambient`, driven by
  unnamed/s0_800716C0.c) and crowd reactions delayed by 30 frames (`sCrowdQueue`).
- Volumes: `gStreamVolumes[4]` (music, music, `gSpeechVolume`, `gAmbientVolume`) times the master volume.

**Code among the data**: some sound routines are hand-written MIPS code that the linker placed in the data
section, three asm modules at the end of the game image (0x800DB550-0x800DB8B8 and, after the mixer state,
0x800DB8E0-0x800DC098):

- asm/game_tunes.s: tunes.c's block decoder (`tunes_decode_adpcm_block`) and its tables (`sTunesAdpcmIndexTable`,
  `sTunesAdpcmStepTable`). A block has a 5-byte header (two s16 samples, the first one's low 2 bits the predictor,
  and the step index), then 126 signed 4-bit codes, high nibble first. A sample is the prediction from the last
  two samples (0, s1, (3 * s1 - s2) >> 1 or 2 * s1 - s2) plus code times IMA ADPCM's step size, clamped; the code's
  magnitude moves the step index (down 1 for 0-3, up 2 to 10 for 4-8), which has no upper bound: the step table
  repeats 32767 past IMA's 89 entries.
- asm/game_softmix.s: softmix.c's voice mixers (`softmix_mix_voices_mono`, `softmix_mix_voices_stereo`) and output
  converters (`softmix_output_8bit`, `softmix_output_16bit`). They take no arguments: softmix.c fills the mixer
  state just before them in the data (`sMixVoices`, `sMixNumVoices`, `sMixCount`, `sMixSrc` the s32 mix buffer,
  `sMixDst`, `sMixVolumeTable`, `sMixBias`, `sMixClampMin` / `sMixClampMax`, `sMixShift`, `sMixOverwrite`). A voice
  steps through 8-bit sample data at a 32.32 fixed-point rate and loops or stops at its end; each sample byte
  indexes the voice's volume row of `sMixVolumeTable` (32 rows of 256 s32) and is added to the mix buffer (the
  first voice stores instead, and the buffer is cleared when none plays). The converters shift the mix down
  (and clamp it for 16-bit output).
- asm/game_decoder.s: decoder.c's block decoders, IMA ADPCM (`decoder_decode_ima_block`, for sample type 3) and
  Microsoft ADPCM (`decoder_decode_msadpcm_block`, types 4 and 5, with 2-byte block headers), and their standard
  tables (`sDecoderImaIndexTable`, `sDecoderImaStepTable`, `sDecoderMsAdpcmCoefs`, `sDecoderMsAdpcmAdaptTable`).

## Saves

`gSaveData` (`SaveData`, include/structs/game.h) is a 0x804-byte image: a magic word (0x5748FB3A) and a checksum (xor 0xFCB1121D), the records of 24
tracks (best times and initials, "?????" by default), season progress (+0x2BF rounds done, +0x2C0 highest difficulty
reached), options (+0x2C4: 1 stereo, 2 high resolution, 0x10 drones off; +0x2C5-0x2C7 music, effects and speech
volumes), tutorial progress (+0x2C1; 7 unlocks Original Excitebike), the NES design track (+0x2D4, 0x220 bytes,
`gSaveData.nesTrack`) and the packed user track (+0x4F4, up to 0x310 bytes). `save_init_defaults` fills it,
`save_load` applies it (records, controller layout, volumes, unlocked tracks in `gUnlockedTracks`), and
`save_write` is called with a section number after records, settings or the user track change.

The Controller Pak holds time-trial ghosts (unnamed/replay.c's format, 0x2800 bytes) and user tracks (pak.c).
EEPROM code exists (`pak_probe_all` probes it, `eeprom_write_verify` writes with read-back) but this build does
not use it (see below).

## Track editor

**edit.c** is game state 2 (`edit_init` / `edit_update` / `edit_draw` / `edit_exit`). Its state, `gEditor`
(an `Editor` of 0x1810 bytes; the editor's structs are in include/structs/editor.h):

- 0x16C menu page (0 main, 1 edit); 0x170 mode (0 idle, 4 move track, 6 user piece heights, 7/8 name entry,
  9 placing a piece, 10/11 warnings); 0x174 flags (1 reverse, 8 flip, 0x10 exit, 0x400 test track, 0x800 message
  box, 0x1000 OK/Cancel dialog);
- 0x17C/0x180/0x184 first, last and current piece, 0x190 start piece, 0x194 a pool of 150 piece nodes of 0x24
  bytes, 0x16AC grid size (15x15, `gEditArena`), 0x16BA name[10];
- 0x16E0/0x16E4/0x16E8 menu, pending menu, slide state; 0x16F4 user piece count; 0x16F8/0x16FC selected height
  point and user piece; 0x1720 the "Pieces" resource set; 0x172C/0x1738 two cameras {distance, pitch, height};
- 0x17F8 dirt type; 0x17FC/0x17FE dialog and choice; 0x1800 message.

A piece node (`EditPiece`): used, next 0x04, previous 0x08, cell x/y 0x0C/0x0E, flags 0x10 (2 flipped, 4 preview, 8 visible),
rotation 0x12, type 0x14, then the rotated connector directions, offsets and size. `gPieceTypes` has 29 `PieceType`s of
0x40 bytes {size[2], connector direction[2], connector offset[2], flags 0xC, name 0x1C, resource ids 0x20-0x2C, mesh
sets 0x30/0x34, path points 0x38, collision 0x3C}: 0-1 straights, 2-5 turns, 6-17 jumps, 18-25 User Piece 1-8,
26 New User Piece, 27 empty ground, 28 the user piece template. Type flags: 2 can start the track, 0x10/0x20 fits
/ fits flipped, 0x40 user piece, 0x80 variable length, 0x100 flippable, 0x200 size locked, 0x400 used twice.
`edit_update_fitting_pieces` works out which pieces fit at the open end (a 30x30 occupancy grid), `edit_insert_piece`
connects one, and `edit_try_close_loop` closes the circuit ("Track Complete!"). User pieces have height profiles
(`gUserPieceHeights`, s16[8][15]) edited in mode 6. Camera moves are eased over 35 frames. Menus are gui.c's.

**arnedgfx.c** draws the editor's 3D view: a grid of 500-unit cells with rotated piece vertices, a checkerboard
floor, the arena walls and the cursor, plus the camera, viewport and scissor setters edit.c uses.

**UsrTrk.c** converts user tracks:

- `usrtrk_pack_track` packs one (at most 0x310 bytes): 0x35, dirt type, name[10], piece count, current piece, user
  piece count, per user piece a segment count and heights, then 3 bytes per piece (x 5 bits, y 5, flags 6, rotation
  2, type 6). `usrtrk_load_layout` unpacks it into `gUserTrackLayout` (`UserTrackLayout`), `usrtrk_load_into_editor` into `gEditor`.
- For a race, `usrtrk_load_pieces` and `usrtrk_build_track` (called from the stadium code) fill `gUserTrackData` (`UserTrackData`):
  two mesh sets of 500x500 cells with rotated vertices and 1-bit textures, the dirt texture, the racing line
  (`gUserTrackData.path`, header 0xAA000003) and the arena's and the pieces' wall meshes merged into one `WallMesh`
  with its 16x16 hash (allocated one cell short of the struct: the cell lists also set cells[256], past the end).

## The LodgeNet build

This is the version LodgeNet ran on hotel-room N64s. What the code shows:

- **Nothing is saved**: `save_write` only copies the tutorial progress inside `gSaveData`; every save the game
  makes (records, options, season progress, the editor's "Overwrite Track On Game Pak", the NES design track)
  reaches it and stops there, and no code writes the EEPROM (`eeprom_write_verify` is never called). The save image
  lives in RAM only.
- **Menu items disabled**: "Reset Saved Data" and "Controller Pak" in the options are always greyed out and play
  the error sound.
- **Development leftovers** in this build: the SN Systems debug runtime and "output.log" writer in the boot
  loader, the D-pad freeze, the lag generator and the screenshot dump in the main loop, the debug menu
  (`gDrawFlags`), the bike tuning table (`sBikeTune`) and the unreferenced CIC/IPL3 checks.
