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

int map_Light;
//texture
extern SDL_Surface *terrain_map_surface;
extern GLuint terrain_map_texture;

extern GLuint block_textures_compatibility; //use for intel mode

void teardown_shader(); // frees surface


class MapShader
{
    public:
    int terrain_map_glsl;
    class SHADER* shader;

    //uniforms

    //attributes
    int Vertex;
    int TexCoord;
    int RGB;
    int map_LightMatrix;
    int map_Light;
    //int Normal;

    MechListShader()
    : terrain_map_surface(NULL), terrain_map_texture(0), terrain_map_glsl(0), shader(NULL)
    {
        init_texture();
        init_shader();
    }

    ~MechListShader()
    {
        if (s != NULL) SDL_FreeSurface(s);
        if (shader != NULL) delete shader;
    }

    void init_shader()
    {
        shader = new SHADER;
        shader->set_debug(true);

        shader->load_shader( "map_shader",
            "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.vsh",
            "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.fsh" );

        TexCoord    =       shader->get_attribute("InTexCoord");
        Brightness  =       shader->get_attribute("InBrightness");

        InVertex =     shader->get_attribute("InVertex");
        InTexCoord =   shader->get_attribute("InTexCoord");
        InRGB =        shader->get_attribute("InRGB");
        InLightMatrix= shader->get_attribute("InLightMatrix");
        InLight=       shader->get_attribute("InLight");

        //int Normal=      shader->get_attribute("InTexCoord");
    }

    void init_texture();

};


class MapCompatibilityShader
{
    public:

    SDL_Surface* terrain_map_surface;
    int terrain_map_texture;

    int terrain_map_glsl;
    //unsigned int texture1;
    class SHADER* shader;

    //uniforms

    //attributes
    int Vertex;
    int TexCoord;
    int RGB;
    int map_LightMatrix;
    int map_Light;
    //int Normal;

    MechListShader()
    : terrain_map_surface(NULL), terrain_map_texture(0), terrain_map_glsl(0), shader(NULL)
    {
        init_texture();
        init_shader();
    }

    ~MechListShader()
    {
        if (s != NULL) SDL_FreeSurface(s);
        if (shader != NULL) delete shader;
    }

    void init_shader()
    {
        shader = new SHADER;
        shader->set_debug(true);

        shader->load_shader( "map_shader",
            "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.vsh",
            "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.fsh" );

        TexCoord    =       shader->get_attribute("InTexCoord");
        Brightness  =       shader->get_attribute("InBrightness");

        InVertex =     shader->get_attribute("InVertex");
        InTexCoord =   shader->get_attribute("InTexCoord");
        InRGB =        shader->get_attribute("InRGB");
        InLightMatrix= shader->get_attribute("InLightMatrix");
        InLight=       shader->get_attribute("InLight");

        //int Normal=      shader->get_attribute("InTexCoord");
    }

    void init_texture();

};



}   // t_map
