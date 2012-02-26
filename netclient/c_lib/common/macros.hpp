#pragma once

#include <defines.h>

/* options headers */

#define OPT_BOOL_HEADER(NAME)\
extern bool NAME;\
void set_##NAME(bool val) GNOMESCROLL_API;

#define OPT_FLOAT_HEADER(NAME)\
extern float NAME;\
void set_##NAME(float val) GNOMESCROLL_API;

#define OPT_INT_HEADER(NAME)\
extern int NAME;\
void set_##NAME(int val) GNOMESCROLL_API;

#define OPT_UINT_HEADER(NAME)\
extern unsigned int NAME;\
void set_##NAME(unsigned int val) GNOMESCROLL_API;

#define OPT_STRING_HEADER(NAME)\
extern char* NAME;\
void set_##NAME(char* val) GNOMESCROLL_API;

/* options definitions */

#define OPT_BOOL(NAME, DEFAULT)\
bool NAME = DEFAULT;\
void set_##NAME(bool val)\
{\
    NAME = val;\
}

#define OPT_FLOAT(NAME, DEFAULT)\
float NAME = DEFAULT;\
void set_##NAME(float val)\
{\
    NAME = val;\
}

#define OPT_INT(NAME, DEFAULT)\
int NAME = DEFAULT;\
void set_##NAME(int val)\
{\
    NAME = val;\
}

#define OPT_UINT(NAME, DEFAULT)\
unsigned int NAME = DEFAULT;\
void set_##NAME(unsigned int val)\
{\
    NAME = val;\
}

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
