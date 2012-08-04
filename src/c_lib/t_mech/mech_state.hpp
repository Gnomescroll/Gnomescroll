#pragma once

class control_node
{
    public:
    
    int x;
    int y;
    int z;
};

class MECH_LIST
{
    public:

    int cpi; //control point index
    int cpm; //control point max
    class MECH* cpa; //control point array;

    bool needs_update; //for drawing

    MECH_LIST()
    {
        cpi = 0;
        cpm = 8;
        cpa = (MECH*) malloc(8*sizeof(class MECH));
        needs_update = true;
    }

    ~MECH_LIST()
    {
        free(cpa);
    }

    void add_mech(int x, int y, int z)
    {
        //needs_update = true;

        cpa[cpi].x = x;
        cpa[cpi].y = y;
        cpa[cpi].z = z;

        cpi++;

        if(cpi == cpm)
        {
            cpm *= 2;
            MECH* new_cpa = (MECH*) realloc(cpa, cpm*sizeof(class MECH));
            if (new_cpa == NULL)
            {
                free(cpa);
                cpa = NULL;
                cpm = 0;
            }
            else cpa = new_cpa;
        }
    }

    void remove_mech(int x, int y, int z)
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


#if DC_SERVER

    void send_mech_list_to_client(int client_id)
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

    void server_add_mech(int x, int y, int z)
    {
        return; //PRODUCTION
        this->add_mech(x,y,z);

        control_node_create_StoC p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.broadcast();
    }

    void server_remove_mech(int x, int y, int z)
    {
        remove_mech(x,y,z);

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
    unsigned int CameraPosition;
    //attributes
    unsigned int TexCoord;
    unsigned int Brightness;

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
        s = create_surface_from_file("./media/sprites/mech_00.png");

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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



}

