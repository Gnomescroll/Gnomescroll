#include "voxel_volume.hpp"

#include <voxel/constants.hpp>

Voxel_volume::Voxel_volume(int __xdim, int __ydim, int __zdim, float _scale)
{
    needs_vbo_update = false;
    scale = _scale;

    v[0] = Vector_init(1.0f,0.0f,0.0f);
    v[1] = Vector_init(0.0f,1.0f,0.0f);
    v[2] = Vector_init(0.0f,0.0f,1.0f);
    //v[3] = Vector_init(0,0,0);  //center
    this->set_center(0.0,0.0,0.0);

    xdim = __xdim;
    ydim = __ydim;
    zdim = __zdim;

    hdx = ((float) xdim) / 2;
    hdy = ((float) ydim) / 2;
    hdz = ((float) zdim) / 2;

    radius2 =  (hdx*hdz + hdy*hdy + hdz*hdz) * (scale*scale); //radius squared of bounding sphere

    _xdim = pow2_2(__xdim);
    _ydim = pow2_2(__ydim);
    _zdim = pow2_2(__zdim);
    
    index1 = pow2_1(__xdim);
    index12 = pow2_1(__xdim) + pow2_1(__ydim);

    printf("__xdim, __ydim, __zdim= %i, %i, %i \n", __xdim, __ydim,__zdim);
    printf("_xdim, _ydim, _zdim= %i, %i, %i \n", _xdim, _ydim, _zdim);
    printf("index1, index12= %i, %i \n", index1, index12);

    voxel = new Voxel[_xdim*_ydim*_zdim];
}

Voxel_volume::~Voxel_volume()
{
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
    glEnable (GL_DEPTH_TEST);

    glDisable(GL_TEXTURE_2D);
    glLineWidth(5.0f);

    int i,j;
    float _x,_y,_z;

    glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)0);
    glPointSize(10.0);
    glBegin(GL_POINTS);
    glVertex3f(center.x,center.y, center.z);
    glEnd();
    glPointSize(1.0);

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
    glColor3ub((unsigned char)0,(unsigned char)0,(unsigned char)255);
    glPointSize(10.0);
    glBegin(GL_POINTS);
    glVertex3f(v[3].x, v[3].y, v[3].z);
    glEnd();
    glPointSize(1.0);

    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);


    glEnable(GL_TEXTURE_2D);
    glLineWidth(1.0f);

#endif
}