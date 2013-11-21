/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace t_mech
{

void apply_mech_dat_changes()
{
    //change_mech("old_name", "new_name");
    change_mech("acadia_flower", "acadia_flower_stage_0");

    mech_name_map->condense();  // finalize
}

// mech_class, mech_nam,e mech_render_type, behavior_type

void load_mech_dat()
{
    mech_def(MECH_CRYSTAL, "grass1", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*0 + 0);

    mech_def(MECH_CRYSTAL, "grass2", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*0 + 1);

    mech_def(MECH_CRYSTAL, "grass3", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*0 + 3);

    mech_def(MECH_CRYSTAL, "blue_crystal", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*4 + 5);
    //s->render_type = MECH_RENDER_TYPE_0;

    mech_def(MECH_CRYSTAL, "red_crystal", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*4 + 1);
    //s->render_type = MECH_RENDER_TYPE_0;

    mech_def(MECH_CRYSTAL, "green_crystal", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*4 + 2);
    //s->render_type = MECH_RENDER_TYPE_0;

    mech_def(MECH_MYCELIUM, "mycelium", MECH_RENDER_TYPE_2, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*1 + 0);
    //s->render_type = MECH_RENDER_TYPE_2;
    s->plant = true;

    mech_def(MECH_CRYSTAL, "mycelium_flower", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*1 + 1);
    //s->render_type = MECH_RENDER_TYPE_0;
    s->plant = true;



    mech_def(MECH_CRYSTAL, "acadia_flower_stage_2", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*6 + 2);
    s->plant = true;

    mech_def(MECH_CRYSTAL, "acadia_flower_stage_1", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_PLANT);
    set_sprite_index(16*6 + 1);
    #if PRODUCTION
    s->growth_ttl = 60*3;
    #else
    s->growth_ttl = 6;
    #endif
    s->growth_stage = get_mech_type("acadia_flower_stage_2");
    s->plant = true;

    mech_def(MECH_CRYSTAL, "acadia_flower_stage_0", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_PLANT);
    set_sprite_index(16*6 + 0);
    #if PRODUCTION
    s->growth_ttl = 60*2;
    #else
    s->growth_ttl = 6;
    #endif
    s->growth_stage = get_mech_type("acadia_flower_stage_1");
    s->plant = true;

    mech_def(MECH_CRYSTAL, "blob_flower", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*7 + 2);
    //set_sprite_index(16*6 + 2);
    ///s->render_type = MECH_RENDER_TYPE_0;
    s->plant = true;


    mech_def(MECH_CRYSTAL, "cryptid_larvae_stage_3", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*9 + 3);

    mech_def(MECH_CRYSTAL, "cryptid_larvae_stage_2", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_PLANT);
    set_sprite_index(16*9 + 2);
    s->growth_ttl = 15*60;
    s->growth_stage = get_mech_type("cryptid_larvae_stage_3");

    mech_def(MECH_CRYSTAL, "cryptid_larvae_stage_1", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_PLANT);
    set_sprite_index(16*9 + 1);
    s->growth_ttl = 15*60;
    s->growth_stage = get_mech_type("cryptid_larvae_stage_3");

    mech_def(MECH_CRYSTAL, "cryptid_larvae_stage_0", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DARK_PLANT);
    set_sprite_index(16*9 + 0);
    s->growth_ttl = 15*60;
    s->growth_stage = get_mech_type("cryptid_larvae_stage_1");


    mech_def(MECH_SIGN, "terminal_basic", MECH_RENDER_TYPE_3, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*5 + 0);

    mech_def(MECH_WALL_OBJECT, "light_crystal", MECH_RENDER_TYPE_4, MECH_BEHAVIOR_TYPE_DEFAULT);
    //set_sprite_index(16*5 + 0);
    set_sprite_index(16*11 + 0);

    mech_def(MECH_WALL_OBJECT, "copper_ladder", MECH_RENDER_TYPE_3, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*11 + 0);


    mech_def(MECH_WALL_OBJECT, "iridium_solar_cell", MECH_RENDER_TYPE_3, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*14 + 1);

    mech_def(MECH_WALL_OBJECT, "iridium_power_converter", MECH_RENDER_TYPE_3, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*15 + 1);

    end_mech_dat();     // finalize
}

}   // t_mech
