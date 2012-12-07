#pragma once

#include <sound/sound.hpp>
#include <common/macros.hpp>

const char* _gs_internal_itoa(int val)
{
    const size_t bufsiz = 32;
    static char buf[bufsiz] = {'\0'};
    snprintf(buf, bufsiz, "%d", val);
    buf[bufsiz-1] = '\0';
    return buf;
}

void _gs_push_str(char* dest, const char* src, size_t& index, const size_t maxlen)
{
    if (maxlen <= index) return;
    size_t avail_len = maxlen - index;
    size_t len = strlen(src);
    len = GS_MIN(len, avail_len);
    strncpy(&dest[index], src, len);
    index += len;
}

void _gs_push_char(char* dest, size_t& index, char c)
{
    dest[index++] = c;
}

const size_t GS_ASSERT_DEFAULT_PRINT_LIMIT = 20;
const size_t GS_ASSERT_MAX = 4096;

char* _GS_ASSERT_ARRAY[GS_ASSERT_MAX] = {NULL};
size_t _GS_ASSERT_COUNT[GS_ASSERT_MAX] = {0};

void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, size_t LINE, size_t LIMIT)
{
    #if !PRODUCTION && DC_CLIENT
    Sound::play_2d_sound("debug_warning");
    #endif
    
    const size_t tlen = 0xFF;
    static char t[tlen+1];

    size_t index = 0;
    const char* LINE_STR = _gs_internal_itoa(LINE);

    _gs_push_str(t, "GS_ASSERT: ", index, tlen);
    _gs_push_str(t, FILE, index, tlen);
    _gs_push_str(t, ":", index, tlen);
    _gs_push_str(t, LINE_STR, index, tlen);
    _gs_push_str(t, " ", index, tlen);
    _gs_push_str(t, FUNC, index, tlen);
    if (index >= tlen) index = tlen;
    t[index] = '\0';

    size_t i = 0;
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
    printf("%s\n", t);
}

inline void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE)   __attribute((always_inline));
inline void _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE)
{
    _GS_ASSERT_INTERNAL(FILE, FUNC, LINE, GS_ASSERT_DEFAULT_PRINT_LIMIT);
}

void _GS_ASSERT_TEARDOWN()
{
    for(size_t i=0; i<GS_ASSERT_MAX; i++) 
        if(_GS_ASSERT_ARRAY[i] != NULL)
            printf("%s triggered %d times \n", _GS_ASSERT_ARRAY[i], _GS_ASSERT_COUNT[i]);

    for(size_t i=0; i<GS_ASSERT_MAX; i++) 
        if(_GS_ASSERT_ARRAY[i] != NULL)
            free(_GS_ASSERT_ARRAY[i]);
}
   
#define GS_ASSERT(CONDITION) if(unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__); }

#define GS_ASSERT_LIMIT(CONDITION, LIMIT) if(unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__, (LIMIT)); }

#define GS_ASSERT_ABORT(CONDITION) if (unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__); GS_ABORT(); }

#if defined ( __MSVC__ )
# define __func__ __FUNCTION__
#endif
