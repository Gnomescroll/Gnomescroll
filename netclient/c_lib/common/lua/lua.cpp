#include "lua.hpp"

/*
int run_lua_test()
{
 return 0;
}
*/


/*
 * test.c
 * Example of a C program that interfaces with Lua.
 * Based on Lua 5.0 code by Pedro Martelletto in November, 2003.
 * Updated to Lua 5.1. David Manura, January 2007.
 */

//#include <luajit/lua.hpp>
//#include <luajit/lauxlib.h>

//#include<luajit-2.0/
/*
 extern "C" 
{
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
}
*/

//#include <luajit/lua.hpp>
//#include <luajit/lauxlib.h>

#include <lua.hpp>
#include <lauxlib.h>

#include <stdlib.h>
#include <stdio.h>

/*
    int s=0;
    lua_State *L = lua_open();
    // load the libs
    luaL_openlibs(L);
    //run a Lua scrip here
    luaL_dofile(L,"./media/lua/test.lua");
    lua_close(L);
 
    return 0;
*/

 extern "C" 
{

int barfunc(int foo)
{
    /* a dummy function to test with FFI */ 
    return foo + 1;
}

}

int run_lua_test()
{


    int status, result;
    //int i;
    //double sum;
    lua_State *L;

    /*
     * All Lua contexts are held in this structure. We work with it almost
     * all the time.
     */
    L = luaL_newstate();

    luaL_openlibs(L); /* Load Lua libraries */

    /* Load the file containing the script we are going to run */
    status = luaL_loadfile(L, "./media/lua/test.lua");
    if (status) 
    {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        exit(1);
    }

 	result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) 
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    lua_close(L);   /* Cya, Lua */

#if 0
    /*
     * Ok, now here we go: We pass data to the lua script on the stack.
     * That is, we first have to prepare Lua's virtual stack the way we
     * want the script to receive it, then ask Lua to run it.
     */
    lua_newtable(L);    /* We will pass a table */

    /*
     * To put values into the table, we first push the index, then the
     * value, and then call lua_rawset() with the index of the table in the
     * stack. Let's see why it's -3: In Lua, the value -1 always refers to
     * the top of the stack. When you create the table with lua_newtable(),
     * the table gets pushed into the top of the stack. When you push the
     * index and then the cell value, the stack looks like:
     *
     * <- [stack bottom] -- table, index, value [top]
     *
     * So the -1 will refer to the cell value, thus -3 is used to refer to
     * the table itself. Note that lua_rawset() pops the two last elements
     * of the stack, so that after it has been called, the table is at the
     * top of the stack.
     */

    for (i = 1; i <= 5; i++) 
    {
        lua_pushnumber(L, i);   /* Push the table index */
        lua_pushnumber(L, i*2); /* Push the cell value */
        lua_rawset(L, -3);      /* Stores the pair in the table */
    }

    /* By what name is the script going to reference our table? */
    lua_setglobal(L, "foo");

    /* Ask Lua to run our little script */
    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) 
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    /* Get the returned value at the top of the stack (index -1) */
    sum = lua_tonumber(L, -1);

    printf("Script returned: %.0f\n", sum);

    lua_pop(L, 1);  /* Take the returned value out of the stack */
#endif

    //printf("run_lua_test: finished \n");

    return 0;

}


int lua_load_block_dat()
{
    int status, result;
    lua_State *L;

    /*
     * All Lua contexts are held in this structure. We work with it almost
     * all the time.
     */
    L = luaL_newstate();

    luaL_openlibs(L); /* Load Lua libraries */

    /* Load the file containing the script we are going to run */
    
    #if DC_CLIENT
        status = luaL_loadfile(L, "./lua/block/client_load_blocks.lua");
    #endif

    #if DC_SERVER
        status = luaL_loadfile(L, "./lua/block/server_load_blocks.lua");
    #endif

    if (status) 
    {
        /* If something went wrong, error message is at the top of the stack */
        fprintf(stderr, "lua_load_map_tiles: Couldn't load file: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) 
    {
        fprintf(stderr, "lua_load_map_tiles: Failed to run script: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    lua_close(L);   /* Cya, Lua */

    printf("lua_load_map_tiles: finished \n");

    return 0;

}