#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_image.h> //temp?

#include <stdio.h>


struct Vertex {
    float x,y,z;
    float tx,ty;
    unsigned char r,g,b,a;
};

struct Quad {
  struct Vertex vertex[4];
};


//int _bind_VBO(struct Quad* quad_list, int v_num);

int _init_draw_terrain();

int _create_vbo(struct Quad* quad_list, int v_num);
int _delete_vbo(unsigned int VBO_id);
int _draw_vbo(unsigned int VBO_id, int v_num);
