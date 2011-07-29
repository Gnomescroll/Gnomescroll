#ifdef linux
    //do nothing
#elif _WIN32
    #include <windows.h>
    #include <GL/GLee.h>
#else
    //mac
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_image.h> //temp?

#include <stdio.h>


#import "types.h"

/*
struct Vertex {
    float x,y,z;
    float tx,ty;
    unsigned char r,g,b,a;
}

struct VBO {
    int v_num;
    Vertex* vlist;
    int VBO_id;
}
*/

//int _bind_VBO(struct Quad* quad_list, int v_num);

int _init_draw_terrain();

int _create_vbo(struct VBO* q_VBO, struct Vertex* vlist, int v_num);
int _delete_vbo(struct VBO* q_VBO);

int _start_vbo_draw();
int _draw_quad_vbo(struct VBO* q_VBO);
int _end_vbo_draw();
