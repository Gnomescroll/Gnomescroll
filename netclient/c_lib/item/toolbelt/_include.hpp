#pragma once

#include <item/toolbelt/common/constant.hpp>
#include <item/toolbelt/common/enum.hpp>
#include <item/toolbelt/common/struct.hpp>

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
