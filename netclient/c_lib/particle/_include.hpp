#pragma once

/* Aggregator */

/* objects */

//#include <particle/_interface.hpp>
#include <particle/particles.cpp>

#include <particle/grenade.cpp>

#include <particle/item_particle.cpp>
#include <particle/item_vox.cpp>

#if DC_CLIENT

#include <particle/shrapnel.cpp>
#include <particle/blood.cpp>
#include <particle/minivox_colored.cpp>
#include <particle/minivox_textured.cpp>
#include <particle/billboard_text.cpp>
#include <particle/billboard_text_hud.cpp>
#include <particle/billboard_sprite.cpp>
#endif

#if DC_SERVER
#include <particle/grenade_shrapnel.cpp>
#endif

#include <particle/net/StoC.cpp>
#include <particle/net/CtoS.cpp>

#include <particle/_interface.cpp>