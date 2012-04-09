#pragma once

/* Aggregator */

/* objects */
#include <c_lib/particles/particles.cpp>

#if DC_CLIENT
#include <c_lib/particles/textures.cpp>

#include <c_lib/particles/shrapnel.cpp>
#include <c_lib/particles/blood.cpp>
#include <c_lib/particles/minivox.cpp>
#include <c_lib/particles/billboard_text.cpp>
#include <c_lib/particles/billboard_text_hud.cpp>
#include <c_lib/particles/billboard_sprite.cpp>
#endif

#include <c_lib/particles/grenade.cpp>
#include <c_lib/particles/cspray.cpp>

#include <c_lib/particles/_interface.cpp>

#if DC_SERVER
#include <c_lib/particles/particles/grenade_shrapnel.cpp>
#endif

#if 0

/* objects */
#include "./particles/particles.hpp"
#include "./particles/textures.hpp"

#if DC_CLIENT
//#include "./particles/circuit_tree.hpp"
#include "./particles/shrapnel.hpp"
#include "./particles/blood.hpp"
#include "./particles/minivox.hpp"
#include "./particles/billboard_text.hpp"
#include "./particles/billboard_text_hud.hpp"
#endif

#include "./particles/grenade.hpp"
//#include "./particles/neutron.hpp"
#include "./particles/cspray.hpp"

#if DC_SERVER
#include "./particles/grenade_shrapnel.hpp"
#endif
#endif