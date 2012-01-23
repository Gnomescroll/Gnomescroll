#include "flag.hpp"

VoxBody flag_vox_dat;

void Flag::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&flag_vox_dat);
}

Flag::Flag()
:
theta(0), phi(0),
id(0),
team(0),
type(OBJ_TYPE_FLAG)
{
    this->vox = new Flag_vox(this);
    this->vox->set_hitscan(false);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Flag::~Flag()
{
    if (this->vox != NULL) delete this->vox;
}
