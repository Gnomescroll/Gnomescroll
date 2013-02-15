#pragma once

#include <sound/sound.hpp>
#include <common/macros.hpp>
#include <common/compat_gl.h>

/*
This prints error/line when gl GS_ASSERTion fails

example:

GL_ASSERT(GL_DEPTH_TEST, true);
GL_ASSERT error: /home/atomos/dc_mmo/netclient/c_lib/./particle/minivox.cpp, line 183
*/

bool _gl_assert(GLenum flag)
{
    GLboolean value;
    glGetBooleanv(flag, &value);
    return (value == GL_TRUE);
}

void _gl_assert_action(const char* file, int line)
{
    fprintf(stderr, "GL_ASSERT error: %s, line %d\n", file, line);
    #if ENABLE_WARNING_SOUND
    printf("FUCK: %d\n", ENABLE_WARNING_SOUND);
    Sound::play_2d_sound("debug_warning");
    #endif
}

// Disable in production
#if PRODUCTION
# define GL_ASSERT(flag, truth) ;
#else
# define GL_ASSERT(flag, truth) \
    if (unlikely(_gl_assert((flag)) !=  (truth))) \
        _gl_assert_action(__FILE__, __LINE__);
#endif
