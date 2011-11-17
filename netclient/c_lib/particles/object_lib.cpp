#pragma once

/* Aggregator */

#ifdef DC_CLIENT
#include "./particles/textures.cpp" /* Textures */

#include "./particles/circuit_tree.cpp"
#include "./particles/shrapnel.cpp"
#include "./particles/blood.cpp"
#include "./particles/minivox.cpp"
#include "./particles/billboard_text.cpp"
#endif

/* objects */
#include "./particles/particles.cpp"
#include "./particles/grenade.cpp"
#include "./particles/neutron.cpp"
#include "./particles/cspray.cpp"
