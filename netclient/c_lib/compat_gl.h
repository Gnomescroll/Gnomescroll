#pragma once


    #ifdef linux
        #define NO_SDL_GLEXT
        #include <GL/glew.h>
        #include <SDL.h>
        #include <SDL_opengl.h>
    #elif _WIN32
		//#define NO_SDL_GLEXT //do not need on windows?
        #include <windows.h>
        #include <SDL.h>
        #include <GL/glew.h>
        //#include <GL/wglew.h> //just in case?
    #elif __APPLE__
        //#define NO_SDL_GLEXT
        #include <GL/glew.h>
        #include <SDL.h>
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        //#include <SDL_opengl.h>
    #endif

    #include <SDL_image.h>

