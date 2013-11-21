/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#ifdef linux
    #include <AL/al.h>
    #include <AL/alc.h>
#elif _WIN32
    #include <AL/al.h>
    #include <AL/alc.h>
#elif __APPLE__
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#endif
