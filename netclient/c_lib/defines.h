#pragma once

#ifdef __WIN32__
    #ifndef uint
        #define uint unsigned int
    #endif
#endif

#ifdef DC_CLIENT
    #define STATE ClientState
#else
    #define STATE ServerState
#endif

#define TICK_DURATION 30.0f
