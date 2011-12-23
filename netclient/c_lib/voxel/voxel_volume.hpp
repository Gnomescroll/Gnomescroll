#pragma once


class Orientation_matrix
{
    
    float v[4][4];
    //x,y,z,offset

    //multiply current by other
    void multiply(Orientation_matrix* m)
    {
        return;
    }

    Orientation_matrix()
    {
        return;
    }

};

unsigned NextPow2( unsigned x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

int pow2_1(int n)
{
    int x = 1;
    int i = 0;
    while(x < n) 
    {
        i++;
        x <<= 1;
    }
    return i;
} 

int pow2_2(int n)
{
    int x = 1;
    //int i = 0;
    while(x < n) 
    {
        //i++;
        x <<= 1;
    }
    return x;
} 

#include <voxel/common.h>
#include <physics/vector.hpp>

//z vector rotated in x,y plane by theta degrees
//theta from 0 to 1
static inline struct Vector unit_z_rot(float theta) __attribute((always_inline));
struct Vector unit_z_rot(float theta)
{   
    Vector u;
    u.x = 0.0;
    u.y = sin(theta * 2*PI);
    u.z = cos(theta * 2*PI);
    return u;
}

/*
void _3D::RotateFill(double *A, double xAngle, double yAngle, double zAngle)
{
   // Fill the rotation matrix, using Euler angles 

   double x[12];
   double y[12];
   double z[12];
   double tempArray[12];
   double cx,cy,cz;
   double sx,sy,sz;

   cx = cos(xAngle);
   cy = cos(yAngle);
   cz = cos(zAngle);

   sx = sin(xAngle);
   sy = sin(yAngle);
   sz = sin(zAngle);

   x[0]=1;     x[1]=0;     x[2] =0;     x[3] =0;
   x[4]=0;     x[5]=cx;    x[6] =-sx;   x[7] =0;
   x[8]=0;     x[9]=sx;    x[10]=cx;    x[11]=0;

   y[0]=cy;    y[1]=0;     y[2] =sy;    y[3] =0;
   y[4]=0;     y[5]=1;     y[6] =0;     y[7] =0;
   y[8]=-sy;   y[9]=0;     y[10]=cy;    y[11]=0;

   z[0]=cz;    z[1]=-sz;   z[2] =0;     z[3] =0;
   z[4]=sz;    z[5]=cz;    z[6] =0;     z[7] =0;
   z[8]=0;     z[9]=0;     z[10]=1;     z[11]=0;

   //Note we are multiplying x*y*z. You can change the order,
   //  but you will get different results. 

   MatrixMultiply(z,y,tempArray);   // multiply 2 matrices
   MatrixMultiply(tempArray,x,A);   // multiply result by 3rd matrix
}
*/

/*
    inline static void rotate_euler(matrix3 &m, float x, float y, float z)
    {
        double cx = cos(x);
        double sx = sin(x);
        double cy = cos(y);
        double sy = sin(y);
        double cz = cos(z);
        double sz = sin(z);
        
        m.col[0].x = (float)(cy*cz); 
        m.col[0].y = (float)(cy*sz);
        m.col[0].z = (float)(-sy);

        double sxsy = sx*sy;
        double cxsy = cx*sy;
        
        m.col[1].x = (float)(sxsy*cz-cx*sz);
        m.col[1].y = (float)(sxsy*sz+cx*cz);
        m.col[1].z = (float)(sx*cy);
    
        m.col[2].x = (float)(cxsy*cz+sx*sz);
        m.col[2].y = (float)(cxsy*sz-sx*cz);
        m.col[2].z = (float)(cx*cy);
    }

*/

struct Vector euler_rotation(Vector v, float theta) __attribute((always_inline));
struct Vector euler_rotation(Vector v, float x, float y, float z)
{   
    float cx = cos(x);
    float sx = sin(x);
    float cy = cos(y);
    float sy = sin(y);
    float cz = cos(z);
    float sz = sin(z);
    
    Vector m[3];
    Vector u;

    m[0].x = (cy*cz); 
    m[0].y = (cy*sz);
    m[0].z = (-sy);

    float sxsy = sx*sy;
    float cxsy = cx*sy;
    
    m[1].x = (sxsy*cz-cx*sz);
    m[1].y = (sxsy*sz+cx*cz);
    m[1].z = (sx*cy);

    m[2].x = (cxsy*cz+sx*sz);
    m[2].y = (cxsy*sz-sx*cz);
    m[2].z = (cx*cy);


    u.x = v.x*m[0].x + v.y*m[1].x + v.z*m[2].x, 
    u.y = v.x*m[0].y + v.y*m[1].y + v.z*m[2].y, 
    u.z = v.x*m[0].z + v.y*m[1].z + v.z*m[2].z;

    return u;
}


class Voxel_volume
{
    public:
    Vector v[4]; // forward, up, right (x,y,z), offset
    float scale;    //size of voxels
    //bounding sphere
    Vector center;
    float radius2;

    bool needs_vbo_update;

    int xdim,ydim,zdim;
    int _xdim,_ydim,_zdim;
    Voxel* voxel;
    int index1, index12;

    float hdx,hdy,hdz;  //half of width, height, depth as floats


    void set_unit_axis()
    {
        v[0] = Vector_init(1.0f,0.0f,0.0f);
        v[1] = Vector_init(0.0f,1.0f,0.0f);
        v[2] = Vector_init(0.0f,0.0f,1.0f);
    }
    //vector_cross(struct Vector* v1, struct Vector* v2, struct Vector* dest)
    //forward and up vector
    void set_axis(Vector* f, Vector* u)
    {
        v[0] = *f;
        v[2] = *u; 
        vector_cross(f, u, &v[1]);
    }

    //applies rotation
    //x_angle from 0 to 2
    //y_angle from 0 to 1
    void set_rotated_unit_axis(float x_angle, float y_angle, float z_angle)
    {
        Vector vx = Vector_init(1.0f,0.0f,0.0f);
        vx = euler_rotation(vx, x_angle, y_angle, z_angle);

        Vector vz = Vector_init(0.0f,0.0f,1.0f);
        vz = euler_rotation(vz, x_angle, y_angle, z_angle);


        vector_cross(&vx, &vz, &v[1]); //set v1
        v[0] = vx;
        v[2] = vz;

        update_center();
    }

    // testing/tweaking
/*
    void translate_center(float dx, float dy, float dz)
    {
        center.x += dx;
        center.y += dy;
        center.z += dz;

        Vector vx = vector_scalar2(&v[0],-1.0*hdx*scale);
        Vector vy = vector_scalar2(&v[1],-1.0*hdy*scale);
        Vector vz = vector_scalar2(&v[2],-1.0*hdz*scale);

        v[4] = vector_add4(&vx,&vy,&vz,&center);
    }
*/
    void update_center()
    {
        printf("WTF\n");
        Vector vx = vector_scalar2(&v[0],-1.0*hdx*scale);
        Vector vy = vector_scalar2(&v[1],-1.0*hdy*scale);
        Vector vz = vector_scalar2(&v[2],-1.0*hdz*scale);

        v[3] = vector_add4(&vx,&vy,&vz,&center);
    }

    void set_center(float x, float y, float z)
    {
        center.x = x;
        center.y = y;
        center.z = z;

        //rintf("0v x,y,z= %f, %f, %f \n", vx.x, vx.y, vx.z);
        printf("0v x,y,z= %f, %f, %f \n", v[0].x, v[0].y, v[0].z);
        printf("1v x,y,z= %f, %f, %f \n", v[1].x, v[1].y, v[1].z);
        printf("2v x,y,z= %f, %f, %f \n", v[2].x, v[2].y, v[2].z);

        Vector vx = vector_scalar2(&v[0],-1.0*hdx*scale);
        Vector vy = vector_scalar2(&v[1],-1.0*hdy*scale);
        Vector vz = vector_scalar2(&v[2],-1.0*hdz*scale);

        //printf("0v x,y,z= %f, %f, %f \n", vx.x, vx.y, vx.z);

        v[3] = vector_add4(&vx,&vy,&vz,&center);


        printf("0c x,y,z= %f, %f, %f \n", v[3].x, v[3].y, v[3].z);

    }

    inline Voxel* get(int x, int y, int z) __attribute((always_inline)) 
    {
        //if(x+(y << index1)+(z << index1) != (x+_xdim*y+(_xdim*_ydim*z) ) ) printf("ERROR!\n");
        //printf("1: %i, %i, %i \n",x, (y << index1), (z << index1) );
        //printf("2: %i, %i, %i \n", x,_xdim*y, _xdim*_ydim*z );
        return &voxel[x+(y << index1)+(z << index1)];
    }

    inline void set(int x, int y, int z, Voxel* v) __attribute((always_inline))  __attribute((always_inline)) 
    {
        voxel[x+(y << index1)+(z << index1)] = *v;
    }

    inline void set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a) __attribute((always_inline))  __attribute((always_inline)) 
    {
        Voxel* v = &voxel[x+(y << index1)+(z << index1)];
        v->r = r;
        v->g = g;
        v->b = b;
        v->a = a;
    }

    Voxel_volume(int __xdim, int __ydim, int __zdim, float _scale)
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

    ~Voxel_volume()
    {
        delete voxel;
    }

    void draw_bounding_box();

};


static int v_set2[3*8] = {
        -1,-1,-1,
        1,-1,-1,
        1,1,-1,
        -1,1,-1,
        -1,-1,1,
        1,-1,1,
        1,1,1,
        -1,1,1,
        };

static int vertex_index2[2*12] = {
        0,1,
        1,2,
        2,3,
        3,0,

        4,5,
        5,6,
        6,7,
        7,4,

        0,4,
        1,5,
        2,6,
        3,7,
};

void Voxel_volume::draw_bounding_box()
{
#ifdef DC_CLIENT

    //printf("dfd\n");
    //glDisable()
    glDisable(GL_TEXTURE_2D);
    glLineWidth(5.0f);

    int i,j;
    float _x,_y,_z;

    glBegin(GL_LINES);
    glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);
    
    for(i=0; i<12; i++) {
            j = 3*vertex_index2[2*i+0];
            _x = v[3].x + v_set2[j+0]*hdx*scale;
            _y = v[3].y + v_set2[j+1]*hdy*scale;
            _z = v[3].z + v_set2[j+2]*hdz*scale;
           // printf("1 x,y,z= %i, %i, %i \n", _x,_y,_z);
            glVertex3f(_x,_y,_z);
            j = 3*vertex_index2[2*i+1];
            _x = v[3].x + v_set2[j+0]*hdx*scale;
            _y = v[3].y + v_set2[j+1]*hdy*scale;
            _z = v[3].z + v_set2[j+2]*hdz*scale;
            //printf("2 x,y,z= %i, %i, %i \n", _x,_y,_z);
            glVertex3f(_x,_y,_z);
    }

    glEnd();
    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);
    glEnable(GL_TEXTURE_2D);
    glLineWidth(1.0f);

#endif
}

/*
inline Voxel* Voxel_volume::get(int x, int y, int z) __attribute((always_inline)) = 0;
{
    if(x+(index1<<y)+(index12<<z) != (x+_xdim*y+(_xdim*_ydim*z) ) ) printf("ERROR!\n");
    return &voxel[x+(index1<<y)+(index12<<z)]   ;

}
*/

void voxel_test()
{

    static Voxel_volume vv(8,8,8, 1.0);
    //Voxel* v = vv.get(4,5,6);

    vv.set_center( -5.0, -5.0, 10.0);

    vv.draw_bounding_box();
}