/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "packets.hpp"

#include <sound/sound.hpp>

namespace Sound
{

#if DC_CLIENT
inline void play_2d_sound_StoC::handle()
{
    play_2d_sound(soundfile_id);
}

inline void play_3d_sound_StoC::handle()
{
    play_3d_sound(soundfile_id, position);
}
#endif

#if DC_SERVER
inline void play_2d_sound_StoC::handle() {}
inline void play_3d_sound_StoC::handle() {}
#endif

}   // Sound
