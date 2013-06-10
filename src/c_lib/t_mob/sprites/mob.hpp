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
    void update();
    void draw();

    explicit SpriteMob(SpriteMobType type);
    ~SpriteMob();
};

typedef UnmanagedObjectList<SpriteMob, SpriteMobID> SpriteMobList;
extern SpriteMobList* sprite_mob_list;

void init_sprite_mob_list();
void teardown_sprite_mob_list();

void draw_sprite_mobs();
void hitscan_sprite_mobs(const Vec3& position, const Vec3& direction, float range);

}   // t_mob
