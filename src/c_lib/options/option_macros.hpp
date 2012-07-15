#pragma once

#include <common/defines.h>

/* Header declarations */

#define OPT_INT_HEADER(NAME)\
extern int NAME;

#define OPT_UINT_HEADER(NAME)\
extern unsigned int NAME;

#define OPT_BOOL_HEADER(NAME)\
extern bool NAME;

#define OPT_FLOAT_HEADER(NAME)\
extern float NAME;

#define OPT_STRING_HEADER(NAME)\
extern char* NAME;

/* Variable definitions */

#define OPT_INT(NAME, DEFAULT)\
int NAME = DEFAULT;

#define OPT_UINT(NAME, DEFAULT)\
unsigned int NAME = DEFAULT;

#define OPT_BOOL(NAME, DEFAULT)\
bool NAME = DEFAULT;

#define OPT_FLOAT(NAME, DEFAULT)\
float NAME = DEFAULT;

#define OPT_STRING(NAME, DEFAULT)\
char* NAME = (char*) DEFAULT;

/* Registrations */

#define OPT_INT_REGISTER(NAME)\
LUA::register_int_option( #NAME, & NAME );\
register_int_option( #NAME, & NAME );

#define OPT_UINT_REGISTER(NAME)\
LUA::register_uint_option( #NAME, & NAME );\
register_uint_option( #NAME, & NAME );

#define OPT_BOOL_REGISTER(NAME)\
LUA::register_bool_option( #NAME, & NAME );\
register_bool_option( #NAME, & NAME );

#define OPT_FLOAT_REGISTER(NAME)\
LUA::register_float_option( #NAME, & NAME );\
register_float_option( #NAME, & NAME );

#define OPT_STRING_REGISTER(NAME)\
LUA::register_string_option( #NAME, & NAME );\
register_string_option( #NAME, & NAME );
