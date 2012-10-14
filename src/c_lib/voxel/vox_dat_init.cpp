#include "vox_dat_init.hpp"

#include <voxel/voxel_loader.hpp>


namespace VoxDats
{

// agent
VoxDat agent;
VoxDat agent_crouched;
VoxDat agent_dead;

// monsters
VoxDat robot_bomb;
VoxDat robot_box;
VoxDat monster_spawner;

// items
VoxDat base;
VoxDat energy_core;
VoxDat agent_spawner;
VoxDat turret;

void init()
{
    // agent
    read_skeleton((char*)"./media/voxel/crouch.skeleton", &agent);
    read_skeleton((char*)"./media/voxel/crouch.skeleton", &agent_crouched);
    
    read_skeleton((char*)"./media/voxel/dead.skeleton", &agent_dead);

    // monsters
    read_skeleton((char*)"./media/voxel/bomb.skeleton", &robot_bomb);
    read_skeleton((char*)"./media/voxel/robot_box.skeleton", &robot_box);
    read_skeleton((char*)"./media/voxel/monster_spawner.skeleton", &monster_spawner);

    // items
    read_skeleton((char*)"./media/voxel/base.skeleton", &base);
    read_skeleton((char*)"./media/voxel/energy_core.skeleton", &energy_core);
    read_skeleton((char*)"./media/voxel/spawner.skeleton", &agent_spawner);
    read_skeleton((char*)"./media/voxel/turret.skeleton", &turret);
}

}   // VoxDats
