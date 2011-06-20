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

struct Quad_VBO {
    int v_num;
    struct Quad* quad_array;
    int VBO_id;
};

//int _bind_VBO(struct Quad* quad_list, int v_num);

int _init_draw_terrain();

int _create_vbo(struct Quad_VBO* q_VBO, struct Quad* quad_list, int v_num);
int _delete_vbo(struct Quad_VBO* q_VBO);
int _draw_vbo(struct Quad_VBO* q_VBO);
