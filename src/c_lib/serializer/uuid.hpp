/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <uuid/uuid.h>
#include <serializer/constants.hpp>

namespace serializer
{

// returns bytes written, excluding NUL
size_t write_uuid(char* str, size_t buffer_size, uuid_t uuid);

}   // serializer
