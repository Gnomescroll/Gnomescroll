#pragma once

#include <t_mech/_interface.hpp>

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace t_gen
{

void place_native_plants(int max)
{
    const size_t n_plants = 5;
    static const MechType plant_1 = t_mech::get_mech_type("acadia_flower_stage_0");
    static const MechType plant_2 = t_mech::get_mech_type("acadia_flower_stage_1");
    static const MechType plant_3 = t_mech::get_mech_type("acadia_flower_stage_2");
    static const MechType plant_4 = t_mech::get_mech_type("cryptid_larvae_stage_0");
    static const MechType plant_5 = t_mech::get_mech_type("cryptid_larvae_stage_3");
    static const MechType plants[n_plants] = { plant_1, plant_2, plant_3,
                                                plant_4, plant_5 };
    static const CubeType regolith = t_map::get_cube_type("regolith");
    IF_ASSERT(!isValid(regolith)) return;

    int ct = 0;
    for (int tries=0; ct < max && tries < max*2; tries++)
    {
        Vec3i position = t_map::random_surface_block();
        CubeType top = t_map::get(position);
        if (top != regolith) continue;
        MechType plant = plants[randrange(0, n_plants-1)];
        MechCreateFailureCode ret = t_mech::create_mech(position, plant);
        if (ret != MCF_OK && ret != MCF_OCCUPIED) break;
        ct++;
    }

    printf("%s, created %d plants\n", __FUNCTION__, ct);
}

}   // t_gen
