#pragma once

#include <c_lib/voxel/voxel_model.hpp>

// BornVox and BornTeamVox are mutually exclusive
template <class Object>
void bornVox(ObjectState* state, Object* object)
{
    state->vox = new Voxel_model(state->vox_dat, state->id, state->type);
}

template <class Object>
inline void bornTeamVox(ObjectState* state, Object* object)
{
    state->vox = new Voxel_model(state->vox_dat, state->id, state->type, object->get_team());
}

// Initializes vox model properties. almost always desired
template <class Object>
inline void bornSetVox(ObjectState* state, Object* object)
{
    state->vox->set_hitscan(state->init_hitscan);
    state->vox->register_hitscan();
    #ifdef DC_CLIENT
    state->vox->set_draw(state->init_draw);
    #endif
}

// BornUpdateVox and BornUpdateFrozenVox are mutually exclusive
template <class Object>
inline void bornUpdateVox(ObjectState* state, Object* object)
{
    Vec3 p = state->get_position();
    state->vox->update(p.x, p.y, p.z, state->theta, state->phi);
}

template <class Object>
inline void bornUpdateFrozenVox(ObjectState* state, Object* object)
{
    state->vox->thaw();
    Vec3 p = state->get_position();
    state->vox->update(p.x, p.y, p.z, state->theta, state->phi);
    state->vox->freeze();
}

template <class Object>
inline void bornCreateMessage(ObjectState* state, Object* object)
{
    #if DC_SERVER
    object->broadcastCreate();
    #endif
}



