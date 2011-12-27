#pragma once

namespace vn {

void init();
void set_pixels(double z=0.5);
void draw();

//cython
void draw_vn();
void generate_frames(float seconds);
}
