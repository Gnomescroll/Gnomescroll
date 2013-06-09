#pragma once

#include <physics/vec3.hpp>
#include <common/template/unmanaged_object_list.hpp>
#include <t_mob/sprites/common.hpp>

namespace t_mob
{

class SpriteMob
{
    public:

    // hitscan
    // animation config
    // rendering

        SpriteMobID id;
        SpriteMobType type;

        Vec3 position;
        Vec3 orientation;

    inline Vec3 get_center();
    inline float get_radius();

    void set_type(const char* name);

    void draw();

    SpriteMob();
    ~SpriteMob();
};

typedef UnmanagedObjectList<SpriteMob, SpriteMobID> SpriteMobList;
extern SpriteMobList* sprite_mob_list;

void init_sprite_mob_list();
void teardown_sprite_mob_list();

void draw_sprite_mobs();
void hitscan_sprite_mobs(const Vec3& position, const Vec3& direction, float range);

}   // t_mob
