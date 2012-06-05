#pragma once


#define ASSERT_NOT_NULL(p) \
if(p == NULL) \
{ \
fprintf (stderr, "ASSERT_NOT_NULL: %s error: %s, line %d \n", __func__, __FILE__, __LINE__); \
return; \
}
