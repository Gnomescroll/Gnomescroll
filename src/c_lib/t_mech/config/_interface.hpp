#pragma once

#include <t_mech/common/common.hpp>
#include <t_mech/config/_interface.hpp>
#include <t_mech/properties.hpp>

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

namespace t_mech
{
    //this is whitespace/alpha dimensions for ray casting box
    int mech_sprite_width[256];  //centered
    int mech_sprite_height[256]; //from bottom
    float mech_sprite_width_f[256];  //centered
    float mech_sprite_height_f[256]; //from bottom
}

namespace t_mech
{

int _current_mech_index = -1;
struct MECH_ATTRIBUTE s;

void _reset_mech()
{
    s.mech_type = -1;
    s.mech_type_class = -1;
    s.render_type = -1;
    s.sprite_index = 255;
    s.item_drop = false;
}

void start_mech_dat()
{
    GS_ASSERT(_current_mech_index == -1);

    _reset_mech();
}

void _push_mech()
{
    GS_ASSERT(s.sprite_index != 255);
    GS_ASSERT(s.mech_type_class != -1);
    GS_ASSERT(s.render_type != -1);
    mech_attribute[_current_mech_index] = s;

    _reset_mech();
}

void mech_def(int mech_type, int mech_type_class, const char* name)
{
    ASSERT_VALID_MECH_TYPE(mech_type);
    IF_INVALID_MECH_TYPE(mech_type) return;
    
    if(_current_mech_index != -1) 
        _push_mech();

    GS_ASSERT(mech_attribute[mech_type].mech_type == -1);
    if(mech_attribute[mech_type].mech_type != -1)
    {
        printf("mech_def error, id used twiced: name=%s id=%i \n", name, mech_type);
        return;
    }
    
    s.mech_type = mech_type;
    s.mech_type_class = mech_type_class;

    set_mech_name(mech_type, name);
    _current_mech_index = mech_type;
}

void end_mech_dat()
{
    _push_mech();

}

}   // t_mech
