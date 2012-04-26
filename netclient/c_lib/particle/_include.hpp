#pragma once

/* Aggregator */

/* objects */

//#include <c_lib/particle/_interface.hpp>
#include <c_lib/particle/particles.cpp>

#include <c_lib/particle/grenade.cpp>

#include <c_lib/particle/item_particle.cpp>
#include <c_lib/particle/item_vox.cpp>

#if DC_CLIENT

#include <c_lib/particle/shrapnel.cpp>
#include <c_lib/particle/blood.cpp>
#include <c_lib/particle/minivox_colored.cpp>
#include <c_lib/particle/minivox_textured.cpp>
#include <c_lib/particle/billboard_text.cpp>
#include <c_lib/particle/billboard_text_hud.cpp>
#include <c_lib/particle/billboard_sprite.cpp>
#endif

#if DC_SERVER
#include <c_lib/particle/grenade_shrapnel.cpp>
#endif

#include <c_lib/particle/net/StoC.cpp>
#include <c_lib/particle/net/CtoS.cpp>

#include <c_lib/particle/_interface.cpp>