#pragma once

#include <t_mech/config/mech_dat.hpp>

namespace t_mech
{

void apply_mech_dat_changes()
{
    //change_mech("old_name", "new_name");

    mech_name_map->condense();  // finalize
}

// mech_class, mech_nam,e mech_render_type, mech_behavior_type

void load_mech_dat()
{
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

    mech_def(MECH_CRYSTAL, "mycelium_flower", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*1 + 1);
    //s->render_type = MECH_RENDER_TYPE_0;

    mech_def(MECH_CRYSTAL, "acadia_flower", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    //set_sprite_index(16*7 + 2);
    set_sprite_index(16*6 + 2);
    //s->render_type = MECH_RENDER_TYPE_0;

    mech_def(MECH_CRYSTAL, "blob_flower", MECH_RENDER_TYPE_0, MECH_BEHAVIOR_TYPE_DEFAULT);
    set_sprite_index(16*7 + 2);
    //set_sprite_index(16*6 + 2);
    ///s->render_type = MECH_RENDER_TYPE_0;

    end_mech_dat();     // finalize

    apply_mech_dat_changes();   // handle renaming/removals
    
    verify_mech_dat();  // validate

    save_mech_names();  // safe to save names now
}

}   // t_mech
