#pragma once

namespace t_mob
{

class SpriteMobProperty: public Property<SpriteMobType>
{
    public:
        SpriteAnimationGroupID animation_group_id;
        float size;     // when rendered in the world
        float width;    // as a fraction of the sprite width
        float height;   // as a fraction of the sprite height
        float radius;   // automatically set

    void set_dimensions()
    {   // multiplies width/height by size
        // TODO -- get width/height from texture sheet loader
        this->width *= this->size;
        this->height *= this->size;
    }

    void set_radius()
    {   // call this once, after width/height have been set
        this->radius = GS_MAX(this->width, this->height) * 0.5f * this->size;
    }

    SpriteMobProperty() :
        Property<SpriteMobType>(NULL_SPRITE_MOB_TYPE),
        animation_group_id(NULL_SPRITE_ANIMATION_GROUP),
        size(1.0f),
        width(1.0f),
        height(1.0f),
        radius(0.5f)
    {}
};

typedef Properties<SpriteMobProperty, SpriteMobType> SpriteMobProperties;
static SpriteMobProperties* sprite_mob_properties = NULL;

void init_sprite_mob_properties();
void teardown_sprite_mob_properties();

SpriteMobType get_mob_type(const char* name);

const SpriteAnimationGroup* get_mob_animation_group(SpriteMobType type);
SpriteAnimationGroupID get_mob_animation_group_id(SpriteMobType type);
float get_mob_radius(SpriteMobType type);
float get_mob_height(SpriteMobType type);
float get_mob_width(SpriteMobType type);
float get_mob_size(SpriteMobType type);
const char* get_sprite_mob_name(SpriteMobType type);
const char* get_sprite_mob_pretty_name(SpriteMobType type);

}   // t_mob
