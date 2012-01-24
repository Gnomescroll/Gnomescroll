#include "flag.hpp"

VoxBody flag_vox_dat;

void Flag::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&flag_vox_dat, x,y,z, this->theta, this->phi);
}

Flag::Flag()
:
theta(0), phi(0),
id(0),
team(0),
type(OBJ_TYPE_FLAG)
{
    this->vox = new Object_vox(FLAG_PART_NUM);
    this->vox->set_hitscan(false);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Flag::Flag(int id, int team)
:
theta(0), phi(0),
id(id), team(team),
type(OBJ_TYPE_FLAG)
{
    this->vox = new Object_vox(FLAG_PART_NUM, &flag_vox_dat, id, type, team);
    this->vox->set_hitscan(false);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Flag::~Flag()
{
    if (this->vox != NULL) delete this->vox;
}
