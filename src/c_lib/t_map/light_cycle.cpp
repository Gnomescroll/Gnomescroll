#pragma once

namespace t_map
{

//what does this do
class DayCycleController
{
    public:

    //start of day is sun up
    //then red fade in
    //then red fade out
    //then sunset

    int dtime; //time in seconds from day start

    float blue_tint;  //blue tint (twilight)
    float red_tint;   //red tint  (radiation)
    float brightness; //total brightness

    DayCycleController()
    {
        dtime = 0;
    }

    #if DC_SERVER
    void advance()
    {
        const int UPDATE_INTERVAL = 1000; //ms between update

        //static int t_start = _GET_MS_TIME();

        static int last = _GET_MS_TIME();
        int current = _GET_MS_TIME();

        static int inc = 0;

        inc += current - last; //ms since last update
        last = current;

        if(inc <= UPDATE_INTERVAL)
            return;

        if(inc > UPDATE_INTERVAL)
        {
            dtime += UPDATE_INTERVAL /1000;
            inc -= UPDATE_INTERVAL;
            //printf("time= %d, ms= %d \n", dtime, last - t_start);
        }

        if(inc > UPDATE_INTERVAL + 1000)
        {
            printf("DayCycleController: Warning, time is %d ms behind \n", inc);
        }

    }
    #endif
    
    void update()
    {
        const int cycle_length = 1000; //1000 seconds per day

        const float sunrise_length = 0.15f;
        const float day_length = 0.40f;
        const float sunset_length = 0.15f;
        const float night_length = 0.30f;

        float ct = dtime % cycle_length; //current time

        ct = ct / cycle_length;

        GS_ASSERT(ct <= 1.0f);
        GS_ASSERT(ct >= 0.0f);

        //sunrise
        if(ct < sunrise_length)
        {
            red_tint = 0.0f;
            blue_tint = 0.0f;
            brightness = ct / sunrise_length; //goes for 0 to 1

            return;
        }
        ct -= sunrise_length;
        
        //day
        if(ct < day_length)
        {
            red_tint = 0.0f;
            blue_tint = 0.0f;
            brightness = 0.0f;

            return;
        }
        ct -= day_length;
        
        //sunset length
        if(ct < sunset_length)
        {
            red_tint = 0.0f;
            blue_tint = 0.0f;
            brightness = 1.0f - (ct / sunset_length); //goes for 1 to 0
            return;
        }

        ct -= sunset_length;
        
        //night
        if(ct < night_length)
        {
            red_tint = 0.0f;
            blue_tint = 0.2f;
            brightness = 0.2;
            return;
        }
        ct -= night_length;
    }

    void advance2()
    {
        //const int cycle_lenth = 1000; //1000 seconds per day

        //const float sunrise_length = 0.15f;
        //const float day_length = 0.40f;
        //const float sunset_length = 0.15f;
        //const float night_length = 0.30f;

        //blue tint
        //red tint
        //brightness (total)


    }
};

#if DC_CLIENT

class LightTextureGenerator2
{
    public:
        static const int dim = 16;
        float values[3*dim*dim];
        //float values2[3*dim*dim];
        GLuint texture_array[1];

    LightTextureGenerator2()
    {
        memset(this->values, 0, sizeof(this->values));
        //memset(this->values2, 0, sizeof(this->values2));
        memset(this->texture_array, 0, sizeof(this->texture_array));
    }

    ~LightTextureGenerator2()
    {

    }

    void init()
    {
        init_debug2();
    }

    static float falloff(int it, float falloff)
    {
        float t = 1.0;
        for (int i=0; i<it; i++)
            t *= falloff;
        return t;
    }

    static inline struct Vec3 vec3_mix(struct Vec3 a, struct Vec3 b, float f)
    {
        return vec3_add(vec3_scalar_mult(a, 1.0-f),  vec3_scalar_mult(b, f));
    }

    //need light values for mourning and dusk
    struct Vec3 get_twist2(int i, float lightv)
    {
        const struct Vec3 b = vec3_init(1.0f, 1.0f, 1.0f);   //white light
        const struct Vec3 a = vec3_init(1.6f, 0.4f, 0.4f);   //gamma danger twist

        const float twist_start = 0.5; //light level when twist starts
        if(lightv < twist_start)
            return b;
        lightv -= twist_start;
        lightv /= (1.0 - twist_start);

        if(lightv <= 0.0 - 0.001 || lightv >= 1.0 + 0.001)
            printf("ERROR: lightv= %f \n", lightv);

        if (i<=10)
            return b;
        if (i==11)
            return vec3_mix(b, vec3_mix(b,a,0.2f), lightv);
        if (i==12)
            return vec3_mix(b, vec3_mix(b,a,0.4f), lightv);
        if (i==13)
            return vec3_mix(b, vec3_mix(b,a,0.6f), lightv);
        if (i==14)
            return vec3_mix(b, vec3_mix(b,a,0.8f), lightv);
        if (i==15)
            return vec3_mix(b, vec3_mix(b,a,1.0f), lightv);
        return b;
    }

    //replaced by the controller
    float calc_lightv(float ttime)
    {
        return 1.0;

    }

    void init3(float ttime)
    {

        float lightv = calc_lightv(ttime);
        //printf("ttime= %f lightv = %f \n", ttime, lightv);

        struct Vec3 d2 = vec3_init(0.0, 1.0, 1.0);

        struct Vec3 L1[16]; //natural light
        struct Vec3 L2[16]; //artificial light

        for (int i=0; i<16; i++)
        {
            float factor = falloff(15-i, 0.75);

            L1[i] = vec3_scalar_mult(get_twist2(i, lightv), factor); //add in gamma twist latter
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
                //int _i = i;
                const int _j = 15-j;

                struct Vec3 t3;
                t3.x = lightv*L1[i].x + L2[_j].x;
                t3.y = lightv*L1[i].y + L2[_j].y;
                t3.z = lightv*L1[i].z + L2[_j].z;

                values[3*(dim*_j+i)+0] = t3.x;
                values[3*(dim*_j+i)+1] = t3.y;
                values[3*(dim*_j+i)+2] = t3.z;
            }
        }
    }

    //no light
    void init_debug()
    {
        //dropping blue looks cool
        struct Vec3 d2 = vec3_init(1.0, 1.0, 1.0);

        for (int i=0; i<dim; i++)
        {
            for (int j=0; j<dim; j++)
            {
                values[3*(dim*(15-j)+i)+0] = d2.x;
                values[3*(dim*(15-j)+i)+1] = d2.y;
                values[3*(dim*(15-j)+i)+2] = d2.z;
            }
        }
    }

    void init_debug2()
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
                values[3*(dim*(15-j)+i)+0] = t3.x;
                values[3*(dim*(15-j)+i)+1] = t3.y;
                values[3*(dim*(15-j)+i)+2] = t3.z;
            }
        }
    }

    void save(const char* filename, bool option)
    {
        save_png_RGB(filename, values, dim, dim, option);
    }

    void gen_textures()
    {
    #if 0
        for (int i=0; i<dim; i++)
        {
            for (int j=0; j<dim; j++)
            {
                values2[3*(j*dim +i)+0] = values[3*((15-j)*dim + i)+0]; //fix
                values2[3*(j*dim +i)+1] = values[3*((15-j)*dim + i)+1];
                values2[3*(j*dim +i)+2] = values[3*((15-j)*dim + i)+2];
            }
        }
    #endif
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, texture_array);


        glBindTexture(GL_TEXTURE_2D, texture_array[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dim, dim, 0, GL_RGB, GL_FLOAT, values);

        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_TEXTURE_2D);

    }
};
#endif

#if DC_CLIENT

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


    struct Vec3 vec3_mix(struct Vec3 a, struct Vec3 b, float f)
    {
        return vec3_add(vec3_scalar_mult(a, 1.0 -f),  vec3_scalar_mult(b, f));
    }

    struct Vec3 get_twist(int i)
    {
        struct Vec3 b = vec3_init(1.0, 1.0, 1.0);   //white light
        struct Vec3 a = vec3_init(1.6f, 0.4f, 0.4f);   //gamma danger twist

        if (i<=10)
            return b;
        if (i==11)
            return vec3_mix(b,a,0.2f);
        if (i==12)
            return vec3_mix(b,a,0.4f);
        if (i==13)
            return vec3_mix(b,a,0.6f);
        if (i==14)
            return vec3_mix(b,a,0.8f);
        if (i==15)
            return vec3_mix(b,a,1.0f);

        return b;
    }


    //need light values for mourning and dusk
    struct Vec3 get_twist2(int i, float lightv)
    {
        struct Vec3 b = vec3_init(1.0f, 1.0f, 1.0f);   //white light
        struct Vec3 a = vec3_init(1.6f, 0.4f, 0.4f);   //gamma danger twist

        const float twist_start = 0.5; //light level when twist starts
        if(lightv < twist_start)
            return b;
        lightv -= twist_start;
        lightv /= (1.0 - twist_start);

        if(lightv <= 0.0 - 0.001 || lightv >= 1.0 + 0.001)
            printf("ERROR: lightv= %f \n", lightv);

        if (i<=10)
            return b;
        if (i==11)
            return vec3_mix(b, vec3_mix(b,a,0.2f), lightv);
        if (i==12)
            return vec3_mix(b, vec3_mix(b,a,0.4f), lightv);
        if (i==13)
            return vec3_mix(b, vec3_mix(b,a,0.6f), lightv);
        if (i==14)
            return vec3_mix(b, vec3_mix(b,a,0.8f), lightv);
        if (i==15)
            return vec3_mix(b, vec3_mix(b,a,1.0f), lightv);
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
        const float sunrise_length = 0.20f;
        const float day_length = 0.30f;
        const float sunset_length = 0.25f;
        const float night_length = 0.25f;

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
        //printf("ttime= %f lightv = %f \n", ttime, lightv);

        struct Vec3 d2 = vec3_init(0.0, 1.0, 1.0);

        struct Vec3 L1[16]; //natural light
        struct Vec3 L2[16]; //artificial light



        for (int i=0; i<16; i++)
        {
            float factor = falloff(15-i, 0.75);

            L1[i] = vec3_scalar_mult(get_twist2(i, lightv), factor); //add in gamma twist latter
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
    }

    //no light
    void init_debug()
    {
        //dropping blue looks cool
        struct Vec3 d2 = vec3_init(1.0, 1.0, 1.0);

        for (int i=0; i<dim; i++)
        {
            for (int j=0; j<dim; j++)
            {
                values[3*(dim*j+i)+0] = d2.x;
                values[3*(dim*j+i)+1] = d2.y;
                values[3*(dim*j+i)+2] = d2.z;
            }
        }
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
#endif

class DayCycleController* DCC = NULL;

#define LIGHTING_DISABLED 0

#if DC_SERVER
//advance the day cycle on server
void update_day_cycle()
{
    if(DCC == NULL)
        DCC = new DayCycleController;

    DCC->advance();

}
#endif


#if DC_CLIENT

class LightTextureGenerator2* LTG;

unsigned int generate_clut_light_texture()
{

#if LIGHTING_DISABLED
    LTG->init_debug();
    LTG->gen_textures();
    return LTG->texture_array[0];
#endif

    static int ttime = 0;

    ttime++;

    if(ttime % 15 == 0)
    {
        static int ttimec = 0;
        ttimec++;
        float _ttime = ttimec % 400;
        _ttime /= 400.0;

        LTG->init3(_ttime);
        LTG->gen_textures();
    }

    return LTG->texture_array[0];
}

void generate_light_texture()
{

    LTG = new LightTextureGenerator2;
    LTG->init_debug2();
    LTG->save("light_texture", true);

    //LTG->gen_textures();
    //LTG->save("light_texture_debug", true);
    //LTG->gen_textures();

    //light_texture_CLUT = LTG.texture_array[0];
}

#endif

}
