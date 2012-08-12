#include "t_vbo_draw.hpp"

#include <t_map/t_vbo_class.hpp>

#include <t_map/glsl/shader.hpp>
#include <t_map/glsl/texture.hpp>

#include <camera/camera.hpp>
#include <camera/fulstrum_test.hpp>
#include <camera/fulstrum_test2.hpp>

#include <t_map/glsl/settings.hpp>

#include <common/qsort.h>

#include <physics/quadrant.hpp>

namespace t_map
{

static const int MAX_DRAWN_VBO = 1024;  //this should not be hardcoded; will piss someone off

static int draw_vbo_n;
//static struct Map_vbo* draw_vbo_array[MAX_DRAWN_VBO];

int vbo_frustrum[32*32*2];  //start and end chunk index


int vbo_frustrum_min[32*32];    //min on frustrum for column
int vbo_frustrum_max[32*32];    //max on frustrum for column

//int vbo_frustrum_bottom[32*32];
int vbo_vertex_frustrum[32*32][12]; //start vertex and end vertex

struct _VBO_DRAW_STRUCT
{
    class Map_vbo* map_vbo;
    float distance;
    //int vertex_start; //start vertex for drawing
    //int vertex_end; //end vertex for drawing
    //int index;
    short i;
    short j;
};

static struct _VBO_DRAW_STRUCT* draw_vbo_array;

void vbo_draw_init()
{
    draw_vbo_array = (_VBO_DRAW_STRUCT*) malloc(MAX_DRAWN_VBO * sizeof(_VBO_DRAW_STRUCT));
}

void vbo_draw_end()
{
    free(draw_vbo_array);
}

void set_frustrum_column_min(int _i, int _j, float x, float y)
{   
    _j %= 32;
    _i %= 32;
    const int index = 32*_j + _i;

    //float cx = x - current_camera_position.x;
    //float cy = y - current_camera_position.y;

/*
    float top =     top_z_projection(cx,cy);
    float bottom =  bottom_z_projection(cx,cy);

    int t = ceil(top / 16);
    int b = floor(bottom / 16);
*/

    int t,b;

    b = -1;
    t = -1;

    for(int i=0; i<=8; i++)
    {
        //point_fulstrum_test(
        if(sphere_fulstrum_test(x,y,16*i, 0.0f) == true)
        {
            b = i;
            break;
        }
    }

    for(int i=8; i>= 0; i--)
    {
        if(sphere_fulstrum_test(x,y,16*i, 0.0f) == true)
        {
            t = i;
            break;
        }
    }


    GS_ASSERT(t >= b);

    if(b == -1 || t == -1)
    {
        if(b == -1 && t == -1)
        {
            b = -1;
            t = -1;
        }
        else
        {
            printf("set_frustrum_column_min error: b= %i t= %i \n", b, t);
        }

    }

    //printf("top= %f bottom= %f t= %i b= %i \n", top, bottom, t, b);

    //if(b < 0) b = 0;
    //if(t > 8) t = 8;
    //b = 0;x
    //t = 8;

    vbo_frustrum_min[index] = b;
    vbo_frustrum_max[index] = t;


}



void Vbo_map::prep_frustrum()
{
    //memset(vbo_frustrum, -1, 32*32*2);
    //for(int i=0; i <32*32*2; i++) vbo_frustrum[i] = -1;
    //for(int i=0; i <32*32; i++) vbo_frustrum_min[i] = -1;
    //for(int i=0; i <32*32; i++) vbo_frustrum_max[i] = -1;


    for(int ix=0;ix<draw_vbo_n;ix++)
    {
        class Map_vbo* col = draw_vbo_array[ix].map_vbo;
        int i = draw_vbo_array[ix].i;
        int j = draw_vbo_array[ix].j;

        set_frustrum_column_min(i,j,     col->wxoff, col->wyoff);
    }
}

void Vbo_map::prep_frustrum_vertices()
{
    for(int i=0; i<32*32; i++)
    for(int j=0; j<12; j++)
    {
        vbo_vertex_frustrum[i][j] = 0;
    }

    for(int i=0;i<draw_vbo_n;i++)
    {
        class Map_vbo* vbo = draw_vbo_array[i].map_vbo;
        int xi = draw_vbo_array[i].i;
        int xj = draw_vbo_array[i].j;

        int index = 32*xj +xi;

        int min = vbo_frustrum_min[index];
        int max = vbo_frustrum_max[index];

        if(min == -1 && max == -1)
        {
            //prune!!!
            min = 0;
            max = 0;
        }
        else
        {
            GS_ASSERT(min <= 8);
            GS_ASSERT(max <= 8);
            GS_ASSERT(min >= 0);
            GS_ASSERT(max >= 0);
        }
        //int xoff,dnum;
        GS_ASSERT(min >= 0);
        GS_ASSERT(max >= 0);
        //printf("i,j= %i %i min,max= %i %i \n", xi,xj, min,max);
        for(int side=0; side<6; side++)
        {
            if(min >= max)
            {
                //dont draw anything, prune
                int voff = vbo->vertex_offset[side];
                int vnum = 0;
                vbo_vertex_frustrum[index][2*side+0] = voff;
                vbo_vertex_frustrum[index][2*side+1] = vnum;

                continue;
            }

            int vs = vbo->voff_array[side][min];
            int ve = vbo->voff_array[side][max];

            int voff = vs;
            int vnum = ve - vs;

            vbo_vertex_frustrum[index][2*side+0] = voff;
            vbo_vertex_frustrum[index][2*side+1] = vnum;


            if(voff+vnum > vbo->vertex_offset[side]+ vbo->vertex_num[side])
            {
                printf("v1= %i v2= %i \n", voff+vnum, vbo->vertex_offset[side]+ vbo->vertex_num[side]);
                printf("voff= %i vnum= %i \n", voff, vnum);
                printf("vbo->vertex_offset[sid]= %i vbo->vertex_num[side]= %i \n", vbo->vertex_offset[side], vbo->vertex_num[side]);
                printf("min= %i max= %i \n", min, max);
                GS_ABORT();
            }

        }


    }
}


/*
    Do distance check
*/
bool chunk_distance_check( float x, float y)
{
    //static const float dist2 = CAMERA_VIEW_DISTANCE*CAMERA_VIEW_DISTANCE;
    //static const float dist2 = CAMERA_VIEW_DISTANCE_SQUARED;

    static const float dist2 = (CAMERA_VIEW_DISTANCE+(sqrt(2)*8))*(CAMERA_VIEW_DISTANCE+(sqrt(2)*8));

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;

    //x = quadrant_translate_f(cx, x);
    //y = quadrant_translate_f(cy, y);
    
    float dx = cx - x;
    float dy = cy - y;

    return (dx*dx + dy*dy > dist2) ? false : true;
}

void Vbo_map::prep_draw()
{

    for(int i=0; i <32*32; i++) vbo_frustrum_min[i] = -1;
    for(int i=0; i <32*32; i++) vbo_frustrum_max[i] = -1;

    class Map_vbo* col;

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(cy);

    //static float chunk_radius = sqrtf(2)*8;
    
    int c_drawn, c_pruned;
    c_drawn=0; c_pruned=0;
    //start_vbo_draw();
    draw_vbo_n = 0;
    //printf("Start Map Draw\n");
    for(int i=0; i<MAP_CHUNK_XDIM; i++) {
    for(int j=0; j<MAP_CHUNK_YDIM; j++) {
        col = vbo_array[j*MAP_CHUNK_XDIM + i ];

        if(col == NULL || col->vnum == 0) continue;

        //col->wxoff = quadrant_translate_f(cx, col->xoff+8.0f);
        //col->wyoff = quadrant_translate_f(cy, col->yoff+8.0f);
        //col->wxoff = quadrant_translate_f(cx, col->xoff) + 8.0f;
        //col->wyoff = quadrant_translate_f(cy, col->yoff) + 8.0f;
        float x = translate_point(col->xoff + 8.0f);
        float y = translate_point(col->yoff + 8.0f);
        col->wxoff = quadrant_translate_f(cx, x);
        col->wyoff = quadrant_translate_f(cy, y);

        // plain chunk distance check has errors in corners
        if (chunk_distance_check( col->wxoff, col->wyoff))
        //if (true)
        //)// && xy_circle_fulstrum_test(col->wxoff, col->wyoff, chunk_radius))
        {
            c_drawn++; 
            /*
                Fulstrum culling
            */
            draw_vbo_array[draw_vbo_n].map_vbo = col;
            draw_vbo_array[draw_vbo_n].i = i;
            draw_vbo_array[draw_vbo_n].j = j;

            draw_vbo_n++;
            
            if(draw_vbo_n == MAX_DRAWN_VBO)
            {
                printf("Vbo_map::prep_draw(), ERROR, draw_vbo == MAX_DRAWN_VBO \n");
                return;
            }
            //printf("drew %0.2f, %0.2f, \n");
        }
        else
        {
            c_pruned++;
        }
    }}
    //printf("drawn: %i pruned: %i \n",  c_drawn, c_pruned);
    //printf("\n");
}

void Vbo_map::sort_draw()
{
    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(cy);

    for(int i=0; i<draw_vbo_n; i++ )
    {
        class Map_vbo* v = draw_vbo_array[i].map_vbo;

        float dx = (v->wxoff - cx);
        float dy = (v->wyoff - cy);

        //dx = quadrant_translate_f(cx, dx);
        //dy = quadrant_translate_f(cx, dy);

        draw_vbo_array[i].distance = dx*dx + dy*dy; //set this
    }

  #define _VBO_DRAW_STRUCT_lt(a,b) ((a)->distance < (b)->distance)
  QSORT(struct  _VBO_DRAW_STRUCT, draw_vbo_array, draw_vbo_n, _VBO_DRAW_STRUCT_lt);

  //if(draw_vbo_n > 10) draw_vbo_n = 10;
}

#define ADV_PRUNE 0

void Vbo_map::draw_map() 
{
    prep_draw();
    sort_draw();
    
    //prep_frustrum();
    //prep_frustrum_vertices();

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);

    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    glColor3ub(255,255,255);

    glUseProgramObjectARB(map_shader[0]);

    glBindTexture( GL_TEXTURE_2D_ARRAY, terrain_map_glsl );

    glEnableVertexAttribArray(map_Vertex);
    glEnableVertexAttribArray(map_TexCoord);
    glEnableVertexAttribArray(map_RGB);
    glEnableVertexAttribArray(map_LightMatrix);

    class Map_vbo* vbo;

    //glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array );

    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    glPushMatrix(); //save matrix
    //glPushMatrix();

#if ADV_PRUNE
    int v_total = 0;
    int v_drawn = 0;
    //int v_pruned = 0;
#endif

    /*
        Test Sorting
    */
    float distance = -1;
    for(int i=0;i<draw_vbo_n;i++)
    {
        if(draw_vbo_array[i].distance >= distance)
        {
            //printf("0 i= %d distance= %f \n", i, sqrt(draw_vbo_array[i].distance)) ;
            distance = draw_vbo_array[i].distance;
        }
        else
        {
            distance = draw_vbo_array[i].distance;
            printf("1 i= %d distance= %f \n", i, sqrt(draw_vbo_array[i].distance));
            //GS_ABORT();
        }
    }

    /*
        Draw
    */
    for(int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i].map_vbo;

        if(vbo->_v_num[0] == 0)
        {
            printf("t_vbo_draw.cpp:376 no blocks\n");
            continue; 
        } 


        glLoadMatrixf(modelview);
        glTranslatef(vbo->wxoff-8.0f, vbo->wyoff-8.0f, 0.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);


        glVertexAttribPointer(map_Vertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);    
        glVertexAttribPointer(map_TexCoord, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)4);
        glVertexAttribPointer(map_RGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        glVertexAttribPointer(map_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);
        
        #if ADV_PRUNE

        int xi = draw_vbo_array[i].i;
        int xj = draw_vbo_array[i].j;
        int index = 32*xj +xi;

        v_total += vbo->_v_num[0];
        for(int side=0; side<6; side++)
        {

            int min = vbo_frustrum_min[index];
            int max = vbo_frustrum_max[index];

            if(min == -1 && max == -1)
                break;

            int voff = vbo_vertex_frustrum[index][2*side+0];
            int vnum = vbo_vertex_frustrum[index][2*side+1];

            //printf("side %i offset: %i vertices: %i \n", side, vbo->vertex_offset[side], vbo->vertex_num[side]);
            

            //vbo->vertex_offset[side], vbo->vertex_num[side]

            //GS_ASSERT(voff >= vbo->vertex_offset[side] )
            if(voff+vnum > vbo->vertex_offset[side]+ vbo->vertex_num[side])
            {
                printf("v1= %i v2= %i \n", voff+vnum, vbo->vertex_offset[side]+ vbo->vertex_num[side]);
                printf("voff= %i vnum= %i \n", voff, vnum);
                printf("vbo->vertex_offset[sid]= %i vbo->vertex_num[side]= %i \n", vbo->vertex_offset[side], vbo->vertex_num[side]);

                GS_ABORT();
            }
            //printf("drawing: offset %i vertices: %i\n", voff, vnum);
            //glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
            //glDrawArrays(GL_QUADS, vbo->vertex_offset[side], vbo->vertex_num[side]);

            //if(abs(voff) > 1000*1000) continue; //wtf
            if(vnum <= 0) continue;
            v_drawn += vnum;
            glDrawArrays(GL_QUADS, voff, vnum);

        }
        #else 
        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
        #endif

        //glPopMatrix();
        //glPushMatrix();
    }

    //printf("v_total= %i v_drawn= %i \n", v_total, v_drawn);

    glPopMatrix(); //restore matrix


    glDisableVertexAttribArray(map_Vertex);
    glDisableVertexAttribArray(map_TexCoord);
    glDisableVertexAttribArray(map_RGB);
    glDisableVertexAttribArray(map_LightMatrix);
    
    glUseProgramObjectARB(0);

    //glEnable(GL_TEXTURE_2D);

    //;
    glDisable(GL_CULL_FACE);

/*
    TEST
*/


    //set_frustrum_column_min(i, j, )
#if 0
    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;

    for(int i=0; i<32; i++)
    for(int j=0; j<32; j++)
    {
        float x = i*16.0 + 8.0;
        float y = j*16.0 + 8.0;


        x = quadrant_translate_f(cx, x);
        y = quadrant_translate_f(cy, y);

        //set_frustrum_column_min
    }

    glColor3ub(255, 0, 0);

    glBegin(GL_POINTS);
    for(int i=0; i<32; i++)
    {
    for(int j=0; j<32; j++)
    {
    for(int k=0; k<=128; k+=16)
    {
        int index = 32*i +j;

        int min = vbo_frustrum_min[index];
        int max = vbo_frustrum_max[index];
        
        if(min == -1 && max == -1)
            continue;
        if(k <= min*16)
        {
            glColor3ub(255, 0, 0);
        } 
        else if(k >= max*16)
        {
            glColor3ub(0, 255, 0);
        }
        else
        {
            glColor3ub(0, 0, 255);
        }

        glVertex3f(16*i+8.0, 16*j+8.0 ,k);
    }

    }}

    glEnd();

    glColor3ub(255, 255, 255);
#endif

    CHECK_GL_ERROR();
}

void Vbo_map::draw_map_compatibility()
{

    prep_draw();
    sort_draw();
    
    //prep_frustrum();
    //prep_frustrum_vertices();

    //GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);

    //;
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);

    glColor3ub(255,255,255);

    glUseProgramObjectARB(map_shader[0]);

    glBindTexture( GL_TEXTURE_2D, block_textures_compatibility );

    glEnableVertexAttribArray(map_Vertex);
    glEnableVertexAttribArray(map_TexCoord);
    glEnableVertexAttribArray(map_RGB);
    glEnableVertexAttribArray(map_LightMatrix);

    class Map_vbo* vbo;

    //glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array );

    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    glPushMatrix(); //save matrix

    for(int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i].map_vbo;

        if(vbo->_v_num[0] == 0)
        {
            //printf("t_vbo_draw.cpp:117 no blocks\n");
            continue; 
        } 

        glLoadMatrixf(modelview);
        glTranslatef(vbo->wxoff-8.0f, vbo->wyoff-8.0f, 0.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);

        glVertexAttribPointer(map_Vertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);         
        glVertexAttribPointer(map_TexCoord, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)4);
        glVertexAttribPointer(map_RGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        glVertexAttribPointer(map_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);

/*
        int xi = draw_vbo_array[i].i;
        int xj = draw_vbo_array[i].j;
        int index = 32*xj +xi;

        for(int side=0; side<6; side++)
        {
            int voff = vbo_vertex_frustrum[index][2*side+0];
            int vnum = vbo_vertex_frustrum[index][2*side+1];

            if(vnum <= 0) continue;
            glDrawArrays(GL_QUADS, voff, vnum);
        }
*/
        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
    }

    glPopMatrix(); //restore matrix

    glDisableVertexAttribArray(map_Vertex);
    glDisableVertexAttribArray(map_TexCoord);
    glDisableVertexAttribArray(map_RGB);
    glDisableVertexAttribArray(map_LightMatrix);


    glUseProgramObjectARB(0);


    //glEnable(GL_TEXTURE_2D);
    //;
    glDisable(GL_CULL_FACE);

}

}
