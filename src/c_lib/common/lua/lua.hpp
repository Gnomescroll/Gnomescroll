#pragma once

int run_lua_test();

namespace LUA
{

void set_options_file(const char* lua_name);

void init_options();
void load_options();
void teardown();

void register_int_option(const char* name, int* var);
void register_uint_option(const char* name, unsigned int* var);
void register_bool_option(const char* name, bool* var);
void register_float_option(const char* name, float* var);
void register_string_option(const char* name, char** var);

}

extern "C"
{
    LUA_EXPORT void LUA_set_int_option(int option_id, int value);
    LUA_EXPORT void LUA_set_uint_option(int option_id, unsigned int value);
    LUA_EXPORT void LUA_set_bool_option(int option_id, int value);
    LUA_EXPORT void LUA_set_float_option(int option_id, float value);
    LUA_EXPORT void LUA_set_string_option(int option_id, char* value);
}
