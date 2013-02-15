#include "voxel_model.hpp"

#if DC_CLIENT
#include <input/handlers.hpp>
#endif

namespace Components
{

void VoxelModelComponent::update(Vec3 position, float theta, float phi, bool state_changed)
{
    if (this->vox == NULL) return;
    #if DC_CLIENT
    this->vox->was_updated = false;   // Reset was_updated flag (Voxels::VoxelModel::update will restore if it does update)
    if (state_changed || input_state.skeleton_editor)
        vox->update(position.x, position.y, position.z, theta, phi);
    Vec3 center = this->get_center();
    float radius = this->get_radius();
    if (!sphere_fulstrum_test_translate(center.x, center.y, center.z, radius))
    {
        vox->set_draw(false);
        vox->set_hitscan(false);
    }
    else
    {
        vox->set_draw(this->should_draw);
        vox->set_hitscan(this->should_hitscan);
    }
    #endif

    #if DC_SERVER
    vox->was_updated = false;
    vox->set_hitscan(this->should_hitscan);
    if (state_changed)
        vox->update(position.x, position.y, position.z, theta, phi);
    #endif
}

void VoxelModelComponent::force_update(Vec3 position, float theta, float phi, bool state_changed)
{
    if (this->vox == NULL) return;
    #if DC_CLIENT
    vox->was_updated = false;   // Reset updated flag (Voxels::VoxelModel::update will restore if it did)
    if (state_changed || input_state.skeleton_editor)
    {
        vox->thaw();
        vox->update(position.x, position.y, position.z, theta, phi);
        vox->freeze();
    }
    Vec3 center = this->get_center();
    float radius = this->get_radius();
    if (!sphere_fulstrum_test_translate(center.x, center.y, center.z, radius))
    {
        vox->set_draw(false);
        vox->set_hitscan(false);
    }
    else
    {
        vox->set_draw(this->should_draw);
        vox->set_hitscan(this->should_hitscan);
    }
    #endif

    #if DC_SERVER
    vox->was_updated = false;
    vox->set_hitscan(this->should_hitscan);
    if (state_changed)
    {
        vox->thaw();
        vox->update(position.x, position.y, position.z, theta, phi);
        vox->freeze();
    }
    #endif
}

} // Entities
