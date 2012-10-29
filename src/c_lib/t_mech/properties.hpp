#pragma once

#include <t_mech/common/common.hpp>

namespace t_mech
{

extern class DatNameMap* mech_name_map;
extern class MechAttribute* mech_attributes;   //index from type to attributes

void init_properties();
void teardown_properties();

class MechAttribute* get_mech_attribute(MechType mech_type);

const char* get_mech_name(MechType type);
bool is_valid_mech_name(const char* name);

MechType get_mech_type(const char* name);
const char* get_compatible_mech_name(const char* name);    // for serializer

MechClass get_mech_class(MechType mech_type);
int get_mech_render_type(MechType mech_type);

bool get_mech_type_in_use(MechType mech_type);

}   // t_mech
