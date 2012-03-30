#pragma once

#include <c_lib/voxel/voxel_model.hpp>

// BornVox and BornTeamVox are mutually exclusive

template <class Super, class Object>
class BornVox: public Super
{
    protected:
    inline void born(ObjectState* state, Object* object)
    {
        state->vox = new Voxel_model(state->vox_dat, state->id, state->type);
        Super::born(state, object);
    }
};

template <class Super, class Object>
class BornTeamVox: public Super
{
    protected:
    inline void born(ObjectState* state, Object* object)
    {
        state->vox = new Voxel_model(state->vox_dat, state->id, state->type, state->team);
        Super::born(state, object);
    }
};

// Initializes vox model properties. almost always desired
template <class Super, class Object>
class BornSetVox: public Super
{
    protected:
    inline void born(ObjectState* state, Object* object)
    {
        state->vox->set_hitscan(state->init_hitscan);
        state->vox->register_hitscan();
        #ifdef DC_CLIENT
        state->vox->set_draw(state->init_draw);
        #endif
        Super::born(state, object);
    }
};

// BornUpdateVox and BornUpdateFrozenVox are mutually exclusive
template <class Super, class Object>
class BornUpdateVox: public Super
{
    protected:
    inline void born(ObjectState* state, Object* object)
    {
        Vec3 p = state->get_position();
        state->vox->update(p.x, p.y, p.z, state->theta, state->phi);
        Super::born(state, object);
    }
};

template <class Super, class Object>
class BornUpdateFrozenVox: public Super
{
    protected:
    inline void born(ObjectState* state, Object* object)
    {
        state->vox->thaw();
        Vec3 p = state->get_position();
        state->vox->update(p.x, p.y, p.z, state->theta, state->phi);
        state->vox->freeze();
        Super::born(state, object);
    }
};

// TODO -- find way to make this type agnostic
template <class Super, class Object>
class BornCreateMessage: public Super
{
    protected:
    inline void born(ObjectState* state, Object* object)
    {
        #if DC_SERVER
        object->broadcastCreate();
        #endif
        Super::born(state, object);
    }
};



