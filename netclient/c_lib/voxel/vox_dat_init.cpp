#include "vox_dat_init.hpp"

#include <c_lib/voxel/voxel_loader.hpp>

// dats
#include <c_lib/agent/agent_vox.hpp>
#include <c_lib/monsters/monsters.hpp>
#include <c_lib/items/items.hpp>

void init_vox_dats()
{
    //read_skeleton((char*)"./media/voxel/agent.skeleton", &agent_vox_dat);
    //read_skeleton((char*)"./media/voxel/robot.skeleton", &agent_vox_dat);
    //read_skeleton((char*)"./media/voxel/robot1/robot1.skeleton", &agent_vox_dat);
    read_skeleton((char*)"./media/voxel/saves/2.skeleton", &agent_vox_dat);
    read_skeleton((char*)"./media/voxel/saves/crouch2.skeleton", &agent_vox_dat_crouched);
    read_skeleton((char*)"./media/voxel/saves/dead1.skeleton", &agent_vox_dat_dead);
    read_skeleton((char*)"./media/voxel/slime.skeleton", &Monsters::slime_vox_dat);
    //read_skeleton((char*)"./media/voxel/flag.skeleton", &flag_vox_dat);
    read_skeleton((char*)"./media/voxel/dbag_flag.skeleton", &flag_vox_dat);
    read_skeleton((char*)"./media/voxel/base.skeleton", &base_vox_dat);
    read_skeleton((char*)"./media/voxel/spawner.skeleton", &spawner_vox_dat);
}
