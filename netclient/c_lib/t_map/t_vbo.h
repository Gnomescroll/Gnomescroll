#pragma once

#include <compat_gl.h>

#include <stdio.h>

//#include "t_map.h"
//#include "t_properties.h"

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

    struct Vertex {
        float x,y,z;
        float tx,ty;
        unsigned char r,g,b,a;
        char normal[4];
    };



int _init_draw_terrain();

int _update_chunks();

int create_vbo(struct VBO* q_VBO, struct Vertex* vlist, int v_num);
void delete_vbo(struct VBO* q_VBO);

int start_vbo_draw();
int draw_quad_vbo(struct VBO* q_VBO);
int end_vbo_draw();

int __inline chunk_render_check(int x_off, int y_off, int tolerance);
int _set_camera(float x, float y, float z, float vx, float vy, float vz, float ux, float uy, float uz, float ratio, float viewangle);
int _set_fulstrum_culling(int value);
int _set_view_distance(int vd);

int* _chunk_request();

//internal
//int update_column_VBO(struct vm_column* column);
void __inline add_quad(float x,float y,float z,int side, int tile_id);
int _draw_terrain();

struct Vertex* _get_quad_cache();

void _toggle_terrain_map_blend_mode();
void _refresh_map_vbo();
void _toggle_z_buffer();

void DRAW_VBOS1();
void DRAW_VBOS1a();
void DRAW_VBOS2(); 
void DRAW_VBOS2b(); 