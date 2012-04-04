#pragma once

#ifdef linux
        #define NO_SDL_GLEXT
        #include <GL/glew.h>
        #include <SDL.h>
        #include <SDL_opengl.h>
#elif _WIN32
        #include <windows.h>
        #include <SDL.h>
        #include <GL/glew.h>
#elif __APPLE__
        #include <GL/glew.h>
        #include <SDL.h>
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
#endif

#include <SDL_image.h>
