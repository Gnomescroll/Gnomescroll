#pragma once

#include <t_map/t_vbo_class.hpp>

namespace t_map
{

class Shader map_ortho_shader;


int map_ortho_Vertex;
int map_ortho_TexCoord;
int map_ortho_RGB;
//int map_ortho_Normal;

int map_ortho_LightMatrix;

int map_ortho_ChunkPosition;   //uniform
//int map_ortho_NormalArray;     //uniform



void load_ortho_shader()
{
    map_ortho_shader.set_debug(true);

    map_ortho_shader.load_shader("t_map ortho shader",
        MEDIA_PATH "shaders/terrain/terrain_map_mipmap_bilinear_ao_ortho.vsh",
        MEDIA_PATH "shaders/terrain/terrain_map_mipmap_bilinear_ao_ortho.fsh");

    map_ortho_Vertex = map_ortho_shader.get_attribute("InVertex");
    map_ortho_TexCoord = map_ortho_shader.get_attribute("InTexCoord");
    map_ortho_RGB = map_ortho_shader.get_attribute("InRGB");
    map_ortho_LightMatrix = map_ortho_shader.get_attribute("InLightMatrix");

    map_ortho_ChunkPosition = map_ortho_shader.get_uniform("ChunkPosition");
}


void render_ortho()
{
    GL_ASSERT(GL_DEPTH_TEST, true);

    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    glUseProgramObjectARB(map_ortho_shader.shader);


    glColor3ub(255,255,255);

    glBindTexture(GL_TEXTURE_2D_ARRAY, terrain_map_glsl);


    glEnableVertexAttribArray(map_ortho_Vertex);
    glEnableVertexAttribArray(map_ortho_TexCoord);
    glEnableVertexAttribArray(map_ortho_RGB);
    glEnableVertexAttribArray(map_ortho_LightMatrix);

    //struct MapVBO* vbo;

    //glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array);

    for (int i=0; i<MAP_CHUNK_XDIM; i++)
    for (int j=0; j<MAP_CHUNK_YDIM; j++)
    {
        int index = j*MAP_CHUNK_XDIM + i;

        class MapVBO* vbo = vbo_map->vbo_array[index];
        if (vbo == NULL) continue;
        if (vbo->_v_num[0] == 0) continue;

        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);

        glUniform3f(map_ortho_ChunkPosition, vbo->xoff, vbo->yoff, 0.0f);
        //glUniform3f(map_ChunkPosition, vbo->wxoff, vbo->wyoff, 0.0f);

        glVertexAttribPointer(map_ortho_Vertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
        glVertexAttribPointer(map_ortho_TexCoord, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)4);
        glVertexAttribPointer(map_ortho_RGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        glVertexAttribPointer(map_ortho_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);

        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
    }

    glDisableVertexAttribArray(map_ortho_Vertex);
    glDisableVertexAttribArray(map_ortho_TexCoord);
    glDisableVertexAttribArray(map_ortho_RGB);
    glDisableVertexAttribArray(map_ortho_LightMatrix);

    glUseProgramObjectARB(0);

    //glEnable(GL_TEXTURE_2D);

    //;
    glDisable(GL_CULL_FACE);
}

OPTIMIZED
void save_map_ortho_projection(const char* filename)
{
    vbo_map->load_all();
    load_ortho_shader();



    unsigned int color_tex, fb, depth_rb;
    const int xres = 4096;
    const int yres = 4096;

    //RGBA8 2D texture, 24 bit depth texture, 256x256
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //NULL means reserve texture memory, but texels are undefined
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xres,yres, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    //-------------------------
    glGenFramebuffersEXT(1, &fb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    //Attach 2D texture to this FBO
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0);
    //-------------------------
    glGenRenderbuffersEXT(1, &depth_rb);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, xres,yres);
    //-------------------------
    //Attach depth buffer to FBO
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
    //-------------------------
    //Does the GPU support current FBO configuration?
    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status)
    {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
        printf("FBO works\n");
        break;

        default:
        printf("FBO error\n");
        break;
    }
    //-------------------------
    //and now you can render to GL_TEXTURE_2D
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //-------------------------
    glViewport(0, 0, xres, yres);

/*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (float) xres, 0.0, (float) yres, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
*/
    //-------------------------
    //glDisable(GL_TEXTURE_2D);
    //glEnable(GL_TEXTURE_2D);
    //glDisable(GL_BLEND);
    //glDisable(GL_DEPTH_TEST);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // DEPRECATE GLU


    #if 0
    float fov = 85.0;
    float ratio = 1.0;

    float z_near = 1.0;
    float z_far = 1024;

    gluPerspective(fov, ratio, z_near, z_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Vec3 right = vec3_init(0.0, 1.0, 0.0);
    Vec3 up = vec3_init(1.0f, 0.0f, 0.0f);
    Vec3 look = vec3_init(0.0, 0.0, -1.0);
    //look = vec3_euler_rotation(look, theta + 1.00, phi - 1.00, 0.0);

    float x = 256.0f;
    float y = 256.0f;
    float z = 128.0f;

    gluLookAt(x,y,z,
              x + look.x, y + look.y, z + look.z,
              up.x, up.y, up.z);
    #else

    float z_near = 1.0f;
    float z_far = 1024;
    glOrtho(-256, 256, -256, 256, z_near, z_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double dist = sqrtf(1.0f / 3.0f);

    float f1 = 128.0f;
    float f2 = 128.0f;
    gluLookAt(f1*dist, f1*dist, f1*dist + f2,  /* position of camera */
              256.0f,  256.0f,  f2,   /* where camera is pointing at */
              0.0f,  0.0f,  1.0f);  /* which direction is up */
    #endif


    /*
        Render
    */

    render_ortho();
    //glFinish();

    unsigned char* pixel_buffer = new unsigned char[4*xres*yres];
    GLenum format = GL_BGRA;

    glReadPixels(0, 0, xres, yres, format, GL_UNSIGNED_BYTE, (void*) pixel_buffer);


    //Delete resources
    glDeleteTextures(1, &color_tex);
    glDeleteRenderbuffersEXT(1, &depth_rb);
    //Bind 0, which means render to back buffer, as a result, fb is unbound
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &fb);

    //http://stackoverflow.com/questions/5102277/resizing-an-image-using-opengl
    // setup another framebuffer here
    // bind the 64 pixel texture
    // draw to a 256x256 framebuffer
    // save surface to png again


    //char FileName[128];
    //sprintf(FileName,SCREENSHOT_PATH "%s.png", (char*) filename);

    {
        unsigned char* PBUFFER = pixel_buffer;
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(4*xres);
        if (NULL == temp_row)
        {
            printf("save_screenshot: not enough memory for surface inversion \n");
            return;
        }
        int pitch = xres * 4;
        int h = yres;

        height_div_2 = (int) (yres * 0.5f);
        for (index = 0; index < height_div_2; index++)
        {
            memcpy((Uint8 *)temp_row, (Uint8 *)(PBUFFER) + pitch * index, pitch);
            memcpy((Uint8 *)(PBUFFER) + pitch * index, (Uint8 *)PBUFFER + pitch * (h - index-1), pitch);
            memcpy((Uint8 *)(PBUFFER) + pitch * (h - index-1), temp_row, pitch);
        }
        free(temp_row);
    }


    char FileName[128];
    sprintf(FileName,SCREENSHOT_PATH "%s.png", (char*) "ortho_test");

    size_t png_size;
    char* PNG_IMAGE = (char*) tdefl_write_image_to_png_file_in_memory(
        (const char*) pixel_buffer, xres, yres, 4, &png_size);

    FILE* pFile;
    pFile = fopen (FileName , "wb");
    if (!pFile)
    {
        printf("Error: could not save image.  Check that screenshot folder exists\n");
        return;
    }
    fwrite (PNG_IMAGE , 1 , png_size, pFile);
    fclose (pFile);

    free(PNG_IMAGE);
    delete[] pixel_buffer;

    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport (0, 0, _xres, _yres);
}

}   // t_map
