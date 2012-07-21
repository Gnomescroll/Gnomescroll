#pragma once

#ifdef __GNUC__
//#define MALLOX(type, array_name, size) type array_name[size];
#define MALLOX(type, array_name, size) type* array_name = (type*) alloca(size);
#endif

#ifdef __MSVC__
#include <malloc.h>
#define MALLOX(type, array_name, size) type* array_name = (type*) _alloca(size);
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
