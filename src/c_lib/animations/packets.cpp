#include "packets.hpp"

#include <animations/_interface.hpp>

namespace Animations
{

#if DC_CLIENT
inline void play_animation_StoC::handle()
{
    ASSERT_BOXED_POSITION(this->position);
    play_animation((AnimationType)this->animation_type, this->position);
}
#endif

#if DC_SERVER
inline void play_animation_StoC::handle() {}
#endif

}   // Animations
