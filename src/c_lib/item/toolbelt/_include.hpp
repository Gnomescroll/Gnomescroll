#pragma once

#include <item/toolbelt/common/constants.hpp>
#include <item/toolbelt/common/types.hpp>

#include <item/toolbelt/config/_state.cpp>
#include <item/toolbelt/config/_interface.cpp>
#include <item/toolbelt/config/config.cpp>
#include <item/toolbelt/config/callbacks.cpp>

#include <item/toolbelt/net/CtoS.cpp>
#include <item/toolbelt/net/StoC.cpp>

#if DC_CLIENT
#include <item/toolbelt/client.cpp>
#endif
#if DC_SERVER
#include <item/toolbelt/server.cpp>
#endif

#include <item/toolbelt/_state.cpp>
#include <item/toolbelt/_interface.cpp>
