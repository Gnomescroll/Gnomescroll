#pragma once

#include <t_mech/common/common.hpp>

namespace t_mech
{
    
extern struct MECH_ATTRIBUTE* mech_attribute;   //index from type to attributes

void init_properties();
void tear_down_properties();

//name
void set_mech_name(int type, const char* name);
const char* get_mech_name(int type);

int get_mech_type_id(const char* name);
int dat_get_mech_type_id(const char* name);

MechClass get_mech_class(int mech_type);

struct MECH_ATTRIBUTE* get_mech_attribute(int mech_type);

}   // t_mech
