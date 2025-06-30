/***************************************************************************
Bourriquet MorphOS
 Copyright (C) 2025 digitally
  
***************************************************************************/
#include <exec/types.h>
#include "SDI_compiler.h"
#include "Global.h"
#include "Stringsizes.h"
#include "revision.h"

// stack cookie for shell v45+
static const char USED_VAR bourriquet_stack_size[] = "$STACK:" STR(SIZE_STACK) "\n";

// some platform/compiler dependent stack and memory definitions.
#if defined(__amigaos4__)
  long __stack_size = SIZE_STACK;        // set the minimum startup stack for clib2
  long __default_pool_size = 128*1024;   // set the pool & puddle size for the
  long __default_puddle_size = 32*1024;  // AllocPool() functions to something more reasonable.
#elif defined(__SASC) || defined(__GNUC__)
  #if defined(__libnix__) || defined(__SASC)
  // GCC (libnix) supports the same as SAS/C!
  long NEAR __stack = SIZE_STACK;
  long NEAR __buffsize = 8192;
  long NEAR _MSTEP = 16384;
  #else
  long __stack_size = SIZE_STACK;    // set the minimum startup stack for clib2
  #endif
#elif defined(__VBCC__) /* starting with VBCC 0.8 release */
  long __stack = SIZE_STACK;
#else
  #error "la sp�cification de la pile/m�moire a �chou�"
#endif

#if defined(__SASC)
  // define the __AMIGA__ symbol if it is yet unknown
  #if !defined(__AMIGA__)
    #define __AMIGA__
  #endif
#endif

// identify the system we are compiling for
#if defined(__amigaos4__)
  #define SYSTEM      "AmigaOS4"
  #define SYSTEMSHORT "OS4"
#elif defined(__MORPHOS__)
  #define SYSTEM      "MorphOS"
  #define SYSTEMSHORT "MOS"
#elif defined(__AROS__)
  #define SYSTEM      "AROS"
  #define SYSTEMSHORT SYSTEM
#elif defined(__AMIGA__)
  #define SYSTEM      "AmigaOS3"
  #define SYSTEMSHORT "OS3"
#else
  #warning "Unsupported System - check SYSTEM define"
  #define SYSTEM      "???"
  #define SYSTEMSHORT "???"
#endif

// identify the CPU model
#if defined(__PPC__) || defined(__powerpc__)
  #define CPU "PPC"
#elif defined(_M68060) || defined(__M68060) || defined(__mc68060)
  #define CPU "m68060"
#elif defined(_M68040) || defined(__M68040) || defined(__mc68040)
  #define CPU "m68040"
#elif defined(_M68030) || defined(__M68030) || defined(__mc68030)
  #define CPU "m68030"
#elif defined(_M68020) || defined(__M68020) || defined(__mc68020)
  #define CPU "m68k"
#elif defined(_M68000) || defined(__M68000) || defined(__mc68000)
  #define CPU "m68000"
#elif defined(__i386__)
  #define CPU "x86"
#elif defined(__x86_64__)
  #define CPU "x86_64"
#elif defined(__arm__)
  #define CPU "ARM"
#else
  #warning "Unsupported CPU model - check CPU define"
  #define CPU "???"
#endif

// for defining the actual version of Bourriquet and mapping it
// to constant variables.
#define __BOURRIQUET "BOURRIQUET"
#define __BOURRIQUET_VERSION   "0.2"
#ifndef __BOURRIQUET_DEVEL
#define __BOURRIQUET_DEVEL     "-dev"
#endif
#define __BOURRIQUET_COPYRIGHT     "Copyright (C) 2025 Digitally"
#define __BOURRIQUET_FULLCOPYRIGHT "Copyright (C) 2025 Digitally\n" __BOURRIQUET_COPYRIGHT

// find out something about the compiler used
#if defined(__GNUC__)
  #if defined(__GNUC_PATCHLEVEL__)
    #define __BOURRIQUET_COMPILER "GCC " STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
  #else
    #define __BOURRIQUET_COMPILER "GCC " STR(__GNUC__) "." STR(__GNUC_MINOR__) ".x"
  #endif
#endif

// __BOURRIQUET_BUILDID is 0 for non-nightly builds
#if !defined(__BOURRIQUET_BUILDID)
const char * const bourriquetbuildid       = "0.00";
#else
const char * const bourriquetbuildid       = STR(__BOURRIQUET_BUILDID);
#endif

#define GIT_REVSTR "0.01"

const char * const bourriquetver           = __BOURRIQUET_VERSION __BOURRIQUET_DEVEL;
const char * const bourriquetversion       = __BOURRIQUET " " __BOURRIQUET_VERSION __BOURRIQUET_DEVEL " [" SYSTEMSHORT "/" CPU ", " GIT_REVSTR ", " __BOURRIQUET_COMPILER "]";
const char * const bourriquetversionver    = __BOURRIQUET_VERSION __BOURRIQUET_DEVEL " [" SYSTEMSHORT "/" CPU "]";
const char * const bourriquetversionstring = __BOURRIQUET " " __BOURRIQUET_VERSION __BOURRIQUET_DEVEL " (" __BOURRIQUET_VERDATE ") " __BOURRIQUET_COPYRIGHT " [" SYSTEMSHORT "/" CPU ", " GIT_REVSTR "]";
const char * const bourriquetuseragent     = __BOURRIQUET "/" __BOURRIQUET_VERSION __BOURRIQUET_DEVEL " (" SYSTEM "; " CPU "; rv:" __BOURRIQUET_BUILDDATE "-" GIT_REVSTR ")";
const char * const bourriquetcopyright     = __BOURRIQUET_COPYRIGHT;
const char * const bourriquetfullcopyright = __BOURRIQUET_FULLCOPYRIGHT;
const char * const bourriquetversiondate   = __BOURRIQUET_VERDATE;
const char * const bourriquetcompiler      = __BOURRIQUET_COMPILER;
const char * const bourriqueturl           = " ";
const unsigned long bourriquetversiondays  = __BOURRIQUET_VERDAYS;

/* no longer external visible, this is done by proto files! */
struct Library* DataTypesBase     = NULL;
struct Library* IconBase          = NULL;
struct Library* IFFParseBase      = NULL;
struct Library* IntuitionBase     = NULL;
struct Library* KeymapBase        = NULL;
struct Library* LocaleBase        = NULL;
struct Library* MUIMasterBase     = NULL;
struct Library* OpenURLBase       = NULL;
struct Library* RexxSysBase       = NULL;
struct Library* WorkbenchBase     = NULL;
struct Device*  TimerBase         = NULL;
struct Library* GfxBase           = NULL;
struct Library* LayersBase        = NULL;
struct Library* DiskfontBase      = NULL;
struct Library* CodesetsBase      = NULL;
struct Library* CyberGfxBase      = NULL;
struct Library* ExpatBase         = NULL;
struct Library* UtilityBase       = NULL;

struct WBStartup *WBmsg = NULL;

const char* const wdays[7] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
const char* const months[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
const char* const SecCodes[5] = { "none","sign","encrypt","sign+encrypt","standard" };

