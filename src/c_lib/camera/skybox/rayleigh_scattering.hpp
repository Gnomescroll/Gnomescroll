#pragma once

#include <SDL/png_save.hpp>

//#include <t_gen/twister.hpp>

namespace Skybox
{

/*

    Todo
    Subtle Perlin Noise Plasma in Sky
    - subtle perlin noise plasma in sky
    - modulate or multiply by sky perlin
    -
*/

/*

    Clouds Todo:
    - !!! cloud motion (feeling of wind or breeze)
    - global fade in and out
    - perlin 2d drift/displacement of individual clouds
    - 2d perlin map for cloud density
    - !   wrap cordinates of clouds (so they dont move with player)
    - !!  Octaves for cloud density (less uniform)
    - should fade out over distances (for wrap, needs shader

    - should have bloom?
    - blur whats behind it?

*/

int print_max_light = 0 ;
unsigned char sun_color[4];

float sun_w[4]; //wavelength_factor for each channel
float sun_g[4]; //g factor for each channels
float sun_h[4];//h0, scale height for each channel
float sun_b[4];//brightness for each channel

int skybox_gl_linear = 0;
int skybox_normalization = 0;
int skybox_corners = 0; //show corners of skybox plans
int skybox_sundial = 0; //draw time indicator
int skybox_debug = 0;   //puts skybox in floating cube

unsigned int cloud_blur_texture = 0;

//cube orientation vectors
static const float _f[6*3] =
{
     0, 0, 1, //top
     0, 0,-1,
     1, 0, 0, //north
    -1, 0, 0,
     0, 1, 0, //west
     0,-1, 0
};

static const float _r[6*3] =
{
     0,-1, 0, //top
     0,-1, 0,
     0, -1,0, //north
     0, 1, 0,
     1, 0, 0, //west
     -1, 0, 0
};

static const float _u[6*3] =
{
    -1, 0, 0, //top
     1, 0, 0,
     0, 0,1, //north
     0, 0, 1,
     0, 0, 1, //west
     0, 0, 1
};

class SkyplaneSettings
{
    public:

    float planet_radius;
    float atmosphere_depth;
    float sun_distance;                 //sun distance from surface

    //float brightness_log_factor;
    float wavelength_factor;
    //float phase_factor;
    float phase_g_factor;
    float light_epsilon;

    float brightness_scale_factor;
    float brightness_sum_factor;

    float H0;
    float skybox_height;
    float camera_z;

    float plane_size;   //size of skybox box

    SkyplaneSettings()
    {
        planet_radius =     512.0;
        atmosphere_depth = 1.0;    //how far until stop raycasting scattering through atomosphere
        sun_distance =      256.0;

        //brightness_log_factor = 1.0;
        wavelength_factor =     1.00;
        //phase_factor =            0.75;   //deprecate
        phase_g_factor  =       0.00;
        light_epsilon =         0.0000001;

        brightness_scale_factor =   1.0;
        brightness_sum_factor =     0.0;

        H0 =            0.25f;  //percent of skybox where atomosphere hits 25%
        skybox_height = 128.0;  //skybox height?? Scales atomosphere 0 to 1
        camera_z =      64.0f;

        plane_size = 128.0;
    }
};

static class SkyplaneSettings SPS; //default sun setting

class Skyplane
{
    public:
        //unsigned char color[4*dim*dim];
        static const int dim = 16;
        const static int samples = 5;

        int sun_i, sun_j, sun_side; //used to find pixel for tracking sun
        float sun_cos_max;

        /*
            Settings
        */
        float planet_radius;
        float atmosphere_depth;
        float sun_distance;                 //sun distance from surface

        //float brightness_log_factor;
        float wavelength_factor;
        //float phase_factor;
        float phase_g_factor;
        float light_epsilon;

        float brightness_scale_factor;
        float brightness_sum_factor;

        float H0;
        float skybox_height;
        float camera_z;

        float plane_size;   //size of skybox box

        float farray[6][dim*dim]; //intensity array

        float gf;
        float gfc; //constant
        float gf2; //gf squared


    Skyplane() :
        sun_i(0), sun_j(0), sun_side(0), sun_cos_max(1.0f),
        planet_radius(0.0f), atmosphere_depth(1.0f), sun_distance(1.0),
        wavelength_factor(0.0f), phase_g_factor(0.0f), light_epsilon(1.0f),
        brightness_scale_factor(1.0f), brightness_sum_factor(1.0f), H0(0.0f),
        skybox_height(0.0f), camera_z(128.0f), plane_size(0.0f),
        gf(0.0f), gfc(0.0f), gf2(0.0f)
    {
        for (int i=0; i<6; i++)
        for (int j=0; j<dim*dim; j++)
            this->farray[i][j] = 0.0f;
    }

    void load_settings(class SkyplaneSettings &s)
    {
        planet_radius = s.planet_radius;
        atmosphere_depth = s.atmosphere_depth;
        sun_distance = s.sun_distance;

        //brightness_log_factor = s.brightness_log_factor;
        wavelength_factor = s.wavelength_factor;
        //phase_factor = s.phase_factor;
        phase_g_factor = s.phase_g_factor;
        light_epsilon = s.light_epsilon;

        brightness_scale_factor = s.brightness_scale_factor;
        brightness_sum_factor = s.brightness_sum_factor;

        H0 = s.H0;
        skybox_height = s.skybox_height;
        camera_z = s.camera_z;

        plane_size = s.plane_size;
    }

    void draw_sun(float theta, float phi, float x,float y,float z)
    {
        const float radius = 64.0;
        const float plane_depth = plane_size/2.0;

        float stheta = 2*3.14159*theta;
        float sphi   = 2*3.14159*phi;

        struct Vec3 s;
        s.x = radius*sinf(stheta)*cosf(sphi);
        s.y = radius*sinf(stheta)*sinf(sphi);
        s.z = radius*cosf(stheta);

        s.z += plane_depth;

        glDisable(GL_DEPTH_TEST);

        glPointSize(5.0);

        glColor3ub(0, 0, 255);

        glBegin(GL_POINTS);

        glVertex3f(x+s.x,y+s.y,z+s.z);

        glEnd();

        glColor3ub(255, 255, 255);
        glEnable(GL_DEPTH_TEST);
        glPointSize(1.0);
    }

    //return false if ray hits planet
    static bool sphere_line_intersection_test0(struct Vec3 a, struct Vec3 b, float radius)
    {
        float d1 = vec3_dot(a,b);
        float d2 = d1*d1 - vec3_dot(a,a) + radius*radius;
        if (d2 <= 0.0)
            return true;
        float d3 = sqrt(d2);
        float s1 =      d3 - d1;
        float s2 = -1.0*d3 - d1;
        return (s1 < 0.0f && s2 < 0.0f);
    }
    //a is start point of line, b is direction
    //cente is 0
    static float sphere_line_intersection(const struct Vec3 &a, const struct Vec3 &b, const float radius)
    {
        //assume b is unit vector
        //assume l is direction

        float d1 = vec3_dot(a,b);
        float d2 = d1*d1 - vec3_dot(a,a) + radius*radius;
        GS_ASSERT(d2 >= 0.0f);

        //no intersection
        if(d2 <= 0.0)
            return -1.0f;

        float d3 = sqrt(d2);

        //if d3 or d2 is close to zero then point is almost at sphere
        float s1 =      d3 - d1;
        float s2 = -1.0*d3 - d1;
        GS_ASSERT(s1 >= 0.0f || s2 >= 0.0f);

        //intersections are both behind the sphere
        if(s1 < 0.0f && s2 < 0.0f)
            return -1.0f;
        //return intersection in front of sphere
        float ret = s1 > s2 ? s1 : s2;
        return ret;
    }

    //planet center is at 0,0,0
    //sun is sun_distance + planet_radius from planet center
    //sun orbits around 0,0,0

    OPTIMIZED
    void update(float theta, float phi)
    {
        update_phase_constants();
        /*
            Cube Stuff
        */
        const float plane_depth = plane_size*0.5;

        const float _df = 2.0/ ((float) dim);

        float stheta = 2*3.14159*theta;
        float sphi   = 2*3.14159*phi;

        //sun position

        const float sd = sun_distance + planet_radius;
        struct Vec3 s;
        s.x = sd*sinf(stheta)*cosf(sphi);
        s.y = sd*sinf(stheta)*sinf(sphi);
        s.z = sd*cosf(stheta);

        //s.z += plane_depth;

        struct Vec3 f[6];
        struct Vec3 r[6];
        struct Vec3 u[6];

        for(int i=0;i<6;i++)
        {
            f[i] = vec3_init(_f[3*i+0],_f[3*i+1],_f[3*i+2] );
            r[i] = vec3_init(_r[3*i+0],_r[3*i+1],_r[3*i+2] );
            u[i] = vec3_init(_u[3*i+0],_u[3*i+1],_u[3*i+2] );
        }

        //center of plane
        struct Vec3 center[6];

        for(int i=0;i<6;i++)
        {
            center[i] = vec3_scalar_mult(f[i], plane_depth);
            center[i].z += plane_depth;

            if(i == 1)
                center[i].z -= -1.0;    //to prevent zero
        }

        //int sun_i, sun_j, sun_side; //used to find pixel for tracking sun
        //float sun_cos_max;
        sun_cos_max = -10.0;

        struct Vec3 c = vec3_init(0.0, 0.0, planet_radius+camera_z);

        //debug
        float _epsilon = 0.1f;
        if(c.z > planet_radius + atmosphere_depth - _epsilon)
            c.z = planet_radius + atmosphere_depth - _epsilon;
        if(c.z < planet_radius + _epsilon)
            c.z = planet_radius + _epsilon;

        //const float _dfp = _df*plane_size; //for speed
        const int dim_half = dim/2;


        if(vec3_length(c) > planet_radius + atmosphere_depth + _epsilon)
        {
            printf("WTF: camera outside of sphere\n");
            GS_ASSERT_ABORT(false);
        }

        if(vec3_length(c) < planet_radius - _epsilon)
        {
            printf("WTF: camera inside of sphere\n");
            GS_ASSERT_ABORT(false);
        }

        float _max_light2 = 0.00;

        const float sphere_radius = planet_radius + atmosphere_depth;

        for(int side=0; side<6; side++)
        {

            struct Vec3 _f = f[side];
            struct Vec3 _r = r[side];
            struct Vec3 _u = u[side];

            for(int i=0; i<dim; i++)
            {

                int _i = i - dim_half;

                for(int j=0; j<dim; j++)
                {
                    int _j = j - dim_half;

                    struct Vec3 b;  //direction from camera (camera is zeroed here)
                    b.x = _f.x + _df*(_i*_r.x + _j*_u.x);
                    b.y = _f.y + _df*(_i*_r.y + _j*_u.y);
                    b.z = _f.z + _df*(_i*_r.z + _j*_u.z);


                    //can inline this even
                    //GS_ASSERT(vec3_length(b) > 0.01);
                    b = vec3_normalize(b); //direction from camera

                    float d = sphere_line_intersection(c, b, sphere_radius - _epsilon);

                /*
                    if(0 &&(side == 0 || side == 2 || side == 3 || side == 4))
                    {
                        printf("b len = %.2f x,y,z= %.2f %.2f %.2f  i,j,side = %d %d %d _i,_j= %d %d d= %.2f \n",
                        vec3_length(b), b.x,b.y,b.z ,i,j,side, _i,_j, d);

                        printf("c= %.2f %.2f %.2f b= %.2f %.2f %.2f r= %.2f r= %.2f \n",
                            c.x,c.y,c.z, b.x,b.y,b.z, vec3_length(vec3_add(c, vec3_scalar_mult(b, d))), sphere_radius);
                    }
                */
                //  GS_ASSERT(vec3_length(c) >= planet_radius);
                //  GS_ASSERT(vec3_length(c) <= sphere_radius);

                    //short circuited; always returns true

                /*
                    b = vec3_add(c, vec3_scalar_mult(b, d));
                    float light = in_scatter(c, b, s);
                    //light = 0.2;
                    farray[side][j*dim+i] = light;

                    if(_max_light2 < light)
                        _max_light2 = light;
                */

                    if(sphere_line_intersection_test0(c, b, planet_radius) == true)
                    {
                        b = vec3_add(c, vec3_scalar_mult(b, d));
                        float light = in_scatter(c, b, s);
                        //light = 0.2;
                        farray[side][j*dim+i] = light;

                        if(_max_light2 < light)
                            _max_light2 = light;
                    }
                    else
                    {
                        farray[side][j*dim+i] = 0.0f;
                        b = vec3_add(c, vec3_scalar_mult(b, d));
                    }

                /*
                    if( vec3_length(vec3_sub(b, c)) < 0.01 || vec3_length(vec3_sub(s, c)) < 0.01 )
                    {
                        vec3_print(vec3_sub(b, c));
                        vec3_print(vec3_sub(s, c));
                    }
                */
                    struct Vec3 tb = vec3_normalize(vec3_sub(b, c));
                    struct Vec3 ts = vec3_normalize(vec3_sub(s, c));

                    float v = vec3_dot(tb, ts);

                    if(v > sun_cos_max)
                    {
                        sun_cos_max = v;
                        sun_side = side;
                        sun_i = i;
                        sun_j = j;
                    }
                }

            }
        }

        if(print_max_light)
            printf("max_light- %f \n", farray[sun_side][sun_j*dim+sun_i]);

        //float light_epsilon = 0.0000001
        //float max_light = farray[sun_side][sun_j*dim+sun_i];
        float max_light = _max_light2;
        float _max_light = 1.0 / (max_light+light_epsilon);  //need to scale factor, so it goes to limit

        //return brightness_scale_factor*tmp + brightness_sum_factor);
        if(skybox_normalization)
        {
            for(int side=0; side<6; side++)
            {
                for(int i=0; i<dim; i++)
                {
                    for(int j=0; j<dim; j++)
                    {
                        float light = farray[side][j*dim+i];
                        light *= _max_light;
                        light = brightness_scale_factor*light + brightness_sum_factor;

                        farray[side][j*dim+i]  = light;
                    }

                }
            }
        }
    }

    int sphere_wedge_test(struct Vec3 v, float inner_radius, float outer_radius)
    {
        const float _epsilon = 0.1;

        float r = vec3_length(v);

        if( r + _epsilon < inner_radius)
            return -1;

        if( r - _epsilon > outer_radius)
            return 1;

        return 0;
    }

    void update_phase_constants()
    {
        gf  = phase_g_factor;
        gf2 = phase_g_factor*phase_g_factor;
        gfc = (3.0*(1-gf2))/(2.0*(2.0+gf2));
    }

    OPTIMIZED
    float phase(const struct Vec3 &v1, const struct Vec3 &v2)
    {
        float _cos2 = vec3_cos2(v1,v2);
        float _cos  = sqrt(_cos2);

        float f = (1.0 + gf*(gf - 2.0*_cos) ) ;
        f = f* sqrt(f);

        return gfc*(1.0+_cos2)/f;

        //return 0.75*(1+vec3_cos2(v1,v2)); // simple phase equation

    }

    //H 0 is the scale height, which is the height at which the atmosphere's average density is found.
    //My implementation uses 0.25, so the average density is found 25 percent of the way up from the ground to the sky dome.
    //the height is scaled so that 0 represents sea level and 1 is at the top of the atmosphere.

    //function of wavelength
    OPTIMIZED
    float out_scatter(const struct Vec3 &v1, const struct Vec3 &v2)
    {
        const float _epsilon = 0.1;

        const float _f = 1.0f / ((float) samples);
        const float _d = _f * vec3_distance(v1, v2);
        const float _d_half = 0.5f* _d;
        const float sbh_norm = -1.0f / (H0*skybox_height); //skybox height normalization

        struct Vec3 vi = vec3_scalar_mult(vec3_sub(v2, v1), _f);

        float _r[samples+1];

        /*
            !!! computute height of sample point and atomosphere density
        */
        for(int i=0; i<=samples; i++)
        {
            struct Vec3 tmp1 = vec3_add(v1, vec3_scalar_mult(vi, i));
            //float _h = tmp1.z *sbh_norm; //divide by -H0*skybox_height

            float s_height = vec3_length(tmp1) - planet_radius - _epsilon;

        /*
            if(s_height > atmosphere_depth)
            {
                printf("ERROR out_scatter: s_height= %.2f %2.f \n", s_height , atmosphere_depth);
            }
        */
            _r[i] = s_height < 0.0 ? 0.0 : exp(s_height *sbh_norm); //intergrate over exp(-height / H0)
            //_r[i] = exp(tmp1.z *sbh_norm); //intergrate over exp(-height / H0)
        }


        float tmp = 0.0f;

        for(int i=0; i<samples; i++)
            tmp += _d_half*(_r[i] + _r[i+1]);  // _d*(_r[i] + _r[i+1])*0.5

        return wavelength_factor*tmp;
        //return 4.0*3.14159*tmp;
    }

    //start position, end position, sun position
    OPTIMIZED
    float in_scatter(const struct Vec3 &a, const struct Vec3 &b, const struct Vec3 &s)
    {
        struct Vec3 c = a;

        const float _epsilon = 0.1;

    /*
        if(sphere_wedge_test(a, planet_radius, planet_radius + atmosphere_depth) != 0)
        {
            printf("WTF 3: %0.2f, min %0.2f \n", vec3_length(a), planet_radius);
        }

        if(sphere_wedge_test(b, planet_radius, planet_radius + atmosphere_depth) != 0)
        {
            printf("WTF 4: %0.2f, max %0.2f \n", vec3_length(b), planet_radius + atmosphere_depth);
        }
    */
        //For each point P along the ray from Pa to Pb ,
        //PPc is the ray from the point to the sun
        //PPa is the ray from the sample point to the camera.

        const float _f = 1.0f / ((float) samples);
        const float _d = _f * vec3_distance(b,a);   //for intergral
        const float _d_half = 0.5f* _d;
        const float sbh_norm = -1.0f / (H0*skybox_height); //skybox height normalization

        const float sphere_radius = planet_radius + atmosphere_depth;
        //GS_ASSERT(abs( vec3_distance(a,b) - vec3_length(vec3_sub(b,a))) < 0.0001 );


        struct Vec3 vi = vec3_scalar_mult(vec3_sub(b, a), _f);

        //compute sample points
        struct Vec3 tmp1[samples+1];    //should be constant for MSVC
        for(int i=0; i<=samples; i++)
            tmp1[i] = vec3_add(a, vec3_scalar_mult(vi, i));

    /*
        for(int i=0; i<=samples; i++)
        {
            if(vec3_length(tmp1[i]) >= sphere_radius) // + _epsilon; this is more serious
            {
                printf("WTF 1: %0.2f, max %0.2f \n", vec3_length(tmp1[i]), planet_radius + atmosphere_depth);
            }
            if(vec3_length(tmp1[i]) <= planet_radius - _epsilon)
            {
                //printf("WTF 2: %0.2f, min %0.2f \n", vec3_length(tmp1[i]), planet_radius);
            }
        }
    */
        struct Vec3 _s[samples+1];
        for(int i=0; i<=samples; i++)
        {
            //do line sphere intersection to get length
            //vector which is 20 units in direction of sun from tmp1

            //_s[i] = vec3_add(tmp1[i], vec3_scalar_mult(vec3_normalize(vec3_sub(tmp1[i], s)), 20.0));

            //WTF supposed to be sun
            //_s[i] = s; //skip to sun without raise limiting
            //dont outscatter to sun; only outscatter in direction of sun (to end of skybox)

            //struct Vec3 dir = vec3_normalize(vec3_sub(s, tmp1[i]) );

            //happens when sun is in atomosphere!
            struct Vec3 sun_dir = vec3_sub(s, tmp1[i]); //t

            if(vec3_length(sun_dir) < _epsilon)
            {
                _s[i] = tmp1[i]; //sun is in atomosphere
            }
            else
            {
                sun_dir = vec3_normalize(sun_dir);
                float d = sphere_line_intersection(tmp1[i], sun_dir, sphere_radius);
                //if(d > planet_radius /2.0)
                //  d = planet_radius /2.0;

                _s[i] = vec3_add(tmp1[i], vec3_scalar_mult(sun_dir,d));

            }
                //printf("tmp1[i] len < 0.01 \n");



            /*
                !!! dont raycast to sun; only to atomosphere ceiling in direction of sun
            */
        }

        float _t0[samples+1]; //exp(h/H0)
        float _t1[samples+1]; //out_scatter PPc
        float _t2[samples+1]; //out_scatter PPa
        //float _r  [samples+1]; //result

        //t0 calculation
        for(int i=0; i<=samples; i++)
        {
            //_t0[i] = expf( -tmp1[i].z / (H0*skybox_height));
            float s_height = vec3_length(tmp1[i]) - planet_radius - _epsilon;
        /*
            if(s_height > atmosphere_depth)
            {
                printf("ERROR in_scatter: s_height= %.2f %2.f \n", s_height , atmosphere_depth);
            }
        */
            _t0[i] = s_height < 0.0 ? 0.0 : expf(sbh_norm*s_height); // expf( -tmp1[i].z / (H0*skybox_height));
        }

        //t1 and t2
        for(int i=0; i<=samples; i++)
        {
            _t1[i] = out_scatter(tmp1[i], _s[i]); //out_scatter from current point to sun
            _t2[i] = out_scatter(tmp1[i], c); //out_scatter from current point to camera
            //printf("%i: _t0,_t1,_t2= %.2f %.2f %.2f \n", i, _t0[i],_t1[i],_t2[i] );
        }

        float _r [samples+1];
        for(int i=0; i<=samples; i++)
            _r[i] = _t0[i]*expf( -1.0*(_t1[i] + _t2[i]) );
        /*
        for(int i=0; i<=samples; i++)
        {
            printf("%i: _r[i] = %.2f, _t0,_t1,_t2= %.2f %.2f %.2f, -_t1[i] -_t2[i]= %.2f, exp= %.2f \n",
                i, _r[i], _t0[i],_t1[i],_t2[i],
                -_t1[i] -_t2[i], exp(-_t1[i] -_t2[i]));
        }
        */

        float tmp = 0.0f;

        for(int i=0; i<samples; i++)
            tmp += _d_half*(_r[i] + _r[i+1]); //  0.5f*_d*(_r[i] + _r[i+1]);//

        //struct Vec3 s2 = vec3_sub(v2, c);

        //printf("phase: %.2f\n", phase(v2, s));
        /*
        printf("a= %.2f %.2f %.2f  b= %.2f %.2f %.2f s= %.2f %.2f %.2f  phase: %.2f \n",
            a.x,a.y,a.z,
            b.x,b.y,b.z,
            s.x,s.y,s.z,
            phase(b, s) );
        */
        //debug
        struct Vec3 bc = vec3_sub(b,c);
        struct Vec3 bs = vec3_sub(s,c);
        tmp *= wavelength_factor*phase(bc, bs);

        //return brightness_scale_factor*(brightness_log_factor*log(tmp) + brightness_sum_factor);
        return tmp;
    }

    static float sun_position(const struct Vec3 &a, const struct Vec3 &b, const struct Vec3 &s)
    {
        struct Vec3 c = a;
        struct Vec3 tb = vec3_normalize(vec3_sub(b, c));
        struct Vec3 ts = vec3_normalize(vec3_sub(s, c));

        float v = vec3_dot(tb, ts);

        if (v <= 0.0) v = 0.0;
        v *= v;
        return v;
    }

    //sun distaWnce
    //atmosphere height
    //planet radius
    /*
    float update_point(const struct Vec3 &c, const struct Vec3 &b, const struct Vec3 &s)
    {
        //const float ATMOSPHERE_DEPTH = 128.0;
        //compute ray from camera to upper atmosphere (use sphere and radius)
        //intersection with upper atomsphere
        //struct Vec3 _b;
        //_b = vec3_scalar_mult(vec3_normalize(b), ATMOSPHERE_DEPTH);

        //ray cast until I hit the sky
        struct Vec3 _b;

        //return sun_position(c,_b,s);
        return in_scatter(c, _b, s);

    }
    */

    void save(const char* filename, int side)
    {
        save_png(filename, farray[side], dim, dim);
    }

    void blit_to_buffer(float* fbuffer, int row)
    {
        if(row == 0)
        {
            for(int i=0; i<dim; i++)
            {
                float _d = 1.0 / ((float) dim);
                farray[0][i*dim+0] = _d*i;
            }
        }

        const int row_size = 6*dim;

        for(int side=0; side<6; side++)
        {
            int xoff = dim*side;
            int yoff = row_size*dim*row;
            for(int j=0; j<dim; j++)
            {
                int _yoff = yoff + j*row_size;

                for(int i=0; i<dim; i++)
                {
                    int _xoff = xoff + i;
                    fbuffer[_xoff + _yoff] = farray[side][i + j*dim];
                }
            }
        }

    }
};

class SkyboxRender
{
    public:
        int time_count;
        int time_speed;

        class Skyplane sun0;        //Rayleigh scattering channel
        class Skyplane sunR;    //Mie aerosol scattering channel
        class Skyplane sunG;
        class Skyplane sunB;

        unsigned char* sun_rgba[6];

        unsigned int texture_array[6];

    SkyboxRender() :
        time_count(0), time_speed(1)
    {
        glEnable(GL_TEXTURE_2D);
        glGenTextures(6, texture_array);

        for(int i=0; i<6; i++)
        {
            glBindTexture(GL_TEXTURE_2D, texture_array[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glDisable(GL_TEXTURE_2D);

        for(int i=0; i<6; i++)
        {
            this->texture_array[i] = 0;
            this->sun_rgba[i] = (unsigned char*)calloc(sun0.dim*sun0.dim*6, sizeof(unsigned char));
        }
    }

    ~SkyboxRender()
    {
        for (int i=0; i<6; i++)
            free(this->sun_rgba[i]);
    }

    void increment_time()
    {
        time_count = (time_count+time_speed) % (30*1200);
    }

    unsigned char gamma_correction[256];

    unsigned char gamma_correct(float _v)
    {
        if( _v < 0.0) _v = 0.0f;
        if( _v > 1.0) _v = 1.0f;
        unsigned char v = ((int) 255.0f*_v );
        return (unsigned char) gamma_correction[v];
    }

    //modifer is 0 to 255
    unsigned char gamma_correct(float _v, float modifier)
    {
        if( _v < 0.0) _v = 0.0f;
        if( _v > 1.0) _v = 1.0f;
        unsigned char v = ((int) modifier*_v );
        return (unsigned char) gamma_correction[v];
    }

    void update_sun(int sun_num)
    {
        int t0 = _GET_MS_TIME();

        float sun_theta = time_count / (30.0*1200.0); //day length
        //float sun_phi = time_count / 3000.0;
        float sun_phi = 0;

        //float sun_w[4];   //wavelength_factor for each channel
        //float sun_g[4];   //g factor for each channels
        //float sun_h[4];//h0, scale height for each channel
        //float sun_b[4];//brightness for each channel

        //load default settings
        if(sun_num==0) sun0.load_settings(SPS);
        if(sun_num==1) sunR.load_settings(SPS);
        if(sun_num==2) sunG.load_settings(SPS);
        if(sun_num==3) sunB.load_settings(SPS);

        sun0.phase_g_factor = sun_g[0];
        sunR.phase_g_factor = sun_g[1];
        sunG.phase_g_factor = sun_g[2];
        sunB.phase_g_factor = sun_g[3];

        sun0.H0 = sun_h[0];
        sunR.H0 = sun_h[1];
        sunG.H0 = sun_h[2];
        sunB.H0 = sun_h[3];

        sun0.wavelength_factor = sun_w[0];
        sunR.wavelength_factor = sun_w[1];
        sunG.wavelength_factor = sun_w[2];
        sunB.wavelength_factor = sun_w[3];

        //run update
        if(sun_num==0) sun0.update(sun_theta, sun_phi); //update float array
        if(sun_num==1) sunR.update(sun_theta, sun_phi); //update float array
        if(sun_num==2) sunG.update(sun_theta, sun_phi); //update float array
        if(sun_num==3) sunB.update(sun_theta, sun_phi); //update float array

        int t1 = _GET_MS_TIME();

        if(false)
            printf("update_sun: %d ms \n", t1-t0);  //10 to 15 ms per update
    }

    void update_skybox()
    {

        //int t0 = _GET_MS_TIME();

        //float sun_theta = time_count / 6750.0; //day length
        //float sun_phi = time_count / 3000.0;
        //float sun_phi = 0;
        //sun0.draw_sun(sun_theta, sun_phi, x,y,z);

        //int t1 = _GET_MS_TIME();

        //set gamma_correction array

        for(int i=0; i < 256; i++)
        {
            float intensity = ((float) i) / 255;
            intensity = powf(intensity, 1.0f/2.2f)*255;
            gamma_correction[i] = (unsigned char)((int) intensity);
        }

        float r_color = sun_color[0];
        float g_color = sun_color[1];
        float b_color = sun_color[2];
        //load RGB from array
        const int dim = sun0.dim;

        for(int side = 0; side<6; side++)
        {
            const float* farray0 = sun0.farray[side];
            const float* farrayR = sunR.farray[side];
            const float* farrayG = sunG.farray[side];
            const float* farrayB = sunB.farray[side];

            for(int i=0; i<dim; i++)
            for(int j=0; j<dim; j++)
            {
                float v0 = farray0[j*dim+i] * sun_b[0];
                float vR = farrayR[j*dim+i] * sun_b[1];
                float vG = farrayG[j*dim+i] * sun_b[2];
                float vB = farrayB[j*dim+i] * sun_b[3];

                //float _v = farray0[j*dim+i];

                sun_rgba[side][4*(i+j*dim)+0] = gamma_correct(v0 + vR, r_color);
                sun_rgba[side][4*(i+j*dim)+1] = gamma_correct(v0 + vG, g_color);
                sun_rgba[side][4*(i+j*dim)+2] = gamma_correct(v0 + vB, b_color);
                sun_rgba[side][4*(i+j*dim)+3] = sun_color[3];


            }

            if(skybox_corners == 1)
            {
                sun_rgba[side][4*(0+0*dim)+0] = 255;    //i=0, j=0
            }
        }

        if(skybox_sundial == 1)
        {
            sun_rgba[sun0.sun_side][4*(sun0.sun_i+sun0.sun_j*sun0.dim)+1] = 255;
        }

        glEnable(GL_TEXTURE_2D);

        for(int side = 0; side<6; side++)
        {
            glBindTexture(GL_TEXTURE_2D, texture_array[side]);

            if(skybox_gl_linear)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sun0.dim, sun0.dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, sun_rgba[side] );
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_TEXTURE_2D);


        CHECK_GL_ERROR();

        //printf("update \n");

        //textures

        //printf("skybox: %d ms, %d ms \n", t1-t0, t2-t1);
    }


    void draw(float x, float y, float z)
    {
        float sun_theta = time_count / (30.0*1200.0); //day length
        //float sun_phi = time_count / 3000.0;
        float sun_phi = 0;

        if(skybox_debug)
        {
            x = 256;
            y = 256;
            z = 128;

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }

        struct Vec3 f[6];
        struct Vec3 r[6];
        struct Vec3 u[6];

        for(int i=0;i<6;i++)
        {
            f[i] = vec3_init(_f[3*i+0],_f[3*i+1],_f[3*i+2] );
            r[i] = vec3_init(_r[3*i+0],_r[3*i+1],_r[3*i+2] );
            u[i] = vec3_init(_u[3*i+0],_u[3*i+1],_u[3*i+2] );
        }

        //center of plane
        struct Vec3 center[6];

        for(int i=0;i<6;i++)
        {
            const float plane_depth = sun0.plane_size*0.5;
            center[i] = vec3_scalar_mult(f[i], plane_depth);
            //center[i].z += plane_depth;

            center[i].x += x;
            center[i].y += y;
            center[i].z += z;
        }


        glEnable(GL_TEXTURE_2D);

        for(int i=0; i<6; i++)
        {
            struct Vec3 rv, uv, lv, dv;
            //f = vec3_scalar_mult(_f[i], sun0.plane_size/2.0);
            rv = vec3_scalar_mult(r[i], sun0.plane_size*0.5);   //right
            uv = vec3_scalar_mult(u[i], sun0.plane_size*0.5);   //up

            //nf = vec3_scalar_mult(_f[i], -sun0.plane_size/2.0);
            lv = vec3_scalar_mult(r[i], -1.0*sun0.plane_size*0.5);  //left
            dv = vec3_scalar_mult(u[i], -1.0*sun0.plane_size*0.5);  //down

            struct Vec3 ul,bl,br,ur;
            ul = vec3_add3(center[i], uv, lv);
            bl = vec3_add3(center[i], dv, lv);
            br = vec3_add3(center[i], dv, rv );
            ur = vec3_add3(center[i], uv, rv);

            glBindTexture(GL_TEXTURE_2D, texture_array[i]);

            //(0,0), 0,1 1,1 1,0

            glBegin(GL_QUADS);

            glTexCoord2f(0.0,1.0);

            glVertex3f(ul.x, ul.y, ul.z);

            glTexCoord2f(0.0,0.0);

            glVertex3f(bl.x, bl.y, bl.z);

            glTexCoord2f(1.0,0.0);

            glVertex3f(br.x, br.y, br.z);

            glTexCoord2f(1.0,1.0);
            glVertex3f(ur.x, ur.y, ur.z);

            glEnd();
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_TEXTURE_2D);


        if(skybox_sundial != 0)
            sun0.draw_sun(sun_theta, sun_phi, x,y,z); //draw sun

        CHECK_GL_ERROR();

    //printf("draw\n");
    }
};


class _PerlinField3D
{

    static inline float mix(float a, float b, float t)
    {
        return a + t*(b-a);   //optimized version
    }

    static inline int fast_floor(float value)
    {
        return (int)(value>=0 ? (int)value : (int)value-1);
    }


    static inline float fade(float t)
    {
        return t*t*t*(t*(t*6-15)+10);
    }

    static inline float dot(int gi, float x, float y, float z)
    {
        static const int g3[12][3] = {
        {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
        {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
        {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}
        };

        return g3[gi][0]*x + g3[gi][1]*y + g3[gi][2]*z;
    }

    public:

    unsigned char* ga;  //gradient array
    float* grad; //gradient vector array
    //static const int ssize = 64*64*32;
    //static const int xsize = 64;

    int ssize;  //number of gradients

    int xsize; int xsize2;
    int zsize;

    float xscale;   //scale multiplier
    float zscale;

    static const int grad_max = 12;   //number of gradients

    //xsize is number of gradients
    _PerlinField3D() : ga(NULL), grad(NULL) {}

    void init(int _xsize, int _zsize)
    {
        if (_xsize < 1) GS_ABORT();
        if (_zsize < 1) GS_ABORT();

        xsize = _xsize;
        xsize2 = xsize*xsize;
        zsize = _zsize;
        ssize = xsize*xsize*zsize;

        ga = new unsigned char[ssize];

        generate_gradient_array();
    }

    ~_PerlinField3D()
    {
        if (this->ga != NULL) delete[] this->ga;
    }

    //OPTIMIZED
    void generate_gradient_array()
    {
        for (int i=0; i<this->ssize; i++) ga[i] = rand() % grad_max; //gradient number
    }

    OPTIMIZED
    inline int get_gradient(int x, int y, int z)
    {
        x = x % xsize; //replace with bitmask
        y = y % xsize;
        z = z % zsize;

        return ga[x + y*xsize + z*xsize2];
    }

    public:

    // Classic Perlin noise, 3D version
    OPTIMIZED
    float base(float x, float y, float z)
    {
        x *= xsize;
        y *= xsize;
        z *= zsize;

        //get grid point
        int X = fast_floor(x);
        int Y = fast_floor(y);
        int Z = fast_floor(z);

        x = x - X;
        y = y - Y;
        z = z - Z;

        int gi000 = get_gradient(X+0,Y+0,Z+0);
        int gi001 = get_gradient(X+0,Y+0,Z+1);
        int gi010 = get_gradient(X+0,Y+1,Z+0);
        int gi011 = get_gradient(X+0,Y+1,Z+1);

        int gi100 = get_gradient(X+1,Y+0,Z+0);
        int gi101 = get_gradient(X+1,Y+0,Z+1);
        int gi110 = get_gradient(X+1,Y+1,Z+0);
        int gi111 = get_gradient(X+1,Y+1,Z+1);

        // Calculate noise contributions from each of the eight corners
        float n000= dot(gi000, x, y, z);
        float n100= dot(gi100, x-1, y, z);
        float n010= dot(gi010, x, y-1, z);
        float n110= dot(gi110, x-1, y-1, z);
        float n001= dot(gi001, x, y, z-1);
        float n101= dot(gi101, x-1, y, z-1);
        float n011= dot(gi011, x, y-1, z-1);
        float n111= dot(gi111, x-1, y-1, z-1);
        // Compute the fade curve value for each of x, y, z

    #if 1
        float u = fade(x);
        float v = fade(y);
        float w = fade(z);
    #else
        float u = x;
        float v = y;
        float w = z;
    #endif

        // Interpolate along x the contributions from each of the corners
        float nx00 = mix(n000, n100, u);
        float nx01 = mix(n001, n101, u);
        float nx10 = mix(n010, n110, u);
        float nx11 = mix(n011, n111, u);
        // Interpolate the four results along y
        float nxy0 = mix(nx00, nx10, v);
        float nxy1 = mix(nx01, nx11, v);
        // Interpolate the two last results along z
        float nxyz = mix(nxy0, nxy1, w);

        return nxyz * 0.707106781f;   //-1 to 1
    }

};

class PerlinClouds
{
    private:
        class _PerlinField3D pf3d;

        static const int xsize = 32;
        static const int zsize = 32;

        float farray[zsize*xsize*xsize];

    public:

    PerlinClouds()
    {
        int xsize = 16;
        int zsize = 32;
        pf3d.init(xsize, zsize);


        float xscale = 1.0/7.0;
        const float of = 0.33;

        for(int i=0; i<xsize; i++)
        {
            for(int j=0; j<xsize; j++)
            {
                for(int k=0; k<zsize; k++)
                {

                    float _if = of + xscale*((float) i);
                    float _jf = of + xscale*((float) j);
                    float _kf = of + xscale*((float) k);

                    farray[k*xsize*xsize + j*xsize + i] = 0.01 + pf3d.base(_if,_jf,_kf);

                    //printf("f= %f \n", farray[k*xsize*xsize + j*xsize + i] );

                }
            }
        }
    }

    ~PerlinClouds()
    {

    }


    void draw(float x, float y, float z)
    {
        //float cloud_size = 64.0;
        //float _cloud_size = 1.0/cloud_size;

        bool TEXTURED = true;

        static int time_counter = 0;

        time_counter++;

        int time_interval = 120*8;

        int tc = (time_counter % time_interval);


        float tfloat = ((float) tc) / ((float) time_interval);


        //printf("%f tfloat \n", tfloat);

        int k0 = (time_counter/time_interval) % zsize;
        int k1 = (time_counter/time_interval +1) % zsize;

        float cloud_spacing = 16.0;
        //float csize = 4.0;
        float size_mult = 12.0+32.0;
        float size_add  = -0.5;

        float _z = t_map::map_dim.z + 5.0f;


        const float ca[2*4] =
        {
            1, 1,
            -1,1,
            -1,-1,
            1,-1
        };

        //printf("k0,k1= %d %d tc= %d \n", k0,k1,tc);
    /*
        glBegin(GL_QUADS);

        for(int _i=0; _i<4; _i++)
            glVertex3f(x + 5.0*ca[_i*2+0], y + 5.0*ca[_i *2+1], z+25.0);

        glEnd();
*/
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glDisable(GL_CULL_FACE);

        if(TEXTURED)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, cloud_blur_texture);
        }

        GS_ASSERT(cloud_blur_texture != 0);

        glColor4ub(0, 0, 127, 127);
        //glColor4ub(255, 255, 255, 255);

        glBegin(GL_QUADS);


        //glDisable(GL_CULL_FACE);
        //glBegin(GL_QUADS);


        for(int i=0; i<xsize; i++)
        {
            for(int j=0; j<xsize; j++)
            {
                float s0 = farray[k0*xsize*xsize + j*xsize + i];
                float s1 = farray[k1*xsize*xsize + j*xsize + i];

                float s = (1.0-tfloat)*s0 + tfloat*s1;
            /*
                if(i==5 && j==5)
                    printf("i,j= %d %d tfloat= %.2f s0= %f s1= %f s= %f k0,k1= %d %d tc= %d \n",
                        i,j, tfloat, s0,s1,s, k0,k1,tc);
            */
                float _if = (float) i;
                float _jf = (float) j;
                float xsizef = (float) xsize;

                float _x = x + cloud_spacing*_if - cloud_spacing*xsizef/2.0;// - cloud_size/2.0;
                float _y = y + cloud_spacing*_jf - cloud_spacing*xsizef/2.0;// - cloud_size/2.0;


                //printf("s= %f, i,j= %d %d misc= %f %f %f \n", s, i,j, x,cloud_spacing*((float)(i)) , cloud_spacing*xsize/2);

                s = s*size_mult + size_add;

                //s = 5.0;
                if(s <= 0.0)
                    continue;

                //printf("i,i= %d %d _x, _y= %f %f \n", i,j, _x, _y);


                //float _csize = s*csize;
            if(!TEXTURED)
            {
                for(int _i=0; _i<4; _i++)
                    glVertex3f(_x + s*ca[_i*2+0], _y + s*ca[_i *2+1], _z);
            }
            else
            {
                glTexCoord2f(0.0,1.0);
                glVertex3f(_x + s*ca[0*2+0], _y + s*ca[0*2+1], _z);

                glTexCoord2f(0.0,0.0);
                glVertex3f(_x + s*ca[1*2+0], _y + s*ca[1*2+1], _z);

                glTexCoord2f(1.0,0.0);
                glVertex3f(_x + s*ca[2*2+0], _y + s*ca[2*2+1], _z);

                glTexCoord2f(1.0,1.0);
                glVertex3f(_x + s*ca[3*2+0], _y + s*ca[3*2+1], _z);


            }
            //glEnd();

            //glColor3ub(255, 255, 255);

            }
        }
        glEnd();


        glColor4ub(255, 255, 255, 255);

        CHECK_GL_ERROR();
    }
};


//perlin clouds


static class SkyboxRender* SR;
static class ConfigFileLoader CFL;
//sfloat test_float = 0.0f;
static class PerlinClouds* PC;

int skybox_update_rate = 15;

//Rayleigh scattering can be approximated by setting g to 0
//Mie aerosol scattering, g is usually set between -0.75 and -0.999
//Negative values of g scatter more light in a forward direction
//positive values of g scatter more light back toward the light source.

void init_rayleigh_scattering()
{

    SR = new SkyboxRender;

    PC = new PerlinClouds;
    //SR->update_skybox(); //wait for time to do this?

    //CFL.set_float("test", &test_float);

    CFL.set_float("planet_radius", &SPS.planet_radius);
    CFL.set_float("atmosphere_depth", &SPS.atmosphere_depth);
    CFL.set_float("sun_distance", &SPS.sun_distance);

    CFL.set_float("wavelength_factor", &SPS.wavelength_factor);
    CFL.set_float("phase_g_factor", &SPS.phase_g_factor);
    CFL.set_color("color", (char*) sun_color);

    CFL.set_float("sun_w0", &sun_w[0]);
    CFL.set_float("sun_wR", &sun_w[1]);
    CFL.set_float("sun_wG", &sun_w[2]);
    CFL.set_float("sun_wB", &sun_w[3]);

    CFL.set_float("sun_g0", &sun_g[0]);
    CFL.set_float("sun_gR", &sun_g[1]);
    CFL.set_float("sun_gG", &sun_g[2]);
    CFL.set_float("sun_gB", &sun_g[3]);

    CFL.set_float("sun_h0", &sun_h[0]);
    CFL.set_float("sun_hR", &sun_h[1]);
    CFL.set_float("sun_hG", &sun_h[2]);
    CFL.set_float("sun_hB", &sun_h[3]);

    CFL.set_float("sun_b0", &sun_b[0]);
    CFL.set_float("sun_bR", &sun_b[1]);
    CFL.set_float("sun_bG", &sun_b[2]);
    CFL.set_float("sun_bB", &sun_b[3]);

    //CFL.set_float("brightness_log_factor", &SPS.brightness_log_factor);
    CFL.set_float("brightness_scale_factor", &SPS.brightness_scale_factor);
    CFL.set_float("brightness_sum_factor", &SPS.brightness_sum_factor);
    CFL.set_float("light_epsilon", &SPS.light_epsilon);

    CFL.set_float("H0", &SPS.H0);
    CFL.set_float("skybox_height", &SPS.skybox_height);
    CFL.set_float("camera_z", &SPS.camera_z);

    CFL.set_int("time_speed", &SR->time_speed);
    CFL.set_int("skybox_update_rate", &skybox_update_rate);
    CFL.set_int("skybox_gl_linear", &skybox_gl_linear);
    CFL.set_int("skybox_normalization", &skybox_normalization);
    CFL.set_int("skybox_corners", &skybox_corners);
    CFL.set_int("skybox_sundial", &skybox_sundial);

    CFL.set_int("skybox_debug", &skybox_debug);
    CFL.set_int("print_max_light", &print_max_light);

    CFL.load_file(MEDIA_PATH "config/skybox.cfg", true);   //load file silently first time


    if(false)
    {
        class Skyplane S;
        S.load_settings(SPS);

        if(true)
        {
            int max_div = 16;
            float _f = 1.0 / ((float) max_div);

            int dim = S.dim;
            float* fbuffer = new float[6*dim*dim*max_div];
            for(int i=0; i<max_div; i++)
            {
                char filename[128];

                snprintf(filename, 128,"light_%.2i", i);
                S.update(_f*((float) i) , 0.0); //height, then rotation
                //S.save(filename);
                S.blit_to_buffer(fbuffer, i);
            }

            save_png("sky", fbuffer, 6*dim, dim*max_div);

        }
        else
        {
            S.update(0.0, 0.0); //height, then rotation
            S.save("sky", 2);
        }
    }

/*
    Load cloudblur texture
*/
    int ret= create_texture_from_file(MEDIA_PATH "sprites/skybox/blur.png", &cloud_blur_texture, GL_LINEAR, GL_LINEAR);
    if(ret != 0)
    {
        GS_ASSERT_ABORT(false);
    }

}

void tick_rayleigh_scattering()
{
    static int update_count = 0;
    update_count++;

    //reload every 4 seconds unless skybox_debug == 1 or production is true
    if( !PRODUCTION || skybox_debug == 1 || update_count % 120 == 0)
    {
        CFL.load_file(MEDIA_PATH "config/skybox.cfg"); //reload file during debug
    }

    SR->increment_time();



    int _skybox_update_rate = skybox_update_rate < 5 ? 5 : skybox_update_rate;
    int v = update_count % _skybox_update_rate;

    if(v==0) SR->sun0.load_settings(SPS);
    if(v==1) SR->sunR.load_settings(SPS);
    if(v==2) SR->sunG.load_settings(SPS);
    if(v==3) SR->sunB.load_settings(SPS);

    if(v==0) SR->update_sun(0);
    if(v==1) SR->update_sun(1);
    if(v==2) SR->update_sun(2);
    if(v==3) SR->update_sun(3);

    //update skybox
    if(v==4) SR->update_skybox();

}

void draw_rayleigh_scattering()
{

    SR->draw(current_camera_position.x, current_camera_position.y, current_camera_position.z);
    PC->draw(current_camera_position.x, current_camera_position.y, current_camera_position.z);
}



}
