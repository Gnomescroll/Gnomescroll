#include "voxel_volume.hpp"

#include <voxel/constants.hpp>


#ifdef DC_CLIENT
    #include <voxel/voxel_render.hpp>

    void Voxel_volume::register_with_renderer(Voxel_render_list* vrl)
    {
        vrl->register_voxel_volume(this);
    }
#endif

Voxel_volume::Voxel_volume(int __xdim, int __ydim, int __zdim, float _scale)
{
    needs_vbo_update = false;
    scale = _scale;

#ifdef DC_CLIENT
    Voxel_render_list* voxel_render_list = NULL;
#endif

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
    int r,g,b,a;
    a = 255;
    for(int i=0; i < _xdim; i++){
    for(int j=0; j < _ydim; j++){
    for(int k=0; k < _zdim; k++){
        r = i*(256/_xdim);
        g = j*(256/_ydim);
        b = k*(256/_zdim);
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
    return;
    
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

void Voxel_volume::set_unit_axis()
{
    v[0] = Vector_init(1.0f,0.0f,0.0f);
    v[1] = Vector_init(0.0f,1.0f,0.0f);
    v[2] = Vector_init(0.0f,0.0f,1.0f);
    update_center();
}

//vector_cross(struct Vector* v1, struct Vector* v2, struct Vector* dest)
//forward and up vector
void Voxel_volume::set_axis(Vector* f, Vector* u)
{
    v[0] = *f;
    v[2] = *u; 
    vector_cross(u, f, &v[1]);
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

    int DEBUG = 0;

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
    //printf("3v x,y,z= %f, %f, %f \n", v[3].x, v[3].y, v[3].z);
}

void Voxel_volume::set_center(float x, float y, float z)
{
    center.x = x;
    center.y = y;
    center.z = z;

    //rintf("0v x,y,z= %f, %f, %f \n", vx.x, vx.y, vx.z);
    //printf("0v x,y,z= %f, %f, %f \n", v[0].x, v[0].y, v[0].z);
    //printf("1v x,y,z= %f, %f, %f \n", v[1].x, v[1].y, v[1].z);
    //printf("2v x,y,z= %f, %f, %f \n", v[2].x, v[2].y, v[2].z);

    Vector vx = vector_scalar2(&v[0],-1.0*hdx*scale);
    Vector vy = vector_scalar2(&v[1],-1.0*hdy*scale);
    Vector vz = vector_scalar2(&v[2],-1.0*hdz*scale);

    //printf("0v x,y,z= %f, %f, %f \n", vx.x, vx.y, vx.z);


    //printf("0v x,y,z= %f, %f, %f \n", v[0].x, v[0].y, v[0].z);
    //printf("1v x,y,z= %f, %f, %f \n", v[1].x, v[1].y, v[1].z);
    //printf("2v x,y,z= %f, %f, %f \n", v[2].x, v[2].y, v[2].z);

    v[3] = vector_add4(&vx,&vy,&vz,&center);
    //printf("3v x,y,z= %f, %f, %f \n", v[3].x, v[3].y, v[3].z);

    //printf("0c x,y,z= %f, %f, %f \n", v[3].x, v[3].y, v[3].z);

}