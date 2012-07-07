#pragma once

/*
#define ASSERT_NOT_NULL(p) \
if(p == NULL) \
{ \
fprintf (stderr, "ASSERT_NOT_NULL: %s error: %s, line %d \n", __func__, __FILE__, __LINE__); \
return; \
}
*/

//char *a = (char *)_alloca(n);

#ifdef __GNUC__
	#if _WIN32
		#define MALLOX(type, array_name, size) type array_name[size];
	#else
		//#define MALLOX(type, array_name, size) type array_name[size];
		#define MALLOX(type, array_name, size) type* array_name = (type*) alloca(size);
	#endif
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
