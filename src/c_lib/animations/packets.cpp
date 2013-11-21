/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "packets.hpp"

#include <animations/_interface.hpp>

namespace Animations
{

#if DC_CLIENT
inline void play_animation_StoC::handle()
{
    GS_ASSERT(is_boxed_position(this->position));
    play_animation((AnimationType)this->animation_type, this->position);
}
#endif

#if DC_SERVER
inline void play_animation_StoC::handle() {}
#endif

}   // Animations
