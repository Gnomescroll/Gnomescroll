#pragma once

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#define DC_SERVER 1

#define __attribute__(x)
#define __attribute(x)
#define __WIN32__
#define __MSVC__

#define S_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0	//secure iterators, slow

#pragma warning(disable: 4244) // possible loss of data
#pragma warning(disable: 4309) // truncation of constant value
#pragma warning(disable: 4146) // unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4800) // forcing value to bool 'true' or 'false' (performance warning)

#define __MSVC__

#define __attribute__(x) 
#define __attribute(x) 

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h> 
#include <winbase.h> // usleep function
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
#include <string.h>
#include <math.h>

extern "C"
{
    #include <lua.h>
    #include <lualib.h> // ok this ony means u maybe fixed the release one but not teh debug one
    #include <lauxlib.h> //wtf!?
}

#define strtoll _strtoi64

#include <common/macros.hpp>

#ifdef __MSVC__
    #include <direct.h>
    #define getcwd _getcwd
    #define chdir _chdir
	#define mkdir(x) _mkdir(x)

	#define snprintf _snprintf
#endif


