#pragma once

/* Aggregator */

/* objects */

#include <particle/constants.hpp>

#include <particle/particles.cpp>
#include <particle/grenade.cpp>

#if DC_CLIENT
#include <particle/shrapnel.cpp>
#include <particle/blood.cpp>
#include <particle/minivox_colored.cpp>
#include <particle/minivox_textured.cpp>
#include <particle/text/text_particle.cpp>
#include <particle/text/billboard_text.cpp>
#include <particle/text/billboard_text_hud.cpp>
#include <particle/billboard_sprite.cpp>
#endif

#include <particle/_interface.cpp>
