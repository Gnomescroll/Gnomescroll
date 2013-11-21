/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <serializer/constants.cpp>
#include <serializer/parse.hpp>
#include <serializer/logger.cpp>
#include <serializer/map.cpp>
#include <serializer/mechs.cpp>
#include <serializer/_state.cpp>
#include <serializer/_interface.cpp>

#if GS_SERIALIZER
# include <serializer/uuid.cpp>
# include <serializer/redis.cpp>
# include <serializer/items.cpp>
# include <serializer/players.cpp>
# include <serializer/containers.cpp>
# include <serializer/entities.cpp>
#endif
