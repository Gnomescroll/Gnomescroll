#pragma once

#include <common/dat/properties.hpp>
#include <animations/constants.hpp>

namespace Animations
{

class AnimationProperty: public Property<AnimationType>
{
    public:
        AnimationMetadataType metadata_type;
        void* metadata;

        anim_callback callback;

        int count;
        int max_count;
        bool use_rand_range;
        Color color;

        float jitter_scale;
        float momentum;

        int ttl_min;
        int ttl_max;

    AnimationProperty() :
        Property<AnimationType>(NULL_ANIMATION_TYPE),
        metadata_type(AnimDataNone), metadata(NULL), callback(NULL),
        count(1), max_count(1), use_rand_range(false),
        color(Color(255,255,255)), jitter_scale(0.0f), momentum(1.0f),
        ttl_min(10), ttl_max(15)
    {
    }

    ~AnimationProperty()
    {
        if (this->metadata != NULL) free(this->metadata);
    }
};


class AnimationProperties: public Properties<AnimationProperty, AnimationType>
{
    public:

    AnimationProperties() :
        Properties<AnimationProperty, AnimationType>(MAX_ANIMATIONS)
    {}
};

extern class AnimationProperties* animation_data;

}   // Animations
