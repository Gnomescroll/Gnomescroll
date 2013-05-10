#pragma once

#include <t_mech/_interface.hpp>
#include <t_mech/mech_state.hpp>
#include <t_mech/_interface.hpp>

#include <t_mech/config/_interface.hpp>
#include <t_mech/mesh_loader.hpp>

namespace t_mech
{

//class MechListVertexList
//class MechListShader
//class MechListRenderer

/*
    Use this as example for other shaders
*/

class MechListVertexList
{
    public:
        // visibility will default to private unless you specify it
        struct Vertex
        {
            float x,y,z; //0
            float tx,ty; //12
            float btx,bty; //20  //texture cordinates for bilinear interpolation
            unsigned char color[4]; //28
            unsigned char brightness[4]; //32
            unsigned char light[4]; //36

        };

        static const int stride = sizeof(Vertex);

        struct Vertex* va;  //vertex array
        int vi; //vertex index
        int vm; //vertex max

        unsigned int VBO; //for drawing

        struct Vertex v; //set this and then push vertex

    MechListVertexList() :
        va(NULL), vi(0), vm(32), VBO(0)
    {
        this->va = (struct Vertex*)malloc(vm*sizeof(struct Vertex));
        memset(&this->v, 0, sizeof(this->v));
    }

    ~MechListVertexList()
    {
        free(va);
    }


    void vertex3f(float x, float y, float z)
    {
        v.x = x;
        v.y = y;
        v.z = z;
    }

    void tex2f(float tx, float ty)
    {
        v.tx = tx;
        v.ty = ty;
    }

    void light(int l0, int l1)
    {
        v.light[0] = 8 + l0*16;
        v.light[1] = 8 + l1*16;
    }

    void push_vertex()
    {
        va[vi] = v;
        vi++;
        if (vi == vm)
        {
            vm *= 2;
            va = (struct Vertex*) realloc(va, vm*sizeof(struct Vertex));
        }
    }

    void reset()
    {
        vi = 0;
    }

    void buffer()
    {
        if (VBO == 0) glGenBuffers(1, &VBO);

        if (vi != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vi*stride, NULL, GL_STATIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, vi*stride, va, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else
        {
            if (vi > 0)
            {
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        }
    }

};



class MechListShader
{
    public:

    SDL_Surface* s;

    unsigned int texture1;
    class Shader* shader;

    //uniforms
    int CameraPosition;
    //attributes
    int TexCoord;
    int Brightness;
    int InLight;

    MechListShader()
    : s(NULL), shader(NULL)
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
        shader = new Shader;
        shader->set_debug(true);

        shader->load_shader("mech_list_shader",
            MEDIA_PATH "shaders/effect/mech_list.vsh",
            MEDIA_PATH "shaders/effect/mech_list.fsh");

        CameraPosition =    shader->get_uniform("CameraPosition");

        TexCoord    =       shader->get_attribute("InTexCoord");
        Brightness  =       shader->get_attribute("InBrightness");
        InLight     =       shader->get_attribute("InLight");
    }

    void init_texture()
    {
        s = create_surface_from_file(MEDIA_PATH "sprites/mech/mech_00.png");

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        GLenum texture_format = get_texture_format(s);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
        glDisable(GL_TEXTURE_2D);

        init_sprite_meta();
    }

    void init_sprite_meta()
    {
        //int mech_sprite_width[256];  //centered
        //int mech_sprite_height[256]; //from bottom

        for (int i=0; i<256; i++)
        {
            mech_sprite_width[i]  = -1;
            mech_sprite_height[i] = -1;
        }

        SDL_LockSurface(s);

        unsigned char *pixels = (unsigned char *)s->pixels;
        GS_ASSERT(s->w == 16*16 && s->h == 16*16);

        for (int i=0; i<256; i++)
        {
            int w = i % 16;
            int h = i / 16;

            int j;

            for (j=0; j<16; j++) //column
            {
                int offset = 256*16*h + 16*w;
                offset += j;

                for (int k=0; k<16; k++) //iterate over column
                {
                    int index = offset + k*256;

                    if (pixels[4*index + 3] > 128)
                    {
                        //empty_column = true;

                        int width = 2*(8-j);
                        if (width > mech_sprite_width[i])
                        {
                            mech_sprite_width[i] = width;

                            //printf("sprite1: %i %i width: %i k: %i \n", h,w, mech_sprite_width[i], k);
                        }
                        break;
                    }
                }
            }

            for (j=0; j<16; j++) //column
            {
                int offset = 256*16*h + 16*w;
                offset += 15-j;

                for (int k=0; k<16; k++) //iterate over column
                {
                    int index = offset + k*256;
                    GS_ASSERT(index < 256*256);

                    if (pixels[4*index + 3] > 128)
                    {
                        //empty_column = true;

                        int width = 2*(8-j);
                        if (width > mech_sprite_width[i])
                        {
                            mech_sprite_width[i] = width;

                            //printf("sprite2: %i %i width: %i k: %i \n", h,w, mech_sprite_width[i], k);
                        }
                        break;
                    }
                }
            }

            if (mech_sprite_width[i] == -1)
                continue;

            bool empty_row = false;

            for (j=0; j<16; j++) //row
            {
                int offset = 256*16*h + 16*w;
                offset += 256*j;

                for (int k=0; k<16; k++) //iterate over row
                {
                    int index = offset + k;
                    GS_ASSERT(index < 256*256);

                    if (pixels[4*index + 3] > 128)
                    {
                        empty_row = true;
                        int height = 16-j;

                        mech_sprite_height[i] = height;
                        //printf("sprite3: %i %i height: %i k: %i \n", h,w, height, k);
                        break;
                    }
                }

                if (empty_row)
                {
                    for (int _k=0; _k<16; _k++)
                    {
                        int index2 = offset + _k;
                        pixels[4*index2 + 3] = 255;
                        pixels[4*index2 + 2] = 255;
                    }

                    break;
                }

            }

        }


        SDL_UnlockSurface(s);

        for (int i=0; i<256; i++)
        {
            mech_sprite_width_f[i]  = 1.0;
            mech_sprite_height_f[i] = 1.0;
        }

        for (int i=0; i<256; i++)
        {
            if (mech_sprite_width[i] != -1)
                mech_sprite_width_f[i] = ((float) mech_sprite_width[i]) / 16.0f;
            if (mech_sprite_height[i] != -1)
                mech_sprite_height_f[i] = ((float) mech_sprite_height[i]) / 16.0f;
        }

    }
};


class MechListRenderer
{
    public:

    class MechListShader shader;
    class MechListVertexList vertex_list;

    void draw()
    {

        //printf("draw: %i \n", vertex_list.vi);

        if (vertex_list.vi == 0) return;
        if (!shader.shader->shader_valid) return;

        //printf("draw \n");

        glColor4ub(255,255,255,255);

        GL_ASSERT(GL_BLEND, false);
        GL_ASSERT(GL_DEPTH_WRITEMASK, true);

        //glEnable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_CULL_FACE);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_list.VBO);


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shader.texture1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, generate_clut_texture2());

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, generate_clut_light_texture());

        GLint base_texture = glGetUniformLocation(shader.shader->shader, "base_texture");
        GLint clut_texture = glGetUniformLocation(shader.shader->shader, "clut_texture");
        GLint clut_light_texture = glGetUniformLocation(shader.shader->shader, "clut_light_texture");

        GS_ASSERT(clut_texture != 0);
        GS_ASSERT(base_texture != 0);
        GS_ASSERT(clut_light_texture != 0);

        shader.shader->enable_attributes();

        glUniform1i(base_texture, 1); //Texture unit 0 is for base_texture
        glUniform1i(clut_texture, 2); //Texture unit 1 is for clut_texture
        glUniform1i(clut_light_texture, 3); //Texture unit 2 is for base_texture


        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, vertex_list.stride, (GLvoid*)0);
        glVertexAttribPointer(shader.TexCoord, 2, GL_FLOAT, GL_FALSE, vertex_list.stride, (GLvoid*)12);
        glVertexAttribPointer(shader.InLight, 2, GL_UNSIGNED_BYTE, GL_TRUE, vertex_list.stride, (GLvoid*)36);


        //glUniform4f(InTranslation, (GLfloat*) p.f);
        //glUniform3fv(InTranslation, 1, (GLfloat*) p.f);

        glDrawArrays(GL_QUADS, 0, vertex_list.vi);

        glDisableClientState(GL_VERTEX_ARRAY);

        shader.shader->disable_attributes();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);

        //glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //glDisable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_CULL_FACE);

        CHECK_GL_ERROR();

    }

    void draw_transparent()
    {
        if (vertex_list.vi == 0) return;
        if (!shader.shader->shader_valid) return;

        glColor4ub(255,255,255,255);

        GL_ASSERT(GL_BLEND, true);
        GL_ASSERT(GL_DEPTH_WRITEMASK, false);

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_list.VBO);


        //glBindTexture(GL_TEXTURE_2D, shader.texture1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shader.texture1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, generate_clut_texture());

        GLint base_texture = glGetUniformLocation(shader.shader->shader, "base_texture");
        GLint clut_texture = glGetUniformLocation(shader.shader->shader, "clut_texture");
        //GLint clut_light_texture = glGetUniformLocation(shader.shader->shader, "clut_light_texture");

        GS_ASSERT(clut_texture != 0);
        GS_ASSERT(base_texture != 0);

        shader.shader->enable_attributes();

        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, vertex_list.stride, (GLvoid*)0);
        glVertexAttribPointer(shader.TexCoord, 2, GL_FLOAT, GL_FALSE, vertex_list.stride, (GLvoid*)32);

        //glUniform4f(InTranslation, (GLfloat*) p.f);
        //glUniform3fv(InTranslation, 1, (GLfloat*) p.f);

        glDrawArrays(GL_QUADS, 0, vertex_list.vi);

        glDisableClientState(GL_VERTEX_ARRAY);

        shader.shader->disable_attributes();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glActiveTexture(GL_TEXTURE0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);
        
        CHECK_GL_ERROR();

    }

    MechListRenderer()
    {}

    ~MechListRenderer()
    {}

    void prep_vbo();

    void push_crystal_vertex(const struct Mech &m);
    void push_render_type_3(const struct Mech &m);
    void push_render_type_4(const struct Mech &m);

};

/*
struct _MECH
{
    int x,y,z; //position
    int type;  //type
    int direction; //direction

    float size;
    float rotation;
    int offset;

    bool active;
};
*/

void MechListRenderer::push_crystal_vertex(const struct Mech &m)
{
/*
    static const float vin[72] =
    {
        1,1,1, 0,1,1, 0,0,1, 1,0,1, //top
        0,1,0, 1,1,0, 1,0,0, 0,0,0, //bottom
        1,0,1, 1,0,0, 1,1,0, 1,1,1, //north
        0,1,1, 0,1,0, 0,0,0, 0,0,1, //south
        1,1,1, 1,1,0, 0,1,0, 0,1,1, //west
        0,0,1, 0,0,0, 1,0,0, 1,0,1  //east
    };
*/

    float wx = (float) (m.x) + 0.5f + m.offset_x;
    float wy = (float) (m.y) + 0.5f + m.offset_y;
    float wz = (float) m.z;
    //int face = m.face;

    //fulstrum test

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;

    wx = quadrant_translate_f(cx, wx);
    wy = quadrant_translate_f(cy, wy);

    /*
        Do radius render test?
    */
    if (!sphere_fulstrum_test(wx, wy, wz, 0.6f))
        return;

    int tex_id = mech_attributes[m.type].sprite;

    GS_ASSERT(mech_attributes[m.type].type != -1);

    const float txmargin = 0.0f;
    float tx_min, ty_min, tx_max, ty_max;

    int ti = tex_id % 16;
    int tj = tex_id / 16;

    const float h = 0.0625f;

    tx_min = ti*h + txmargin;
    ty_min = tj*h + txmargin;
    tx_max = ti*h + h - txmargin;
    ty_max = tj*h + h - txmargin;

    //int s = 2;

    float vn[3*4];
/*
    float tx[8];

    tx[2*0+0] = tx_min;
    tx[2*0+1] = ty_min;

    tx[2*1+0] = tx_min;
    tx[2*1+1] = ty_max;

    tx[2*2+0] = tx_max;
    tx[2*2+1] = ty_max;

    tx[2*3+0] = tx_max;
    tx[2*3+1] = ty_min;
*/

    const float size = m.size/2.0f;
    const float size2 = m.size;

    float dx,dy;

    //printf("rot= %f \n", m.rotation);

    dx = sin(m.rotation * PI);
    dy = cos(m.rotation * PI);

    vn[3*0+0] = wx - size*dx;
    vn[3*0+1] = wy - size*dy;
    vn[3*0+2] = wz + size2;

    vn[3*1+0] = wx - size*dx;
    vn[3*1+1] = wy - size*dy;
    vn[3*1+2] = wz;

    vn[3*2+0] = wx + size*dx;
    vn[3*2+1] = wy + size*dy;
    vn[3*2+2] = wz;

    vn[3*3+0] = wx + size*dx;
    vn[3*3+1] = wy + size*dy;
    vn[3*3+2] = wz + size2;

    int env_light = t_map::get_envlight(m.x,m.y,m.z);
    int sky_light = t_map::get_skylight(m.x,m.y,m.z); 
    vertex_list.light(sky_light, env_light);

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_max,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_max,ty_min);
    vertex_list.push_vertex();


    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_max,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_max,ty_min);
    vertex_list.push_vertex();

    dx = sin((m.rotation+0.5) * PI);
    dy = cos((m.rotation+0.5) * PI);

    vn[3*0+0] = wx - size*dx;
    vn[3*0+1] = wy - size*dy;
    vn[3*0+2] = wz + size2;

    vn[3*1+0] = wx - size*dx;
    vn[3*1+1] = wy - size*dy;
    vn[3*1+2] = wz;

    vn[3*2+0] = wx + size*dx;
    vn[3*2+1] = wy + size*dy;
    vn[3*2+2] = wz;

    vn[3*3+0] = wx + size*dx;
    vn[3*3+1] = wy + size*dy;
    vn[3*3+2] = wz + size2;

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_max,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_max,ty_min);
    vertex_list.push_vertex();


    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_max,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_max,ty_min);
    vertex_list.push_vertex();

}

void MechListRenderer::push_render_type_3(const struct Mech &m)
{
/*
    static const float vin[72] =
    {
        1,1,1, 0,1,1, 0,0,1, 1,0,1, //top
        0,1,0, 1,1,0, 1,0,0, 0,0,0, //bottom
        1,0,1, 1,0,0, 1,1,0, 1,1,1, //north
        0,1,1, 0,1,0, 0,0,0, 0,0,1, //south
        1,1,1, 1,1,0, 0,1,0, 0,1,1, //west
        0,0,1, 0,0,0, 1,0,0, 1,0,1  //east
    };
*/

    float wx = (float) (m.x) + 0.5f;
    float wy = (float) (m.y) + 0.5f;
    float wz = (float) (m.z) + 0.5f;
    //int face = m.face;

    //fulstrum test

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;

    wx = quadrant_translate_f(cx, wx);
    wy = quadrant_translate_f(cy, wy);

    /*
        Do radius render test?
    */
    if (!sphere_fulstrum_test(wx, wy, wz, 0.6f))
        return;

    int tex_id = mech_attributes[m.type].sprite;

    GS_ASSERT(mech_attributes[m.type].type != -1);

    const float txmargin = 0.0f;
    float tx_min, ty_min, tx_max, ty_max;

    int ti = tex_id % 16;
    int tj = tex_id / 16;

    const float h = 0.0625f;

    tx_min = ti*h + txmargin;
    ty_min = tj*h + txmargin;
    tx_max = ti*h + h - txmargin;
    ty_max = tj*h + h - txmargin;

    float vn[3*4];
    const float size = m.size/2.0f;
    //const float size2 = m.size;

    //orientation

    //for each direction need up and x,y,z


    //up
    static const struct Vec3 vou[6] =
    {
        {{{0,0,1}}}, //top
        {{{0,0,1}}}, //bottom
        {{{0,0,1}}}, //north
        {{{0,0,1}}}, //south
        {{{0,0,1}}}, //west
        {{{0,0,1}}}, //east
    };

    //normal
    static const struct  Vec3 vof[6] =
    {
        {{{0,0,0}}}, //top
        {{{0,0,0}}}, //bottom
        {{{-1,0,0}}}, //north
        {{{1,0,0}}}, //south
        {{{0,-1,0}}}, //west
        {{{0,1,0}}}, //east
    };

    float _for = 0.9f / 2.0f;

    int side = m.side;
    //side = 3;

    struct Vec3 vf = vof[side];
    struct Vec3 vu = vou[side];
    struct Vec3 vr = vec3_cross(vou[side], vof[side]);

    vn[3*0+0] = wx + size*(-vr.x + vu.x) + _for*vf.x;
    vn[3*0+1] = wy + size*(-vr.y + vu.y) + _for*vf.y;
    vn[3*0+2] = wz + size*(-vr.z + vu.z) + _for*vf.z;

    vn[3*1+0] = wx + size*(-vr.x - vu.x) + _for*vf.x;
    vn[3*1+1] = wy + size*(-vr.y - vu.y) + _for*vf.y;
    vn[3*1+2] = wz + size*(-vr.z - vu.z) + _for*vf.z;

    vn[3*2+0] = wx + size*(vr.x - vu.x) + _for*vf.x;
    vn[3*2+1] = wy + size*(vr.y - vu.y) + _for*vf.y;
    vn[3*2+2] = wz + size*(vr.z - vu.z) + _for*vf.z;

    vn[3*3+0] = wx + size*(vr.x + vu.x) + _for*vf.x;
    vn[3*3+1] = wy + size*(vr.y + vu.y) + _for*vf.y;
    vn[3*3+2] = wz + size*(vr.z + vu.z) + _for*vf.z;


    int env_light = t_map::get_envlight(m.x,m.y,m.z);
    int sky_light = t_map::get_skylight(m.x,m.y,m.z); 
    vertex_list.light(sky_light, env_light);

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_max,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_max,ty_min);
    vertex_list.push_vertex();


    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_max,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_max,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();
}


//torch
void MechListRenderer::push_render_type_4(const struct Mech &m)
{

    return;
/*
    static const float vin[72] =
    {
        1,1,1, 0,1,1, 0,0,1, 1,0,1, //top
        0,1,0, 1,1,0, 1,0,0, 0,0,0, //bottom
        1,0,1, 1,0,0, 1,1,0, 1,1,1, //north
        0,1,1, 0,1,0, 0,0,0, 0,0,1, //south
        1,1,1, 1,1,0, 0,1,0, 0,1,1, //west
        0,0,1, 0,0,0, 1,0,0, 1,0,1  //east
    };
*/

    static class MeshInstance* MI; // load_mesh(const char* filename)
    static class MeshLoader* ML = NULL;
    if(ML == NULL)
    {
        printf("loading mesh: \n");
        ML = new MeshLoader;
        MI = ML->load_mesh(MEDIA_PATH "sprites/mech/mesh/test.mesh");
    }

    static int _counter = 0;
    _counter++;

    if(_counter % 60 == 1)
    {
        delete MI;
        MI = NULL;
        MI = ML->load_mesh(MEDIA_PATH "sprites/mech/mesh/test.mesh");
    }

    float wx = (float) (m.x) + 0.001f;
    float wy = (float) (m.y) + 0.001;
    float wz = (float) (m.z) + 0.0f;

    //fulstrum test
    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;

    wx = quadrant_translate_f(cx, wx);
    wy = quadrant_translate_f(cy, wy);

    if (!sphere_fulstrum_test(wx, wy, wz, 0.6f))
        return;

    //int tex_id = mech_attributes[m.type].sprite;
    GS_ASSERT(mech_attributes[m.type].type != -1);

    //int side = m.side;


    int env_light = t_map::get_envlight(m.x,m.y,m.z);
    int sky_light = t_map::get_skylight(m.x,m.y,m.z); 
    vertex_list.light(sky_light, env_light);

    const int imax = MI->van;
    const MeshInstance::Vertex* va = MI->va;
    for(int i=0; i<imax; i++)
    {
        vertex_list.vertex3f(wx+va[i].x, wy+va[i].y, wz+va[i].z);
        vertex_list.tex2f(va[i].tx, va[i].ty);
        vertex_list.push_vertex();
    }

}

/*

enum MECH_TYPE
{
    CRYSTAL,
    CROP,
    WIRE,
    SWITCH
};
*/

void MechListRenderer::prep_vbo()
{
/*
    static const float vin[72] =
    {
        1,1,1, 0,1,1, 0,0,1, 1,0,1, //top
        0,1,0, 1,1,0, 1,0,0, 0,0,0, //bottom
        1,0,1, 1,0,0, 1,1,0, 1,1,1, //north
        0,1,1, 0,1,0, 0,0,0, 0,0,1, //south
        1,1,1, 1,1,0, 0,1,0, 0,1,1, //west
        0,0,1, 0,0,0, 1,0,0, 1,0,1  //east
    };
*/

    vertex_list.reset();

    const int mlm = mech_list->mlm;
    const struct Mech* mla = mech_list->mla;

    int num =0;

    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == -1) continue;

        switch (mla[i].render_type)
        {
            case MECH_RENDER_TYPE_0:
                push_crystal_vertex(mla[i]);
                break;
            case MECH_RENDER_TYPE_1:
                break;
            case MECH_RENDER_TYPE_2:
                break;
            case MECH_RENDER_TYPE_3:
                push_render_type_3(mla[i]);
                break;
            case MECH_RENDER_TYPE_4: //torch
                push_render_type_4(mla[i]);
                break;
            case MECH_RENDER_TYPE_NONE:
                GS_ASSERT(false);
                break;
        }

        num++;
    }

    if (num != mech_list->mln)
    {
        printf("num= %i mln= %i \n", num, mech_list->mln);
    }
    vertex_list.buffer();

}

}   // t_mech
