#include "voxel_volume.hpp"

#include <voxel/constants.hpp>


#ifdef DC_CLIENT
    #include <voxel/voxel_render.hpp>

    void Voxel_volume::register_with_renderer(Voxel_render_list* vrl)
    {
        vrl->register_voxel_volume(this);
        voxel_render_list = vrl;
    }
#endif

void Voxel_volume::set_parameters(int xdim, int ydim, int zdim, float scale) {
    this->xdim = xdim;
    this->ydim = ydim;
    this->zdim = zdim;
    this->scale = scale;
}

void Voxel_volume::init(int xdim, int ydim, int zdim, float scale) {
    this->set_parameters(xdim, ydim, zdim, scale);
    needs_vbo_update = false;

#ifdef DC_CLIENT
    voxel_render_list = NULL;
#endif
  
    v[0] = Vector_init(1.0f,0.0f,0.0f);
    v[1] = Vector_init(0.0f,1.0f,0.0f);
    v[2] = Vector_init(0.0f,0.0f,1.0f);
    this->set_center(0.0,0.0,0.0);

    this->hdx = ((float) xdim) / 2;
    this->hdy = ((float) ydim) / 2;
    this->hdz = ((float) zdim) / 2;

    this->index1 = pow2_1(xdim);
    this->index12 = pow2_1(xdim) + pow2_1(xdim);

    this->radius2 =  (hdx*hdz + hdy*hdy + hdz*hdz) * (scale*scale); //radius squared of bounding sphere

    int powx = pow2_2(xdim);
    int powy = pow2_2(ydim);
    int powz = pow2_2(zdim);

    voxel = new Voxel[powx*powy*powz];
    int r,g,b,a;
    a = 255;
    for(int i=0; i < powx; i++){
    for(int j=0; j < powy; j++){
    for(int k=0; k < powz; k++){
        r = i*(256/powx);
        g = j*(256/powy);
        b = k*(256/powz);
        _set(i,j,k,r,g,b,a);
    }}}
    needs_vbo_update = true;

}

Voxel_volume::Voxel_volume()
:
id(-1)
{}

Voxel_volume::Voxel_volume(int xdim, int ydim, int zdim, float scale)
:
id(-1)
{
    this->set_parameters(xdim, ydim, zdim, scale);
    needs_vbo_update = false;

#ifdef DC_CLIENT
    voxel_render_list = NULL;
#endif
  
    v[0] = Vector_init(1.0f,0.0f,0.0f);
    v[1] = Vector_init(0.0f,1.0f,0.0f);
    v[2] = Vector_init(0.0f,0.0f,1.0f);
    this->set_center(0.0,0.0,0.0);

    this->hdx = ((float) xdim) / 2;
    this->hdy = ((float) ydim) / 2;
    this->hdz = ((float) zdim) / 2;

    this->radius2 =  (hdx*hdz + hdy*hdy + hdz*hdz) * (scale*scale); //radius squared of bounding sphere

    this->index1 = pow2_1(xdim);
    this->index12 = pow2_1(xdim) + pow2_1(xdim);

    int powx = pow2_2(xdim);
    int powy = pow2_2(ydim);
    int powz = pow2_2(zdim);

    voxel = new Voxel[powx*powy*powz];
    int r,g,b,a;
    a = 255;
    for(int i=0; i < powx; i++){
    for(int j=0; j < powy; j++){
    for(int k=0; k < powz; k++){
        r = i*(256/powx);
        g = j*(256/powy);
        b = k*(256/powz);
        _set(i,j,k,r,g,b,a);
    }}}
    needs_vbo_update = true;

}

Voxel_volume::~Voxel_volume()
{
    #ifdef DC_CLIENT
    if(voxel_render_list != NULL) voxel_render_list->unregister_voxel_volume(this);
    #endif
    delete voxel;
}


//external methods
void Voxel_volume::set(int x, int y, int z, Voxel* v)
{
    set(x,y,z,v);
    needs_vbo_update = true;
}

inline void Voxel_volume::set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    set(x,y,z,r,g,b,a);
    needs_vbo_update = true;
}


void Voxel_volume::draw_bounding_box()
{
#ifdef DC_CLIENT

    //disable to draw over
    //glEnable (GL_DEPTH_TEST);

    glDisable (GL_DEPTH_TEST);

    glDisable(GL_TEXTURE_2D);
    glLineWidth(5.0f);

    int i,j;
    float _x,_y,_z;

    glBegin(GL_LINES);
    glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);

    Vector vx;
    Vector vy;
    Vector vz;

    Vector u;

    for(i=0; i<12; i++) {
            j = 3*vertex_index2[2*i+0];
            
            vx = vector_scalar2(&v[0], 2.0*v_set[j+0]*hdx*scale);
            vy = vector_scalar2(&v[1], 2.0*v_set[j+1]*hdy*scale);
            vz = vector_scalar2(&v[2], 2.0*v_set[j+2]*hdz*scale);

            u = vector_add4(&vx,&vy,&vz,&v[3]);

            _x = u.x;
            _y = u.y;
            _z = u.z;

            glVertex3f(_x,_y,_z);
            j = 3*vertex_index2[2*i+1];

            vx = vector_scalar2(&v[0], 2.0*v_set[j+0]*hdx*scale);
            vy = vector_scalar2(&v[1], 2.0*v_set[j+1]*hdy*scale);
            vz = vector_scalar2(&v[2], 2.0*v_set[j+2]*hdz*scale);
            
            u = vector_add4(&vx,&vy,&vz,&v[3]);

            _x = u.x;
            _y = u.y;
            _z = u.z;

            glVertex3f(_x,_y,_z);
    }

    glEnd();

    glDisable (GL_DEPTH_TEST);

    //offset
    glColor3ub((unsigned char)0,(unsigned char)0,(unsigned char)255);
    glPointSize(10.0);
    glBegin(GL_POINTS);
    glVertex3f(v[3].x, v[3].y, v[3].z);
    glEnd();
    glPointSize(1.0);

    //center
    glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)0);
    glPointSize(10.0);
    glBegin(GL_POINTS);
    glVertex3f(center.x,center.y, center.z);
    glEnd();
    glPointSize(1.0);

    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);


    glEnable(GL_TEXTURE_2D);
    glLineWidth(1.0f);

    glEnable (GL_DEPTH_TEST);

#endif
}

void Voxel_volume::set_unit_axis()
{
    v[0] = Vector_init(1.0f,0.0f,0.0f);
    v[1] = Vector_init(0.0f,1.0f,0.0f);
    v[2] = Vector_init(0.0f,0.0f,1.0f);
    update_center();
}

//vector_cross(struct Vector* v1, struct Vector* v2, struct Vector* dest)
//forward and up Vector
void Voxel_volume::set_axis(Vector* f, Vector* u)
{
    v[0] = *f;
    v[2] = *u; 
    vector_cross(u, f, &v[1]);

    update_center();
}

void Voxel_volume::set_rotated_unit_axis(float x_angle, float y_angle, float z_angle)
{
    Vector vx = Vector_init(1.0f,0.0f,0.0f);
    vx = euler_rotation(vx, x_angle, y_angle, z_angle);

    Vector vz = Vector_init(0.0f,0.0f,1.0f);
    vz = euler_rotation(vz, x_angle, y_angle, z_angle);
    vector_cross(&vz, &vx, &v[1]); //set v1
    v[0] = vx;
    v[2] = vz;

    update_center();
}


void Voxel_volume::update_center()
{

    const int DEBUG = 0;

    if(DEBUG)
    {
        printf("v[0] x,y,z= %f, %f, %f \n", v[0].x, v[0].y, v[0].z);
        printf("v[1] x,y,z= %f, %f, %f \n", v[1].x, v[1].y, v[1].z);
        printf("v[2] x,y,z= %f, %f, %f \n", v[2].x, v[2].y, v[2].z);
        printf("v[3] x,y,z= %f, %f, %f \n", v[3].x, v[3].y, v[3].z);
    }

    Vector vx = vector_scalar2(&v[0],-1.0*hdx*scale);
    Vector vy = vector_scalar2(&v[1],-1.0*hdy*scale);
    Vector vz = vector_scalar2(&v[2],-1.0*hdz*scale);

    if(DEBUG)
    {
        printf("vx x,y,z= %f, %f, %f \n", vx.x, vx.y, vx.z);
        printf("vy x,y,z= %f, %f, %f \n", vy.x, vy.y, vy.z);
        printf("vz x,y,z= %f, %f, %f \n", vz.x, vz.y, vz.z);
    }
    v[3] = vector_add4(&vx,&vy,&vz,&center);

    if(DEBUG)
    {
        printf("out_sum v[3] x,y,z= %f, %f, %f \n", v[3].x, v[3].y, v[3].z);
    }
/*
    v[0].w = 0.0f;
    v[1].w = 0.0f;
    v[2].w = 0.0f;
    v[3].w = 1.0f;
*/
}

void Voxel_volume::set_center(float x, float y, float z)
{
    center.x = x;
    center.y = y;
    center.z = z;

    Vector vx = vector_scalar2(&v[0],-1.0*hdx*scale);
    Vector vy = vector_scalar2(&v[1],-1.0*hdy*scale);
    Vector vz = vector_scalar2(&v[2],-1.0*hdz*scale);

    v[3] = vector_add4(&vx,&vy,&vz,&center);
/*
    v[0].w = 0.0f;
    v[1].w = 0.0f;
    v[2].w = 0.0f;
    v[3].w = 1.0f;
*/
}

#ifdef DC_CLIENT

/*
    struct Voxel_vertex
    {
        unsigned char x,y,z,t;
        unsigned char rgba[4]; //8
    };
*/

/*
inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) {
    return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}
*/

#include <stdio.h>


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


static const float vset[72] = { 1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , //top
        0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , //bottom
        1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , //north
        0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , //south
        1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , //west
        0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , //east
};

static const int vnset[18] = { 0,0,1, 
0,0,-1, 
1,0,0 , 
-1,0,0 ,
0,1,0 , 
0,-1,0 
};
 
static const struct Voxel_normal voxel_normal_array[6] = { 
{{{0,0,1,0}}},
{{{0,0,-1,0}}},
{{{1,0,0,0}}},
{{{-1,0,0,0}}},
{{{0,1,0,0}}},
{{{0,-1,0,0}}},
};

inline void push_voxel_quad(Voxel_vertex* scratch, int* index, int x, int y, int z, int side);

void Voxel_volume::push_voxel_quad(Voxel_vertex* scratch, int* index, int x, int y, int z, int side)
{
    float fx, fy, fz;

    fx = (float) x;
    fy = (float) y;
    fz = (float) z;

    //*(int*)&tmp.rgba 

    //set rgba
    unsigned int color = get_as_int(x,y,z);
    scratch[*index + 0].color = color;
    scratch[*index + 1].color = color;
    scratch[*index + 2].color = color;
    scratch[*index + 3].color = color;

//optimized version
/*
    int normal = (vnset[3*side + 0 ] << 24) | (vnset[3*side + 1 ] << 16) | (vnset[3*side + 2 ] << 8) | 0;

    *(int*)scratch[*index + 0].normal = normal;
    *(int*)scratch[*index + 1].normal = normal;
    *(int*)scratch[*index + 2].normal = normal;
    *(int*)scratch[*index + 3].normal = normal;
*/
    int _side = side*3;

    Voxel_normal normal;
    normal.n = voxel_normal_array[side].n;

    scratch[*index + 0].n = normal.n;
    scratch[*index + 1].n = normal.n;
    scratch[*index + 2].n = normal.n;
    scratch[*index + 3].n = normal.n;

    /*
    Voxel_normal normal;
    normal.n = 0;
    normal.normal[0] = vnset[_side + 0 ];
    normal.normal[1] = vnset[_side + 1 ];
    normal.normal[2] = vnset[_side + 2 ];  
    */

/*
    scratch[*index + 0].normal[0] = vnset[_side + 0 ];
    scratch[*index + 0].normal[1] = vnset[_side + 1 ];
    scratch[*index + 0].normal[2] = vnset[_side + 2 ];
    
    scratch[*index + 1].normal[0] = vnset[_side + 0 ];
    scratch[*index + 1].normal[1] = vnset[_side + 1 ];
    scratch[*index + 1].normal[2] = vnset[_side + 2 ];

    scratch[*index + 2].normal[0] = vnset[_side + 0 ];
    scratch[*index + 2].normal[1] = vnset[_side + 1 ];
    scratch[*index + 2].normal[2] = vnset[_side + 2 ];

    scratch[*index + 3].normal[0] = vnset[_side + 0 ];
    scratch[*index + 3].normal[1] = vnset[_side + 1 ];
    scratch[*index + 3].normal[2] = vnset[_side + 2 ];
*/

    //set x,y,z
    _side = side*12;

    scratch[*index + 0].x = fx + vset[_side + 0 ];
    scratch[*index + 0].y = fy + vset[_side + 1 ];
    scratch[*index + 0].z = fz + vset[_side + 2 ];
    
    scratch[*index + 1].x = fx + vset[_side + 3 ];
    scratch[*index + 1].y = fy + vset[_side + 4 ];
    scratch[*index + 1].z = fz + vset[_side + 5 ];
    
    scratch[*index + 2].x = fx + vset[_side + 6 ];
    scratch[*index + 2].y = fy + vset[_side + 7 ];
    scratch[*index + 2].z = fz + vset[_side + 8 ];
    
    scratch[*index + 3].x = fx + vset[_side + 9 ];
    scratch[*index + 3].y = fy + vset[_side + 10];
    scratch[*index + 3].z = fz + vset[_side + 11];

/*
    printf("start side %i \n", side);
    for(int j=0; j<4; j++)
    {
        printf("x,y,z= %f, %f, %f \n", scratch[*index + j].x, scratch[*index + j].y, scratch[*index + j].z);
    }
*/
    //if(*index >= 65536) printf("BUFFER OVERFLOW!!! \n");
    *index += 4;
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


void Voxel_volume::update_vertex_list()
{   
    static Voxel_vertex* scratch = new Voxel_vertex[65536]; //64 k of memory
    int index = 0;


    //push_voxel_quad(scratch, &index, -5,-5,10, 0);
    //push_voxel_quad(scratch, &index, 5,5,10, 0);

    for(int x=0; x < xdim; x++){
    for(int y=0; y < ydim; y++){
    for(int z=0; z < zdim; z++){
        if( get_as_int(x,y,z) == 0) continue;


        //push_voxel_quad(Voxel_vertex* scratch, int* index, int x, int y, int z, int side, int color)
 
        if(z+1 == zdim || get_as_int(x,y,z+1) == 0)
        {
            push_voxel_quad(scratch, &index, x,y,z, 0);
        }
        

        if(z == 0 || get_as_int(x,y,z-1) == 0)
        {
            push_voxel_quad(scratch, &index, x,y,z, 1);
        }

        if(x+1 == xdim || get_as_int(x+1,y,z) == 0)
        {
            push_voxel_quad(scratch, &index, x,y,z, 2);
        }

        if(x == 0 || get_as_int(x-1,y,z) == 0)
        {
            push_voxel_quad(scratch, &index, x,y,z, 3);
        }
 
        if(y+1 ==ydim || get_as_int(x,y+1,z) == 0)
        {
            push_voxel_quad(scratch, &index, x,y,z, 4);
        }

        if(y == 0 || get_as_int(x,y-1,z) == 0)
        {
            push_voxel_quad(scratch, &index, x,y,z, 5);
        }

    }}}

    printf("Voxel_volume::update_vertex_list: created %i vertices, %i bytes \n", index, index*sizeof(Voxel_vertex) );

    if(index == 0)
    {
        printf("Warning: generate vertex voxel list, 0 voxels\n");

    }

    if(vvl.vertex_list != NULL) delete vvl.vertex_list;
    vvl.vertex_list = new Voxel_vertex[index];

    //void * memcpy ( void * destination, const void * source, size_t num );
    memcpy(vvl.vertex_list, scratch, index*sizeof(Voxel_vertex));
    vvl.size = index*sizeof(Voxel_vertex);  //wtf is size for
    vvl.vnum = index;

    printf("Voxel_volume::update_vertex_list finished \n");
}

#endif
void voxel_test()
{
    //static Voxel_volume vv(1,1,1, 1.0);
    static Voxel_volume vv(4,4,4, 1.0);
    //Voxel* v = vv.get(4,5,6);

    //static float c = 0.0;
    //c -= 0.02;
    
    //vv.set_center( -5.0, -5.0, 10.0);


    static float c0 = 0.0;
    static float c1 = 0.0;
    static float c2 = 0.0;
    c0 += 0.0050 / (2*PI);
    c1 += 0.0025 / (2*PI);
    c2 += 0.0100 / (2*PI);

    //printf("start\n");
    //printf("1 v[3] x,y,z= %f, %f, %f \n", vv.v[3].x, vv.v[3].y, vv.v[3].z);
    
    vv.set_rotated_unit_axis(c0, c1, c2);

    vv.set_center( 2.0, 2.0, 2.0);

    //vv.draw_bounding_box();
}


