#include "base.hpp"

VoxBody base_vox_dat;

void Base::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&base_vox_dat, x,y,z, this->theta, this->phi);
}

Base::Base()
:
theta(0), phi(0),
team(0),
type(OBJ_TYPE_BASE)
{
    this->vox = new Object_vox(BASE_PART_NUM);
    this->vox->set_hitscan(false);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Base::Base(int id, int team)
:
theta(0), phi(0),
id(id), team(team),
type(OBJ_TYPE_BASE)
{
    this->vox = new Object_vox(BASE_PART_NUM, &base_vox_dat, id, type, team);
    this->vox->set_hitscan(false);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Base::~Base()
{
    if (this->vox != NULL) delete this->vox;
}
