#ifdef DC_CLIENT
    #define STATE ClientState
#else
    #define STATE ServerState
#endif

#define TICK_DURATION 30.0f
