#pragma once


#define ASSERT_NOT_NULL(p) \
if(p == NULL) \
{ \
fprintf (stderr, "ASSERT_NOT_NULL: %s error: %s, line %d \n", __func__, __FILE__, __LINE__); \
return; \
}\

/*
	Disable in production
*/

/*
#if PRODUCTION
	#define GL_ASSERT(flag, truth) ;
#else
	#define GL_ASSERT(flag, truth) \
	if(_gl_assert(flag) !=  truth) \
	fprintf (stderr, "GL_ASSERT error: %s, line %d \n", __FILE__, __LINE__);
#endif
*/