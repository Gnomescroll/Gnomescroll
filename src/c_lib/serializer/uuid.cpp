/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "uuid.hpp"

namespace serializer
{

// returns bytes written, excluding NUL
size_t write_uuid(char* str, size_t buffer_size, uuid_t uuid)
{
    GS_ASSERT(buffer_size > UUID_STRING_LENGTH);
    if (buffer_size <= UUID_STRING_LENGTH) return 0;

    uuid_unparse_lower(uuid, str);
    return UUID_STRING_LENGTH;
}

}   // serializer
