#pragma once

const size_t MAX_ANIMATIONS = 256;

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
}   AnimationMetadataType;


typedef enum
{
    NULL_ANIMATION_TYPE = MAX_ANIMATIONS
}   AnimationType;


typedef void (*anim_callback)(AnimationType, void*);
