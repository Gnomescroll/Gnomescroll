#pragma once

#include <t_mech/_interface.hpp>

#if DC_CLIENT
dont_include_this_file_in_client
#endif

/*
Add native plants
*/

namespace t_gen
{

void place_native_plants(int max)
{
    const size_t n_plants = 5;
    static const MechType plant_1 = t_mech::get_mech_type_dat("acadia_flower_stage_0");
    static const MechType plant_2 = t_mech::get_mech_type_dat("acadia_flower_stage_1");
    static const MechType plant_3 = t_mech::get_mech_type_dat("acadia_flower_stage_2");
    static const MechType plant_4 = t_mech::get_mech_type_dat("cryptid_larvae_stage_0");
    static const MechType plant_5 = t_mech::get_mech_type_dat("cryptid_larvae_stage_3");
    static const MechType plants[n_plants] = { plant_1, plant_2, plant_3,
                                                plant_4, plant_5 };
    static const CubeType regolith = t_map::get_cube_type("regolith");
    GS_ASSERT(isValid(regolith));

    int ct = 0;
    for (int tries=0; ct < max && tries < max*2; tries++)
    {
        int x = randrange(0, map_dim.x-1);
        int y = randrange(0, map_dim.y-1);
        int z = t_map::get_highest_open_block(x, y);
        CubeType top = t_map::get(x, y, z);
        if (top != regolith) continue;
        MechType plant = plants[randrange(0, n_plants-1)];
        if (!t_mech::create_mech(x, y, z, plant)) break;
        ct++;
    }

    #if !PRODUCTION
    printf("%s, created %d plants\n", __FUNCTION__, ct);
    #endif
}

}   // t_gen
