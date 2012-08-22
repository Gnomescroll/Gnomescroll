#pragma once

namespace t_mech
{
    extern struct MECH_ATTRIBUTE* mech_attribute;   //index from type to attributes

    void init_properties();
    void tear_down_properties();

    //name
    void set_mech_name(int type, const char* name);
    char* get_mech_name(int type);
    
    int get_mech_type(const char* name);
    int dat_get_mech_type(const char* name);

    struct MECH_ATTRIBUTE* get_mech_attribute(int mech_type);

}   // t_mech
