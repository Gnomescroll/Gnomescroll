#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <serializer/constants.hpp>

#if GS_SERIALIZER
# include <serializer/redis/_interface.cpp>
# include <serializer/items.cpp>
# include <serializer/map.cpp>
# include <serializer/_interface.cpp>
#else
# include <serializer/_interface.hpp> 
#endif
