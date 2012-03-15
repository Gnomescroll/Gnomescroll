#pragma once

const float DEFAULT_MASS = 1.0f;

/* Aggregator */

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
