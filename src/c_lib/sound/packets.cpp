#include "packets.hpp"

#include <sound/sound.hpp>

namespace Sound
{

#if DC_CLIENT
inline void play_2d_sound_StoC::handle()
{
    printf("Playing sound id: %d\n", sound_id);
    const char event_name[] = "spawner_activate";
    printf("Sound id for %s: %d\n", event_name, get_soundfile_id_for_name(event_name));
    play_2d_sound(sound_id);
}

inline void play_3d_sound_StoC::handle()
{
    play_3d_sound(sound_id, position, vec3_init(0,0,0));
}
#endif

#if DC_SERVER
inline void play_2d_sound_StoC::handle() {}
inline void play_3d_sound_StoC::handle() {}
#endif

}   // Sound
