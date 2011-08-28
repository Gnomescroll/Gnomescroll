#ifndef compat_gl_h
#define compat_gl_h

    #ifndef __APPLE__
        #define NO_SDL_GLEXT
        #include <GL/glew.h>
    #endif

    #include <SDL.h>
    #include <SDL_opengl.h>

    #include <SDL_image.h> //temp?

#endif
