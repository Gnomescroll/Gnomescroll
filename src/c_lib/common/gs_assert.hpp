#pragma once

#include <common/macros.hpp>

const char* _gs_internal_itoa(int val)
{
    const size_t bufsiz = 32;
    static char buf[bufsiz] = {'\0'};
    snprintf(buf, bufsiz, "%d", val);
    buf[bufsiz-1] = '\0';
    return buf;
}

void _gs_push_str(char* tstr, int* index, char* istr)
{
    int len = strlen(istr);
    strncpy(tstr+*index, istr, len);
    *index += len;
}

void _gs_push_str(char* tstr, int* index, const char* const_str)
{   
    int len = strlen(const_str);
    strncpy(tstr+*index, const_str, len);
    *index += len;
}

void _gs_push_char(char* tstr, int* index, char c)
{
    tstr[*index] = c;
    (*index)++;
}

const int GS_ASSERT_DEFAULT_PRINT_LIMIT = 20;
const int GS_ASSERT_MAX = 4096;

char* _GS_ASSERT_ARRAY[GS_ASSERT_MAX] = {NULL};
int   _GS_ASSERT_COUNT[GS_ASSERT_MAX] = {0};

void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE, int LIMIT)
{
    static char t[256];

    int index = 0;
    const char* LINE_STR = _gs_internal_itoa(LINE);

    _gs_push_str(t,&index, "GS_ASSERT: ");
    _gs_push_str(t,&index, FILE);
    _gs_push_str(t,&index, ":");
    _gs_push_str(t,&index, LINE_STR);
    _gs_push_str(t,&index, " ");
    _gs_push_str(t,&index, FUNC);
    _gs_push_char(t,&index, '\0');
    if (index > 256) printf("ERROR: _GS_ASSERT_INTERNAL string overflow\n");
            
    int i=0;

    while (_GS_ASSERT_ARRAY[i] != NULL && i < GS_ASSERT_MAX)
    {
        if (strcmp(t, _GS_ASSERT_ARRAY[i]) == 0)
        {   //match
            _GS_ASSERT_COUNT[i]++;
            if(_GS_ASSERT_COUNT[i] >= LIMIT) return;
            //print and return;
            print_trace(2);
            puts(t);
            return;
        }
        i++;
    }

    if (i >= GS_ASSERT_MAX) return;

    //insert into array
    _GS_ASSERT_ARRAY[i] = (char*) malloc(strlen(t)+1);
    strcpy(_GS_ASSERT_ARRAY[i], t);

    _GS_ASSERT_COUNT[i]++;
    print_trace(2);
    puts(t);
}

inline void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE)   __attribute((always_inline));
inline void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE)
{
    _GS_ASSERT_INTERNAL(FILE, FUNC, LINE, GS_ASSERT_DEFAULT_PRINT_LIMIT);
}

void _GS_ASSERT_TEARDOWN()
{
    for(int i=0; i<GS_ASSERT_MAX; i++) 
        if(_GS_ASSERT_ARRAY[i] != NULL)
            printf("%s triggered %d times \n", _GS_ASSERT_ARRAY[i], _GS_ASSERT_COUNT[i]);

    for(int i=0; i<GS_ASSERT_MAX; i++) 
        if(_GS_ASSERT_ARRAY[i] != NULL)
            free(_GS_ASSERT_ARRAY[i]);
}
   
#define GS_ASSERT(CONDITION) if(unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__); }

#define GS_ASSERT_LIMIT(CONDITION, LIMIT) if(unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__, (LIMIT)); }

#define GS_ASSERT_ABORT(CONDITION) if (unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__); GS_ABORT(); }

#if defined ( __MSVC__ )
# define __func__ __FUNCTION__
#endif
