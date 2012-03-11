#pragma once

#include "compat_gl.h"

/*
This prints error/line when gl assertion fails
*/


#define GL_ASSERT_DEBUG 1

bool gl_assert(GLenum flag)
{
	GLboolean value;
	glGetBooleanv(flag, &value);

	if(value == GL_FALSE)
	{
		return false;
	}
	else
	{
		return true;
	}
}


#if GL_ASSERT_DEBUG
	#define GL_ASSERT(flag, truth) \
	if(gl_assert(flag) !=  truth) \
	fprintf (stderr, "GL_ASSERT error: %s, line %d \n", __FILE__, __LINE__);
#else
#define GL_ASSERT(flag) ;

#endif