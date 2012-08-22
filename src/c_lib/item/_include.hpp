#pragma once

#include <item/common/constant.hpp>
#include <item/common/enum.hpp>
#include <item/common/struct.hpp>

#include <item/properties.cpp>
#include <item/item.cpp>

#include <item/net/CtoS.cpp>
#include <item/net/StoC.cpp>

#include <item/_state.cpp>
#include <item/_interface.cpp>

#include <item/config/_interface.hpp>
#include <item/config/item_dat.hpp>
#include <item/config/synthesizer_dat.hpp>
#include <item/config/crafting_dat.hpp>
#include <item/config/smelting_dat.hpp>
#if DC_SERVER
#include <item/config/item_drop.cpp>
#include <item/config/item_drop_alt.cpp>
#endif


// Client
#if DC_CLIENT
#include <item/client.cpp>
#endif

// Server
#if DC_SERVER
#include <item/server.cpp>
#endif
