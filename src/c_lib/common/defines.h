#pragma once

typedef unsigned int GLuint;

#ifdef __WIN32__
# ifndef uint
#  define uint unsigned int
# endif
#endif

#ifdef DC_CLIENT
# define STATE ClientState
#else
# define STATE ServerState
#endif

#ifndef PRODUCTION
# define  PRODUCTION 0
#endif

#ifndef PRODUCTION_DEV
# define  PRODUCTION_DEV 0
#endif

#ifdef __MSVC__
# include <float.h>
# define isfinite( x ) ( _finite( x ) && x==x )  
float cbrt(float arg) { return powf(arg, 1.0f/3.0f); }
static int S_IRWXU = 0;
#endif
///*
  //Turn off GS_ASSERTs when production is enabled
//*/
//#if PRODUCTION
    //#define NDEBUG 
//#endif

//Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define GNOMESCROLL_DSO_IMPORT __declspec(dllimport)
  #define GNOMESCROLL_DSO_EXPORT __declspec(dllexport)
  #define GNOMESCROLL_DSO_LOCAL
#else
  #if __GNUC__ >= 4
    #define GNOMESCROLL_DSO_IMPORT __attribute__ ((visibility ("default")))
    #define GNOMESCROLL_DSO_EXPORT __attribute__ ((visibility ("default")))
    #define GNOMESCROLL_DSO_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define GNOMESCROLL_DSO_IMPORT
    #define GNOMESCROLL_DSO_EXPORT
    #define GNOMESCROLL_DSO_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define GNOMESCROLL_API and GNOMESCROLL_LOCAL.
// GNOMESCROLL_API is used for the public API symbols. It either DSO imports or DSO exports (or does nothing for static build)
// GNOMESCROLL_LOCAL is used for non-api symbols.

#ifdef GNOMESCROLL_DSO // defined if GNOMESCROLL is compiled as a DSO
  #ifdef GNOMESCROLL_DSO_EXPORTS // defined if we are building the GNOMESCROLL DSO (instead of using it)
    #define GNOMESCROLL_API GNOMESCROLL_DSO_EXPORT
  #else
    #define GNOMESCROLL_API GNOMESCROLL_DSO_IMPORT
  #endif
  #define GNOMESCROLL_LOCAL GNOMESCROLL_DSO_LOCAL
#else // GNOMESCROLL_DSO is not defined: this means GNOMESCROLL is a static lib.
  #define GNOMESCROLL_API
  #define GNOMESCROLL_LOCAL
#endif

#ifdef __GNUC__
#define LUA_EXPORT GNOMESCROLL_API
#endif

#ifdef __MSVC__
#define LUA_EXPORT __declspec(dllexport)
#endif

#ifdef _WIN32
# define LLD_FMT "%I64d"
#else
# define LLD_FMT "%lld"
#endif

const int GAME_OBJECTS_MAX = 4096 * 4;
const float TICK_DURATION = 30.0f;
const int PLAYERS_MAX = 32;

#define DAT_NAME_MAX_LENGTH 24
#define DATA_PATH "./data/"
#define SCREENSHOT_PATH "./screenshot/"
