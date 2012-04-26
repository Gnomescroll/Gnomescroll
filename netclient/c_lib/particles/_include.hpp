#pragma once

/* Aggregator */

/* objects */

//#include <c_lib/particles/_interface.hpp>
#include <c_lib/particles/particles.cpp>

#include <c_lib/particles/grenade.cpp>
#include <c_lib/particles/item_particle.cpp>

#if DC_CLIENT

#include <c_lib/particles/shrapnel.cpp>
#include <c_lib/particles/blood.cpp>
#include <c_lib/particles/minivox_colored.cpp>
#include <c_lib/particles/minivox_textured.cpp>
#include <c_lib/particles/billboard_text.cpp>
#include <c_lib/particles/billboard_text_hud.cpp>
#include <c_lib/particles/billboard_sprite.cpp>
#endif

#if DC_SERVER
#include <c_lib/particles/grenade_shrapnel.cpp>
#endif

#include <c_lib/particles/net/StoC.cpp>
#include <c_lib/particles/net/CtoS.cpp>

#include <c_lib/particles/_interface.cpp>