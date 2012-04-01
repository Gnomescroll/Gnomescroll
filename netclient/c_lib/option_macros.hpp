#pragma once

#include <c_lib/defines.h>

/* options headers */

#define OPT_INT_HEADER(NAME)\
extern int NAME;

#define OPT_BOOL_HEADER(NAME)\
extern bool NAME;

#define OPT_FLOAT_HEADER(NAME)\
extern float NAME;

#define OPT_STRING_HEADER(NAME)\
extern char* NAME;

/* options definitions */


#define OPT_INT(NAME, DEFAULT)\
int NAME = DEFAULT;

#define OPT_BOOL(NAME, DEFAULT)\
bool NAME = DEFAULT;

#define OPT_FLOAT(NAME, DEFAULT)\
float NAME = DEFAULT;

#define OPT_STRING(NAME, DEFAULT)\
char* NAME = (char*) DEFAULT;



#define OPT_INT_REGISTER(NAME)\
LUA::register_int_option( #NAME, & NAME );

#define OPT_BOOL_REGISTER(NAME)\
LUA::register_bool_option( #NAME, & NAME );

#define OPT_FLOAT_REGISTER(NAME)\
LUA::register_float_option( #NAME, & NAME );

#define OPT_STRING_REGISTER(NAME)\
LUA::register_string_option( #NAME,  NAME );



/*
#define OPT_STRING(NAME, DEFAULT)\
static char NAME##_default[] = DEFAULT;\
char* NAME = NAME##_default;\
void set_##NAME(char* val)\
{\
    unsigned int len = strlen(val);\
    if (NAME == NULL || NAME == NAME##_default)\
    {\
        NAME = (char*)malloc(sizeof(char) * (len+1));\
    }\
    else\
    {\
        if (len != strlen(NAME))\
        {\
            NAME = (char*)realloc(NAME, sizeof(char)*(len+1));\
        }\
    }\
    strcpy(NAME, val);\
}
*/