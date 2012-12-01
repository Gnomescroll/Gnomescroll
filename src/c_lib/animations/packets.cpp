#include "packets.hpp"

#include <animations/_interface.hpp>

namespace Animations
{

#if DC_CLIENT
inline void play_animation_StoC::handle()
{
    ASSERT_BOXED_POSITION(this->position);
    play_animation(this->animation_id, this->position);
}
#endif

#if DC_SERVER
inline void play_animation_StoC::handle() {}
#endif

}   // Animations
