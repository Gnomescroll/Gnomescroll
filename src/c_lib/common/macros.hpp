#pragma once

#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

// branch prediction
#ifdef __MSVC__
# define likely  (x) !!(x)
# define unlikely(x) !!(x)
#else
# define likely  (x) __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define GS_MAX(a,b) ((a) > (b)) ? (a) : (b);
#define GS_MIN(a,b) ((a) < (b)) ? (a) : (b);

#ifdef __MSVC__
# define fmin(x,y) ((x) < (y)) ? (x) : (y)
# define fmax(x,y) ((x) > (y)) ? (x) : (y)
#endif

#define ASSERT_VALID_SPAWNER_ID(spawner_id) GS_ASSERT((spawner_id) == BASE_SPAWN_ID || ((spawner_id) >= 0 && (spawner_id) < MAX_SPAWNERS));
#define IF_INVALID_SPAWNER_ID(spawner_id) if (unlikely((spawner_id) != BASE_SPAWN_ID && ((spawner_id) < 0 || (spawner_id) >= MAX_SPAWNERS)))

#define ASSERT_VALID_OBJECT_TYPE(object_type) GS_ASSERT((object_type) >= 0 && (object_type) < MAX_OBJECT_TYPES);
#define IF_INVALID_OBJECT_TYPE(object_type) if (unlikely((object_type) < 0 || (object_type) >= MAX_OBJECT_TYPES))

#define GS_ABORT() \
    do \
    { \
        printf("GS_ABORT error: %s, line %d function: %s\n", __FILE__, __LINE__, __FUNCTION__); \
        print_trace(); \
        exit(1); \
    } while(0);

// macro stringification
#define GS_STR2(x) #x
#define GS_STR(x) GS_STR2(x)

// __GNUG__
// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus).
// preprocessor macros
// http://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html

#ifdef __GNUC__
# define ALWAYS_INLINE __attribute__((__always_inline__)) inline
# if __clang__ && (!__has_attribute(optimize))
#  define OPTIMIZED
# else
#  define OPTIMIZED __attribute__((optimize("-O3")))
# endif
#endif

#ifdef __MSVC__
# define OPTIMIZED
# define ALWAYS_INLINE __forceinline
#endif
