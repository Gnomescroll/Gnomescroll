#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <serializer/constants.cpp>
#include <serializer/parse.hpp>
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
#endif
