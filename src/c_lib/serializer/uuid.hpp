#pragma once

#include <uuid/uuid.h>

namespace serializer
{

const size_t UUID_STRING_LENGTH = 36;

// returns bytes written, excluding NUL
size_t write_uuid(char* str, size_t buffer_size, uuid_t uuid);

}   // serializer
