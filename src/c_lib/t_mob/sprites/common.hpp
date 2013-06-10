#pragma once

const int ANIMATION_SPEED = ONE_SECOND * 2;

const size_t MAX_SPRITE_MOBS = 1024;
const size_t MAX_SPRITE_MOB_TYPES = 0xFF;

typedef enum
{
    NULL_SPRITE_MOB = MAX_SPRITE_MOBS
} SpriteMobID;

typedef enum
{
    NULL_SPRITE_MOB_TYPE = MAX_SPRITE_MOB_TYPES
} SpriteMobType;

typedef enum
{
    NULL_SPRITE_ANIMATION = 0xFF
} SpriteAnimationID;

typedef enum
{
    NULL_SPRITE_ANIMATION_GROUP = 0xFF
} SpriteAnimationGroupID;

inline bool isValid(SpriteMobID id)
{
    return (id >= 0 && id < MAX_SPRITE_MOBS);
}

inline bool isValid(SpriteMobType type)
{
    return (type >= 0 && type < MAX_SPRITE_MOB_TYPES);
}

inline bool isValid(SpriteAnimationID id)
{
    return (id >= 0 && id < NULL_SPRITE_ANIMATION);
}

inline bool isValid(SpriteAnimationGroupID id)
{
    return (id >= 0 && id < NULL_SPRITE_ANIMATION_GROUP);
}
