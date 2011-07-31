#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_image.h> //temp?

#include <stdio.h>


#ifdef linux
    //do nothing
#elif _WIN32
    #include <windows.h>
    #include <GL/GLee.h>
#else
    //mac
#endif

#ifndef t_map
    #include "t_map.h"
#endif



#ifndef t_vbo
#define t_vbo


int _test3(int x, int y, int z);


    struct Vertex {
        float x,y,z;
        float tx,ty;
        unsigned char r,g,b,a;
    };

    struct VBO {
        int v_num;
        struct Vertex* v_list;
        int VBO_id;
    };

int _init_draw_terrain();

int _update_chunks();

int create_vbo(struct VBO* q_VBO, struct Vertex* vlist, int v_num);
int delete_vbo(struct VBO* q_VBO);

int start_vbo_draw();
int draw_quad_vbo(struct VBO* q_VBO);
int end_vbo_draw();

//internal
//int update_column_VBO(struct vm_column* column);
void inline add_quad(float x,float y,float z,int side, int tile_id);
int _draw_terrain();

#endif
