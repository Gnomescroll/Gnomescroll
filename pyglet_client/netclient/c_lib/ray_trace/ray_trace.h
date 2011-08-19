
#include <stdio.h>
#include <math.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1);
int ray_cast_lax(float x0,float y0,float z0, float x1,float y1,float z1);

int* _ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1, float* distance);
