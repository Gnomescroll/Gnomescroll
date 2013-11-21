/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <agent/constants.hpp>
#include <agent/agent.cpp>
#include <agent/agent_list.cpp>
#include <agent/net_agent.cpp>
#include <agent/attribute_callbacks.cpp>
#include <agent/attributes.cpp>
#include <agent/agent_status.cpp>
#include <agent/agent_physics.cpp>
#include <agent/_state.cpp>
#include <agent/_interface.cpp>

#if DC_CLIENT
# include <agent/client/player_agent.cpp>
# include <agent/client/agent_draw.cpp>
# include <agent/client/agent_event.cpp>
# include <agent/client/player_agent_action.cpp>
#endif
