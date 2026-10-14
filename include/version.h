/* The versions this tree builds. The build passes VERSION, one of these values, to the compiler (and to the
   assembler), from the version it builds (tools/common.py; versions/<name>/ holds each version's tables): the C
   tests it with `#if VERSION == VERSION_US_REV1`, `#if VERSION != VERSION_LODGENET` and the like. Each version
   directory needs its VERSION_<NAME> here; the numbers only have to differ. */
#ifndef VERSION_H
#define VERSION_H

#define VERSION_LODGENET 1  /* Excitebike 64 (USA) (LodgeNet): the hotel build, the reference */
#define VERSION_US_REV1 2   /* Excitebike 64 (USA) (Rev 1) */

#ifndef VERSION
#error "VERSION is not defined: build with the tools (tools/common.py passes -DVERSION=...)"
#endif

#endif
