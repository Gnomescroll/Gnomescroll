#pragma once

namespace Animations
{

const int MAX_ANIMATIONS = 256;

typedef void (*anim_callback)(int, void*);

// use for void* metadata
class AnimationStateMetadata
{
    public:
        struct Vec3 position;
        struct Vec3 velocity;
};  

typedef enum
{
    AnimDataNone = 0,
    AnimDataSimple,
} AnimationMetadataType;

class AnimationData
{
    public:
        int id;
        char* name;
        unsigned int hash;
        bool loaded;

        AnimationMetadataType metadata_type;
        void* metadata;

        anim_callback callback;

        int count;
        int max_count;
        bool use_rand_range;
        struct Color color;

        float jitter_scale;
        float momentum;

        int ttl_min;
        int ttl_max;

    AnimationData():
    id(-1), name(NULL), hash(0), loaded(false),
    metadata_type(AnimDataNone), metadata(NULL), callback(NULL),
    count(1), max_count(1), use_rand_range(false),
    color(color_init(255,255,255)), jitter_scale(0.0f), momentum(1.0f),
    ttl_min(10), ttl_max(15)
    {
    }

    ~AnimationData()
    {
        if (this->name != NULL) free(this->name);
        if (this->metadata != NULL) free(this->metadata);
    }
};

extern class AnimationData* animation_data;

}   // Animations
