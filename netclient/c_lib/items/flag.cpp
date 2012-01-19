#include "flag.hpp"

VoxBody flag_vox_dat;

void Flag::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&flag_vox_dat);
    printf("Flag: %0.2f %0.2f %0.2f\n", x,y,z);
}

Flag::Flag()
:
theta(0), phi(0),
type(OBJ_TYPE_FLAG)
{
    this->vox = new Flag_vox(this, &flag_vox_dat);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Flag::~Flag()
{
    if (this->vox != NULL) free(this->vox);
}
