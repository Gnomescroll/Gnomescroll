#include "skybox.hpp"

#include <SDL/texture_loader.hpp>
#include <camera/camera.hpp>
#include <physics/vec3.hpp>
#include <animations/common.hpp>

namespace Skybox
{

const int NORTH_STAR_TYPE = 16;
GLuint star_sheet = 0;

struct STAR* star_list = NULL;
int star_num = 1024;

// glsl
class Shader star_shader;
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
    //star_CameraPos = star_shader.get_uniform("CameraPos");
}

void init_texture()
{
    SDL_Surface* surface = create_surface_from_file("./media/sprites/skybox/stars_and_north.png");

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
    GS_ASSERT(star_num > 0);
    if (star_num <= 0) return;
    star_list = new STAR[star_num];

    float r = 256.0f;

    for(int i=0; i < star_num-1; i++)
    {
        STAR s;

        float z = 2.0f*randf()-1.0f;
        float t = randf()*(3.14159265f*2.0f);

        float _r = sqrtf(1- z*z);

        float x = _r * cosf(t);
        float y = _r * sinf(t);

        s.x = r*x;
        s.y = r*y;
        s.z = r*z;
        
        s.brightness = 0.2f + (0.8f)*randf();
        s.size = 1 + 3*randf();
        
        s.type = rand()%16;
        s.tx_min = (float)(s.type%4) * (1.0f/4.0f);
        s.tx_max = s.tx_min + (1.0f/4.0f);
        s.ty_min = (float)(s.type/4) * (1.0f/4.0f) * 0.5f;
        s.ty_max = s.ty_min + (1.0f/4.0f) * 0.5f;
        
        star_list[i] = s;
    }

    // add north star
    STAR s;
    float t = 1.5f;
    float z = 0.75f;
    float _r = sqrtf(1.0f - z*z);
    float x = _r * cosf(t);
    float y = _r * sinf(t);
    s.size = 12.0f;
    s.x = r * x;
    s.y = r * y;
    s.z = r * z;
    s.brightness = 1.0f;
    s.type = NORTH_STAR_TYPE;
    s.tx_min = 0.0f;
    s.tx_max = 1.0f;
    s.ty_min = 0.5f;
    s.ty_max = 1.0f;
    star_list[star_num-1] = s;

    star_num = star_num;
}

void init()
{
    init_texture();
    generate_sky();
    init_shader();
    GS_ASSERT(star_vlist == NULL);
    star_vlist = new Animations::VertexElementList1;
}

void teardown()
{
    if (star_vlist != NULL) delete star_vlist;
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

    for(int i=0; i<star_num-1; i++)
    {
        Vec3 v;
        v.x = star_list[i].x + cx;
        v.y = star_list[i].y + cy;
        v.z = star_list[i].z + cz;

        if (!point_fulstrum_test_no_view_distance(v.x, v.y, v.z)) continue;

        float scale = star_list[i].size / 2.0f;

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
    
    // north star
    STAR s = star_list[star_num-1];
    Vec3 v;
    v.x = s.x + cx;
    v.y = s.y + cy;
    v.z = s.z + cz;
    
    float scale = s.size / 2.0f;

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

    // align the star to the point that it was created at
    v = vec3_sub(v, up);
    v = vec3_sub(v, right);

    Vec3 pt1 = vec3_sub(v, vec3_add(right, up));
    Vec3 pt2 = vec3_add(v, vec3_sub(up, right));
    Vec3 pt3 = vec3_add(v, vec3_add(up, right));
    Vec3 pt4 = vec3_add(v, vec3_sub(right, up));
    
    if (
        !point_fulstrum_test_no_view_distance(pt1.x, pt1.y, pt1.z) &&
        !point_fulstrum_test_no_view_distance(pt2.x, pt2.y, pt2.z) &&
        !point_fulstrum_test_no_view_distance(pt3.x, pt3.y, pt3.z) &&
        !point_fulstrum_test_no_view_distance(pt4.x, pt4.y, pt4.z)
    ) return;

    star_vlist->push_vertex(pt1, s.tx_min,s.ty_max);
    star_vlist->push_vertex(pt2, s.tx_max,s.ty_max);
    star_vlist->push_vertex(pt3, s.tx_max,s.ty_min);
    star_vlist->push_vertex(pt4, s.tx_min,s.ty_min);
}

void prep_skybox()
{
    pack_vertex_list();
    GS_ASSERT(star_vlist != NULL);
    if (star_vlist == NULL) return;
    star_vlist->buffer();
}

void draw()
{
    if (!star_shader.shader_valid) return;
    
    GS_ASSERT(current_camera != NULL);
    if (current_camera == NULL) return;

    GS_ASSERT(star_sheet != 0);
    if (star_sheet == 0) return;
    
    GS_ASSERT(star_vlist != NULL);
    if (star_vlist == NULL) return;

    if (star_vlist->vertex_number <= 0) return;
    
    glBindBuffer(GL_ARRAY_BUFFER, star_vlist->VBO);

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, false);
    GL_ASSERT(GL_BLEND, true);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE, GL_ONE);
    
    glBindTexture(GL_TEXTURE_2D, star_sheet);

    glUseProgramObjectARB(star_shader.shader);

    //Vec3 p = current_camera->get_position();
    //glUniform3f(star_CameraPos, p.x, p.y, p.z);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(star_TexCoord);
    glVertexPointer(3, GL_FLOAT, star_vlist->stride, (GLvoid*)0);
    glVertexAttribPointer(star_TexCoord, 2, GL_FLOAT, GL_FALSE, star_vlist->stride, (GLvoid*)12);

    glDrawArrays(GL_QUADS, 0, star_vlist->vertex_number);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(star_TexCoord);
    glUseProgramObjectARB(0);

    glDisable(GL_TEXTURE_2D);

}

}    // Skybox
