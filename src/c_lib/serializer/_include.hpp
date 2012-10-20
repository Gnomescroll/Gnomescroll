#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <serializer/constants.hpp>

#if GS_SERIALIZER
# include <serializer/parse.hpp>
# include <serializer/_state.cpp>
# include <serializer/uuid.cpp>
# include <serializer/redis.cpp>
# include <serializer/items.cpp>
# include <serializer/players.cpp>
# include <serializer/containers.cpp>
# include <serializer/mechs.cpp>
# include <serializer/map.cpp>
# include <serializer/_interface.cpp>
#else
# include <serializer/_interface.hpp> 
#endif
