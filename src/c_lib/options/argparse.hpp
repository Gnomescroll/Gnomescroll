#pragma once

namespace Options
{

const int MAX_OPTIONS = 64;

/* Init/Teardown */

void init_option_tables();
void teardown_option_tables();

/* Registration */

void register_uint_option(const char* key, unsigned int* val);
void register_int_option(const char* key, int* val);
void register_float_option(const char* key, float* val);
void register_bool_option(const char* key, bool* val);
void register_string_option(const char* key, char* val);

/* Entrance */

int parse_args(int argc, char* argv[]);

}   // Options
