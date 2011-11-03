#pragma once

/* Aggregator */

/* objects */
#include "./objects/particles.cpp"
#include "./objects/grenade.cpp"
#include "./objects/neutron.cpp"
#include "./objects/cspray.cpp"

#ifdef DC_CLIENT
#include "./objects/circuit_tree.cpp"
#include "./objects/shrapnel.cpp"
#include "./objects/blood.cpp"
#include "./objects/minivox.cpp"
#endif
