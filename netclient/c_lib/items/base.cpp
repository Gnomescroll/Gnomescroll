#include "base.hpp"

VoxBody base_vox_dat;

void Base::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&base_vox_dat);
    printf("Base: %0.2f %0.2f %0.2f\n", x,y,z);
}

void Base::set_color(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

Base::Base()
:
theta(0), phi(0),
type(OBJ_TYPE_BASE)
{
    this->vox = new Base_vox(this, &base_vox_dat);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Base::~Base()
{
    if (this->vox != NULL) free(this->vox);
}
