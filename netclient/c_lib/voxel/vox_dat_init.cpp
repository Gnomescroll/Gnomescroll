#include "vox_dat_init.hpp"

#include <c_lib/voxel/voxel_loader.hpp>

// dats
#include <c_lib/agent/agent_vox.hpp>
#include <c_lib/monsters/monsters.hpp>
#include <c_lib/items/items.hpp>

void init_vox_dats()
{
    read_skeleton((char*)"./media/voxel/saves/agent.skeleton", &agent_vox_dat);
    read_skeleton((char*)"./media/voxel/saves/crouch.skeleton", &agent_vox_dat_crouched);
    read_skeleton((char*)"./media/voxel/saves/dead.skeleton", &agent_vox_dat_dead);
    //read_skeleton((char*)"./media/voxel/slime.skeleton", &Monsters::slime_vox_dat);
    read_skeleton((char*)"./media/voxel/bomb.skeleton", &Monsters::slime_vox_dat);
    read_skeleton((char*)"./media/voxel/robot_box.skeleton", &Monsters::box_vox_dat);
    //read_skeleton((char*)"./media/voxel/flag.skeleton", &flag_vox_dat);
    read_skeleton((char*)"./media/voxel/dbag_flag.skeleton", &flag_vox_dat);
    read_skeleton((char*)"./media/voxel/base.skeleton", &base_vox_dat);
    read_skeleton((char*)"./media/voxel/spawner.skeleton", &spawner_vox_dat);
    read_skeleton((char*)"./media/voxel/turret.skeleton", &turret_vox_dat);
}
