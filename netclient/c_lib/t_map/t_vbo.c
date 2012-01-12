#include "./t_vbo.h"

#include <stdio.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.h>

#include <c_lib/SDL/shader_loader.hpp>

//shaders


/*
Optimizations: 
0> implement benchmarks for measurement
1> minimize working set size of map to 16x16x16 for cache locality. (dont iterate through whole thing multiple times)
-try 16x16x8 also
-the map wont get pushed out of cache multiple times
2> reduce size of map element to minimize cache size and number of cache lines required
2> replace int arrays with chars to improve cache performance (benchmark)
3> replace if statements with switch statements (jump table)
4> cacheline align map properties and use 64 bit flag table (procomputed, make them less than 64 bytes or enable jump table)
5> use internal map chunk get()/lookup instead of the pointer one, except for edges

Map Fetches:
-use the map-get that is not error checked
-prefetch pointer to ajacent map chunk
-check each vertex direction seperately (reduces polygon count at later stages, iteration through in cache data)

possibly faster:
5> first pass through, pushing work onto list for dispatch later
-map array /property list and quad-cache wont compete for L1/L2 cache
*/

const int SHADER_ON = 1;


    static GLenum shader_vert = 0;
    static GLenum shader_frag = 0;
    static GLenum shader_prog = 0;



void setShaders2() 
{

    int DEBUG = 0;

    shader_prog = glCreateProgramObjectARB();
    shader_vert = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    shader_frag = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;
    //vs = textFileRead((char*) "./media/shaders/lsd.vsh");
    //fs = textFileRead((char*) "./media/shaders/lsd.fsh");

    if(DEBUG) printf("setShaders2: \n");

    vs = textFileRead((char*) "./media/shaders/terrain/terrain_map_00.vsh");
    fs = textFileRead((char*) "./media/shaders/terrain/terrain_map_00.fsh");

    //glShaderSourceARB(shader_vert, 1, (const GLcharARB**)&shader_vert_source, &len);
    //glShaderSourceARB(shader_frag, 1, (const GLcharARB**)&shader_frag_source, &len);

    glShaderSourceARB(shader_vert, 1, (const GLcharARB**)&vs, NULL);
    glShaderSourceARB(shader_frag, 1, (const GLcharARB**)&fs, NULL);
    glCompileShaderARB(shader_vert);
    if(DEBUG) printShaderInfoLog(shader_vert);

    glCompileShaderARB(shader_frag);
    if(DEBUG) printShaderInfoLog(shader_frag);
    
    glAttachObjectARB(shader_prog, shader_vert);
    glAttachObjectARB(shader_prog, shader_frag);

    /* Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color */
    /* Attribute locations must be setup before calling glLinkProgram. */
    
    //glBindAttribLocation(shaderprogram, 0, "in_Position");
    //glBindAttribLocation(shaderprogram, 1, "in_Color");

    //glBindAttribLocation(shader_prog, 0, "vert_pos");
    //glBindAttribLocation(shader_prog, 1, "vert_uv");
    glBindAttribLocation(shader_prog, 2, "vert_rgba");
    //glBindAttribLocation(shaderprogram, 3, "vert_normal");

    glLinkProgramARB(shader_prog);

    if(DEBUG) printProgramInfoLog(shader_prog); // print diagonostic information     
}

static GLenum shader_vert3 = 0;
static GLenum shader_frag3 = 0;
static GLenum shader_prog3 = 0;

int texCoord0Loc; // = glGetAttribLocation(MyShader, "InTexCoord0");

void setShaders3() 
{

    int DEBUG = 0;

    shader_prog3 = glCreateProgramObjectARB();
    shader_vert3 = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    shader_frag3 = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;

    if(DEBUG) printf("setShaders3: \n");

    vs = textFileRead((char*) "./media/shaders/terrain_map_mipmap.vsh");
    fs = textFileRead((char*) "./media/shaders/terrain_map_mipmap.fsh");


    glShaderSourceARB(shader_vert3, 1, (const GLcharARB**)&vs, NULL);
    glShaderSourceARB(shader_frag3, 1, (const GLcharARB**)&fs, NULL);
    glCompileShaderARB(shader_vert3);
    if(DEBUG) printShaderInfoLog(shader_vert3);

    glCompileShaderARB(shader_frag3);
    if(DEBUG) printShaderInfoLog(shader_frag3);
    
    glAttachObjectARB(shader_prog3, shader_vert3);
    glAttachObjectARB(shader_prog3, shader_frag3);

    glLinkProgramARB(shader_prog3);

    if(DEBUG) printProgramInfoLog(shader_prog3); // print diagonostic information
    
    texCoord0Loc = glGetAttribLocation(shader_prog3, "InTexCoord0");
}

static GLenum shader_vert4 = 0;
static GLenum shader_frag4 = 0;
static GLenum shader_prog4 = 0;

int texCoord0Loc_4;
int LightMatrix0Loc_4;
int LightMatrix1Loc_4;

void setShaders4() 
{

    int DEBUG = 0;

    shader_prog4 = glCreateProgramObjectARB();
    shader_vert4 = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    shader_frag4 = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;

    if(DEBUG) printf("setShaders4: \n");

    vs = textFileRead((char*) "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.vsh");
    fs = textFileRead((char*) "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.fsh");

    //vs = textFileRead((char*) "./media/shaders/terrain_map_mipmap.vsh");
    //fs = textFileRead((char*) "./media/shaders/terrain_map_mipmap.fsh");

    glShaderSourceARB(shader_vert4, 1, (const GLcharARB**)&vs, NULL);
    glShaderSourceARB(shader_frag4, 1, (const GLcharARB**)&fs, NULL);
    glCompileShaderARB(shader_vert4);
    if(DEBUG) printShaderInfoLog(shader_vert4);

    glCompileShaderARB(shader_frag4);
    if(DEBUG) printShaderInfoLog(shader_frag4);
    
    glAttachObjectARB(shader_prog4, shader_vert4);
    glAttachObjectARB(shader_prog4, shader_frag4);

    glLinkProgramARB(shader_prog4);

    if(DEBUG) printProgramInfoLog(shader_prog4); // print diagonostic information
    
    texCoord0Loc_4 = glGetAttribLocation(shader_prog4, "InTexCoord0");
    LightMatrix0Loc_4 = glGetAttribLocation(shader_prog4, "InLightMatrix0");
    //LightMatrix1Loc_4 = glGetAttribLocation(shader_prog4, "InLightMatrix1");      
}

SDL_Surface *terrain_map_glsl_surface_1;
GLuint terrain_map_glsl_1;

void initTexture3()
{
/*
    GLenum internalFormat = GL_RGBA;
    GLenum format = GL_RGBA;
    s32 w = 128;
    s32 h = 128;
    s32 d = numberOfTilesets * 16;
    u8 * Pixels = new u8[w * h * d * 4];

    glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, Pixels);
*/

//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    terrain_map_glsl_surface_1=IMG_Load("media/texture/blocks_01.png");
    if(!terrain_map_glsl_surface_1) {printf("IMG_Load: %s \n", IMG_GetError());return;}


    GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
    
    GLuint format;
    if (terrain_map_glsl_surface_1->format->Rmask == 0x000000ff) format = GL_RGBA;
    if (terrain_map_glsl_surface_1->format->Rmask != 0x000000ff) format = GL_BGRA;

    int w = 32;
    int h = 32;
    int d = 256;
    Uint32* Pixels = new Uint32[w * h * d];

    SDL_LockSurface(terrain_map_glsl_surface_1);

    //int px, py;
    //unsigned char r,b,g,a;
    int index;
    Uint32 pix;

for(int _i=0; _i < 16; _i++) {
for(int _j=0; _j < 16; _j++) {

    //px = 32*_i;
    //py = 512*_j;
    index = _i + 16*_j;

    for(int i=0; i < 32; i++) {
    for(int j=0; j < 32; j++) {
        pix = ((Uint32*) terrain_map_glsl_surface_1->pixels)[ 512*(j+32*_j) + (i+32*_i) ];
        Pixels[ 32*32*index + (j*32+i) ] = pix;
    }
    }
}
}

/*
    for(int i = 0; i < w*h*d; i++) 
    {
        ((Uint32*) terrain_map_glsl_surface_1->pixels)[i] = Pixels[i];
    }
*/
    SDL_UnlockSurface(terrain_map_glsl_surface_1);
               
    //SDL_SaveBMP(terrain_map_glsl_surface_1, (const char*) "test.bmp");
    
    glEnable(GL_TEXTURE_2D);

    glGenTextures( 1, &terrain_map_glsl_1 );
    glBindTexture(GL_TEXTURE_2D_ARRAY, terrain_map_glsl_1);

    //glGenTextures( 1, &terrain_map_glsl_1 );
    //glBindTexture( GL_TEXTURE_2D, terrain_map_glsl_1 );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

if(0)
{
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
} else {
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 6);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_TRUE);
}

if(0)
{
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, NULL);
    
    if(0) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, 0, 0, 0, w, h, 0, format, GL_UNSIGNED_BYTE, Pixels);
    } else {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, 0, 0, 0, w, h, 0, format, GL_UNSIGNED_BYTE, terrain_map_glsl_surface_1->pixels);
    }

    //glTexSubImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, 0, 0, 0, w, h, 256, format, GL_UNSIGNED_BYTE, Pixels);
    //glTexSubImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, 0, 0, 0, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, Pixels+1*32*32);
    //glTexSubImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, 0, 0, 0, w, h, 2, GL_RGBA, GL_UNSIGNED_BYTE, Pixels+2*32*32);
} else {
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, Pixels);
    }
    //glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, terrain_map_glsl_surface_1->pixels);

    //possible texture problem
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );
    //glTexImage2D(GL_TEXTURE_2D, 0, texture_format, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );

    //glGenTextures( 1, &terrain_map_glsl_1_no_gamma_correction );
    //glBindTexture( GL_TEXTURE_2D, terrain_map_glsl_1_no_gamma_correction );

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexImage2D(GL_TEXTURE_2D, 0, 4, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );

    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glDisable(GL_TEXTURE_2D);
    

}



/*
void glVertexAttribPointer( GLuint      index,
    GLint   size,
    GLenum      type,
    GLboolean   normalized,
    GLsizei     stride,
    const GLvoid *      pointer);
*/

/*
glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
//glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
*/

//glBindAttribLocation(shaderprogram, 0, "vert_xyz");
//glBindAttribLocation(shaderprogram, 1, "vert_uv");
//glBindAttribLocation(shaderprogram, 2, "vert_rgba");
//glBindAttribLocation(shaderprogram, 3, "vert_normal");

//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0)
//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12)
//glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)24)
//glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)24)


GLuint block_texture = 0;
GLuint block_texture_no_gamma_correction = 0; //deprecate if nothing is using this
 
int must_lock_block_surface;
SDL_Surface *block_surface;
SDL_PixelFormat *block_surface_pixel_format;
int block_surface_width, block_surface_height;

int draw_mode_enabled = 0;

unsigned int _garbage_collection_counter = 0;
#define garbage_collection_frequency 10

int T_MAP_BLEND_MODE = 0;

void _toggle_terrain_map_blend_mode() {
    T_MAP_BLEND_MODE = (T_MAP_BLEND_MODE+1)%7;
    printf("toggle_terrain_map_blend_mode: blend mode= %i\n", T_MAP_BLEND_MODE);
}

int T_MAP_Z_BUFFER = 0;
void _toggle_z_buffer() {
    T_MAP_Z_BUFFER = (T_MAP_Z_BUFFER +1) %2;
    if(T_MAP_Z_BUFFER==0) printf("z_buffer off\n");
    if(T_MAP_Z_BUFFER==1) printf("z_buffer on\n");

}


//fulstrum culling globals
struct _Camera {
float x,y,z;
float vx,vy,vz;
float ux,uy,uz; //up direction
float ratio;
float viewangle;
};

int fulstrum_culling;
int view_distance = 40;
struct _Camera c;


/*
glDepthMask — enable or disable writing into the depth buffer

void glDepthMask(   GLboolean   flag);

Specifies whether the depth buffer is enabled for writing. If flag is GL_FALSE, depth buffer writing is disabled. Otherwise, it is enabled. Initially, depth buffer writing is enabled.
*/


///advice
/*
It turns out that if you disable depth testing (glDisable(GL_DEPTH_TEST)),
GL also disables writes to the depth buffer. The correct solution is to tell GL to ignore the depth test
results with glDepthFunc (glDepthFunc(GL_ALWAYS)). Be careful because in this state, if you render a far away
object last, the depth buffer will contain the values of that far object.
 ---
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
RenderScene();
SwapBuffers(hdc);  //For Windows
 *
 */

/*
 OpenGL has a function that can prevent pixels with a specified set of alpha values from writing the colour or Z buffers. For example:


     glAlphaFunc ( GL_GREATER, 0.1 ) ;
     glEnable ( GL_ALPHA_TEST ) ;
*/

//struct Vertex quad_cache[max_cubes*6*4];
struct Vertex* quad_cache = NULL;

struct Vertex* _get_quad_cache() {
    if(quad_cache == NULL) quad_cache = (struct Vertex*) malloc( max_cubes*6*4 * sizeof(struct Vertex));
    return quad_cache;
    //return (struct Vertex*) &quad_cache;
}

/*
#north/south is +/- x
#west/east is +/- y
l = [
        1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , #top
        0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , #bottom
        1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #north
        0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #south
        1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , #west
        0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #east
]
*/

const int _lighting = 0;


GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_diffuse[] = {1.0, 0.0, 0.0};
GLfloat mat_shininess[] = { 50.0 };
GLfloat light_position[] = {256.0, 256.0, 128.0, 0.0};
GLfloat light_color[] = {1.0, 1.0, 1.0, 0.0};
GLfloat diffuse_color[] =  {1.0, 1.0, 1.0, 0.0};
//GLfloat lmodel_ambient[] = {0.8, 0.8, 0.8, 0.0};
GLfloat lmodel_ambient[] = {0.8, 0.8, 0.8, 0.8};

int _init_quad_cache_normals() {
    //return;
    /*
    This function is bugged and writes out of bounds
    */
    //int i,j;
    int cube_id, side, i;

    int n[3];
    for(cube_id=0;cube_id<max_cubes;cube_id++) {
        for(side=0;side<6;side++) {

            if(side == 0) {
                n[0]= 0;
                n[1]= 0;
                n[2]= 1;
            }
            if(side == 1) {
                n[0]= 0;
                n[1]= 0;
                n[2]= -1;
            }
            if(side == 2) {
                n[0]= 1;
                n[1]= 0;
                n[2]= 0;
            }

            if(side == 3) {
                n[0]= -1;
                n[1]= 0;
                n[2]= 0;
            }
            if(side == 4) {
                n[0]= 0;
                n[1]= 1;
                n[2]= 0;
            }
            if(side == 5) {
                n[0]= 0;
                n[1]= -1;
                n[2]= 0;
            }
        /*
            if(side == 1) n = {0,0,-1};
            if(side == 2) n = {1,0,0};
            if(side == 3) n = {-1,0,0};
            if(side == 4) n = {0,1,0};
            if(side == 5) n = {0,-1,0};
        */      
            int index; 
            for(i=0;i<4;i++) {
                index = cube_id*6*4+ 4*side+ i;
                quad_cache[index].normal[0] = n[0];
                quad_cache[index].normal[1] = n[1];
                quad_cache[index].normal[2] = n[2];
                quad_cache[index].normal[3] = 0;
            }
        }
    }

    return 0;
}

//performance counters

static const bool GL_PERF = 0;

GLuint gl_perf_queries[64];
int gl_per_queries_index = 0;

SDL_Surface *px_surface;

int _init_draw_terrain() {

    printf("init: LSD shader \n");
    setShaders2();

    printf("init glsl mipmapp shader \n");
    
    setShaders3();
    setShaders4();

    initTexture3();


    printf("init: draw_terrain \n");
    if( quad_cache == NULL) quad_cache = (struct Vertex*) malloc( max_cubes*6*4 * sizeof(struct Vertex));

    if(GL_PERF) glGenQueries(64, gl_perf_queries);  //generate timer queries for rendering

    //quad_cache = _get_quad_cache();
    _init_quad_cache_normals();

    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_POLYGON_SMOOTH);
    printf("Terrain map: vertex size is %i bytes \n", sizeof(struct Vertex));

    if(block_texture == 0) 
    { 
        printf("init terrain_map: block texture load \n");
        //load block_texture if block_texture is not set
        //block_surface=IMG_Load("media/block_texture/textures_03.png");  //should this be freed?
        block_surface=IMG_Load("media/texture/blocks_01.png");
        if(!block_surface) {printf("IMG_Load: %s \n", IMG_GetError());return 1;
    }
    
    block_surface_pixel_format = block_surface->format;
    must_lock_block_surface = SDL_MUSTLOCK(block_surface);
    block_surface_width = (int)block_surface->w;
    block_surface_height = (int)block_surface->h;
    //SDL_Surface *_surface = IMG_Load("media/block_texture/textures_03.png");
    //if(!_surface) {printf("IMG_Load: %s \n", IMG_GetError());return 0;}

    //SDL_FreeSurface(_surface);

    //Uint32 GL_PIXEL_TYPE = GL_BGR;


    //sdl_pixel_format = surface->format;
    //block_surface_width = (int)surface->w;
    //block_surface_height = (int)surface->h;

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &block_texture );
    glBindTexture( GL_TEXTURE_2D, block_texture );

/*
 * Replace with tiles and use mipmapping for filter
*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);


    int texture_format;
    if (block_surface->format->Rmask == 0x000000ff)
    {
        texture_format = GL_RGBA;
    } else {
        texture_format = GL_BGRA;
    }

    //texture_format2 = GL_BGRA_EXT;
    //texture_format2 = GL_SRGBA_EXT;
    //GL_BGRA_EXT
    //GL_SRGB_ALPHA_EXT

    //internal format, input format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8_EXT, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );

    //possible texture problem
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );
    //glTexImage2D(GL_TEXTURE_2D, 0, texture_format, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );

    glGenTextures( 1, &block_texture_no_gamma_correction );
    glBindTexture( GL_TEXTURE_2D, block_texture_no_gamma_correction );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );

    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glDisable(GL_TEXTURE_2D);
    }

    block_texture = block_texture;

    return 0;
}
/*
deprecated

int create_vbo(struct VBO* q_VBO, struct Vertex* v_list, int v_num) {
    GLuint VBO_id;
    VBO_id = 0;
    if (v_num == 0) { return 0; }
    glEnable(GL_TEXTURE_2D);
    q_VBO->v_list = (struct Vertex* )malloc(v_num*sizeof(struct Vertex)); ///dont forget to free this!!!
    q_VBO->v_num = v_num;
    memcpy(q_VBO->v_list, v_list, v_num*sizeof(struct Vertex));
    glGenBuffers(1, &VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
    glBufferData(GL_ARRAY_BUFFER, v_num*sizeof(struct Vertex), q_VBO->v_list, GL_STATIC_DRAW); // size, pointer to array, usecase
    q_VBO->VBO_id = VBO_id;
    glDisable(GL_TEXTURE_2D);
    //printf("inside: VBO_id= %i \n", VBO_id);
    return VBO_id;
}
*/

void delete_vbo(struct VBO* vbo) {

    if(vbo->VBO_id != 0) glDeleteBuffers(1, &vbo->VBO_id);
    ///free the system memory copy of the vertex buffer
    
    if(vbo->v_list != NULL) {
        free(vbo->v_list);
        vbo->v_list = NULL;
    }
    vbo->VBO_id = 0;
    vbo->v_num = 0;
    vbo->v_list_max_size = 0;
}

int print_vertex(struct Vertex* v) {
printf("x,y,z= %f, %f, %f\n", v->x,v->y,v->z);
printf("tx,ty= %f, %f\n", v->tx, v->ty);
printf("r,g,b,a= %i, %i, %i, %i\n", v->r,v->g,v->b,v->a);
printf("\n");
return 0;
}

/// start VBO

//buffers for VBO stuff
//struct Vertex cs[(128*8*8)*4*6]; //chunk scratch
//unsigned int cs_n; //number of vertices in chunk scratch

//cache line optimization; minimize size
static const int CI[6*8*3] = {1, 1, 1, 0, 1, 1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1,
-1, 1, -1, 0, 1, -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1,
1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1, 1, 1, 1, 0, 1,
-1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1, -1, 1, -1, 0, 1,
1, 1, 1, 1, 1, 0, 1, 1, -1, 0, 1, -1, -1, 1, -1, -1, 1, 0, -1, 1, 1, 0, 1, 1,
-1, -1, 1, -1, -1, 0, -1, -1, -1, 0, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 0, -1, 1 };


static inline int calcAdj(int side_1, int side_2, int corner)  __attribute((always_inline));
static inline int _is_occluded(int x,int y,int z, int side_num) __attribute((always_inline));
static inline int _is_occluded_transparent(int x,int y,int z, int side_num, int _tile_id) __attribute((always_inline));
static inline void add_quad2(struct Vertex* v_list, int offset, int x, int y, int z, int side, int tile_id)  __attribute((always_inline));

/*
will be 1 if is adjacent to any side
will be 2 only if both sides are occluded
 */

//const static int occ_array[3] = { 255, 177, 177 };


//255, 177, 100 

void occ_debug(int side_1, int side_2, int corner, struct Vertex* v_list) 
{
    int occ = (side_1 | side_2 | corner) + (side_1 & side_2);   
    int r,g,b;
    if(occ == 0)
    {
        r = 255;
        g = 0;
        b = 0;
    }
    if(occ == 1)
    {
        r = 0;
        g = 255;
        b = 0;
    }
    if(occ == 2)
    {
        r = 0;
        g = 0;
        b = 255;
    }

    v_list->r = r;
    v_list->g = g;
    v_list->b = b;
}

//const static int occ_array[3] = { 255, 177, 100 };

const static int occ_array[3] = { 255, 128, 64 };

static inline int calcAdj(int side_1, int side_2, int corner) 
{
    int occ = (side_1 | side_2 | corner) + (side_1 & side_2);
    return occ_array[occ];
}

const static int s_array[18] = {
        0,0,1,  //top
        0,0,-1, //bottom
        1,0,0,  //north
        -1,0,0, //south
        0,1,0,  //west
        0,-1,0, //east
        };

static inline int _is_occluded(int x,int y,int z, int side_num)
{
    int i = 3*side_num;
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];

    return isOccludes(_get(x,y,z));
}

static inline int _is_occluded_transparent(int x,int y,int z, int side_num, int _tile_id) {
    int i;
    i = 3*side_num;
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];

    int tile_id = _get(x,y,z);
    if(tile_id == _tile_id) return 1;
    return isActive(tile_id);
}

//inline?
//__attribute((always_inline))
/*
static inline void _set_quad_local_ambient_occlusion(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{
    int i;
    int index;
    int occ;
    int CX[8];
    for(i=0; i<8; i++) 
    {
        index = side*8*3+i*3;
        CX[i] = isOccludes(_get(x+CI[index+0],y+CI[index+1],z+CI[index+2]));
    }

    if(1)
    {
        occ = calcAdj(CX[7], CX[1], CX[0]);
        v_list[offset+0].r = occ;
        v_list[offset+0].g = occ;
        v_list[offset+0].b = occ;

        occ = calcAdj(CX[1], CX[3], CX[2]);
        v_list[offset+1].r = occ;
        v_list[offset+1].g = occ;
        v_list[offset+1].b = occ;

        occ = calcAdj(CX[3], CX[5], CX[4]);
        v_list[offset+2].r = occ;
        v_list[offset+2].g = occ;
        v_list[offset+2].b = occ;

        occ = calcAdj(CX[5], CX[7], CX[6]);
        v_list[offset+3].r = occ;
        v_list[offset+3].g = occ;
        v_list[offset+3].b = occ;
    } else {
        occ_debug(CX[7], CX[1], CX[0], &v_list[offset+0]);
        occ_debug(CX[1], CX[3], CX[2], &v_list[offset+1]);
        occ_debug(CX[3], CX[5], CX[4], &v_list[offset+2]);
        occ_debug(CX[5], CX[7], CX[6], &v_list[offset+3]);
    }
}
*/

static inline void _set_quad_local_ambient_occlusion(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{
    int i;
    int index;
    int CX[8];
    for(i=0; i<8; i++) 
    {
        index = side*8*3+i*3;
        CX[i] = isOccludes(_get(x+CI[index+0],y+CI[index+1],z+CI[index+2]));
    }

    {
        int occ1, occ2, occ3, occ4;

        occ1 = calcAdj(CX[7], CX[1], CX[0]);
        occ3 = calcAdj(CX[1], CX[3], CX[2]);
        occ4 = calcAdj(CX[3], CX[5], CX[4]);
        occ2 = calcAdj(CX[5], CX[7], CX[6]);

        //occ1 = 255;
        //occ2 = 255;
        //occ3 = 255;
        //occ4 = 255;

        //printf("occ1= %i \n", occ1);

        //int value = occ1
        v_list[offset+0].ao[0] = occ1;
        v_list[offset+0].ao[1] = occ2;
        v_list[offset+0].ao[2] = occ3;
        v_list[offset+0].ao[3] = occ4;

        v_list[offset+1].ao[0] = occ1;
        v_list[offset+1].ao[1] = occ2;
        v_list[offset+1].ao[2] = occ3;
        v_list[offset+1].ao[3] = occ4;

        v_list[offset+2].ao[0] = occ1;
        v_list[offset+2].ao[1] = occ2;
        v_list[offset+2].ao[2] = occ3;
        v_list[offset+2].ao[3] = occ4;

        v_list[offset+3].ao[0] = occ1;
        v_list[offset+3].ao[1] = occ2;
        v_list[offset+3].ao[2] = occ3;
        v_list[offset+3].ao[3] = occ4;

        //deprecate when done

        v_list[offset+0].r = occ1;
        v_list[offset+0].g = occ1;
        v_list[offset+0].b = occ1;

        v_list[offset+1].r = occ3;
        v_list[offset+1].g = occ3;
        v_list[offset+1].b = occ3;

        v_list[offset+2].r = occ4;
        v_list[offset+2].g = occ4;
        v_list[offset+2].b = occ4;

        v_list[offset+3].r = occ2;
        v_list[offset+3].g = occ2;
        v_list[offset+3].b = occ2;
    }
}



static inline void add_quad2(struct Vertex* v_list, int offset, int x, int y, int z, int side, int tile_id) {
    int i;
    //struct Vertex* v;
    //memcpy(&cs[cs_n], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num

    memcpy(&v_list[offset], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num

    float _x = x;
    float _y = y;
    float _z = z;
    for(i=0; i<=3;i++) {
        v_list[offset+i].x += _x;
        v_list[offset+i].y += _y;
        v_list[offset+i].z += _z;
    }


    _set_quad_local_ambient_occlusion(v_list, offset, x, y, z, side);
}



static inline int hash_function2(int x,int y,int z) __attribute((always_inline));
static inline int hash_function3(int x,int y,int z) __attribute((always_inline));


static inline int hash_function2(int x,int y,int z)
 {
    unsigned int v = ((x*967 + y)*337 + z);
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    v &= 0xf;
    return (0x6996 >> v) & 1;
}

static inline int hash_function3(int x,int y,int z)
{
    unsigned int v = ((x*967 + y)*337 + z);
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    return v % 3;
    //take modulos of the last byte
}

static const int test_array[16] = {7,9,2,3,10,1,0,4,13,12,8,6,15,11,5,14};
static const int test_array2[16] = { 6,5,2,3,7,14,11,0,10,1,4,13,9,8,15,12};

static inline void add_inf_tex_quad(struct Vertex* v_list, int offset, int x, int y, int z, int side, int tile_id, int infinite_texture) {
    int i;
    //struct Vertex* v;
    //memcpy(&cs[cs_n], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num

    memcpy(&v_list[offset], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num

    //compute hash values

    int id;
    {
        int ul, ur, bl, br;
        ul = hash_function2(x+1, y+1, z);
        ur = hash_function2(x+1, y,   z);
        bl = hash_function2(x,   y,   z);
        br = hash_function2(x,   y+1, z);
    /*
        ul = 0;
        ur = 0;
        bl = 0;
        br = 0;
    */

    /* Working
        int h1 = 3-(y%4);
        int h2 = 3-(x%4);
        int hf = h1+4*h2;
        id= get_infinite_texture(infinite_texture + hf);
    */
       // printf("hash = %i, tex_id= %i tex_base = %i \n", 8*ul + 4*ur + 2*bl + br, infinite_texture + 8*ul + 4*ur + 2*bl + br, infinite_texture);
        //id = infinite_texture + 8*ul + 4*ur + 2*bl + br; //infinite block_texture level 2
        id = get_infinite_texture(infinite_texture + test_array2[1*ul + 2*ur + 4*bl + 8*br]);

        //id = get_infinite_texture(infinite_texture);
    }
    {
        float _x = x;
        float _y = y;
        float _z = z;

        int _tx = id % 16;
        int _ty = id / 16;

        for(i=0; i<=3;i++) {
            v_list[offset+i].x += _x;
            v_list[offset+i].y += _y;
            v_list[offset+i].z += _z;

            v_list[offset+i].tx += _tx*0.0625;
            v_list[offset+i].ty += _ty*0.0625;
        }
    }
    //compute hash values


    _set_quad_local_ambient_occlusion(v_list, offset, x, y, z, side);
}

static inline int _is_occluded_transparent(int x,int y,int z, int side_num, int _tile_id);
static inline int _is_occluded(int x,int y,int z, int side_num);



//Non-normal blocks:vertex count for each
static int oddBlockVCount[7] = {0, 8, 8, 8, 8, 8, 8};

#define _0 0.00
#define _1 1.00

static float oddBlockV[2*12] = {
_0,_1,1, _1,_0,1, _1,_0,0, _0,_1,0,
_1,_1,1, _0,_0,1, _0,_0,0, _1,_1,0
};

//assume 8 vertices
inline void insert_oddBlock(struct Vertex* v_list, int offset, int x, int y, int z, int active, int tile_id) {
    //memcpy(&v_list[offset], &quad_cache[tile_id*6*4+4*side], 4*2*sizeof(struct Vertex)); //id*6*4+4*side+vert_num
    memcpy(&v_list[offset], &quad_cache[tile_id*6*4+0], 4*2*sizeof(struct Vertex));

    float _x = x;
    float _y = y;
    float _z = z;
    int j = active -2;
    int i;
    for(i=0; i<8;i++) {
        v_list[offset+i].x = _x + oddBlockV[12*j+3*i+0];
        v_list[offset+i].y = _y + oddBlockV[12*j+3*i+1];
        v_list[offset+i].z = _z + oddBlockV[12*j+3*i+2];
    }

}


static const int VERTEX_SLACK = 128;

int update_column_VBO(struct vm_column* column) {
    int tile_id, side_num;
    int _x, _y, _z;

    struct VBO* vbo = &column->vbo;
    struct vm_chunk* chunk;
    int i;

    int cube_vertex_count[4] = {0, 0, 0, 0};
    int vertex_count = 0; //clear counter

    //first pass, count quads
    int active;
    int transparency;
    for(i = 0; i < vm_column_max; i++) {
        if(column->chunk[i] == NULL) { continue; }
        //printf("chunk= %i\n", i);
        chunk = column->chunk[i];
        //chunk->vbo_needs_update = 0;
        //printf("1,2,3 = %i, %i, %i \n", 8*chunk->x_off, 8*chunk->y_off, 8*chunk->z_off);
        for(_x = 8*chunk->x_off; _x < 8*chunk->x_off +8 ; _x++) {
        for(_y = 8*chunk->y_off; _y < 8*chunk->y_off +8 ; _y++) {
        for(_z = 8*chunk->z_off; _z < 8*chunk->z_off +8 ; _z++) {
            tile_id = _get(_x,_y,_z);
            //printf("test %i, %i, %i tile= %i\n", _x,_y,_z,tile_id );
            active = isActive(tile_id);
            if(active == 0) continue;

            transparency = isTransparent(tile_id);
            if(active == 1)  
            {
                if(transparency == 0)
                { 
                    for(side_num=0; side_num<6; side_num++) {
                        if(! _is_occluded(_x,_y,_z,side_num)) {
                            //add_quad(_x,_y,_z,side_num,tile_id);
                            vertex_count += 4;
                            cube_vertex_count[0] += 4;
                        }
                    }
                } 
                else
                {
                    //active block that does not occlude
                    for(side_num=0; side_num<6; side_num++) 
                    {
                        if(! _is_occluded_transparent(_x,_y,_z,side_num, tile_id)) 
                        {
                            //add_quad(_x,_y,_z,side_num,tile_id);
                            vertex_count += 4;
                            cube_vertex_count[transparency] += 4;
                        }
                    }
                }
                continue;    
            }
        if(active > 1)  {
            vertex_count += oddBlockVCount[active];
            cube_vertex_count[transparency] += oddBlockVCount[active];
            continue;
        }

        }}}
    }
    //set offsets
    
    //vbo->_v_num[4];       //parameters for draw pass
    //vbo->_v_offset[4];

    vbo->_v_num[0] = cube_vertex_count[0];
    vbo->_v_num[1] = cube_vertex_count[1];
    vbo->_v_num[2] = cube_vertex_count[2];
    vbo->_v_num[3] = cube_vertex_count[3];

    vbo->_v_offset[0] = 0;
    vbo->_v_offset[1] = cube_vertex_count[0];
    vbo->_v_offset[2] = cube_vertex_count[0]+cube_vertex_count[1];
    vbo->_v_offset[3] = cube_vertex_count[0]+cube_vertex_count[1]+cube_vertex_count[2];

    /*
        Now that quads have been counted, malloc memory
        -malloc is slow, so over allocated and reuse if possible 
    */

    //if(cs_n == 0 ) {

    if(vertex_count == 0) {
        //vbo->VBO_id = 0;
        vbo->v_num = 0;

        set_flag(column, VBO_loaded, 0);
        set_flag(column, VBO_needs_update, 0);
        set_flag(column, VBO_has_blocks, 0);
        return 0;
    } else {
        set_flag(column, VBO_loaded, 1);
        set_flag(column, VBO_needs_update, 0);
        set_flag(column, VBO_has_blocks, 1);

        //create_vbo(&column->vbo, cs, cs_n);

        //printf("Malloc: %i vertex \n", vertex_count);
        vbo->v_num = vertex_count; //total vertices, size of VBO

        if(vertex_count > vbo->v_list_max_size) {
            if(vbo->v_list != NULL) {
                free(vbo->v_list);
                vbo->v_list = NULL;
            }
            vbo->v_list_max_size = VERTEX_SLACK+vertex_count;
            vbo->v_list = (struct Vertex*)malloc(vbo->v_list_max_size*sizeof(struct Vertex)); 
        }
    }
    //printf("vbo_id= %i v_num= %i \n", vbo->VBO_id,vbo->v_num);


    int _cube_vertex_count[4];
    _cube_vertex_count[0] = vbo->_v_offset[0];
    _cube_vertex_count[1] = vbo->_v_offset[1];
    _cube_vertex_count[2] = vbo->_v_offset[2];
    _cube_vertex_count[3] = vbo->_v_offset[3];

    struct Vertex* v_list2 = vbo->v_list;

    int infinite_texture;
    for(i = 0; i < vm_column_max; i++) {
        if(column->chunk[i] == NULL) { continue; }
        //printf("chunk= %i\n", i);
        chunk = column->chunk[i];
        //chunk->vbo_needs_update = 0;
        //printf("1,2,3 = %i, %i, %i \n", 8*chunk->x_off, 8*chunk->y_off, 8*chunk->z_off);
        for(_x = 8*chunk->x_off; _x < 8*chunk->x_off +8 ; _x++) {
        for(_y = 8*chunk->y_off; _y < 8*chunk->y_off +8 ; _y++) {
        for(_z = 8*chunk->z_off; _z < 8*chunk->z_off +8 ; _z++) {
            tile_id = _get(_x,_y,_z);
            //printf("test %i, %i, %i tile= %i\n", _x,_y,_z,tile_id );
            active = isActive(tile_id);
            infinite_texture = getInfiniteTexture(tile_id);
            if(active == 0) continue;
            transparency = isTransparent(tile_id);
            if(active == 1)
            {
                if(transparency == 0)
                { 
                    //no infinite block_texture
                    if(infinite_texture == 0) {
                        for(side_num=0; side_num<6; side_num++) {
                            if(! _is_occluded(_x,_y,_z,side_num)) {
                                //printf("count= %i\n",_cube_vertex_count[0]);
                                add_quad2(v_list2, _cube_vertex_count[0], _x,_y,_z,side_num,tile_id);
                                _cube_vertex_count[0] += 4;
                            }
                        }
                    }
                    else  //infinite block_texture
                    {  
                        for(side_num=0; side_num<6; side_num++) {
                            if(! _is_occluded(_x,_y,_z,side_num)) {
                                //add_quad2(v_list2, _cube_vertex_count[0], _x,_y,_z,side_num,tile_id);
                                add_inf_tex_quad(v_list2, _cube_vertex_count[0], _x,_y,_z,side_num,tile_id, infinite_texture);

                                _cube_vertex_count[0] += 4;
                            }
                        }
                    }
                } 
                else
                {
                    //active block that does not occlude
                    for(side_num=0; side_num<6; side_num++) 
                    {
                        if(! _is_occluded_transparent(_x,_y,_z,side_num, tile_id)) 
                        {
                            add_quad2(v_list2, _cube_vertex_count[transparency],_x,_y,_z,side_num,tile_id);
                            _cube_vertex_count[transparency] += 4;
                        }
                    }
                }
                continue;
            }

            if(active > 1)  {
                insert_oddBlock(v_list2, _cube_vertex_count[transparency],_x,_y,_z, active, tile_id);
                _cube_vertex_count[transparency] += oddBlockVCount[active];
                continue;
            }
        }}}
    }
/*
    if(vbo->_v_num[3] > 0) {
        printf("vnum= %i\n", vbo->v_num);
        printf("counts= %i, %i, %i, %i \n", _cube_vertex_count[0],_cube_vertex_count[1],_cube_vertex_count[2],_cube_vertex_count[3]);
        printf("offsets= %i, %i, %i, %i \n", vbo->_v_offset[0], vbo->_v_offset[1], vbo->_v_offset[2], vbo->_v_offset[3]);
        printf("vnum= %i, %i, %i, %i \n", vbo->_v_num[0], vbo->_v_num[1], vbo->_v_num[2], vbo->_v_num[3]);
    }
*/
    /*
        Now that memory is full of quads, push to graphics card
    */

//  GLuint VBO_id;
    //glEnable(GL_TEXTURE_2D);

    if(vbo->VBO_id == 0)  glGenBuffers(1, &vbo->VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
    glBufferData(GL_ARRAY_BUFFER, vbo->v_list_max_size*sizeof(struct Vertex), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vbo->v_list_max_size*sizeof(struct Vertex), vbo->v_list, GL_STATIC_DRAW);

/*
    if( BUFFER_ORPHANING ) {
        if(vbo->VBO_id == 0)  glGenBuffers(1, &vbo->VBO_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        glBufferData(GL_ARRAY_BUFFER, vbo->v_list_max_size*sizeof(struct Vertex), NULL, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, vbo->v_list_max_size*sizeof(struct Vertex), vbo->v_list, GL_STATIC_DRAW);
    } else {
        if(vbo->VBO_id == 0)
        {
            glGenBuffers(1, &vbo->VBO_id);
        } 
        else 
        {
            glDeleteBuffers(1, &vbo->VBO_id);
            glGenBuffers(1, &vbo->VBO_id);
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        glBufferData(GL_ARRAY_BUFFER, vbo->v_list_max_size*sizeof(struct Vertex), vbo->v_list, GL_STATIC_DRAW);
    }
*/
    //glDisable(GL_TEXTURE_2D);

    return 0;
}


int _update_chunks() {
    //printf("_update_chunks \n");
    struct vm_map* m;
    struct vm_column* c;
    int i,j;
    m = _get_map();
    //garbage collection

    _garbage_collection_counter++;
    if(_garbage_collection_counter >= garbage_collection_frequency) {
        _garbage_collection_counter = 0;
        for(i=0; i<vm_map_dim; i++) {
        for(j=0; j<vm_map_dim;j++) {
            c = &m->column[j*vm_map_dim+i];
            //if VBO is farther than 10 viewing units, delete it
            if (flag_is_true(c, VBO_loaded) && !chunk_render_check(c->x_off, c->y_off, 15)) {
                //printf("unloaded VBO: %i, %i \n", c->x_off, c->y_off);
                delete_vbo(&c->vbo);
                set_flag(c, VBO_loaded, 0);
            }
        }}
    }

    //VBO construction loop

    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        c = &m->column[j*vm_map_dim+i];
        //update or create VBO for chunks within 10 units of viewing distance
        if(chunk_render_check(c->x_off, c->y_off, 10)) {
            //if(c->vbo_needs_update == 1 || (c->vbo_loaded==0 && c->vbo_needs_update)) {

        if(flag_is_true(c, VBO_has_blocks) && ( flag_is_true(c, VBO_needs_update) || flag_is_false(c, VBO_loaded))) {
            /*
                if(c->vbo.VBO_id == 0) {
                    //printf("create VBO: %i, %i \n", c->x_off, c->y_off);
                } else {
                    //printf("update VBO: %i, %i \n", c->x_off, c->y_off);
                }
            */
                update_column_VBO(c);
                return 0;
            }
        }


    }}
        return 0;

}

GLuint filter = 0; ;                                // Which Filter To Use
GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
GLuint fogfilter= 2;                    // Which Fog To Use
GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};      // Fog Color

int _draw_fog() {

glClearColor(0.5f,0.5f,0.5f,1.0f);          // We'll Clear To The Color Of The Fog ( Modified )

glFogi(GL_FOG_MODE, fogMode[fogfilter]);        // Fog Mode
glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
glFogf(GL_FOG_DENSITY, 0.35f);              // How Dense Will The Fog Be
glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
glFogf(GL_FOG_START, 1.0f);             // Fog Start Depth
glFogf(GL_FOG_END, 5.0f);               // Fog End Depth
glEnable(GL_FOG);                   // Enables GL_FOG

    return 0;

}
/*
int _t_ = 0;
int _c_ = 0;
*/

static const int MAX_DRAWN_VBO = 1024;  //hard coded value, bad choice

static int draw_vbo_n;
static struct VBO* draw_vbo_array[MAX_DRAWN_VBO];  //this should not be hardcoded; will piss someone off

//GLuint gl_perf_queries[64];
//int gl_per_queries_index = 0;

int _draw_terrain() {


    struct vm_map* m;
    struct vm_column* col;
    int i,j;
    int c_drawn, c_pruned;
    c_drawn=0; c_pruned=0;
    //start_vbo_draw();
    draw_vbo_n = 0;
    m = _get_map();
    //printf("Start Map Draw\n");
    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim; j++) {
        col = &m->column[j*vm_map_dim+i];
        if(flag_is_true(col, VBO_loaded) && chunk_render_check(col->x_off, col->y_off, 0)) {
            c_drawn++;
            set_flag(col,VBO_drawn,1);
            //draw_quad_vbo(&col->vbo);
            /*
                Que up map VBOs to be drawn
                !!! May want to sort VBOs in front to back order
            */

            draw_vbo_array[draw_vbo_n] = &col->vbo;
            draw_vbo_n++;

        } else {
            set_flag(col,VBO_drawn,0);
            c_pruned++;
        }
    }}

    /*
    static GLuint queries[32];
    static GLint available = 0;

    glGenQueries(32, queries);
    */

    //toggle to enable printing performance numbers


    //ShadeModel(GL_FLAT);

        if(SHADER_ON) 
        {
            //DRAW_VBOS3();  
            //DRAW_VBOS4();
            DRAW_VBOS5();
        } 
        else 
        {
            DRAW_VBOS2();
        } 

/*
    if(! GL_PERF) {
       //DRAW_VBOS1();

       //DRAW_VBOS1a();

       if(SHADER_ON) {
       //DRAW_VBOS3();  
       DRAW_VBOS4();
        } else {
        DRAW_VBOS2();
        } 
    } else {

        glBeginQuery(GL_TIME_ELAPSED_EXT, gl_perf_queries[gl_per_queries_index]);
                
        GLuint available = 0;
        GLuint _result = 0;

        //DRAW_VBOS1();
        DRAW_VBOS2();
        //DRAW_VBOS3();

        glEndQuery(GL_TIME_ELAPSED_EXT);

        gl_per_queries_index = (gl_per_queries_index+1) % 32;

        glGetQueryObjectuiv(gl_perf_queries[gl_per_queries_index], GL_QUERY_RESULT_AVAILABLE, &available);
        if(available) {
            glGetQueryObjectuiv(gl_perf_queries[gl_per_queries_index], GL_QUERY_RESULT, &_result);
            printf("querry done: %i us\n", _result/1000);
        }

    }
*/

    //GL_QUERY_RESULT_AVAILABLE
    //GL_QUERY_RESULT  
    
    //printf("drawn chunks= %i, pruned chunks= %i \n", c_drawn, c_pruned);
    //_draw_fog();

    return 0;

}

int __inline chunk_render_check(int x_off, int y_off, int tolerance) {
float x,y;
x = 8*x_off + 4;
y = 8*y_off + 4;

//printf("c.x= %f, c.y= %f, x_off= %i, y_off= %i \n", c.x,c.y,x_off, y_off);
if((c.x-x)*(c.x-x) + (c.y-y)*(c.y-y) < (tolerance+view_distance)*(tolerance+view_distance))
    {
        return 1;
    }
//else
    //{
        return 0;
    //}
}


//DEPRECATE!!!
//DEPRECATE!!!
int _set_camera(float x, float y, float z, float vx, float vy, float vz, float ux, float uy, float uz, float ratio, float viewangle) {
c.x = x; c.y = y; c.z = z;
c.ux=ux; c.uy=uy; c.uz=uz;
c.vx=vx; c.vy=vy; c.vz=vz;
c.ratio = ratio;
c.viewangle = viewangle;
return 0;
}

int _set_view_distance(int vd) {
    view_distance = vd;
    return 0;
}

int _set_fulstrum_culling(int value) {
    fulstrum_culling = value;
    return 0;
}


/*
This loop is 65536 iterations per frame
*/

/*
!!!    Only update map chunks that are near camera
*/

#define SCORE_LOWEST_DEFAULT 1000000
int crb[3]; //the request buffer
int* _chunk_request() {



    struct vm_map* m;
    struct vm_chunk* ch;
    struct vm_column* col;
    int x; int y;
    struct vm_chunk* ch_lowest = NULL;
    int score, score_lowest;
    int i,j,k;

    score_lowest = SCORE_LOWEST_DEFAULT;

    //int _x_ = SDL_GetTicks();
    //int counter = 0;

    m = _get_map();
    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        col = &m->column[j*vm_map_dim+i];
        for(k = 0; k < vm_column_max; k++) {
            //counter++;
            if(col->chunk[k] == NULL) { continue; }
            ch = col->chunk[k];
            if(ch->server_version == ch->local_version || ch->requested == 1) { continue; }
            x = 8*col->x_off + 4;
            y = 8*col->y_off + 4;
            score = (c.x-x)*(c.x-x) + (c.y-y)*(c.y-y);
            //printf("score= %i, x,y,z= %i, %i, %i \n", score, ch->x_off, ch->y_off, ch->z_off);
            if(score < score_lowest) {
                score_lowest = score;
                ch_lowest = ch;
            }
        }
    }}

    //printf("=== chunk_request: took %i ms\n", SDL_GetTicks()- _x_);
    //printf("counter=- %i\n", counter);

    if(score_lowest == SCORE_LOWEST_DEFAULT) {
        return NULL;
    } else {
        //ch = ch_lowest;
        //printf("score= %i, x,y,z= %i, %i, %i \n", score_lowest, ch->x_off, ch->y_off, ch->z_off);
        ch_lowest->requested = 1;
        crb[0] = ch_lowest->x_off;
        crb[1] = ch_lowest->y_off;
        crb[2] = ch_lowest->z_off;
        return crb;
    }
    return NULL;
}

void _refresh_map_vbo() {
    printf("Refreshing map VBOs\n");
    struct vm_map* m;
    struct vm_column* c;
    int i,j;
    m = _get_map();

    //VBO construction loop

    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        c = &m->column[j*vm_map_dim+i];
        if(flag_is_true(c, VBO_has_blocks)) {
                //c->vbo.VBO_id == 0
                set_flag(c, VBO_needs_update, 1);
                set_flag(c, VBO_loaded, 0);
                //flag_is_false(c, VBO_loaded)
            }

    }}
}

//BIG FUNCTION


//int draw_vbo_n;
//struct VBO* draw_vbo_array[512];  

void DRAW_VBOS1() {
        
    int _vnum = 0;

    glColor3b(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable (GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    glAlphaFunc ( GL_GREATER, 0.1 ) ;

    glBindTexture( GL_TEXTURE_2D, block_texture );

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    int i;
    struct VBO* vbo;

    //if(draw_vbo_n != 0)
    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];

        if(vbo->v_num == 0) continue;

        _vnum += vbo->v_num;

        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        //start_vbo_draw();

        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
        //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);

        glEnable(GL_CULL_FACE);
        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable (GL_ALPHA_TEST);
        if(vbo->_v_num[1] != 0) 
        {
            glDrawArrays(GL_QUADS, vbo->_v_offset[1], vbo->_v_num[1]);
        }
    
        glDisable(GL_CULL_FACE);
        if(vbo->_v_num[2] != 0) 
        {
            glDrawArrays(GL_QUADS, vbo->_v_offset[2], vbo->_v_num[2]);
        }
        glDisable(GL_ALPHA_TEST);      
    /*
        if(vbo->_v_num[2] != 0) 
        {
            glDepthMask(false);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
            glDepthMask(true);
        }
    */
        glDisable(GL_BLEND);
    }

/*
    glEnable(GL_BLEND);
    glDepthMask(false);
    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];
        if(vbo->_v_num[3] == 0) continue; 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
    }
    glDepthMask(true); 
    glDisable(GL_BLEND);
*/
    glEnable(GL_BLEND);
    glDepthMask(false);
    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];
        if(vbo->_v_num[3] == 0) continue; 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
        //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
        glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
    }
    glDepthMask(true); 
    glDisable(GL_BLEND);

//end draw
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);

glShadeModel(GL_FLAT);

/*
glDisable (GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);
*/

glDisable(GL_TEXTURE_2D);

    //printf("vnum= %i\n", _vnum);
}

void DRAW_VBOS1a() {

    if(SHADER_ON) glUseProgramObjectARB(shader_prog);

    glColor3b(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    //glEnable(GL_CULL_FACE);

    glAlphaFunc ( GL_GREATER, 0.1 ) ;

    glBindTexture( GL_TEXTURE_2D, block_texture );

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    int i;
    struct VBO* vbo;
    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];

        if(vbo->v_num == 0) continue;

        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        //start_vbo_draw();

        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
        //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);


        glEnable(GL_CULL_FACE);
        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable (GL_ALPHA_TEST);
    
        glDrawArrays(GL_QUADS, vbo->_v_offset[1], vbo->_v_num[1]);
    
        glDisable(GL_CULL_FACE);

        glDrawArrays(GL_QUADS, vbo->_v_offset[2], vbo->_v_num[2]);

        glDisable(GL_ALPHA_TEST);      
    /*
        if(vbo->_v_num[2] != 0) 
        {
            glDepthMask(false);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
            glDepthMask(true);
        }
    */
        glDisable(GL_BLEND);
    }

    glEnable(GL_BLEND);
    glDepthMask(false);
    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];
        if(vbo->v_num == 0) continue; 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);

        glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
    }
    glDepthMask(true); 
    glDisable(GL_BLEND);

/*
    Do Stuff
*/

/*
    vbo->_v_num[0]
    vbo->_v_num[1]
    vbo->_v_num[2]
    vbo->_v_num[3]

    vbo->_v_offset[0]
    vbo->_v_offset[1]
    vbo->_v_offset[2]
    vbo->_v_offset[3]
*/

//end draw
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);

glShadeModel(GL_FLAT);

/*
glDisable (GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);
*/

glDisable(GL_TEXTURE_2D);

}

//minimizes state changes
void DRAW_VBOS2() {
    
        //int _vnum = 0; //unused

        if(SHADER_ON) glUseProgramObjectARB(shader_prog);

        glColor3b(255,255,255);

        glEnable(GL_TEXTURE_2D);
        glEnable (GL_DEPTH_TEST);
        glShadeModel(GL_SMOOTH);
        glEnable (GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);

        glAlphaFunc ( GL_GREATER, 0.1 ) ;

        glBindTexture( GL_TEXTURE_2D, block_texture );

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        int i;
        struct VBO* vbo;

        //if(draw_vbo_n != 0)


        glEnable(GL_CULL_FACE);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[0] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable (GL_ALPHA_TEST);

        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[1] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[1], vbo->_v_num[1]);
        }

        glDisable(GL_CULL_FACE);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[2] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[2], vbo->_v_num[2]);
        }
        
        glDisable(GL_ALPHA_TEST);   

        glDepthMask(false);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[3] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
        }
        glDepthMask(true); 
        glDisable(GL_BLEND);
        
    //end draw
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glShadeModel(GL_FLAT);

    /*
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    */

    glDisable(GL_TEXTURE_2D);

    glUseProgramObjectARB(0);
        //printf("vnum= %i\n", _vnum);

}

//shader version
void DRAW_VBOS3() {
    
        //int _vnum = 0; //unused

        if(SHADER_ON) glUseProgramObjectARB(shader_prog);

        glColor3b(255,255,255);

        glEnable(GL_TEXTURE_2D);
        glEnable (GL_DEPTH_TEST);
        

        glShadeModel(GL_SMOOTH);
        //glShadeModel(GL_FLAT);

        glEnable (GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);

        glAlphaFunc ( GL_GREATER, 0.1 ) ;

        glBindTexture( GL_TEXTURE_2D, block_texture );

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);


        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
        //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)24);
        //glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)24);
        
        int i;
        struct VBO* vbo;

        //if(draw_vbo_n != 0)


        glEnable(GL_CULL_FACE);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[0] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);

            //comment out and test
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)24);
            //glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)24);
            //glVertexAttribPointer(4, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)28);

            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
        }


        glUseProgramObjectARB(0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable (GL_ALPHA_TEST);

        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[1] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[1], vbo->_v_num[1]);
        }

        glDisable(GL_CULL_FACE);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[2] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[2], vbo->_v_num[2]);
        }
        
        glDisable(GL_ALPHA_TEST);   

        glDepthMask(false);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[3] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
        }
        glDepthMask(true); 
        glDisable(GL_BLEND);
        
    //end draw
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    //glShadeModel(GL_FLAT);

    /*
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    */

    glDisable(GL_TEXTURE_2D);
        //printf("vnum= %i\n", _vnum);

}



void DRAW_VBOS4() {
    
        //int _vnum = 0; //unused

        //printf("attribute loc= %i \n", texCoord0Loc);

        if(SHADER_ON) glUseProgramObjectARB(shader_prog3);

        glEnableVertexAttribArray(texCoord0Loc);

        glColor3b(255,255,255);

        glEnable(GL_TEXTURE_2D);
        glEnable (GL_DEPTH_TEST);
        

        glShadeModel(GL_SMOOTH);
        //glShadeModel(GL_FLAT);

        glEnable (GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);

        glAlphaFunc ( GL_GREATER, 0.1 ) ;


        //terrain_map_glsl_1
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //glActiveTexture(0);


        glBindTexture( GL_TEXTURE_2D_ARRAY, terrain_map_glsl_1 );

        //glBindTexture( GL_TEXTURE_2D, terrain_map_glsl_1 );

        glEnableClientState(GL_VERTEX_ARRAY);
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);


        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
        //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);
        //glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)24);
        //glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)24);
        
        //glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)28);

        int i;
        struct VBO* vbo;

        //if(draw_vbo_n != 0)

        glEnable(GL_CULL_FACE);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[0] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            //glTexCoordPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glTexCoordPointer(3, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)28);
            
            glVertexAttribPointer(texCoord0Loc, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);

            //glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)28);

            //glVertexAttribPointer(20, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)28);

            //comment out and test
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
            //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);
            //glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)24);
            //glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)24);
            //glVertexAttribPointer(4, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)28);
            //glBindAttribLocation(shader_prog2, 4, "tex");

            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
        }

        glDisableVertexAttribArray(texCoord0Loc);

        glUseProgramObjectARB(0);
        glActiveTexture(GL_TEXTURE0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable (GL_ALPHA_TEST);

        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[1] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[1], vbo->_v_num[1]);
        }

        glDisable(GL_CULL_FACE);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[2] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[2], vbo->_v_num[2]);
        }
        
        glDisable(GL_ALPHA_TEST);   

        glDepthMask(false);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[3] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
        }
        glDepthMask(true); 
        glDisable(GL_BLEND);
        
    //end draw
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    //glShadeModel(GL_FLAT);

    /*
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    */

    glDisable(GL_TEXTURE_2D);
        //printf("vnum= %i\n", _vnum);

}


void DRAW_VBOS5() {
    
        //printf("attribute loc= %i \n", texCoord0Loc);
        //printf("attribute loc= %i \n", texCoord0Loc);
        glUseProgramObjectARB(shader_prog4);

        //LightMatrix0Loc = glGetAttribLocation(shader_prog3, "InLightMatrix0");
        //LightMatrix0Loc = glGetAttribLocation(shader_prog3, "InTexCoord0");

        glEnableVertexAttribArray(texCoord0Loc_4);
        glEnableVertexAttribArray(LightMatrix0Loc_4);
        //glEnableVertexAttribArray(LightMatrix1Loc_4);

        glColor3b(255,255,255);

        glEnable(GL_TEXTURE_2D);
        glEnable (GL_DEPTH_TEST);
        

        glShadeModel(GL_SMOOTH);
        //glShadeModel(GL_FLAT);

        glEnable (GL_DEPTH_TEST);
        glAlphaFunc ( GL_GREATER, 0.1 ) ;


        glBindTexture( GL_TEXTURE_2D_ARRAY, terrain_map_glsl_1 );
        //glBindTexture( GL_TEXTURE_2D, terrain_map_glsl_1 );

        glEnableClientState(GL_VERTEX_ARRAY);
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);


        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
        //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);
        //glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)24);
        //glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)24);
        
        //glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)28);

        int i;
        struct VBO* vbo;

        glEnable(GL_CULL_FACE);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[0] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            //glTexCoordPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glTexCoordPointer(3, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)28);
            
            glVertexAttribPointer(texCoord0Loc_4, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);

            glVertexAttribPointer(LightMatrix0Loc_4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)32);
            //glVertexAttribPointer(LightMatrix1Loc_4, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)34);
            
            //glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)28);

            //glVertexAttribPointer(20, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)28);

            //comment out and test
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
            //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);
            //glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)24);
            //glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)24);
            //glVertexAttribPointer(4, 3, GL_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)28);
            //glBindAttribLocation(shader_prog2, 4, "tex");

            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
        }

        glDisableVertexAttribArray(texCoord0Loc_4);
        glDisableVertexAttribArray(LightMatrix0Loc_4);

        glUseProgramObjectARB(0);
        glActiveTexture(GL_TEXTURE0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable (GL_ALPHA_TEST);

        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[1] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[1], vbo->_v_num[1]);
        }

        glDisable(GL_CULL_FACE);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[2] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[2], vbo->_v_num[2]);
        }
        
        glDisable(GL_ALPHA_TEST);   

        glDepthMask(false);
        for(i=0;i<draw_vbo_n;i++) {
            vbo = draw_vbo_array[i];
            if(vbo->_v_num[3] == 0) continue; 
            glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
            glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            //glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);
            glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
        }
        glDepthMask(true); 
        glDisable(GL_BLEND);
        
    //end draw
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    //glShadeModel(GL_FLAT);

    /*
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    */
    glDisable(GL_TEXTURE_2D);
        //printf("vnum= %i\n", _vnum);

}

/*

if(T_MAP_BLEND_MODE != 0) {
    glEnable(GL_BLEND);
    if(T_MAP_BLEND_MODE ==1 ) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if(T_MAP_BLEND_MODE ==2 ) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(T_MAP_BLEND_MODE ==3 ) glBlendFunc(GL_ONE, GL_ONE);
    if(T_MAP_BLEND_MODE ==4 ) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    if(T_MAP_BLEND_MODE ==5 ) glBlendFunc(GL_ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
    if(T_MAP_BLEND_MODE ==6 ) glBlendFunc(GL_ONE, GL_ALPHA);
}
*/
