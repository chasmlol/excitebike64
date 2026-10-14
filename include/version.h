/* The versions this tree builds. The build passes VERSION, one of these values, to the compiler (and to the
   assembler), from the version it builds (tools/common.py; versions/<name>/ holds each version's tables): the C
   tests it with `#if VERSION == VERSION_US_REV1`, `#if VERSION != VERSION_LODGENET` and the like. Each version
   directory needs its VERSION_<NAME> here; the numbers only have to differ. */
#ifndef VERSION_H
#define VERSION_H

#define VERSION_LODGENET 1  /* Excitebike 64 (USA) (LodgeNet): the hotel build, the reference */
#define VERSION_US_REV1 2   /* Excitebike 64 (USA) (Rev 1) */
#define VERSION_EU 3        /* Excitebike 64 (Europe): the PAL conversion of USA Rev 1 */

#ifndef VERSION
#error "VERSION is not defined: build with the tools (tools/common.py passes -DVERSION=...)"
#endif

/* A line number the code passes (__LINE__ of the original source, to the debug allocator), where lines were added or
   removed above it between versions: SRC_LINE(LodgeNet's, USA Rev 1's). */
#if VERSION == VERSION_LODGENET
#define SRC_LINE(lodgenet, us_rev1) (lodgenet)
#elif VERSION == VERSION_US_REV1 || VERSION == VERSION_EU
#define SRC_LINE(lodgenet, us_rev1) (us_rev1)
#else
#error "SRC_LINE: no line numbers for this version"
#endif

/* A RESOURCE set id (the set of models, textures, animations, maps...), written with LodgeNet's number: the retail
   versions dropped 17 resources LodgeNet has (its 74, 82, 86, 88, 94, 151, 191, 193, 202, 208, 212, 331, 578, 581,
   585, 611 and 614), which renumbers every id after them. tools/version_data.py reads these definitions. */
#if VERSION == VERSION_LODGENET
#define RES(id) (id)
#elif VERSION == VERSION_US_REV1 || VERSION == VERSION_EU
#define RES(id) ((id) - ((id) > 74) - ((id) > 82) - ((id) > 86) - ((id) > 88) - ((id) > 94) - ((id) > 151) \
                  - ((id) > 191) - ((id) > 193) - ((id) > 202) - ((id) > 208) - ((id) > 212) - ((id) > 331) \
                  - ((id) > 578) - ((id) > 581) - ((id) > 585) - ((id) > 611) - ((id) > 614))
#else
#error "RES: no resource numbering for this version"
#endif

#endif
