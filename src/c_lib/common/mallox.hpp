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

#if __MSVC__
#define MALLOX(type, array_name, size) type array_name[size];
#else
#define MALLOX(type, array_name, size) type* array_name = (type*) _alloca(size);
#endif