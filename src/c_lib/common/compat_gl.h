#pragma once

#ifdef linux
        #define NO_SDL_GLEXT
        #include <GL/glew.h>
        #include <SDL.h>
        #include <SDL_opengl.h>
#elif _WIN32
    //#include <windows.h>

    #ifdef __GNUC__
    #include <SDL.h>
    #include <GL/glew.h>
    #endif

    #ifdef __MSVC__
    //#define NO_SDL_GLEXT
    
    #include <SDL.h>
    #include <GL/glew.h>
    //#include <SDL_opengl.h>
    #endif

#elif __APPLE__
        #include <GL/glew.h>
        #include <SDL.h>
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
#endif

#include <SDL_image.h>
