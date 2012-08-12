#pragma once


namespace t_map
{ 

void init_shaders();

void set_map_shader_0();

void toggle_3d_texture_settings();
void init_map_3d_texture();

void set_map_shader_0_compatibility(int level);
void init_map_3d_texture_compatibility();
void init_block_texture_normal();
//void init_block_item_sheet();

const int num_map_shaders = 1;

GLenum map_vert_shader[num_map_shaders] = {0};
GLenum map_frag_shader[num_map_shaders] = {0};
GLenum map_shader[num_map_shaders] = {0};

//shader 0
int map_Vertex;
int map_TexCoord;
int map_RGB;
int map_Normal;

int map_ChunkPosition;   //uniform
int map_NormalArray;     //uniform

int map_LightMatrix;

//texture
extern SDL_Surface *terrain_map_surface;
extern GLuint terrain_map_texture;

extern GLuint block_textures_compatibility; //use for intel mode

void teardown_shader(); // frees surface

}   // t_map
