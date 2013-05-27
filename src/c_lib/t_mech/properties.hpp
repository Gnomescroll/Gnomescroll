#pragma once

#include <t_mech/common/common.hpp>

namespace t_mech
{

extern class DatNameMap* mech_name_map;
extern class MechAttribute* mech_attributes;   //index from type to attributes

void init_properties();
void teardown_properties();

void update_dimensions();

class MechAttribute* get_mech_attribute(MechType type);

const char* get_mech_name(MechType type);
bool is_valid_mech_name(const char* name);

MechType get_mech_type(const char* name);
MechType get_mech_type(const char* name); //fatal on failure

const char* get_compatible_mech_name(const char* name);    // for serializer

MechClassType get_mech_class(MechType type);
MechRenderType get_mech_render_type(MechType type);
MechBehaviorType get_mech_behavior_type(MechType type);

Vec3 get_mech_box_dimensions(MechType type);
float get_mech_radius(MechType type);
float get_mech_size(MechType type);

// requires state
Vec3 get_mech_center(const struct Mech& mech);

inline bool type_in_use(MechType type);
inline bool is_plant(MechType type);

}   // t_mech
