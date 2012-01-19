#include "base.hpp"

VoxBody base_vox_dat;

void Base::set_position(float x, float y, float z) {
    printf("Base: %0.2f %0.2f %0.2f\n", x,y,z);
    this->x = x;
    this->y = y;
    this->z = z;
}

void Base::set_color(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

Base::Base()
:
type(OBJ_TYPE_BASE)
{
    this->vox = new Base_vox(this, &base_vox_dat);
}

Base::~Base()
{
    if (this->vox != NULL) free(this->vox);
}
