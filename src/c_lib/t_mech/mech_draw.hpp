#pragma once 

#include <t_mech/_interface.hpp>
#include <t_mech/mech_state.hpp>
#include <t_mech/_interface.hpp>

#include <t_mech/config/_interface.hpp>

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
        float x,y,z;
        float tx,ty;
        float btx,bty;  //texture cordinates for bilinear interpolation
        unsigned char color[4];
        unsigned char brightness[4];
    };

    static const int stride = sizeof(Vertex);

    struct Vertex* va;  //vertex array
    int vi; //vertex index
    int vm; //vertex max

    unsigned int VBO; //for drawing 

    struct Vertex v; //set this and then push vertex

    MechListVertexList()
    {
        vm = 32;
        vi = 0;
        va = (struct Vertex*) malloc(vm*sizeof(struct Vertex));
        VBO = 0;
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

    void push_vertex()
    {
        va[vi] = v;
        vi++;
        if(vi == vm)
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
        if(VBO == 0) glGenBuffers(1, &VBO);

        if(vi != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vi*stride, NULL, GL_STATIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, vi*stride, va, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        } 
        else
        {
            if(vi > 0) 
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
    class SHADER* shader;

    //uniforms
    int CameraPosition;
    //attributes
    int TexCoord;
    int Brightness;

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
        shader = new SHADER;
        shader->set_debug(true);

        shader->load_shader( "mech_list_shader",
            "./media/shaders/effect/mech_list.vsh",
            "./media/shaders/effect/mech_list.fsh" );

        CameraPosition =    shader->get_uniform("CameraPosition");

        TexCoord    =       shader->get_attribute("InTexCoord");
        Brightness  =       shader->get_attribute("InBrightness");
    }

    void init_texture()
    {
        s = create_surface_from_file("./media/sprites/mech/mech_00.png");

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        GLenum texture_format;
        if (s->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
        
        glDisable(GL_TEXTURE_2D);

        init_sprite_meta();
    }

    void init_sprite_meta()
    {
        //int mech_sprite_width[256];  //centered
        //int mech_sprite_height[256]; //from bottom

        for(int i=0; i<256; i++)
        {
            mech_sprite_width[i]  = -1;
            mech_sprite_height[i] = 32;
        }

        SDL_LockSurface(s);

        unsigned char *pixels = (unsigned char *)s->pixels;
        GS_ASSERT(s->w == 32*16 && s->h == 32*16);

        for(int i=0; i<256; i++)
        {
            int w = i % 16;
            int h = i / 16;

            //left to right


            bool empty_column = false;
            int j;

            for(j=0; j<16; j++) //column
            {
                int offset = 512*32*h + 32*w;
                offset += j;

                for(int k=0; k<32; k++) //iterate over column
                {
                    int index = offset + k*512;

                    if(pixels[4*index + 3] > 128)
                    {
                        empty_column = true;

                        int width = 2*(16-j);
                        if(width > mech_sprite_width[i])
                        {
                            mech_sprite_width[i] = width;

                            printf("sprite1: %i %i width: %i k: %i \n", h,w, mech_sprite_width[i], k);
                        }
                        break;
                    }
                }

                if(empty_column == true)
                {
                    for(int _k=0; _k<32; _k++)
                    {
                        int index2 = offset + _k*512;
                        pixels[4*index2 + 3] = 255;
                        pixels[4*index2 + 0] = 255;
                    }

                    break;
                }
            }

            empty_column = false;

            for(j=0; j<16; j++) //column
            {
                int offset = 512*32*h + 32*w;
                offset += 31-j;

                for(int k=0; k<32; k++) //iterate over column
                {
                    int index = offset + k*512;

                    if(pixels[4*index + 3] > 128)
                    {
                        empty_column = true;

                        int width = 2*(16-j);
                        if(width > mech_sprite_width[i])
                        {
                            mech_sprite_width[i] = width;

                            printf("sprite2: %i %i width: %i k: %i \n", h,w, mech_sprite_width[i], k);
                        }
                        break;
                    }
                }

                if(empty_column == true)
                {
                    for(int _k=0; _k<32; _k++)
                    {
                        int index2 = offset + (31-_k)*512;
                        pixels[4*index2 + 3] = 255;
                        pixels[4*index2 + 0] = 255;
                    }

                    break;
                }
            }

            //if(mech_sprite_width[i] != -1)
            //    printf("sprite: %i %i width: %i \n", w,h, mech_sprite_width[i]);

        }


        SDL_UnlockSurface(s);

        save_surface_to_png(s, "./screenshot/test.png");

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

        if(vertex_list.vi == 0) return;
        if(shader.shader->shader_valid == false) return;

        //printf("draw \n");

        glColor4ub(255,255,255,255);

        GL_ASSERT(GL_BLEND, false);
        GL_ASSERT(GL_DEPTH_WRITEMASK, true);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_list.VBO);

        glBindTexture( GL_TEXTURE_2D, shader.texture1 );

        shader.shader->enable_attributes();

        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, vertex_list.stride, (GLvoid*)0);
        glVertexAttribPointer(shader.TexCoord, 2, GL_FLOAT, GL_FALSE, vertex_list.stride, (GLvoid*)12);

        //glUniform4f(InTranslation, (GLfloat*) p.f );
        //glUniform3fv(InTranslation, 1, (GLfloat*) p.f );

        glDrawArrays(GL_QUADS, 0, vertex_list.vi);

        glDisableClientState(GL_VERTEX_ARRAY);

        shader.shader->disable_attributes();

        glBindTexture( GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_CULL_FACE);

    }

    void draw_transparent()
    {
        if(vertex_list.vi == 0) return;
        if(shader.shader->shader_valid == false) return;

        glColor4ub(255,255,255,255);

        GL_ASSERT(GL_BLEND, true);
        GL_ASSERT(GL_DEPTH_WRITEMASK, false);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_list.VBO);


        glBindTexture( GL_TEXTURE_2D, shader.texture1 );

        shader.shader->enable_attributes();

        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, vertex_list.stride, (GLvoid*)0);
        glVertexAttribPointer(shader.TexCoord, 2, GL_FLOAT, GL_FALSE, vertex_list.stride, (GLvoid*)12);

        //glUniform4f(InTranslation, (GLfloat*) p.f );
        //glUniform3fv(InTranslation, 1, (GLfloat*) p.f );

        glDrawArrays(GL_QUADS, 0, vertex_list.vi);

        glDisableClientState(GL_VERTEX_ARRAY);

        shader.shader->disable_attributes();

        glBindTexture( GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);

    }

    MechListRenderer()
    {}

    ~MechListRenderer()
    {}

    void prep_vbo();

    void push_crystal_vertex(const struct MECH &m);

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

void MechListRenderer::push_crystal_vertex(const struct MECH &m)
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
    if( sphere_fulstrum_test(wx, wy, wz, 0.6f) == false)
        return;

    int tex_id = mech_attribute[m.mech_type].sprite_index;
    
    GS_ASSERT(mech_attribute[m.mech_type].mech_type != -1);

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

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_max,ty_max );
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_max,ty_min );
    vertex_list.push_vertex();


    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_max,ty_max );
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_max,ty_min );
    vertex_list.push_vertex();

    dx = sin( (m.rotation+0.5) * PI );
    dy = cos( (m.rotation+0.5) * PI );

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
    vertex_list.tex2f(tx_max,ty_max );
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_max,ty_min );
    vertex_list.push_vertex();


    vertex_list.vertex3f(vn[3*3+0], vn[3*3+1], vn[3*3+2]);
    vertex_list.tex2f(tx_min,ty_min);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*2+0], vn[3*2+1], vn[3*2+2]);
    vertex_list.tex2f(tx_min,ty_max);
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*1+0], vn[3*1+1], vn[3*1+2]);
    vertex_list.tex2f(tx_max,ty_max );
    vertex_list.push_vertex();

    vertex_list.vertex3f(vn[3*0+0], vn[3*0+1], vn[3*0+2]);
    vertex_list.tex2f(tx_max,ty_min );
    vertex_list.push_vertex();

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
    const struct MECH* mla = mech_list->mla;
    
    int num =0;

    for(int i=0; i<mlm; i++)
    {
        if( mla[i].id == -1) continue;

        switch ( mech_attribute[mla[i].mech_type].render_type)
        {
            case MECH_RENDER_TYPE_0:
                push_crystal_vertex(mla[i]);
                break;
            case MECH_RENDER_TYPE_1:
                break;
            case MECH_RENDER_TYPE_2:
                break;
            default:
                GS_ASSERT(false);
                break;
        }

        num++;
    }

    if(num != mech_list->mln)
    {
        printf("num= %i mln= %i \n", num, mech_list->mln);
    }
    vertex_list.buffer();

}

}   // t_mech
