#pragma once

#ifdef __GNUC__

#ifdef __MINGW32__
# include <malloc.h> //alloca for mingw gcc
#else
# include <alloca.h> //normal alloca
#endif

#define MALLOX(type, array_name, count) type* array_name = (type*) alloca(count * sizeof(type));
//#define MALLOX(type, array_name, count) type* array_name = (type*) alloca(count * sizeof(type));
#endif

#ifdef __MSVC__
#include <malloc.h>
#define MALLOX(type, array_name, count) type* array_name = (type*) _alloca(count * sizeof(type));
#endif

/*
// define MALLOX(type, array_name, size) \
// static type* array_name = NULL\
// static array_name##__max int = 0;\
// static array_name##__index int = 0; \
*/

/*
// define MALLOX(type, array_name, size) \
// static type* array_name = NULL\
// static array_name##__max int = 0;\
// static array_name##__index int = 0; \
*/
