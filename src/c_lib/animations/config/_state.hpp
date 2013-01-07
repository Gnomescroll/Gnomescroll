#pragma once

namespace Animations
{

const size_t ANIMATION_NAME_MAX_LENGTH = 63;

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

bool is_valid_animation_name(const char* name)
{
    return (name != NULL && strlen(name) <= ANIMATION_NAME_MAX_LENGTH);
}

class AnimationData
{
    public:
        int id;
        bool loaded;

        char name[ANIMATION_NAME_MAX_LENGTH+1];

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

    AnimationData():
        id(-1), loaded(false),
        metadata_type(AnimDataNone), metadata(NULL), callback(NULL),
        count(1), max_count(1), use_rand_range(false),
        color(Color(255,255,255)), jitter_scale(0.0f), momentum(1.0f),
        ttl_min(10), ttl_max(15)
    {
        memset(this->name, 0, sizeof(this->name));
    }

    ~AnimationData()
    {
        if (this->metadata != NULL) free(this->metadata);
    }
};

extern class AnimationData* animation_data;

}   // Animations
