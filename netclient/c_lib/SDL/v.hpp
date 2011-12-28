#pragma once

namespace vn {

void init();
void set_pixels(double z=0.5);
void draw();

//cython
void init_vn(int w, int h, int grad) ;
void set_params(double dz, double frequency, int seed, bool distance, bool turbulence_enabled, double turbulence_frequency, double turbulence_power);
//void set_params(double dz, double frequency, int seed, bool distance, double turbulence_frequency, double turbulence_power);
void draw_vn();
void generate_frames(float seconds);
}
