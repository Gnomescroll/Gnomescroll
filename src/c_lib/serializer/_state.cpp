#include "_state.hpp"

namespace serializer
{

char* _buffer = NULL;

void init_state()
{
    GS_ASSERT(_buffer == NULL);
    _buffer = (char*)calloc(BUF_SIZE+1, sizeof(char));
}

void teardown_state()
{
    if (_buffer != NULL) free(_buffer);
}

}   // serializer
