#pragma once

#if DC_CLIENT
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/input/handlers.hpp>
#endif


void updateFrozenVox(Voxel_model* vox, Vec3 position, float theta, float phi)
{
    if (vox != NULL)
    {
        #if DC_CLIENT
        vox->was_updated = false;
        if (current_camera == NULL || !current_camera->in_view(position.x, position.y, position.z))
        {
            vox->set_draw(false);
            vox->set_hitscan(false);
        }
        else
        {
            vox->set_draw(true);
            vox->set_hitscan(true);
        }
        if (input_state.skeleton_editor)
        {
            vox->thaw();
            vox->update(position.x, position.y, position.z, theta, phi);
            vox->freeze();
        }
        #endif

        #if DC_SERVER
        vox->was_updated = false;
        vox->set_hitscan(true);
        #endif
    }
}
