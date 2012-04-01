#pragma once

#if DC_CLIENT
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/input/handlers.hpp>
#endif


template <class Object>
void updateFrozenVox(ObjectState* state, Object* object)
{
    if (state->vox != NULL)
    {
        #if DC_CLIENT
        Vec3 p = state->get_position();
        state->vox->was_updated = false;
        if (current_camera == NULL || !current_camera->in_view(p.x, p.y, p.z))
        {
            state->vox->set_draw(false);
            state->vox->set_hitscan(false);
        }
        else
        {
            state->vox->set_draw(true);
            state->vox->set_hitscan(true);
        }
        if (input_state.skeleton_editor)
        {
            state->vox->thaw();
            state->vox->update(p.x, p.y, p.z, state->theta, state->phi);
            state->vox->freeze();
        }
        #endif

        #if DC_SERVER
        state->vox->was_updated = false;
        state->vox->set_hitscan(true);
        #endif
    }
}
