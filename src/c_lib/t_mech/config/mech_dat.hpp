#pragma once

#include <t_mech/config/mech_dat.hpp>

namespace t_mech
{

/*
struct MECH_ATTRIBUTE
{
    int mech_type;
    int mech_type_class;
    int render_type;
    int sprite_index;
};

enum MECH_CLASS
{
    MECH_CRYSTAL,
    MECH_CROP,
    MECH_MYCELIUM,
    MECH_WIRE,
    MECH_SWITCH
};

enum MECH_RENDER_TYPE
{
    MECH_RENDER_TYPE_0, //X shaped
    MECH_RENDER_TYPE_1, //crop/wheat type
    MECH_RENDER_TYPE_2, //mycelium/ladder type
};
*/

void load_mech_dat()
{
    start_mech_dat();

    mech_def(0, MECH_CRYSTAL, "blue_crystal");
    s.sprite_index = 16*4 + 5;
    s.render_type = MECH_RENDER_TYPE_0;

    mech_def(1, MECH_CRYSTAL, "red_crystal");
    s.sprite_index = 16*4 + 1;
    s.render_type = MECH_RENDER_TYPE_0;

    mech_def(2, MECH_CRYSTAL, "green_crystal");
    s.sprite_index = 16*4 + 2;
    s.render_type = MECH_RENDER_TYPE_0;

    mech_def(3, MECH_MYCELIUM, "mycelium");
    s.sprite_index = 16*1 + 0;
    s.render_type = MECH_RENDER_TYPE_2;

    mech_def(4, MECH_CRYSTAL, "mycelium_flower");
    s.sprite_index = 16*1 + 1;
    s.render_type = MECH_RENDER_TYPE_0;

    mech_def(5, MECH_CRYSTAL, "acadia_flower");
    s.sprite_index = 16*7 + 2;
    s.render_type = MECH_RENDER_TYPE_0;

    mech_def(6, MECH_CRYSTAL, "blob_flower");
    s.sprite_index = 16*6 + 2;
    s.render_type = MECH_RENDER_TYPE_0;

    end_mech_dat(); // finalize
}

}   // t_mech
