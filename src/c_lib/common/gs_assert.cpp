#include "gs_assert.hpp"

#include <sound/sound.hpp>

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

int _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, size_t LINE, size_t LIMIT)
{   // always return 1, so IF_ASSERT works correctly
    #if !PRODUCTION && DC_CLIENT && (!unix)
    if (c_lib_inited)
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
            if(_GS_ASSERT_COUNT[i] >= LIMIT) return 1;
            //print and return;
            print_trace(2);
            puts(t);
            return 1;
        }
        i++;
    }

    if (i >= GS_ASSERT_MAX) return 1;

    //insert into array
    _GS_ASSERT_ARRAY[i] = (char*) malloc(strlen(t)+1);
    strcpy(_GS_ASSERT_ARRAY[i], t);

    _GS_ASSERT_COUNT[i]++;
    print_trace(2);
    printf("%s\n", t);

    return 1;
}

inline int _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE)
{
    return _GS_ASSERT_INTERNAL(FILE, FUNC, LINE, GS_ASSERT_DEFAULT_PRINT_LIMIT);
}

void _GS_ASSERT_TEARDOWN()
{
    for(size_t i=0; i<GS_ASSERT_MAX; i++) 
        if(_GS_ASSERT_ARRAY[i] != NULL)
            printf("%s triggered %lu times \n", _GS_ASSERT_ARRAY[i], (unsigned long)_GS_ASSERT_COUNT[i]);

    for(size_t i=0; i<GS_ASSERT_MAX; i++) 
        if(_GS_ASSERT_ARRAY[i] != NULL)
            free(_GS_ASSERT_ARRAY[i]);
}
