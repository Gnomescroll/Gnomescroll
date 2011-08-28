#ifndef map_get_density_h
#define map_get_density_h

#include <math.h>

#include <compat.h>
#include <compat_gl.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>


struct Map_density_element {
    float density;

};

void init_map_gen_density();
void update_density_map(int iterations);
void map_density_visualize(int density, float min, float max);

#endif
