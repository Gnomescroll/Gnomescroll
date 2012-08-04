#pragma once 

namespace t_mech 
{

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


    struct MECH_LIST_RENDER
    {
        short x,y,z;
        unsigned char face; //block side and edge
        unsigned char tex;
        unsigned char r,g,b;    //color
    };

    struct MECH_LIST_RENDER* mlra; // MECH_LIST_RENDER_list array
    int mlri;   //index
    int mlrm;   //max

    class MECH_LIST* ml; //control node list

    MechListRenderer(class MECH_LIST* _ml)
    {
        this->mlri = 0;
        this->mlrm = 32;
        this->ml = _ml;
        this->mlra = (struct MECH_LIST_RENDER*) malloc(this->mlrm*sizeof(struct MECH_LIST_RENDER));
    }

    ~MechListRenderer()
    {
        free(mlra);
    }

    void update() 
    {
        MECH_LIST_RENDER_update();
    };

    void prep_vbo();
    void draw();

};


void MechListRenderer::prep_vbo()
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


    vertex_list.reset();

    for(int i=0; i<mlri; i++)
    {

        float x = (float) mlra[i].x;
        float y = (float) mlra[i].y;
        float z = (float) mlra[i].z;
        int face = mlra[i].face;

        int tex_id = mlra[i].tex;

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

}

void MechListRenderer::MECH_LIST_RENDER_update()
{
    static int counter = 0; //refresh, deals with loading before terrain map
    counter++;

    //if(ml->needs_update == false && mlri != 0 && counter%30!=0) return;
    if(ml->needs_update == false && mlri != 0 ) return;
    ml->needs_update = false;
    mlri = 0; //reset index
}



void mech_list_render_init(class MECH_LIST* _ml)
{
    GS_ASSERT(MECH_LIST_RENDERer == NULL);
    MECH_LIST_RENDERer = new MechListRenderer(_ml);
}

void mech_list_render_teardown()
{
    if (MECH_LIST_RENDERer != NULL) delete MECH_LIST_RENDERer;
}

void mech_list_render_prep()
{
    MECH_LIST_RENDERer->update();
    MECH_LIST_RENDERer->prep_vbo();
}

void mech_list_render_draw()
{
    MECH_LIST_RENDERer->draw();
}


}