#ifndef t_properties
#define t_properties

#include <stdio.h>
#include <stdlib.h>

// cube properties

#define max_cubes 1024

struct cubeProperties {
    int active;
    int occludes;
    int solid;
    int gravity;
    int transparent;
};

struct cubeProperties* _get_cube_list();
struct cubeProperties* _get_cube(int id);

int inline _isActive(int id);

/*
//buffer

struct Vertex* _get_quad_cache();
*/
#endif
