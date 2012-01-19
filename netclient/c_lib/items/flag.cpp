#include "flag.hpp"

VoxBody flag_vox_dat;

void Flag::draw() {
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
