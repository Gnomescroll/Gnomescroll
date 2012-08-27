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

//texture
extern SDL_Surface *terrain_map_surface;
extern GLuint terrain_map_texture;

extern GLuint block_textures_compatibility; //use for intel mode

void teardown_shader(); // frees surface


class MapShader
{
    public:
    GLuint terrain_map_glsl;
    class SHADER* shader;

    //uniforms
    int InOffset;

    //attributes
    int InVertex;
    int InTexCoord;
    int InRGB;
    int InLightMatrix;
    int InLight;
    //int Normal;

    MapShader()
    : terrain_map_glsl(0), shader(NULL)
    {}

    ~MapShader()
    {
        if (shader != NULL) delete shader;
    }

    void init_shader()
    {
        GS_ASSERT(this->shader == NULL);
        if (this->shader != NULL) delete this->shader;
        this->shader = new SHADER;
        this->shader->set_debug(true);

        this->shader->load_shader( "map_shader",
            "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.vsh",
            "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.fsh" );
        //uniform
        InOffset  = shader->get_uniform("InOffset");
        //attribute
        InVertex =     shader->get_attribute("InVertex");
        InTexCoord =   shader->get_attribute("InTexCoord");
        InRGB =        shader->get_attribute("InRGB");
        InLightMatrix= shader->get_attribute("InLightMatrix");
        InLight=       shader->get_attribute("InLight");

        //int Normal=      shader->get_attribute("InTexCoord");
    }

    void init_texture();

};

extern class MapShader map_shader;

class MapCompatibilityShader
{
    public:

    SDL_Surface* terrain_map_surface;
    int terrain_map_texture;

    int terrain_map_glsl;
    //unsigned int texture1;
    class SHADER* shader;

    //uniforms
    int InOffset;

    //attributes
    int InVertex;
    int InTexCoord;
    int InTexCoord2;
    int InRGB;
    int InLightMatrix;
    int InLight;
    //int Normal;

    MapCompatibilityShader()
    : terrain_map_surface(NULL), terrain_map_texture(0), terrain_map_glsl(0), shader(NULL)
    {

    }
    
    ~MapCompatibilityShader()
    {
        if (shader != NULL) delete shader;
    }

    void init_shader(int level)
    {
        if (this->shader != NULL) delete this->shader;
        this->shader = new SHADER;
            this->shader->set_debug(true);

        static bool tried_mesa = false;
        if (!tried_mesa && level == 0)
        {
            bool mesa = false;
            if (Options::mesa_shader)
            {
                const char* gl_v = (const char*)glGetString(GL_VERSION);
                if (strstr(gl_v, "Mesa") != NULL)
                {
                    printf("%s: using Mesa driver shader\n", __FUNCTION__);
                    mesa = true;
                }
            }
            if (mesa) level = 2;
            tried_mesa = mesa;
        }

        if (level == 0)
        {
            shader->load_shader( "map_compatibility_shader level 0",
                "./media/shaders/terrain/terrain_map_bilinear_ao_compatibility.vsh",
                "./media/shaders/terrain/terrain_map_bilinear_ao_compatibility.fsh" );
        }
        else if (level ==1)
        {
            shader->load_shader( "map_compatibility_shader level 1",
                "./media/shaders/terrain/terrain_map_bilinear_ao_compatibility_backup.vsh",
                "./media/shaders/terrain/terrain_map_bilinear_ao_compatibility_backup.fsh" );
        }
        else if (level == 2)
        {
            shader->load_shader( "mesa map_compatibility_shader",
                "./media/shaders/terrain/terrain_map_mesa.vsh",
                "./media/shaders/terrain/terrain_map_mesa.fsh" );
        }


        InVertex =     shader->get_attribute("InVertex");
        InTexCoord =   shader->get_attribute("InTexCoord");
        InTexCoord2 =  shader->get_attribute("InTexCoord2");
        InRGB =        shader->get_attribute("InRGB");
        InLightMatrix= shader->get_attribute("InLightMatrix");
        InLight=       shader->get_attribute("InLight");
        //int Normal=      shader->get_attribute("InTexCoord");
    }

    void init_texture();

};

extern class MapCompatibilityShader map_compatibility_shader;


}   // t_map
