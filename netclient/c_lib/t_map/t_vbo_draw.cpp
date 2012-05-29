#include "t_vbo_draw.hpp"

#include <t_map/t_vbo_class.hpp>

#include <t_map/glsl/shader.hpp>
#include <t_map/glsl/texture.hpp>

#include <camera/camera.hpp>
#include <camera/fulstrum_test.hpp>

#include <t_map/glsl/settings.hpp>


#include <common/qsort.h>

#include <physics/quadrant.hpp>

namespace t_map
{

static const int MAX_DRAWN_VBO = 1024;  //this should not be hardcoded; will piss someone off

static int draw_vbo_n;
//static struct Map_vbo* draw_vbo_array[MAX_DRAWN_VBO];

struct _VBO_DRAW_STRUCT
{
    class Map_vbo* map_vbo;
    float distance;
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


/*
    Do distance check
*/
bool chunk_render_check( float x, float y)
{
    static const float dist2 = CAMERA_VIEW_DISTANCE*CAMERA_VIEW_DISTANCE;

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(cy);
    float dx = cx - x;
    float dy = cy - y;

    return (dx*dx + dy*dy > dist2) ? false : true;
}

void Vbo_map::prep_draw()
{
    struct Map_vbo* col;

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(cy);
    
/*
    for(int i=0; i<map->xchunk_dim; i++)
    for(int j=0; j<map->ychunk_dim; j++)
    {
        col = vbo_array[j*xchunk_dim + i ];
        if(col == NULL) continue;

        col->wxoff = quadrant_translate_f(cx, col->xoff);
        col->wyoff = quadrant_translate_f(cy, col->yoff);

    }
*/
    int c_drawn, c_pruned;
    c_drawn=0; c_pruned=0;
    //start_vbo_draw();
    draw_vbo_n = 0;
    //printf("Start Map Draw\n");
    for(int i=0; i<map->xchunk_dim; i++) {
    for(int j=0; j<map->ychunk_dim; j++) {
        col = vbo_array[j*xchunk_dim + i ];

        if(col == NULL || col->vnum == 0) continue;

        col->wxoff = quadrant_translate_f(cx, col->xoff);
        col->wyoff = quadrant_translate_f(cy, col->yoff);

        if( chunk_render_check( col->wxoff+8.0, col->wyoff+8.0) && xy_circle_fulstrum_test( col->wxoff+8.0, col->wyoff+8.0, 32.0) )
        {
            c_drawn++; 
            /*
                Fulstrum culling
            */
            draw_vbo_array[draw_vbo_n].map_vbo = col;
            draw_vbo_n++;
            
            if(draw_vbo_n == MAX_DRAWN_VBO)
            {
                printf("Vbo_map::prep_draw(), ERROR, draw_vbo == MAX_DRAWN_VBO \n");
                return;
            }
        }
        else
        {
            c_pruned++;
        }
    }}
    //printf("drawn: %i pruned: %i \n",  c_drawn, c_pruned);
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

        draw_vbo_array[i].distance = dx*dx + dy*dy; //set this

    }

  #define _VBO_DRAW_STRUCT_lt(a,b) ((a)->distance < (b)->distance)
  QSORT(struct  _VBO_DRAW_STRUCT, draw_vbo_array, draw_vbo_n, _VBO_DRAW_STRUCT_lt);

  //if(draw_vbo_n > 10) draw_vbo_n = 10;
}

//float _normal_array[3*6];
float _chunk_position[3] = {0};

void Vbo_map::draw_map() 
{
    if(T_MAP_BACKUP_SHADER != 0)
    {
        draw_map_comptability();
        return;
    }

    prep_draw();
    sort_draw();

    //GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);

    //glShadeModel(GL_FLAT);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    glUseProgramObjectARB(map_shader[0]);


    glColor3ub(255,255,255);

    glBindTexture( GL_TEXTURE_2D_ARRAY, terrain_map_glsl );


    glEnableVertexAttribArray(map_Vertex);
    glEnableVertexAttribArray(map_TexCoord);
    glEnableVertexAttribArray(map_RGB);
    glEnableVertexAttribArray(map_LightMatrix);

    struct Map_vbo* vbo;

    //glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array );

    for(int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i].map_vbo;

        if(vbo->_v_num[0] == 0)
        {
            //printf("t_vbo_draw.cpp:117 no blocks\n");
            continue; 
        } 

        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);

        //glUniform3f(map_ChunkPosition, vbo->xoff, vbo->yoff, 0.0f);
        glUniform3f(map_ChunkPosition, vbo->wxoff, vbo->wyoff, 0.0f);

        glVertexAttribPointer(map_Vertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);    
        glVertexAttribPointer(map_TexCoord, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)4);
        glVertexAttribPointer(map_RGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        glVertexAttribPointer(map_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);

        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
    }

    glDisableVertexAttribArray(map_Vertex);
    glDisableVertexAttribArray(map_TexCoord);
    glDisableVertexAttribArray(map_RGB);
    glDisableVertexAttribArray(map_LightMatrix);
    
    glUseProgramObjectARB(0);

    //glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
}

void Vbo_map::draw_map_comptability()
{
    //printf("comptability \n");

    prep_draw();
    sort_draw();

    //GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);


    glColor3ub(255,255,255);

    glUseProgramObjectARB(map_shader[0]);

    glBindTexture( GL_TEXTURE_2D, block_textures_compatibility );

    glEnableVertexAttribArray(map_Vertex);
    glEnableVertexAttribArray(map_TexCoord);
    glEnableVertexAttribArray(map_RGB);
    glEnableVertexAttribArray(map_LightMatrix);

    struct Map_vbo* vbo;

    //glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array );

    for(int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i].map_vbo;

        if(vbo->_v_num[0] == 0)
        {
            //printf("t_vbo_draw.cpp:117 no blocks\n");
            continue; 
        } 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);
        
        glUniform3f(map_ChunkPosition, vbo->xoff, vbo->yoff, 0.0f);

        glVertexAttribPointer(map_Vertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);         
        glVertexAttribPointer(map_TexCoord, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)4);

        glVertexAttribPointer(map_RGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        glVertexAttribPointer(map_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);


        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
    }

    glDisableVertexAttribArray(map_Vertex);
    glDisableVertexAttribArray(map_TexCoord);
    glDisableVertexAttribArray(map_RGB);
    glDisableVertexAttribArray(map_LightMatrix);


    glUseProgramObjectARB(0);


    //glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);

}

}
