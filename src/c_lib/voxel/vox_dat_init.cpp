#include "vox_dat_init.hpp"

#include <voxel/voxel_loader.hpp>

namespace VoxDats
{

// agent
Voxels::VoxDat agent;
Voxels::VoxDat agent_crouched;
Voxels::VoxDat agent_dead;

// monsters
Voxels::VoxDat robot_bomb;
Voxels::VoxDat robot_box;
Voxels::VoxDat monster_spawner;
Voxels::VoxDat slime;

// items
Voxels::VoxDat base;
Voxels::VoxDat energy_core;
Voxels::VoxDat agent_spawner;
Voxels::VoxDat turret;

void init()
{
    // agent
    read_skeleton(MEDIA_PATH "voxel/crouch.skeleton", &agent);
    read_skeleton(MEDIA_PATH "voxel/crouch.skeleton", &agent_crouched);

    read_skeleton(MEDIA_PATH "voxel/dead.skeleton", &agent_dead);

    // monsters
    read_skeleton(MEDIA_PATH "voxel/bomb.skeleton", &robot_bomb);
    read_skeleton(MEDIA_PATH "voxel/robot_box.skeleton", &robot_box);
    read_skeleton(MEDIA_PATH "voxel/monster_spawner.skeleton", &monster_spawner);
    read_skeleton(MEDIA_PATH "voxel/slime.skeleton", &slime);

    // items
    read_skeleton(MEDIA_PATH "voxel/base.skeleton", &base);
    read_skeleton(MEDIA_PATH "voxel/energy_core.skeleton", &energy_core);
    read_skeleton(MEDIA_PATH "voxel/spawner.skeleton", &agent_spawner);
    read_skeleton(MEDIA_PATH "voxel/turret.skeleton", &turret);
}

}   // VoxDats
