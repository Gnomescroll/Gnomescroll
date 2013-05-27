#pragma once

#include <t_mech/common/common.hpp>

namespace t_mech
{


#if DC_CLIENT
void draw(const Mech &m);
void draw_selected_mech_bounding_box();
bool ray_cast_mech(const Vec3& position, const Vec3& direction, float range,
                   int& mech_id, float& distance);
#endif

}   // t_mech
