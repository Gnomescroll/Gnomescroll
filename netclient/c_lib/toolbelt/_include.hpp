#pragma once

#include <toolbelt/common/constant.hpp>
#include <toolbelt/common/enum.hpp>
#include <toolbelt/common/struct.hpp>

#include <toolbelt/net/CtoS.cpp>
#include <toolbelt/net/StoC.cpp>

#if DC_CLIENT
#include <toolbelt/client.cpp>
#endif
#if DC_SERVER
#include <toolbelt/server.cpp>
#endif

#include <toolbelt/_state.cpp>
#include <toolbelt/_interface.cpp>
