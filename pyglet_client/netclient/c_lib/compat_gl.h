#ifndef compat_gl_h
#define compat_gl_h


    #ifdef linux
        #define NO_SDL_GLEXT
        #include <GL/glew.h>
        #include <SDL.h>
        #include <SDL_opengl.h>
    #elif _WIN32
        #include <SDL.h>
        #include <windows.h>
        #include <GL/glew.h>
    #elof __APPLE__
        //#define NO_SDL_GLEXT
        #include <GL/glew.h>
        #include <SDL.h>
        //#include <SDL_opengl.h>
    #endif

    #include <SDL_image.h>

#endif
