#pragma once

#include <c_lib/voxel/voxel_model.hpp>

// BornVox and BornTeamVox are mutually exclusive
inline Voxel_model* bornVox(VoxDat* vox_dat, int id, ObjectType type)
{
    return new Voxel_model(vox_dat, id, type);
}

inline Voxel_model* bornTeamVox(VoxDat* vox_dat, int id, ObjectType type, int team)
{
    return new Voxel_model(vox_dat, id, type, team);
}

// Initializes vox model properties. almost always desired
inline void bornSetVox(Voxel_model* vox, bool init_hitscan, bool init_draw)
{
    vox->set_hitscan(init_hitscan);
    vox->register_hitscan();
    #if DC_CLIENT
    vox->set_draw(init_draw);
    #endif
}

// BornUpdateVox and BornUpdateFrozenVox are mutually exclusive
inline void bornUpdateVox(Voxel_model* vox, Vec3 position, float theta, float phi)
{
    vox->update(position.x, position.y, position.z, theta, phi);
}

inline void bornUpdateFrozenVox(Voxel_model* vox, Vec3 position, float theta, float phi)
{
    vox->thaw();
    vox->update(position.x, position.y, position.z, theta, phi);
    vox->freeze();
}
