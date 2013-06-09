#pragma once

namespace t_mob
{

class SpriteMobProperty: public Property<SpriteMobType>
{
    public:

        SpriteMobType type;
        SpriteAnimationGroupID animation_group;
        float width;
        float height;
        float radius;   // automatically set

    void set_radius()
    {   // call this once, after width/height have been set
        this->radius = GS_MAX(this->width, this->height) * 0.5f;
    }

    SpriteMobProperty() :
        Property<SpriteMobType>(NULL_SPRITE_MOB_TYPE),
        type(NULL_SPRITE_MOB_TYPE),
        animation_group(NULL_SPRITE_ANIMATION_GROUP),
        width(1.0f),
        height(1.0f),
        radius(0.5f)
    {}
};

typedef Properties<SpriteMobProperty, SpriteMobType> SpriteMobProperties;
static SpriteMobProperties* sprite_mob_properties = NULL;

void init_sprite_mob_properties();
void teardown_sprite_mob_properties();

float get_mob_radius(SpriteMobType type);
float get_mob_height(SpriteMobType type);
SpriteMobType get_mob_type(const char* name);

}   // t_mob
