#pragma once

#if DC_CLIENT
#include <c_lib/camera/camera.hpp>
#include <c_lib/input/input.hpp>
#endif

#include <c_lib/voxel/voxel_model.hpp>

template <class Super, class Object>
class UpdateFrozenVox: public Super
{
    void update(ObjectState* state, Object* object)
    {
        if (state->vox == NULL) return;

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

        Super::update(state, object);
    }
};

template <class Super, class Object>
class BornVox: public Super
{
    void born(ObjectState* state, Object* object)
    {
        state->vox = new Voxel_model(state->vox_dat, state->id, state->type);
        Super::born(state, object);
    }
};

template <class Super, class Object>
class BornTeamVox: public Super
{
    void born(ObjectState* state, Object* object)
    {
        if (this->team == 0) printf("WARNING Turret::init_vox() -- team not set\n");
        state->vox = new Voxel_model(state->vox_dat, state->id, state->type, state->team);
        Super::born(state, object);
    }
};

template <class Super, class Object>
class BornSetVox: public Super
{
    void born(ObjectState* state, Object* object)
    {
        this->vox->set_hitscan(state->init_hitscan);
        this->vox->register_hitscan();
        #ifdef DC_CLIENT
        this->vox->set_draw(state->init_draw);
        #endif
        Super::born(state, object);
    }
};

template <class Super, class Object>
class BornUpdateVox: public Super
{
    void born(ObjectState* state, Object* object)
    {
        this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
        Super::born(state, object);
    }
};

template <class Super, class Object>
class BornUpdateFrozenVox: public Super
{
    void born(ObjectState* state, Object* object)
    {
        this->vox->thaw();
        this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
        this->vox->freeze();
        Super::born(state, object);
    }
};
