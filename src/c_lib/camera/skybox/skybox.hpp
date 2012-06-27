#pragma once

#include <SDL/texture_loader.hpp>

#include <physics/vec3.hpp>
#include <camera/camera.hpp>

#include <animations/common.hpp>

namespace Skybox
{

GLuint star_sheet = 0;

struct STAR
{
    int type;
    float brightness;
    float size;
    //float phi;
    //float theta;

    float x;
    float y;
    float z;

    float tx_min;
    float ty_min;
    float tx_max;
    float ty_max;
};

struct STAR* star_list;
int star_num = 0;

// glsl
class SHADER star_shader;
GLuint star_TexCoord;
GLint star_CameraPos; 
Animations::VertexElementList1* star_vlist = NULL;

void init_shader()
{
    star_shader.set_debug(false);

    star_shader.load_shader( "star shader",
        "./media/shaders/skybox/stars.vsh",
        "./media/shaders/skybox/stars.fsh" );
    star_TexCoord = star_shader.get_attribute("InTexCoord");
    star_CameraPos = star_shader.get_uniform("CameraPos");
}

void init_texture()
{
    SDL_Surface* surface = create_surface_from_file((char*) "./media/texture/skybox/Starsheet2.png");

    GS_ASSERT(surface != NULL);
    if (surface == NULL) return;

    GLenum tex_format = GL_BGRA;
    if (surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &star_sheet);
    glBindTexture(GL_TEXTURE_2D, star_sheet);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(
        GL_TEXTURE_2D,  //target
        0,  // level
        4,  // internalformat, use 4 for 4 bytes per pixel
        surface->w, surface->h, //width, height
        0,  // border
        tex_format, // format
        GL_UNSIGNED_BYTE,   // type
        surface->pixels // pixels
    );

    glDisable(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);
}

void generate_sky()
{
    //printf("generate_sky \n");

    const int N = 1024;
    star_list = new STAR[N];

    float r = 256.0;

    for(int i=0; i < N; i++)
    {
        STAR s;
    /*
        float theta = randf()*(3.14159265);
        float phi = randf()*(3.14159265*2.0);

        //printf("theta= %f\n", theta);

        float x = r*cos(phi)*sin(theta);
        float y = r*sin(phi)*sin(theta);
        float z = r*cos(theta);
    */

        //printf("theta= %f\n", randf());

        float z = 2.0*randf()-1.0;
        float t = randf()*(3.14159265*2.0);

        float _r = sqrt(1- z*z);

        float x = _r * cos(t);
        float y = _r * sin(t);

        s.x = r*x;
        s.y = r*y;
        s.z = r*z;
        
        s.brightness = 0.2 + (0.8)*randf();
        s.size = 1 + 3*randf();
        s.type = rand()%16;

        s.tx_min = (float)(s.type%4)* (1.0/4.0);
        s.tx_max = s.tx_min + (1.0/4.0);
        s.ty_min = (float)(s.type/4)* (1.0/4.0);
        s.ty_max = s.ty_min + (1.0/4.0);

        star_list[i] = s;

    }

    star_num = N;
}

void init()
{
    init_texture();
    generate_sky();
    init_shader();
    star_vlist = new Animations::VertexElementList1;
}

void teardown()
{
    delete star_vlist;
}

void pack_vertex_list()
{
    GS_ASSERT(current_camera != NULL);
    if (current_camera == NULL)
    {
        star_vlist->vertex_number = 0;
        return;
    }

    Vec3 p = current_camera->get_position();
    float cx = p.x; 
    float cy = p.y; 
    float cz = p.z; 

    for(int i=0; i<star_num; i++)
    {
        //STAR s = star_list[i];

        Vec3 v;
        v.x = star_list[i].x + cx;
        v.y = star_list[i].y + cy;
        v.z = star_list[i].z + cz;

        if(point_fulstrum_test2(v.x, v.y, v.z) == false) continue;

        float scale = star_list[i].size / 2.0;

        Vec3 up = vec3_init(
            model_view_matrix[0]*scale,
            model_view_matrix[4]*scale,
            model_view_matrix[8]*scale
        );
        Vec3 right = vec3_init(
            model_view_matrix[1]*scale,
            model_view_matrix[5]*scale,
            model_view_matrix[9]*scale
        );

        Vec3 p = vec3_sub(v, vec3_add(right, up));
        star_vlist->push_vertex(p, star_list[i].tx_min,star_list[i].ty_max);

        p = vec3_add(v, vec3_sub(up, right));
        star_vlist->push_vertex(p, star_list[i].tx_max,star_list[i].ty_max);

        p = vec3_add(v, vec3_add(up, right));
        star_vlist->push_vertex(p, star_list[i].tx_max,star_list[i].ty_min);

        p = vec3_add(v, vec3_sub(right, up));
        star_vlist->push_vertex(p, star_list[i].tx_min,star_list[i].ty_min);
    }
}

void prep_skybox()
{
    GS_ASSERT(star_vlist != NULL);
    pack_vertex_list();
    star_vlist->buffer();
}

void draw()
{
    GS_ASSERT(current_camera != NULL);
    if (current_camera == NULL) return;

    GS_ASSERT(star_sheet != 0);
    if (star_sheet == 0) return;

    if (star_vlist->vertex_number <= 0) return;

    GS_ASSERT(star_vlist->VBO != 0);
    if (star_vlist->VBO == 0) return;
    
    glBindBuffer(GL_ARRAY_BUFFER, star_vlist->VBO);

    glColor3ub(255,255,255);

    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);
   
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE, GL_ONE);

    glDepthMask(GL_FALSE);
    
    glBindTexture(GL_TEXTURE_2D, star_sheet);

    glUseProgramObjectARB(star_shader.shader);

    Vec3 p = current_camera->get_position();
    glUniform3f(star_CameraPos, p.x, p.y, p.z);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(star_TexCoord);
    glVertexPointer(3, GL_FLOAT, star_vlist->stride, (GLvoid*)0);
    glVertexAttribPointer(star_TexCoord, 2, GL_FLOAT, GL_FALSE, star_vlist->stride, (GLvoid*)12);

    glDrawArrays(GL_QUADS, 0, star_vlist->vertex_number);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(star_TexCoord);
    glUseProgramObjectARB(0);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

}

extern "C"
{
    void add_star(int type, float brightness, float size, float phi, float theta)
    {

    }

    void clear_stars()
    {
        Skybox::star_num = 0;
    }
}
