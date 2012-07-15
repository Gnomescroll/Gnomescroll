#include "lua.hpp"


extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}


#include <stdlib.h>
#include <stdio.h>

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
    status = luaL_loadfile(L, "settings/test.lua");
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

#include <options/options.hpp>

namespace LUA
{


lua_State* LUA_options_table = NULL;

#if DC_CLIENT
    #if PRODUCTION
        const char* default_options_file = "./settings/settings.lua";
    #else
        const char* default_options_file = "./settings/settings_dev.lua";
    #endif
#endif
#if DC_SERVER
const char* default_options_file = "./settings/settings_server.lua";
#endif

char* options_file = NULL;

void set_options_file(char* path)
{
    GS_ASSERT(path != NULL);
    if (path == NULL) return;
    if (options_file != NULL)
        free(options_file);
    options_file = (char*)malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(options_file, path);
}

/*
    Load or reload options
*/

void init_options()
{
    if (LUA_options_table != NULL) return;
    
    char* options_path = options_file;
    if (options_path == NULL)
        options_path = (char*)default_options_file;

    LUA_options_table = luaL_newstate();
    lua_State *L = LUA_options_table;

    luaL_openlibs(L); /* Load Lua libraries */

    if (luaL_loadfile(L, options_path)) 
    {
        fprintf(stderr, "%s:%d -- Couldn't load file: %s\n", __FUNCTION__, __LINE__, lua_tostring(L, -1));
        GS_ABORT();
    }

    lua_newtable(L);    //create options table
}

void load_options()
{
    char* options_path = options_file;
    if (options_path == NULL)
        options_path = (char*)default_options_file;

    printf("Loading settings file %s\n", options_path);

    static int inited = 0;

    lua_State *L = LUA_options_table;
    GS_ASSERT(L != NULL);
    
    if (!inited)
    {
        lua_setglobal(L, "options_table"); //name options

        if (lua_pcall(L, 0, LUA_MULTRET, 0)) 
        {
            fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
            GS_ABORT();
        }
    }
    else
    {
        printf("Reloading Settings\n");

        //if (luaL_loadfile(L, "settings/settings.lua")) 
        if (luaL_loadfile(L, options_path)) 
        {
            fprintf(stderr, "register_int_option: Couldn't load file: %s\n", lua_tostring(L, -1));
            GS_ABORT();
        }

        if (lua_pcall(L, 0, LUA_MULTRET, 0)) 
        {
            fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
            GS_ABORT();
        }
    }

    inited++;
}

/*
    Types: int, float, bool, string
*/

int LUA_int_option_index = 0;
int* LUA_int_option_table[256] = {0};

void register_int_option(const char* name, int* var)
{
    LUA_int_option_table[LUA_int_option_index] = var;
    lua_State *L = LUA_options_table;

    lua_pushstring(L, name);    //key

    lua_createtable(L, 0, 2);   //value

        lua_pushstring(L, "id");
        lua_pushnumber(L, LUA_int_option_index);
        lua_rawset(L, -3);

        lua_pushstring(L, "type");
        lua_pushstring(L, "int");
        lua_rawset(L, -3);

    lua_rawset(L, -3);          //create table

    LUA_int_option_index++;
    if(LUA_int_option_index >= 256)
    {
        printf("LUA OPTION ERROR\n");
        GS_ABORT();
    }
}

int LUA_uint_option_index = 0;
unsigned int* LUA_uint_option_table[256] = {0};

void register_uint_option(const char* name, unsigned int* var)
{
    LUA_uint_option_table[LUA_uint_option_index] = var;
    lua_State *L = LUA_options_table;

    lua_pushstring(L, name);    //key

    lua_createtable(L, 0, 2);   //value

        lua_pushstring(L, "id");
        lua_pushnumber(L, LUA_uint_option_index);
        lua_rawset(L, -3);

        lua_pushstring(L, "type");
        lua_pushstring(L, "uint");
        lua_rawset(L, -3);

    lua_rawset(L, -3);          //create table

    LUA_uint_option_index++;
    if(LUA_uint_option_index >= 256)
    {
        printf("LUA OPTION ERROR\n");
        GS_ABORT();
    }
}

int LUA_bool_option_index = 0;
bool* LUA_bool_option_table[256] = {0};

void register_bool_option(const char* name, bool* var)
{
    LUA_bool_option_table[LUA_bool_option_index] = var;
    lua_State *L = LUA_options_table;

    lua_pushstring(L, name);    //key

    lua_createtable(L, 0, 2);   //value

        lua_pushstring(L, "id");
        lua_pushnumber(L, LUA_bool_option_index);
        lua_rawset(L, -3);

        lua_pushstring(L, "type");
        lua_pushstring(L, "bool");
        lua_rawset(L, -3);

    lua_rawset(L, -3);          //create table

    LUA_bool_option_index++;
    if(LUA_bool_option_index >= 256)
    {
        printf("LUA BOOL OPTION ERROR\n");
        GS_ABORT();
    }
}

int LUA_float_option_index = 0;
float* LUA_float_option_table[256] = {0};

void register_float_option(const char* name, float* var)
{
    LUA_float_option_table[LUA_float_option_index] = var;
    lua_State *L = LUA_options_table;

    lua_pushstring(L, name);    //key

    lua_createtable(L, 0, 2);   //value

        lua_pushstring(L, "id");
        lua_pushnumber(L, LUA_float_option_index);
        lua_rawset(L, -3);

        lua_pushstring(L, "type");
        lua_pushstring(L, "float");
        lua_rawset(L, -3);

    lua_rawset(L, -3);          //create table

    LUA_float_option_index++;
    if(LUA_float_option_index >= 256)
    {
        printf("LUA FLOAT OPTION ERROR\n");
        GS_ABORT();
    }
}

int LUA_string_option_index = 0;
char** LUA_string_option_table[256] = {0};

void register_string_option(const char* name, char** var)
{
    LUA_string_option_table[LUA_string_option_index] = var;
    lua_State *L = LUA_options_table;

    lua_pushstring(L, name);    //key

    lua_createtable(L, 0, 2);   //value

        lua_pushstring(L, "id");
        lua_pushnumber(L, LUA_string_option_index);
        lua_rawset(L, -3);      /* segfaults? */

        lua_pushstring(L, "type");
        lua_pushstring(L, "string");
        lua_rawset(L, -3);

    lua_rawset(L, -3);          //create table

    LUA_string_option_index++;
    if(LUA_string_option_index >= 256)
    {
        printf("LUA STRING OPTION ERROR\n");
        GS_ABORT();
    }
}

}
/*
    void LUA_set_int_option(int option_id, int value);
    void LUA_set_bool_option(int option_id, int value);
    void LUA_set_float_option(int option_id, float value);
    void LUA_set_string_option(int option_id, char* value);
*/

extern "C"
{

    void LUA_set_int_option(int option_id, int value)
    {

        if(LUA::LUA_int_option_table[option_id] == NULL)
        {
            printf("LUA_set_int_option: error \n");
            GS_ABORT();
        }
        *LUA::LUA_int_option_table[option_id] = value;
    }

    void LUA_set_uint_option(int option_id, unsigned int value)
    {

        if(LUA::LUA_uint_option_table[option_id] == NULL)
        {
            printf("LUA_set_uint_option: error \n");
            GS_ABORT();
        }
        *LUA::LUA_uint_option_table[option_id] = value;
    }

    void LUA_set_bool_option(int option_id, int value)
    {
        if(LUA::LUA_bool_option_table[option_id] == NULL)
        {
            printf("LUA_set_bool_option: error \n");
            GS_ABORT();
        }
        if(value == 0)
        {
            *LUA::LUA_bool_option_table[option_id] = false;
        }
        else
        {
            *LUA::LUA_bool_option_table[option_id] = true;
        }

    }
    
    void LUA_set_float_option(int option_id, float value)
    {
        if(LUA::LUA_float_option_table[option_id] == NULL)
        {
            printf("LUA_set_float_option: error \n");
            GS_ABORT();
        }
        *LUA::LUA_float_option_table[option_id] = value;
    }

    void LUA_set_string_option(int option_id, char* value)
    {
        if(LUA::LUA_string_option_table[option_id] == NULL)
        {
            printf("LUA_set_string_option: error \n");
            GS_ABORT();
        }
        *LUA::LUA_string_option_table[option_id] = value;
    }

}
