#include "base.hpp"

VoxBody base_vox_dat;

void Base::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&base_vox_dat);
}

Base::Base()
:
theta(0), phi(0),
team(0),
type(OBJ_TYPE_BASE)
{
    this->vox = new Base_vox(this);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Base::~Base()
{
    if (this->vox != NULL) delete this->vox;
}
