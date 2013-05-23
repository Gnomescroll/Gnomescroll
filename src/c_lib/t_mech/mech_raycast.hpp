#pragma once

#include <t_mech/common/common.hpp>

namespace t_mech
{


#if DC_CLIENT
void draw(const Mech &m);
void draw_selected_mech_bounding_box();
#endif

bool ray_cast_mech(const Vec3& position, const Vec3& direction,
                   int& mech_id, float& distance);

}   // t_mech
