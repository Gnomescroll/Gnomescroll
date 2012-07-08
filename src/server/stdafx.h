// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define DC_SERVER 1

#define __attribute__(x)
#define __attribute(x)
#define __WIN32__
#define __MSVC__

#pragma warning(disable: 4244) // possible loss of data

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

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


//_getcwd()/_wgetcwd() Gets the current working directory
//_chdir()/_wchdir() Sets the current working directory

#ifdef __MSVC__
    #include <direct.h>
    #define getcwd _getcwd
    #define chdir _chdir
#else
    #include <unistd.h>
#endif


#include <common/version.h>
#include <common/defines.h>
//#include <common/macros.hpp>
//#include <common/crash_report/stack_trace.hpp>
#include <common/mallox.hpp>

//compression
//#include <common/compression/miniz.c>

//template
//#include <common/template/object_list.hpp>
#include <common/template/multi_object_list.hpp>
#include <common/template/dynamic_object_list.hpp>
#include <common/template/dynamic_multi_object_list.hpp>
#include <common/template/simple_object_list.hpp>
#include <common/template/object_pool.hpp>