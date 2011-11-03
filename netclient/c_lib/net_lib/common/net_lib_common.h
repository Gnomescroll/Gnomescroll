#ifndef net_lib_common_h
#define net_lib_common_h



//if PLATFORM == PLATFORM_WINDOWS
//pragma comment( lib, "wsock32.lib" )
//endif

    #define PLATFORM_WINDOWS  1
    #define PLATFORM_MAC      2
    #define PLATFORM_UNIX     3

    #if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
    #elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
    #else
    #define PLATFORM PLATFORM_UNIX
    #endif

    #if PLATFORM == PLATFORM_WINDOWS

        #include <winsock2.h>

    #elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <fcntl.h>

    #endif

    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>

    #include "type_pack.h"

    static inline void shutdown_net_lib()
    {
        #if PLATFORM == PLATFORM_WINDOWS
        WSACleanup();
        #endif
    }

    static inline int init_net_lib()
    {
        #if PLATFORM == PLATFORM_WINDOWS
        WSADATA WsaData;
        return WSAStartup( MAKEWORD(2,2), &WsaData ) == NO_ERROR;
        #else
        return 1;
        #endif
    }

#ifndef _WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

#endif
