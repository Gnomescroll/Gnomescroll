#include "t_vbo_draw.hpp"

#include <t_map/t_vbo_class.hpp>
#include <t_map/glsl/shader.hpp>
#include <t_map/glsl/texture.hpp>
#include <camera/camera.hpp>
#include <camera/fulstrum_test.hpp>
#include <camera/fulstrum_test2.hpp>
#include <t_map/glsl/settings.hpp>
#include <t_map/glsl/shader.hpp>
#include <common/qsort.h>
#include <physics/quadrant.hpp>

//#include <t_map/glsl/light_texture.hpp>
//#include <SDL/haldCLUT/hald_clut.hpp>

namespace t_map
{

static const int MAX_DRAWN_VBO = 1024;  //this should not be hardcoded; will piss someone off

static int draw_vbo_n;

//int vbo_frustrum[32*32*2];  //start and end chunk index

int map_vbo_draw_state[32*32];

int vbo_frustrum_voff[32*32][7];
int vbo_frustrum_vnum[32*32][7];

struct VBODrawStruct
{
    class MapVBO* map_vbo;
    float distance;
    //int vertex_start; //start vertex for drawing
    //int vertex_end; //end vertex for drawing
    //int index;
    short i;
    short j;
};

static struct VBODrawStruct* draw_vbo_array;

void vbo_draw_init()
{
    draw_vbo_array = (VBODrawStruct*) malloc(MAX_DRAWN_VBO * sizeof(VBODrawStruct));
}

void vbo_draw_end()
{
    free(draw_vbo_array);
}


void VBOMap::prep_frustrum_vertices()
{

    for (int i=0; i<32*32; i++)
    for (int j=0; j<7; j++)
    {
        vbo_frustrum_voff[i][j] = -1;
        vbo_frustrum_vnum[i][j] = -1;
    }

    for (int i=0;i<draw_vbo_n;i++)
    {
        class MapVBO* vbo = draw_vbo_array[i].map_vbo;
        int xi = draw_vbo_array[i].i;
        int xj = draw_vbo_array[i].j;

        int index = 32*xj +xi;

        int min;
        int max;

        float zmin = 0.0f;
        float zmax = 128.0f;
        chunk_top_z_projection(vbo->wxoff, vbo->wyoff, &zmin, &zmax);

        //printf("zmin, zmaz= %f %f \n", zmin, zmax);

        min = floor(zmin / 16.0);
        if (min < 0) min = 0;

        max = ceil(zmax / 16.0);
        if (max > 8) max = 8;

        GS_ASSERT(min <= 8);
        GS_ASSERT(max <= 8);
        GS_ASSERT(min >= 0);
        GS_ASSERT(max >= 0);

        GS_ASSERT(min <= max);  //will be true after AABB test?

        //if (min < max) printf("i,j= %i %i min,max= %i %i \n", xi,xj, min,max);

        //min = 0;
        //max = 8;

        for (int side=0; side<6; side++)
        {
            //means no points in box are inside frustrum? error?
            if (min > max)
            {
                int voff = vbo->vertex_offset[side];
                int vnum = 0;

                vbo_frustrum_voff[index][side] = voff;
                vbo_frustrum_vnum[index][side] = vnum;
                continue;
            }

            if (min == max)
            {
                //dont draw anything, prune
                int voff = vbo->vertex_offset[side];
                int vnum = 0;

                vbo_frustrum_voff[index][side] = voff;
                vbo_frustrum_vnum[index][side] = vnum;
                continue;
            }

            int vs = vbo->voff_array[side][min];
            int ve = vbo->voff_array[side][max];

            //if (min==0) printf("vs=%i \n", vs);

            if (min ==0) GS_ASSERT(vbo->voff_array[side][min] == vbo->vertex_offset[side]);

            int voff = vs;
            int vnum = ve - vs;

            vbo_frustrum_voff[index][side] = voff;
            vbo_frustrum_vnum[index][side] = vnum;


            if (voff+vnum > vbo->vertex_offset[side]+ vbo->vertex_num[side])
            {
                printf("v1= %i v2= %i \n", voff+vnum, vbo->vertex_offset[side]+ vbo->vertex_num[side]);
                printf("voff= %i vnum= %i \n", voff, vnum);
                printf("vbo->vertex_offset[sid]= %i vbo->vertex_num[side]= %i \n", vbo->vertex_offset[side], vbo->vertex_num[side]);
                printf("min= %i max= %i \n", min, max);

                vbo_frustrum_vnum[index][side] = 0;
            }

        }


    }
}


/*
    Do distance check
*/
bool chunk_distance_check(float x, float y)
{
    //static const float dist2 = CAMERA_VIEW_DISTANCE*CAMERA_VIEW_DISTANCE;
    //static const float dist2 = CAMERA_VIEW_DISTANCE_SQUARED;

    static const float sqrt_2f = sqrt(2.0f);
    static const float dist2 = (CAMERA_VIEW_DISTANCE+(sqrt_2f*8))*(CAMERA_VIEW_DISTANCE+(sqrt_2f*8));

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;

    //x = quadrant_translate_f(cx, x);
    //y = quadrant_translate_f(cy, y);

    float dx = cx - x;
    float dy = cy - y;

    return (dx*dx + dy*dy > dist2) ? false : true;
}

void VBOMap::prep_draw()
{
    for (int i=0; i<32*32; i++)
        map_vbo_draw_state[i] = -1;

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;
    GS_ASSERT(is_boxed_point(cx));
    GS_ASSERT(is_boxed_point(cy));

    int c_drawn, c_pruned;
    c_drawn=0; c_pruned=0;
    draw_vbo_n = 0;

    for (int i=0; i<MAP_CHUNK_XDIM; i++)
    for (int j=0; j<MAP_CHUNK_YDIM; j++)
    {
        class MapVBO* col = vbo_array[j*MAP_CHUNK_XDIM + i];

        if (col == NULL || col->vnum == 0) continue;

        float x = translate_point(col->xoff);
        float y = translate_point(col->yoff);
        col->wxoff = quadrant_translate_f(cx, x);
        col->wyoff = quadrant_translate_f(cy, y);

        /*
            Add bounding box check
        */
        // plain chunk distance check has errors in corners


        if (AABB_test(col->wxoff,col->wyoff,0.0f, 16.0,16.0,128.0) != 0)
         {
            map_vbo_draw_state[32*j+i] = 1; //failed AABB test
        }
        else
        if (!chunk_distance_check(col->wxoff+8.0f, col->wyoff+8.0f) ||
            AABB_test(col->wxoff,col->wyoff,0.0f, 16.0,16.0,128.0) == 0)
        {
            map_vbo_draw_state[32*j+i] = 0; //failed distance test
        }

        if (chunk_distance_check(col->wxoff+8.0f, col->wyoff+8.0f) &&
            AABB_test(col->wxoff,col->wyoff,0.0f, 16.0,16.0,128.0) != 0)
        {
            c_drawn++;
            /*
                Fulstrum culling
            */
            draw_vbo_array[draw_vbo_n].map_vbo = col;
            draw_vbo_array[draw_vbo_n].i = i;
            draw_vbo_array[draw_vbo_n].j = j;

            draw_vbo_n++;

            if (draw_vbo_n == MAX_DRAWN_VBO)
            {
                printf("VBOMap::prep_draw(), ERROR, draw_vbo == MAX_DRAWN_VBO \n");
                return;
            }
            //printf("drew %0.2f, %0.2f, \n");
        }
        else
        {
            c_pruned++;
        }
    }
    //printf("drawn: %i pruned: %i \n",  c_drawn, c_pruned);
    //printf("\n");
}

void VBOMap::sort_draw()
{
    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;
    GS_ASSERT(is_boxed_point(cx));
    GS_ASSERT(is_boxed_point(cy));

    for (int i=0; i<draw_vbo_n; i++)
    {
        class MapVBO* v = draw_vbo_array[i].map_vbo;

        float dx = (v->wxoff - cx);
        float dy = (v->wyoff - cy);

        draw_vbo_array[i].distance = dx*dx + dy*dy; //set this
    }

  #define _VBO_DRAW_STRUCT_lt(a,b) ((a)->distance < (b)->distance)
  QSORT(struct VBODrawStruct, draw_vbo_array, draw_vbo_n, _VBO_DRAW_STRUCT_lt);

  //if (draw_vbo_n > 10) draw_vbo_n = 10;
}

#define ADV_PRUNE 1

/*
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
GL_MODELVIEW_MATRIX or GL_PROJECTION_MATRIX

Compute matrix by hand and pass in uniform
*/

void VBOMap::draw_map()
{
    IF_ASSERT(map_shader.shader == NULL) return;

    prep_draw();
    sort_draw();
    prep_frustrum_vertices();

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);

    glEnable(GL_CULL_FACE);

    //glEnable(GL_ALPHA_TEST);
    //glAlphaFunc(GL_GEQUAL, 0.5);

    glDisable(GL_TEXTURE_2D);

    //glActiveTextureARB(GL_TEXTURE0_ARB);
    //glBindTexture(GL_TEXTURE_2D, texture1);
    //glEnable(GL_TEXTURE_2D);

    glColor3ub(255,255,255);


    //glActiveTexture(GL_TEXTURE0);

    //glBindTexture(GL_TEXTURE_3D, generate_clut_texture());

    //glBindTexture(GL_TEXTURE_2D_ARRAY, map_shader.terrain_map_glsl);


    //uniform sampler3D clut_texture;
    //uniform sampler2DArray base_texture;

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, generate_clut_light_texture());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, generate_clut_texture());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, map_shader.terrain_map_glsl);

    GLint clut_light_texture = glGetUniformLocation(map_shader.shader->shader, "clut_light_texture");
    GLint clut_texture = glGetUniformLocation(map_shader.shader->shader, "clut_texture");
    GLint base_texture = glGetUniformLocation(map_shader.shader->shader, "base_texture");

    GS_ASSERT(clut_light_texture != 0);
    GS_ASSERT(clut_texture != 0);
    GS_ASSERT(base_texture != 0);

    glUseProgramObjectARB(map_shader.shader->shader);

    //CHECK_GL_ERROR();
    //glActiveTextureARB(GL_TEXTURE0);

    glUniform1i(clut_light_texture, 1); //Texture unit 0 is for clut_light_texture
    glUniform1i(clut_texture, 2); //Texture unit 1 is for clut_texture
    glUniform1i(base_texture, 3); //Texture unit 2 is for base_texture

    //glEnable(GL_TEXTURE_2D);


    //glActiveTexture(GL_TEXTURE1);
    //glEnable(GL_TEXTURE_3D);

    //glActiveTexture(GL_TEXTURE0);
    //glEnable(GL_TEXTURE_3D);

    //glActiveTextureARB(GL_TEXTURE1);


    //CHECK_GL_ERROR();



    glEnableVertexAttribArray(map_shader.InVertex);
    glEnableVertexAttribArray(map_shader.InTexCoord);
    glEnableVertexAttribArray(map_shader.InRGB);
    glEnableVertexAttribArray(map_shader.InLightMatrix);
    glEnableVertexAttribArray(map_shader.InLight);

    //CHECK_GL_ERROR();

    class MapVBO* vbo;

    //float modelview[16];
    //glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    //glPushMatrix();

    #if ADV_PRUNE
    //int v_total = 0;
    //int v_drawn = 0;
    //int v_pruned = 0;
    #endif


    /*
        Draw
    */
    for (int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i].map_vbo;

        if (vbo->_v_num[0] == 0) continue;

        //glLoadMatrixf(modelview);
        //glTranslatef(vbo->wxoff, vbo->wyoff, 0.0f);

        //GS_ASSERT(vbo->vbo_id != 0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);

        glUniform4f(map_shader.InOffset, vbo->wxoff, vbo->wyoff, 0.0f, 0.0f);

        glVertexAttribPointer(map_shader.InVertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
        glVertexAttribPointer(map_shader.InTexCoord, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)4);
        glVertexAttribPointer(map_shader.InRGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        glVertexAttribPointer(map_shader.InLightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);
        glVertexAttribPointer(map_shader.InLight, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)16);

        #if ADV_PRUNE

        int xi = draw_vbo_array[i].i;
        int xj = draw_vbo_array[i].j;
        int index = 32*xj +xi;

        //v_total += vbo->_v_num[0];
        for (int side=0; side<6; side++)
        {
            int voff = vbo_frustrum_voff[index][side];
            int vnum = vbo_frustrum_vnum[index][side];

            if (vnum == 0) continue;

            //v_drawn += vnum;
            glDrawArrays(GL_QUADS, voff, vnum);
        }
        #else
        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
        #endif

        //glPopMatrix();
        //glPushMatrix();
    }

    //printf("v_total= %i v_drawn= %i \n", v_total, v_drawn);

    //glPopMatrix(); //restore matrix


    glDisableVertexAttribArray(map_shader.InVertex);
    glDisableVertexAttribArray(map_shader.InTexCoord);
    glDisableVertexAttribArray(map_shader.InRGB);
    glDisableVertexAttribArray(map_shader.InLightMatrix);
    glDisableVertexAttribArray(map_shader.InLight);

    //printf("%d \n", map_shader.InLight);

    //CHECK_GL_ERROR();

    glUseProgramObjectARB(0);

    //CHECK_GL_ERROR();
    //glEnable(GL_TEXTURE_2D);

    //;
    glDisable(GL_CULL_FACE);
    //glDisable(GL_ALPHA_TEST);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glActiveTexture(GL_TEXTURE0);

    //glDisable(GL_TEXTURE_3D);
    //glDisable(GL_TEXTURE_2D);


    CHECK_GL_ERROR();
}

void VBOMap::draw_map_compatibility()
{
    IF_ASSERT(map_compatibility_shader.shader == NULL) return;

    prep_draw();
    sort_draw();
    prep_frustrum_vertices();

    //GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);

    //;
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);

    glColor3ub(255,255,255);

    glUseProgramObjectARB(map_compatibility_shader.shader->shader);

    glBindTexture(GL_TEXTURE_2D, block_textures_compatibility);

    glEnableVertexAttribArray(map_compatibility_shader.InVertex);
    glEnableVertexAttribArray(map_compatibility_shader.InTexCoord);
    glEnableVertexAttribArray(map_compatibility_shader.InTexCoord2);
    glEnableVertexAttribArray(map_compatibility_shader.InRGB);
    glEnableVertexAttribArray(map_compatibility_shader.InLightMatrix);
    glEnableVertexAttribArray(map_compatibility_shader.InLight);

    class MapVBO* vbo;

    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    glPushMatrix(); //save matrix

    #if ADV_PRUNE
    //int v_total = 0;
    //int v_drawn = 0;
    #endif

    for (int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i].map_vbo;

        if (vbo->_v_num[0] == 0)
        {
            //printf("t_vbo_draw.cpp:117 no blocks\n");
            continue;
        }

        glLoadMatrixf(modelview);
        glTranslatef(vbo->wxoff, vbo->wyoff, 0.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);

        glVertexAttribPointer(map_compatibility_shader.InVertex, 3, GL_FLOAT, GL_FALSE, sizeof(struct VertexBackup), (GLvoid*)0);
        glVertexAttribPointer(map_compatibility_shader.InTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(struct VertexBackup), (GLvoid*)12);
        glVertexAttribPointer(map_compatibility_shader.InTexCoord2, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct VertexBackup), (GLvoid*)20);
        glVertexAttribPointer(map_compatibility_shader.InRGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct VertexBackup), (GLvoid*)24);
        glVertexAttribPointer(map_compatibility_shader.InLightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct VertexBackup), (GLvoid*)28);
        glVertexAttribPointer(map_compatibility_shader.InLight, 2, GL_FLOAT, GL_FALSE, sizeof(struct VertexBackup), (GLvoid*)32);

        #if ADV_PRUNE

        int xi = draw_vbo_array[i].i;
        int xj = draw_vbo_array[i].j;
        int index = 32*xj +xi;

        //v_total += vbo->_v_num[0];
        for (int side=0; side<6; side++)
        {
            int voff = vbo_frustrum_voff[index][side];
            int vnum = vbo_frustrum_vnum[index][side];

            if (vnum == 0) continue;

            //v_drawn += vnum;
            glDrawArrays(GL_QUADS, voff, vnum);
        }
        #else
        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
        #endif

    }

    //printf("v_total= %i v_drawn= %i \n", v_total, v_drawn);

    glPopMatrix(); //restore matrix

    glDisableVertexAttribArray(map_compatibility_shader.InVertex);
    glDisableVertexAttribArray(map_compatibility_shader.InTexCoord);
    glDisableVertexAttribArray(map_compatibility_shader.InTexCoord2);
    glDisableVertexAttribArray(map_compatibility_shader.InRGB);
    glDisableVertexAttribArray(map_compatibility_shader.InLightMatrix);
    glDisableVertexAttribArray(map_compatibility_shader.InLight);

    glUseProgramObjectARB(0);


    //glEnable(GL_TEXTURE_2D);
    //;
    glDisable(GL_CULL_FACE);
    CHECK_GL_ERROR();

}


void draw_vbo_debug(int x, int y)
{
    static float psize = 8.0;
    static float off = 0.5;
    static float sep = 2.0;

    int cx = current_camera_position.x;
    int cy = current_camera_position.y;

    cx = cx / 16;
    cy = cy / 16;

    glPointSize(psize);

    glBegin(GL_POINTS);

    for (int i=0; i<32; i++)
    for (int j=0; j<32; j++)
    {

        int index = 32*j +i;
        int v = map_vbo_draw_state[index];

        if (v==-1) glColor3ub(255, 255, 255);
        if (v==0) glColor3ub(255, 0, 0);
        if (v==1) glColor3ub(0, 255, 0);
        if (v==2) glColor3ub(0, 0, 255);

        if (i==cx && j == cy)
            glColor3ub(255, 255, 0);

        glVertex3f((float) x+(psize+sep)*i+off, (float) y+(psize+sep)*j+off, -0.1f);

    }

    glEnd();

    //loaded/not loaded

    glColor3ub(255, 255, 255);

    glBegin(GL_POINTS);

    for (int i=0; i<32; i++)
    for (int j=0; j<32; j++)
    {

        int index = 32*j +i;
        int v = map_vbo_draw_state[index];

        if (v==-1) glColor3ub(255, 255, 255);
        if (v==0) glColor3ub(255, 0, 0);
        if (v==1) glColor3ub(0, 255, 0);
        if (v==2) glColor3ub(0, 0, 255);

        if (i==cx && j == cy)
            glColor3ub(255, 255, 0);

        glVertex3f((float) x+(psize+sep)*i+off, (float) y+(psize+sep)*j+off, -0.1f);

    }

    glEnd();

    const float yoff_1 = -32*(psize+sep) - 16;

    glBegin(GL_POINTS);
    for (int i=0; i<MAP_CHUNK_XDIM; i++)
    for (int j=0; j<MAP_CHUNK_YDIM; j++)
    {
        int index = 32*j +i;
        class MapVBO* col = vbo_map->vbo_array[index];

        glColor3ub(0, 255, 0); // everything find/loaded

        if (main_map->chunk[index] == NULL)
        {
            glColor3ub(255, 0, 0);
        }
        else
        {
            if (col == NULL)
            {
                glColor3ub(127, 0, 0);
            }
            else if (col->vnum == 0)
            {
                glColor3ub(0,255,255);
            }
        }
        if (i==cx && j == cy)
            glColor3ub(255, 255, 0);

        glVertex3f((float) x+(psize+sep)*i+off, (float) yoff_1+y+(psize+sep)*j+off, -0.1f);
    }
    glEnd();


    const float xoff_1 = 32*(psize+sep) + 16;

    glBegin(GL_POINTS);
    for (int i=0; i<MAP_CHUNK_XDIM; i++)
    for (int j=0; j<MAP_CHUNK_YDIM; j++)
    {
        int index = 32*j +i;
        //class MapVBO* col = vbo_map->vbo_array[index];

        glColor3ub(0, 255, 0); // everything find/loaded

        if (main_map->chunk[index] == NULL)
        {
            glColor3ub(255, 0, 0);
        }
        else
        {
            if (main_map->chunk[index]->needs_update)
            {
                glColor3ub(0, 0, 255);
            }
            else
            {
                glColor3ub(0, 255, 255);
            }
        }
        if (i==cx && j == cy)
            glColor3ub(255, 255, 0);

        glVertex3f(xoff_1+x+(psize+sep)*i+off, yoff_1+y+(psize+sep)*j+off, -0.1f);
    }
    glEnd();


    glPointSize(1.0);
}

}   // t_map
