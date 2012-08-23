#pragma once

namespace t_map
{

class control_node
{
    public:
    
    int x;
    int y;
    int z;
};

class CONTROL_NODE_LIST
{
    public:

    int cpi; //control point index
    int cpm; //control point max
    class control_node* cpa; //control point array;

    bool needs_update; //for drawing

    CONTROL_NODE_LIST()
    {
        cpi = 0;
        cpm = 8;
        cpa = (control_node*) malloc(8*sizeof(class control_node));
        needs_update = true;
    }

    ~CONTROL_NODE_LIST()
    {
        free(cpa);
    }

    void add_control_node(int x, int y, int z)
    {
        //needs_update = true;

        cpa[cpi].x = x;
        cpa[cpi].y = y;
        cpa[cpi].z = z;

        cpi++;

        if(cpi == cpm)
        {
            cpm *= 2;
            control_node* new_cpa = (control_node*) realloc(cpa, cpm*sizeof(class control_node));
            if (new_cpa == NULL)
            {
                free(cpa);
                cpa = NULL;
                cpm = 0;
            }
            else cpa = new_cpa;
        }
    }

    void remove_control_node(int x, int y, int z)
    {
        //needs_update = true;

        for(int i=0; i<cpi; i++)
        {
            if(x==cpa[i].x && y==cpa[i].y && z==cpa[i].z)
            {
                cpa[i] = cpa[cpi-1];
                cpi--;
                GS_ASSERT(cpi >= 0);
            }
        }

        printf("Error: tried to remove control point that does not exist!\n");
        GS_ABORT(); //should never reach this point
    }


    bool control_node_in_range_check(int x, int y, int z)
    {
        for(int i=0; i<cpi; i++)
        {
            if( abs(x-cpa[i].x) <= 6 && abs(y-cpa[i].y) <= 6 && abs(z-cpa[i].z) <= 6 )  
                return true;
        }

        return false;
    }

#if DC_SERVER

    void send_control_nodes_to_client(int client_id)
    {
        for(int i=0; i<cpi; i++)
        {
            control_node_create_StoC p;
            p.x = cpa[i].x;
            p.y = cpa[i].y;
            p.z = cpa[i].z;
            p.sendToClient(client_id);
        }
    }

    void server_add_control_node(int x, int y, int z)
    {
        return; //PRODUCTION
        this->add_control_node(x,y,z);

        control_node_create_StoC p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.broadcast();
    }

    void server_remove_control_node(int x, int y, int z)
    {
        remove_control_node(x,y,z);

        control_node_delete_StoC p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.broadcast();
    }

#endif

};

#if DC_CLIENT


/*
    Use this as example for other shaders
*/

class ControlNodeVertexList 
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

    ControlNodeVertexList()
    {
        vm = 32;
        vi = 0;
        va = (struct Vertex*) malloc(vm*sizeof(struct Vertex));
        VBO = 0;
    }

    ~ControlNodeVertexList()
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



class ControlNodeShader
{
    public:

    SDL_Surface* s;

    unsigned int texture1;
    class SHADER* shader;

    //uniforms
    unsigned int CameraPosition;
    //attributes
    unsigned int TexCoord;
    unsigned int Brightness;

    ControlNodeShader()
    : s(NULL), shader(NULL)
    {
        init_texture();
        init_shader();
    }

    ~ControlNodeShader()
    {
        if (s != NULL) SDL_FreeSurface(s);
        if (shader != NULL) delete shader;
    }

    void init_shader()
    {
        shader = new SHADER;
        shader->set_debug(true);

        shader->load_shader( "control_node_shader",
            "./media/shaders/effect/control_node.vsh",
            "./media/shaders/effect/control_node.fsh" );

        CameraPosition =    shader->get_uniform("CameraPosition");


        TexCoord    =       shader->get_attribute("InTexCoord");
        Brightness  =       shader->get_attribute("InBrightness");
    }

    void init_texture()
    {
        s = create_surface_from_file("./media/sprites/mech/territory_00.png");

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        GLenum texture_format;
        if (s->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
        
        glDisable(GL_TEXTURE_2D);
    }

};


class ControlNodeRenderer
{
    public:

    class ControlNodeShader shader;
    class ControlNodeVertexList vertex_list;

    void draw()
    {

        //printf("draw: %i \n", vertex_list.vi);

        if(vertex_list.vi == 0) return;
        if(shader.shader->shader_valid == false) return;

        //printf("draw \n");

        glColor3ub(255,255,255);

        GL_ASSERT(GL_DEPTH_TEST, true);
        GL_ASSERT(GL_DEPTH_WRITEMASK, false);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_list.VBO);

        glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, shader.texture1 );
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

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
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }


    struct CONTROL_NODE_RENDER
    {
        short x,y,z;
        unsigned char face; //block side and edge
        unsigned char tex;
        unsigned char r,g,b;    //color
    };

    struct CONTROL_NODE_RENDER* cnra; // control_node_render_list array
    int cnri;   //index
    int cnrm;   //max

    class CONTROL_NODE_LIST* cnl; //control node list

    ControlNodeRenderer(class CONTROL_NODE_LIST* _cnl)
    {
        this->cnri = 0;
        this->cnrm = 32;
        this->cnl = _cnl;
        this->cnra = (struct CONTROL_NODE_RENDER*) malloc(this->cnrm*sizeof(struct CONTROL_NODE_RENDER));
    }

    ~ControlNodeRenderer()
    {
        free(cnra);
    }

    void update() 
    {
        control_node_render_update();
    };

    void _insert_control_node_render_element(short x, short y, short z, unsigned char face, unsigned char tex);

    void control_node_render_update();
    //int cpi; //control point index
    //int cpm; //control point max
    //class control_node* cpa; //control point array;

    void draw_intermediate();

};


class ControlNodeRenderer* control_node_renderer = NULL;


void control_node_render_init(class CONTROL_NODE_LIST* _cnl)
{
    GS_ASSERT(control_node_renderer == NULL);
    control_node_renderer = new ControlNodeRenderer(_cnl);
}

void control_node_render_teardown()
{
    if (control_node_renderer != NULL) delete control_node_renderer;
}

void control_node_render_update()
{
    control_node_renderer->update();
}

void control_node_render_draw()
{
    control_node_renderer->draw_intermediate();
    control_node_renderer->draw();
}


void ControlNodeRenderer::draw_intermediate()
{
    static const float vin[72] = 
    {
        1,1,1, 0,1,1, 0,0,1, 1,0,1, //top
        0,1,0, 1,1,0, 1,0,0, 0,0,0, //bottom
        1,0,1, 1,0,0, 1,1,0, 1,1,1, //north
        0,1,1, 0,1,0, 0,0,0, 0,0,1, //south
        1,1,1, 1,1,0, 0,1,0, 0,1,1, //west
        0,0,1, 0,0,0, 1,0,0, 1,0,1  //east
    };

#if 0
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, false);


    //glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBindTexture(GL_TEXTURE_2D, shader.texture1);


    glColor4ub(127,0,0,127);

    glColor4ub(255,255,255,255);

    glBegin(GL_QUADS);
    for(int i=0; i<cnri; i++)
    {
        float x = (float) cnra[i].x;
        float y = (float) cnra[i].y;
        float z = (float) cnra[i].z;
        int face = cnra[i].face;

        int tex_id = 0;

        const float txmargin = 0.0f;
        float tx_min, ty_min, tx_max, ty_max;

        int ti = tex_id % 4;
        int tj = tex_id / 4;

        tx_min = ti*0.25f + txmargin;
        ty_min = tj*0.25f + txmargin;
        tx_max = ti*0.25f + 0.25f - txmargin;
        ty_max = tj*0.25f + 0.25f - txmargin;

        tx_min = 0.0;
        ty_min = 0.0;
        tx_max = 0.25;
        ty_max = 0.25;

        int s = face;

        //glTexCoord2f(tx_min,ty_min);
        glTexCoord2f(tx_min,ty_min);
        glVertex3f(x+vin[12*s +3*0 +0], y+vin[12*s+ 3*0 +1], z+vin[12*s +3*0 +2]);


        glTexCoord2f(tx_min,ty_max);
        glVertex3f(x+vin[12*s +3*1 +0], y+vin[12*s+ 3*1 +1], z+vin[12*s +3*1 +2]);


        glTexCoord2f(tx_max,ty_max );
        glVertex3f(x+vin[12*s +3*2 +0], y+vin[12*s+ 3*2 +1], z+vin[12*s +3*2 +2]);

        glTexCoord2f(tx_max,ty_min );
        glVertex3f(x+vin[12*s +3*3 +0], y+vin[12*s+ 3*3 +1], z+vin[12*s +3*3 +2]);

    }

    glEnd();

    //glDepthMask(GL_TRUE);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);

    glColor4ub(255,255,255,255);
#else
    vertex_list.reset();

    for(int i=0; i<cnri; i++)
    {

        float x = (float) cnra[i].x;
        float y = (float) cnra[i].y;
        float z = (float) cnra[i].z;
        int face = cnra[i].face;

        int tex_id = cnra[i].tex;

        const float txmargin = 0.0f;
        float tx_min, ty_min, tx_max, ty_max;

        int ti = tex_id % 16;
        int tj = tex_id / 16;

        const float h = 0.0625f;

        tx_min = ti*h + txmargin;
        ty_min = tj*h + txmargin;
        tx_max = ti*h + h - txmargin;
        ty_max = tj*h + h - txmargin;

        int s = face;

        vertex_list.vertex3f(x+vin[12*s +3*0 +0], y+vin[12*s+ 3*0 +1], z+vin[12*s +3*0 +2]);
        vertex_list.tex2f(tx_min,ty_min);
        vertex_list.push_vertex();

        vertex_list.vertex3f(x+vin[12*s +3*1 +0], y+vin[12*s+ 3*1 +1], z+vin[12*s +3*1 +2]);
        vertex_list.tex2f(tx_min,ty_max);
        vertex_list.push_vertex();

        vertex_list.vertex3f(x+vin[12*s +3*2 +0], y+vin[12*s+ 3*2 +1], z+vin[12*s +3*2 +2]);
        vertex_list.tex2f(tx_max,ty_max );
        vertex_list.push_vertex();

        vertex_list.vertex3f(x+vin[12*s +3*3 +0], y+vin[12*s+ 3*3 +1], z+vin[12*s +3*3 +2]);
        vertex_list.tex2f(tx_max,ty_min );
        vertex_list.push_vertex();
    }

    vertex_list.buffer();
#endif

}

//utility
void ControlNodeRenderer::_insert_control_node_render_element(short x, short y, short z, unsigned char face, unsigned char tex)
{
    struct CONTROL_NODE_RENDER cnr;
    cnr.x = x;
    cnr.y = y;
    cnr.z = z;
    cnr.face = face;
    cnr.tex = tex;

    cnra[cnri] = cnr; //insert
    cnri++;

    if(cnri == cnrm)
    {
        cnrm *= 2;
        cnra = (struct CONTROL_NODE_RENDER*) realloc(cnra, cnrm*sizeof(struct CONTROL_NODE_RENDER));
    }
}



int calculate_tex(int face, int i, int j)
{
    const int UL = 17;
    const int UM = 18;
    const int UR = 19;

    const int ML = 33;
    const int MM = 34;
    const int MR = 35;

    const int BL = 49;
    const int BM = 50;
    const int BR = 51;

    const int size = 6;

    if(i == -size)
    {
        if(j == -size) return BL;
        if(j == size) return UL;
        return ML;
    }

    if(i == size)
    {
        if(j == -size) return BR;
        if(j == size) return UR;
        return MR;
    }

    if(j == -size)
    {
        return BM;
    }

    if(j == size)
    {
        return UM;
    }
    return MM;

}

void ControlNodeRenderer::control_node_render_update()
{
    static int counter = 0; //refresh, deals with loading before terrain map
    counter++;

    //if(cnl->needs_update == false && cnri != 0 && counter%30!=0) return;
    if(cnl->needs_update == false && cnri != 0 ) return;
    cnl->needs_update = false;
    cnri = 0; //reset index

    const int size = 6; //size of control grid to left or right of block

    for(int _i=0; _i<cnl->cpi; _i++)
    {
        int _x = cnl->cpa[_i].x;
        int _y = cnl->cpa[_i].y;
        int _z = cnl->cpa[_i].z;

        for(int i=-size; i<=size; i++)
        for(int j=-size; j<=size; j++)
        {
            //top
            int x = _x + i;
            int y = _y + j;
            int z = _z + size;

            if(!isSolid(x,y,z) && !isSolid(x,y,z+1) )
            {
                int face = 0;   //orientation
                int tex = calculate_tex(face, i,j);
                _insert_control_node_render_element(x,y,z, face, tex);
            }
        }

        //bottom
        for(int i=-size; i<=size; i++)
        for(int j=-size; j<=size; j++)
        {
            int x = _x + i;
            int y = _y + j;
            int z = _z - size;

            if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
            {
                int face = 1;   //orientation
                int tex = calculate_tex(face, i,j);
                _insert_control_node_render_element(x,y,z, face, tex);
            }
        }

        //north
        for(int i=-size; i<=size; i++)
        for(int j=-size; j<=size; j++)
        {
            int x = _x + size;
            int y = _y + i;
            int z = _z + j;

            if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
            {
                int face = 2;   //orientation
                int tex = calculate_tex(face, i,j);
                _insert_control_node_render_element(x,y,z, face, tex);
            }
        }

        //south
        for(int i=-size; i<=size; i++)
        for(int j=-size; j<=size; j++)
        {
            int x = _x - size;
            int y = _y + i;
            int z = _z + j;

            if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
            {
                int face = 3;   //orientation
                int tex = calculate_tex(face, i,j);
                _insert_control_node_render_element(x,y,z, face, tex);
            }
        }

        //west
        for(int i=-size; i<=size; i++)
        for(int j=-size; j<=size; j++)
        {
            int x = _x + i;
            int y = _y + size;
            int z = _z + j;

            if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
            {
                int face = 4;   //orientation
                int tex = calculate_tex(face, i,j);
                _insert_control_node_render_element(x,y,z, face, tex);
            }
        }

        //east
        for(int i=-size; i<=size; i++)
        for(int j=-size; j<=size; j++)
        {
            int x = _x + i;
            int y = _y - size;
            int z = _z + j;

            if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
            {
                int face = 5;   //orientation
                int tex = calculate_tex(face, i,j);
                _insert_control_node_render_element(x,y,z, face, tex);
            }
        }
    }
}

#endif


}

