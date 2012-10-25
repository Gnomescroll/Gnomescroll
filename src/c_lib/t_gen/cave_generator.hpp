#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_gen/noise_map2.hpp>


#include <t_gen/twister.hpp>

#include <t_map/t_map.hpp>

#include <t_mech/_interface.hpp>
#include <t_mech/properties.hpp>


namespace t_gen 
{

// v is starting point of line
// vz is unit vector for direction of line
// px is the point
// returns square of distance between line and point
__attribute((always_inline, optimize("-O3")))
float point_line_distance2(float vx, float vy, float vz, float wx, float wy, float wz, float px, float py, float pz)
{

    px -= vx;
    py -= vy;
    pz -= vz;

    float t = px*wx + py*wy + pz*wz;

    float x = t*wx - px;
    float y = t*wy - py;
    float z = t*wz - pz;

    return x*x + y*y + z*z;

}

__attribute__((optimize("-O3")))
void generate_node(float xs, float ys, float zs, float theta, float phi, float cave_size)
{
    const int baseline = 0;
    
    while( genrand_real1() < 0.999f )
    {
            const static float length = 2.0f;

        /*
            float dx = length*sin(phi)*cos(theta)
            float dy = length*sin(phi)*sin(theta);
            float dz = length*cos(phi);
        */

            const float _theta = theta*2*3.14159f;
            const float _phi = phi*2*3.14159f;

            float dx = (float)(sin(_phi)*cos(_theta));
            float dy = (float)(sin(_phi)*sin(_theta));
            float dz = cosf(0.75f*_phi);
            dz *= dz;

            float size = cave_size * ((float)genrand_real1()*1.25f + 0.5f);  // variable diameter

            float xm = abs(dx) + size;
            float ym = abs(dy) + size;
            float zm = abs(dz) + size;

            int xmin = xs - xm;
            int xmax = xs + xm;
    
            int ymin = ys - ym;
            int ymax = ys + ym;

            int zmin = zs - zm;
            int zmax = zs + zm;


            bool hits_bottom = false;
            //can speed up by 8
            for(int i=xmin; i<=xmax; i++)
            for(int j=ymin; j<=ymax; j++)
            for(int k=zmin; k<=zmax; k++)
            {
                if(k < baseline || k >= 128) 
                {
                    hits_bottom = true;
                    continue;
                }

                float x = ((float)i) + 0.5f;
                float y = ((float)j) + 0.5f;
                float z = ((float)k) + 0.5f;

                int ii = i%512;
                int jj = j%512;

                float d = point_line_distance2(xs,ys,zs, dx,dy,dz, x,y,z);
                if(d < size*size) t_map::set(ii, jj, k, EMPTY_CUBE);
            }

            if(hits_bottom == true) phi *= -1;

            xs += length*dx;
            ys += length*dy;
            zs += length*dz;

            static const float theta_adj = 0.15f;
            static const float phi_adj = 0.10f;

            theta += theta_adj*((float)(2.0*genrand_real1() - 1.0));
            phi += phi_adj*((float)(2.0*genrand_real1() - 1.0));

            if(phi < 0) phi += 1;
            if(phi > 1) phi -= 1;

            //static const float phi_target = 0.0f;
            //static const float phi_damp = 0.03f;

            //phi -= phi_damp*(phi - phi_target);
        /*
            if(phi < 0) phi += 1;
            if(phi > 1) phi -= 1;

            if(zs < 32 && genrand_real1() < 0.20)
            {
                if(phi > 0.25) phi -= 0.10;
            } 
        */
    }

}


void start_cave_generator()
{
    const int nodes = 300;
    const float cave_size = 2.0f;

    const float baseline = 0.0f;
    const int try_limit = 10000;
    
    init_genrand(rand());

    for (int i=0; i<nodes; i++)
    {
        float x,y,z;
        int tries = 0;
        do
        {
            x = (float)genrand_real1()*(float)XMAX;
            y = (float)genrand_real1()*(float)YMAX;
            z = (float)genrand_real1()*((float)ZMAX-baseline) + baseline;
        } while (t_map::get(x,y,z) == 0 && tries++ < try_limit);

        if (tries >= try_limit) return; 

        float phi = (float)genrand_real1()*2*3.14159f;
        float theta = (float)genrand_real1()*2*3.14159f;
        
        generate_node(x,y,z, theta, phi, cave_size);
    }
}


}   // t_gen
