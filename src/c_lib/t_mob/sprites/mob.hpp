#pragma once

#include <physics/vec3.hpp>
#include <common/template/unmanaged_object_list.hpp>
#include <t_mob/sprites/common.hpp>

namespace t_mob
{

class SpriteMob
{
    private:
    const SpriteAnimationGroup* _begin_animation();

    public:
        SpriteMobID id;
        SpriteMobType type;

        Vec3 position;
        Vec3 orientation;

        int animation_tick;
        const SpriteAnimation* current_animation;
        int current_animation_frame;

    inline Vec3 get_center();
    inline float get_radius();

    void set_type(const char* name);

    void begin_animation(const char* name);
    void begin_animation(SpriteAnimationID id);
    void begin_default_animation();
    void stop_animation();
    void tick();

    #if DC_CLIENT
    void draw();
    #endif

    void init(SpriteMobType type);
    void init(const char* name);

    SpriteMob();
    ~SpriteMob();
};

typedef UnmanagedObjectList<SpriteMob, SpriteMobID> SpriteMobList;
extern SpriteMobList* sprite_mob_list;

void init_sprite_mob_list();
void teardown_sprite_mob_list();

void tick_mob_sprites();
void draw_sprite_mobs();
bool hitscan_sprite_mobs(const Vec3& position, const Vec3& direction, float range,
                         SpriteMobID& id, float& distance);

}   // t_mob
