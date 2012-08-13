#include "packets.hpp"

namespace Animations
{

#if DC_CLIENT
inline void play_animation_StoC::handle()
{
    ASSERT_BOXED_POSITION(this->position);

    class AnimationData* data = get_animation_data(this->animation_id);
    GS_ASSERT(data != NULL);
    if (data == NULL) return;
    GS_ASSERT(data->callback != NULL);
    if (data->callback == NULL) return;
    GS_ASSERT(data->metadata != NULL);
    if (data->metadata == NULL) return;

    switch (data->metadata_type)
    {
        case AnimDataNone:
            break;

        case AnimDataSimple:
            ((class AnimationStateMetadata*)data->metadata)->position = this->position;
            ((class AnimationStateMetadata*)data->metadata)->velocity = vec3_scalar_mult(vec3_init(1,1,1), data->momentum);
            break;

        default:
            GS_ASSERT(false);
            break;
    }

    data->callback(this->animation_id, data->metadata);
}
#endif

#if DC_SERVER
inline void play_animation_StoC::handle() {}
#endif

}   // Animations
