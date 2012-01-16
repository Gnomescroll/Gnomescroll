#include "base.hpp"

void Base::draw() {
}

void Base::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Base::set_color(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}
