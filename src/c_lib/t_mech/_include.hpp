#pragma once

#include <t_mech/common/common.hpp>

#include "_interface.hpp"

#include <t_mech/net/CtoS.cpp>
#include <t_mech/net/StoC.cpp>

#ifdef DC_SERVER
#include <t_mech/mech_state.hpp>
#endif

#ifdef DC_CLIENT
#include <t_mech/mech_draw.hpp>
#endif

#include <t_mech/properties.cpp>
#include <t_mech/config/_interface.hpp>
#include <t_mech/config/mech_dat.hpp>

#include "_interface.cpp"