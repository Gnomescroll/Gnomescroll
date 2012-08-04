#pragma once

#include "_interface.hpp"

#include <t_mech/net/CtoS.hpp>
#include <t_mech/net/StoC.hpp>

#ifdef DC_SERVER
#include <t_mech/mech_state.hpp>
#endif

#ifdef DC_CLIENT
#include <t_mech/mech_draw.hpp>
#endif
