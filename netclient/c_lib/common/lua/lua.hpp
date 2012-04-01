#pragma once

int run_lua_test();

int lua_load_block_dat();

namespace LUA
{

void load_options();

void register_int_option(const char* name, int* var);
void register_bool_option(const char* name, bool* var);
void register_float_option(const char* name, float* var);
void register_sting_option(const char* name, const char* var);

}