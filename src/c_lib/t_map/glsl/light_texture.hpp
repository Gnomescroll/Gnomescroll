#pragma once

namespace t_map
{

class LightTextureGenerator
{
    public:
        static const int dim = 16;
        float values[3*dim*dim];
        float values2[3*dim*dim];
        GLuint texture_array[1];

    LightTextureGenerator()
    {
        memset(this->values, 0, sizeof(this->values));
        memset(this->values2, 0, sizeof(this->values2));
        memset(this->texture_array, 0, sizeof(this->texture_array));
    }

    ~LightTextureGenerator()
    {

    }

    void init()
    {
        struct Vec3 ul = vec3_init(0.0, 1.0, 0.0);  //darkness
        struct Vec3 br = vec3_init(1.0, 0.0, 0.0);  //light
        struct Vec3 ur = vec3_init(0.0, 0.0, 0.0);
        struct Vec3 bl = vec3_init(0.0, 0.0, 0.0);

        for (int i=0; i<dim; i++)
        {
            float xlerp = float(i) / float(dim-1);
            struct Vec3 t1 = vec3_lerp(ul, ur, xlerp);
            struct Vec3 t2 = vec3_lerp(bl, br, xlerp);

            for (int j=0; j<dim; j++)
            {
                float ylerp = 1.0 - float(j) / float(dim-1);
                struct Vec3 t3 = vec3_lerp(t1, t2, ylerp);
                values[3*(dim*j+i)+0] = t3.x;
                values[3*(dim*j+i)+1] = t3.y;
                values[3*(dim*j+i)+2] = t3.z;

            }
        }
    }

    /*
        Adjustments:
        - brightest 3 sunlight textures should be very close together
        - need to adjust darkness of darkest light level

    */
    /*
        As sunset proceeds
        - the falloff should approach 1
        - total brightness should decrease

        During peak sunlight
        - the highest 3-4 brightness levels should have "gamma twist"
        - gamma twist represents the danger of the radiation
        - gamma twist is modulated by artificial light (shifted towards blue)

        Possibly:
        - use alpha channel of the color for saturation/desaturation CLUT operation before applying in lighting
        - apply desaturation CLUT, then use alpha channel as mixing between desaturated color and base color

        (independent of original pixel color, only brightness)
        Desaturation Blending vs Multiplicative Blending
        - desaturate with CLUT
        - apply lighting to desaturated pixel
        - blend the desaturated pixel with the sunlight color
        - mix the the original pixel and desaturated pixel for resultant

        (cheaper is to average in pixel brightness before applying)
    */

    static float falloff(int it, float falloff)
    {
        float t = 1.0;
        for (int i=0; i<it; i++)
            t *= falloff;
        return t;
    }

    struct Vec3 get_twist(int i)
    {
        struct Vec3 b = vec3_init(1.0, 1.0, 1.0);   //white light
        struct Vec3 a = vec3_init(1.6, 0.4, 0.4);   //gamma danger twist

        if (i<=11)
            return b;
        if (i==12)
            return vec3_mix(a,b,0.4);
        if (i==13)
            return vec3_mix(a,b,0.6);
        if (i==14)
            return vec3_mix(a,b,0.8);
        if (i==15)
            return vec3_mix(a,b,1.0);

        return b;
    }

    struct Vec3 vec3_mix(struct Vec3 a, struct Vec3 b, float f)
    {
        return vec3_add(vec3_scalar_mult(a, 1.0 -f),  vec3_scalar_mult(b, f));
    }

    struct Vec3 get_twist2(int i, float lightv)
    {
        struct Vec3 b = vec3_init(1.0, 1.0, 1.0);   //white light
        struct Vec3 a = vec3_init(1.6, 0.4, 0.4);   //gamma danger twist

        if(lightv < 0.5)
            return b;
        lightv -= 0.5;
        lightv /= 0.5;


        if (i<=11)
            return b;
        if (i==12)
            return vec3_mix(a,b,0.4);
            //return vec3_add(vec3_scalar_mult(a, 0.4),  vec3_scalar_mult(b, 0.6));

        if (i==13)
            return vec3_mix(a,b,0.6);

            //return vec3_add(vec3_scalar_mult(a, 0.6),  vec3_scalar_mult(b, 0.4));
        if (i==14)
            return vec3_mix(a,b,0.8);

            //return vec3_add(vec3_scalar_mult(a, 0.8),  vec3_scalar_mult(b, 0.2));
        if (i==15)
            return vec3_mix(a,b,1.0);
            //return vec3_add(vec3_scalar_mult(a, 1.0),  vec3_scalar_mult(b, 0.0));

        return b;
    }

    void init2()
    {

        //struct Vec3 d1 = vec3_init(1.6, 0.4, 0.4);
        struct Vec3 d2 = vec3_init(0.0, 1.0, 1.0);

        struct Vec3 L1[16]; //natural light
        struct Vec3 L2[16]; //artificial light


        for (int i=0; i<16; i++)
        {
            float factor = falloff(15-i, 0.75);
            L1[i] = vec3_scalar_mult(get_twist(i), factor); //add in gamma twist latter
        }

        for (int i=0; i<16; i++)
        {
            float factor = falloff(15-i, 0.75);
            L2[i] = vec3_scalar_mult(d2, factor); //add in twist latter
        }

        for (int i=0; i<dim; i++)
        {

            for (int j=0; j<dim; j++)
            {
                int _i = i;
                int _j = 15-j;

                struct Vec3 t3;

                t3.x = L1[_i].x + L2[_j].x;
                t3.y = L1[_i].y + L2[_j].y;
                t3.z = L1[_i].z + L2[_j].z;

                values[3*(dim*j+i)+0] = t3.x;
                values[3*(dim*j+i)+1] = t3.y;
                values[3*(dim*j+i)+2] = t3.z;

            }
        }

/*
        struct Vec3 ul;
        struct Vec3 ur;
        struct Vec3 br;
        struct Vec3 bl;

        ul = vec3_init(0.0, 1.0, 0.0);  //darkness
        br = vec3_init(1.0, 0.0, 0.0);  //light

        ur = vec3_init(0.0, 0.0, 0.0);
        bl = vec3_init(0.0, 0.0, 0.0);

        for (int i=0; i<dim; i++)
        {
            float xlerp = ((float) i) / ((float) (dim-1));

            struct Vec3 t1 = vec3_lerp(ul, ur, xlerp);
            struct Vec3 t2 = vec3_lerp(bl, br, xlerp);

            for (int j=0; j<dim; j++)
            {
                float ylerp = 1.0 - ((float) j) / ((float) (dim-1));

                struct Vec3 t3 = vec3_lerp(t1, t2, ylerp);

                values[3*(dim*j+i)+0] = t3.x;
                values[3*(dim*j+i)+1] = t3.y;
                values[3*(dim*j+i)+2] = t3.z;

            }
        }
*/
    }

    float calc_lightv(float ttime)
    {
        GS_ASSERT(ttime >= 0.0f && ttime <= 1.0f);

        //sunrise is start of day
        const float sunrise_length = 0.15;
        const float day_length = 0.35;
        const float sunset_length = 0.15;
        const float night_length = 0.35;

        float lightv = 0.0f; //how much day and how much night?

        if(ttime < sunrise_length)
        {
            lightv = ttime / sunrise_length;
            return lightv;
        }

        ttime -= sunrise_length;

        if(ttime < day_length)
        {
            return 1.0;
        }

        ttime -= day_length;

        if(ttime < sunset_length)
        {
            return 1.0 - ttime / sunset_length;
        }

        ttime -= sunset_length;

        if(ttime <= night_length)
        {
            return 0.0f;
        }

        GS_ASSERT(false);

        return 1.0f;
    }

    void init3(float ttime)
    {

        float lightv = calc_lightv(ttime);

        struct Vec3 d2 = vec3_init(0.0, 1.0, 1.0);

        struct Vec3 L1[16]; //natural light
        struct Vec3 L2[16]; //artificial light



        for (int i=0; i<16; i++)
        {
            float factor = falloff(15-i, 0.75);

            L1[i] = vec3_scalar_mult(get_twist(i), factor); //add in gamma twist latter
        }

        for (int i=0; i<16; i++)
        {
            float factor = falloff(15-i, 0.75);
            L2[i] = vec3_scalar_mult(d2, factor); //add in twist latter
        }

        for (int i=0; i<dim; i++)
        {

            for (int j=0; j<dim; j++)
            {
                int _i = i;
                int _j = 15-j;

                struct Vec3 t3;

                t3.x = lightv*L1[_i].x + L2[_j].x;
                t3.y = lightv*L1[_i].y + L2[_j].y;
                t3.z = lightv*L1[_i].z + L2[_j].z;

                values[3*(dim*j+i)+0] = t3.x;
                values[3*(dim*j+i)+1] = t3.y;
                values[3*(dim*j+i)+2] = t3.z;

            }
        }

        //printf("ttime= %f lightv = %f \n", ttime, lightv);
    }

    void save(const char* filename)
    {


        //save_png_RGB(const char* filename, float* in, int xres, int yres)
        save_png_RGB("light_texture0", values, dim, dim, false);
        save_png_RGB("light_texture1", values, dim, dim, true);
    }

    void gen_textures()
    {

        for (int i=0; i<dim; i++)
        {
            for (int j=0; j<dim; j++)
            {
                values2[3*(j*dim +i)+0] = values[3*((15-j)*dim + i)+0];
                values2[3*(j*dim +i)+1] = values[3*((15-j)*dim + i)+1];
                values2[3*(j*dim +i)+2] = values[3*((15-j)*dim + i)+2];
            }
        }

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, texture_array);

        //for (int i=0; i<1; i++)
        //{
            glBindTexture(GL_TEXTURE_2D, texture_array[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dim, dim, 0, GL_RGB, GL_FLOAT, values2 );
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, dim, dim, 0, GL_RGB, GL_FLOAT, values );

        //}
        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_TEXTURE_2D);


    }
};

class LightTextureGenerator* LTG;

unsigned int generate_clut_light_texture()
{
    static int ttime = 0;

    ttime++;

    if(ttime % 120 == 0)
    {   
        float _ttime = ttime % 3600;
        _ttime /= 360.0;

        LTG->init3(_ttime);
        LTG->gen_textures();
    }

    return LTG->texture_array[0];
}

void generate_light_texture()
{

    LTG = new LightTextureGenerator;
    LTG->init2();
    LTG->save("light_texture");
    LTG->gen_textures();

    //light_texture_CLUT = LTG.texture_array[0];
}

}   // t_map
