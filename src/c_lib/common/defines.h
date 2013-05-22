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
# include <math.h>
# define isfinite(x) (_finite((x)) && (x) == (x))
float cbrt(float arg) { return powf(arg, 1.0f/3.0f); }
static int S_IRWXU = 0;

# define M_PI       3.14159265358979323846  /* pi */
# define M_PI_2     1.57079632679489661923  /* pi/2 */
# define M_PI_4     0.78539816339744830962  /* pi/4 */
# define M_PIl      3.1415926535897932384626433832795029L  /* pi */
# define M_PI_2l    1.5707963267948966192313216916397514L  /* pi/2 */
# define M_PI_4l    0.7853981633974483096156608458198757L  /* pi/4 */

// We must not do 'num + 0.5' or 'num - 0.5' because they can cause precision loss.
static double round(double num)
{
    double integer = ceil(num);
    if (num > 0)
        return (integer - num > 0.5) ? (integer - 1.0) : integer;
    return (integer - num >= 0.5) ? (integer - 1.0) : integer;
}
static float roundf(float num)
{
    float integer = ceilf(num);
    if (num > 0)
        return (integer - num > 0.5f) ? (integer - 1.0f) : integer;
    return (integer - num >= 0.5f) ? (integer - 1.0f) : integer;
}
inline long long llround(double num) { return static_cast<long long>(round(num)); }
inline long long llroundf(float num) { return static_cast<long long>(roundf(num)); }
inline long lround(double num) { return static_cast<long>(round(num)); }
inline long lroundf(float num) { return static_cast<long>(roundf(num)); }
inline double trunc(double num) { return num > 0 ? floor(num) : ceil(num); }
#endif
///*
  //Turn off GS_ASSERTs when production is enabled
//*/
//#if PRODUCTION
    //#define NDEBUG
//#endif

//Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
# define GNOMESCROLL_DSO_IMPORT __declspec(dllimport)
# define GNOMESCROLL_DSO_EXPORT __declspec(dllexport)
# define GNOMESCROLL_DSO_LOCAL
#else
# if __GNUC__ >= 4
#  define GNOMESCROLL_DSO_IMPORT __attribute__ ((visibility ("default")))
#  define GNOMESCROLL_DSO_EXPORT __attribute__ ((visibility ("default")))
#  define GNOMESCROLL_DSO_LOCAL  __attribute__ ((visibility ("hidden")))
# else
#  define GNOMESCROLL_DSO_IMPORT
#  define GNOMESCROLL_DSO_EXPORT
#  define GNOMESCROLL_DSO_LOCAL
# endif
#endif

// Now we use the generic helper definitions above to define GNOMESCROLL_API and GNOMESCROLL_LOCAL.
// GNOMESCROLL_API is used for the public API symbols. It either DSO imports or DSO exports (or does nothing for static build)
// GNOMESCROLL_LOCAL is used for non-api symbols.

#ifdef GNOMESCROLL_DSO // defined if GNOMESCROLL is compiled as a DSO
# ifdef GNOMESCROLL_DSO_EXPORTS // defined if we are building the GNOMESCROLL DSO (instead of using it)
#  define GNOMESCROLL_API GNOMESCROLL_DSO_EXPORT
# else
#  define GNOMESCROLL_API GNOMESCROLL_DSO_IMPORT
# endif
# define GNOMESCROLL_LOCAL GNOMESCROLL_DSO_LOCAL
#else // GNOMESCROLL_DSO is not defined: this means GNOMESCROLL is a static lib.
# define GNOMESCROLL_API
# define GNOMESCROLL_LOCAL
#endif

#ifdef __GNUC__
# define LUA_EXPORT GNOMESCROLL_API
#endif

#ifdef __MSVC__
# define LUA_EXPORT __declspec(dllexport)
#endif

#ifdef _WIN32
# define LLD_FMT "%I64d"
#else
# define LLD_FMT "%lld"
#endif

const int GAME_OBJECTS_MAX = 4096 * 4;
const float TICK_DURATION = 30.0f;
const int PLAYERS_MAX = 32;

#define DAT_NAME_MAX_LENGTH 32
#define DATA_PATH "./data/"
#define SCREENSHOT_PATH "./screenshot/"
#define MEDIA_PATH "./media/"
#define SYSTEM_DATA_FOLDER ".gnomescroll/"

// filename max length (no reliable system macro available)
#define GS_FN_MAX 0xFF

// tick time
#define ONE_SECOND 30
#define ONE_MINUTE ONE_SECOND * 60
#define ONE_HOUR   ONE_MINUTE * 60
#define ONE_DAY    ONE_HOUR * 60
#define ONE_WEEK   ONE_DAY * 7
#define ONE_MONTH  ONE_DAY * 30
#define ONE_YEAR   ONE_DAY * 365
