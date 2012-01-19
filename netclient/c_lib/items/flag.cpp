#include "flag.hpp"

VoxBody flag_vox_dat;

void Flag::set_position(float x, float y, float z) {
    printf("Flag: %0.2f %0.2f %0.2f\n", x,y,z);
    this->x = x;
    this->y = y;
    this->z = z;
}

Flag::Flag()
:
type(OBJ_TYPE_FLAG)
{
    this->vox = new Flag_vox(this, &flag_vox_dat);
}

Flag::~Flag()
{
    if (this->vox != NULL) free(this->vox);
}
