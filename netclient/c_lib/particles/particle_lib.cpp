#pragma once

/* Aggregator */

/* objects */
#include "./particles/particles.cpp"
#include "./particles/textures.cpp"

#if DC_CLIENT
//#include "./particles/circuit_tree.cpp"
#include "./particles/shrapnel.cpp"
#include "./particles/blood.cpp"
#include "./particles/minivox.cpp"
#include "./particles/billboard_text.cpp"
#include "./particles/billboard_text_hud.cpp"
#endif

#include "./particles/grenade.cpp"
#include "./particles/cspray.cpp"

#if DC_SERVER
#include "./particles/grenade_shrapnel.cpp"
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