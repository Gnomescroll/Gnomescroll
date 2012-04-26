#include "voxel_model.hpp"

#if DC_CLIENT
#include <input/handlers.hpp>
#endif

namespace Components
{

void VoxelModelComponent::update(Vec3 position, float theta, float phi, bool state_changed)
{
    #if DC_CLIENT
    this->vox->was_updated = false;   // Reset was_updated flag (Voxel_model::update will restore if it does update)
    Vec3 center = this->get_center();
    float radius = this->get_radius();
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
        vox->update(position.x, position.y, position.z, theta, phi);
    #endif

    #if DC_SERVER
    vox->was_updated = false;
    vox->set_hitscan(true);
    if (state_changed)
        vox->update(position.x, position.y, position.z, theta, phi);
    #endif
}

void VoxelModelComponent::force_update(Vec3 position, float theta, float phi, bool state_changed)
{
    #if DC_CLIENT
    vox->was_updated = false;   // Reset updated flag (Voxel_model::update will restore if it did)
    Vec3 center = this->get_center();
    float radius = this->get_radius();
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
        vox->update(position.x, position.y, position.z, theta, phi);
        vox->freeze();
    }
    #endif

    #if DC_SERVER
    vox->was_updated = false;
    vox->set_hitscan(true);
    if (state_changed)
    {
        vox->thaw();
        vox->update(position.x, position.y, position.z, theta, phi);
        vox->freeze();
    }
    #endif
}

} // Objects
