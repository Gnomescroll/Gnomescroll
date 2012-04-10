#pragma once

#if DC_CLIENT
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/input/handlers.hpp>
#endif


void updateFrozenVox(Voxel_model* vox, Vec3 position, Vec3 angles, bool state_changed)
{
    if (vox == NULL) return;

    #if DC_CLIENT
    vox->was_updated = false;   // Reset updated flag (Voxel_model::update will restore if it did)
    Vec3 center = vox->get_part(0)->get_center();
    float radius = vox->get_part(0)->radius;
    if (sphere_fulstrum_test(center.x, center.y, center.z, radius) == false)
    {
        vox->set_draw(false);
        vox->set_hitscan(false);
    }
    else
    {
        vox->set_draw(true);
        vox->set_hitscan(true);
    }
    if (state_changed || input_state.skeleton_editor)
    {
        vox->thaw();
        vox->update(position.x, position.y, position.z, angles.x, angles.y);
        vox->freeze();
    }
    #endif

    #if DC_SERVER
    vox->was_updated = false;
    vox->set_hitscan(true);
    #endif
}

void updateVox(Voxel_model* vox, Vec3 position, Vec3 angles, bool state_changed)
{
    if (vox == NULL) return;

    #if DC_CLIENT
    vox->was_updated = false;   // Reset was_updated flag (Voxel_model::update will restore if it does update)
    Vec3 center = vox->get_part(0)->get_center();
    float radius = vox->get_part(0)->radius;
    if (sphere_fulstrum_test(center.x, center.y, center.z, radius) == false)
    {
        vox->set_draw(false);
        vox->set_hitscan(false);
    }
    else
    {
        vox->set_draw(true);
        vox->set_hitscan(true);
    }
    if (state_changed || input_state.skeleton_editor)
    {   // TODO : voxel accepts Vec3 angles
        vox->update(position.x, position.y, position.z, angles.x, angles.y);
    }
    #endif

    #if DC_SERVER
    vox->was_updated = false;
    vox->set_hitscan(true);
    #endif
}
